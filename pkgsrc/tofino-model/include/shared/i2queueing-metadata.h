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

#ifndef _SHARED_I2QUEUEING_METADATA_
#define _SHARED_I2QUEUEING_METADATA_
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <boost/optional.hpp>
#include <rmt-defs.h>
#include <afc-metadata.h>

namespace MODEL_CHIP_NAMESPACE {

  // Metadata passed from ingress deparser to queueing block.
  class I2QueueingMetadata {

  public:
    // Getter functions.
    inline uint8_t  version()        const { return version_; }
    inline uint8_t  icos()           const { return icos_; }
    inline bool     cpu_needs_copy() const { return copy_to_cpu_; }
    inline bool     needs_mc_copy()  const { return mgid1_valid_ || mgid2_valid_; }
    inline uint8_t  cpu_cos()        const { return copy_to_cpu_cos_; }
    inline bool     is_egress_uc()   const { return egress_unicast_valid_; }
    inline uint16_t egress_uc_port_regardless() const { return egress_unicast_port_; }
    inline uint16_t egress_uc_port() const { return is_egress_uc() ? egress_uc_port_regardless() : 0; }
    inline uint16_t has_mgid1()      const { return mgid1_valid_; }
    inline uint16_t has_mgid2()      const { return mgid2_valid_; }
    inline uint16_t mgid1()          const { return has_mgid1() ? mgid1_ : 0; }
    inline uint16_t mgid2()          const { return has_mgid2() ? mgid2_ : 0; }
    inline uint16_t hash1()          const { return hash1_; }
    inline uint16_t hash2()          const { return hash2_; }
    inline uint16_t physical_ingress_port() const {
                      return physical_ingress_port_; }
    inline uint16_t multicast_pipe_vector() const { return multicast_pipe_vector_; }
    inline uint16_t pipe_mask()      const { return multicast_pipe_vector(); }
    inline bool     ct_disable_mode()const { return ct_disable_mode_; }
    inline bool     ct_mcast_mode()  const { return ct_mcast_mode_; }
    inline bool     dod()            const { return dod_; }
    inline uint8_t  meter_color()    const { return meter_color_; }
    inline uint8_t  tm_vec()         const { return tm_vec_; }
    inline uint8_t  qid()            const { return qid_; }
    inline uint16_t irid()           const { return rid_; }
    inline bool     use_yid_tbl()    const { return use_yid_tbl_; }
    inline bool     bypass_egr_mode() const { return bypass_egress_mode_; }
    inline uint16_t xid()            const { return xid_; }
    inline uint16_t yid()            const { return yid_; }
    inline uint8_t c2c_qid()         const { return c2c_qid_; }
    inline uint16_t mirror_bmp()     const { return mirror_bmp_; }
    inline uint8_t mirror_cos()      const { return mirror_cos_; }

    // new for JBay
    inline boost::optional<AFCMetadata> afc()        const { return afc_; }
    inline boost::optional<int>    mtu_trunc_len()   const { return mtu_trunc_len_; }
    inline int      mtu_trunc_err_f() const { return mtu_trunc_err_f_; }

    // Setter functions.
    inline void     set_version(uint64_t x)          { version_ = (x & 0x03); }
    inline void     set_icos(uint64_t x)             { icos_ = (x & 0x07); }
    inline void     set_copy_to_cpu(uint64_t x)      { copy_to_cpu_ = (bool)(x & 0x01); }
    inline void     set_copy_to_cpu_cos(uint64_t x)  { copy_to_cpu_cos_ = (x & 0x07); }
    inline void     set_egress_unicast_port_only(uint64_t x) {
      egress_unicast_port_ = (x & ((1 << RmtDefs::kDeparserEgressUnicastPortWidth) - 1));
      // Does not modify valid
    }
    inline void     set_egress_unicast_port(uint64_t x) {
      set_egress_unicast_port_only(x);
      egress_unicast_valid_ = true;
    }
    inline void     set_mgid1(uint64_t x) {
                      mgid1_valid_ = true;
                      mgid1_ = (x & 0xFFFF); }
    inline void     set_mgid2(uint64_t x) {
                      mgid2_valid_ = true;
                      mgid2_ = (x & 0xFFFF); }
    inline void     clr_egress_unicast_port() {
                      egress_unicast_valid_ = false; }
    inline void     clr_mgid1() {
                      mgid1_valid_ = false;
                      mgid1_ = 0; }
    inline void     clr_mgid2() {
                      mgid2_valid_ = false;
                      mgid2_ = 0; }
    inline void     set_hash1(uint64_t x)    { hash1_ = (x & 0x1FFF); }
    inline void     set_hash2(uint64_t x)    { hash2_ = (x & 0x1FFF); }
    inline void     set_physical_ingress_port(uint16_t x) {
                      physical_ingress_port_ = (x & ((1 << RmtDefs::kPortWidth) - 1)); }
    inline void     set_pipe_mask(uint16_t m) {  set_multicast_pipe_vector(m); }
    inline void     set_ct_disable_mode(uint64_t x) { ct_disable_mode_ = (bool)(x & 0x01); }
    inline void     set_ct_mcast_mode(uint64_t x)   { ct_mcast_mode_ = (bool)(x & 0x01); }
    inline void     set_dod(uint64_t x)      { dod_ = (bool)(x & 0x01); }
    inline void     set_meter_color(uint64_t x) { meter_color_ = (x & 0x03); }
    inline void     set_multicast_pipe_vector(uint16_t x) {
                      multicast_pipe_vector_ = (x & RmtDefs::kI2qMulticastPipeVectorMask); }
    inline void     set_tm_vec(uint8_t x)    { tm_vec_ = (x & 0x03); }
    inline void     set_qid(uint64_t x)      { qid_ = (x & RmtDefs::kI2qQidMask ); }
    inline void     set_irid(uint64_t x)     { rid_ = (x & 0xFFFF); }
    inline void     set_use_yid_tbl(uint64_t x) { use_yid_tbl_ = (x & 0x01); }
    inline void     set_bypass_egr_mode(uint64_t x) { bypass_egress_mode_ = (x & 0x01); }
    inline void     set_xid(uint64_t x)      { xid_ = (x & 0xFFFF); }
    inline void     set_yid(uint64_t x)      { yid_ = (x & ((1 << RmtDefs::kDeparserXidL2Width) - 1)); }
    inline void     set_c2c_qid(uint64_t x)  { c2c_qid_ = (x & RmtDefs::kI2qQidMask); }
    inline void     set_mirror_bmp(uint64_t x) { mirror_bmp_ = (x & ((1 << RmtDefs::kMirrorsPerPacket) - 1)); }
    inline void     set_mirror_cos(uint64_t x)  { mirror_cos_ = (x & 0x07); }

    // new for JBay
    inline void     set_afc(AFCMetadata x)        { afc_ = x; }
    inline void     set_mtu_trunc_len(int x)   { mtu_trunc_len_ = x; }
    inline void     set_mtu_trunc_err_f(int x) { mtu_trunc_err_f_ = x; }

    // Reset function.
    inline void     reset() {
                      set_version(0);
                      set_icos(0);
                      set_copy_to_cpu(false);
                      set_copy_to_cpu_cos(0);
                      clr_egress_unicast_port();
                      clr_mgid1();
                      clr_mgid2();
                      set_hash1(0);
                      set_hash2(0);
                      set_physical_ingress_port(0);
                      set_pipe_mask(0);
                      set_ct_disable_mode(false);
                      set_ct_mcast_mode(false);
                      set_dod(false);
                      set_meter_color(0);
                      set_multicast_pipe_vector(0);
                      set_qid(0);
                      set_irid(0);
                      set_use_yid_tbl(0);
                      set_bypass_egr_mode(0);
                      set_xid(0);
                      set_yid(0);
                      set_c2c_qid(0);
                      set_mirror_bmp(0);
                      set_mirror_cos(0);
                      afc_.reset();
                      mtu_trunc_len_ = boost::none;
                      set_mtu_trunc_err_f(0);
    }

    // Assign from.
    inline void     copy_from(const I2QueueingMetadata s) {
                                version_               = s.version_;
                                icos_                  = s.icos_;
                                copy_to_cpu_           = s.copy_to_cpu_;
                                copy_to_cpu_cos_       = s.copy_to_cpu_cos_;
                                egress_unicast_valid_  = s.egress_unicast_valid_;
                                egress_unicast_port_   = s.egress_unicast_port_;
                                mgid1_valid_           = s.mgid1_valid_;
                                mgid1_                 = s.mgid1_;
                                mgid2_valid_           = s.mgid2_valid_;
                                mgid2_                 = s.mgid2_;
                                hash1_                 = s.hash1_;
                                hash2_                 = s.hash2_;
                                physical_ingress_port_ = s.physical_ingress_port_;
                                ct_disable_mode_       = s.ct_disable_mode_;
                                ct_mcast_mode_         = s.ct_mcast_mode_;
                                dod_                   = s.dod_; // Deflect-on-Drop
                                meter_color_           = s.meter_color_; // Meter Color
                                multicast_pipe_vector_ = s.multicast_pipe_vector_;
                                tm_vec_                = s.tm_vec_;
                                qid_                   = s.qid_; // Queue ID
                                use_yid_tbl_           = s.use_yid_tbl_;
                                bypass_egress_mode_    = s.bypass_egress_mode_;
                                xid_                   = s.xid_;
                                yid_                   = s.yid_;
                                rid_                   = s.rid_;
                                c2c_qid_               = s.c2c_qid_; // C2C Queue ID
                                mirror_bmp_            = s.mirror_bmp_;
                                mirror_cos_            = s.mirror_cos_;
                                afc_                   = s.afc_;
                                mtu_trunc_len_         = s.mtu_trunc_len_;
                                mtu_trunc_err_f_       = s.mtu_trunc_err_f_;
    }
    std::string to_string(std::string indent_string = "") const {
      std::string r("");

      r += indent_string + std::string("version = ") + boost::str( boost::format("%d") % static_cast<uint>(version_)) + "\n";
      r += indent_string + std::string("icos = ") + boost::str( boost::format("%d") % static_cast<uint>(icos_)) + "\n";
      r += indent_string + std::string("copy_to_cpu = ") + boost::str( boost::format("%d") % static_cast<uint>(copy_to_cpu_)) + "\n";
      r += indent_string + std::string("copy_to_cpu_cos = ") + boost::str( boost::format("%d") % static_cast<uint>(copy_to_cpu_cos_)) + "\n";
      r += indent_string + std::string("egress_unicast_valid = ") + boost::str( boost::format("%d") % static_cast<uint>(egress_unicast_valid_)) + "\n";
      r += indent_string + std::string("egress_unicast_port = ") + boost::str( boost::format("%d") % static_cast<uint>(egress_unicast_port_)) + "\n";
      r += indent_string + std::string("mgid1_valid = ") + boost::str( boost::format("%d") % static_cast<uint>(mgid1_valid_)) + "\n";
      r += indent_string + std::string("mgid1 = ") + "0x" + boost::str( boost::format("%x") % static_cast<uint>(mgid1_)) + "\n";
      r += indent_string + std::string("mgid2_valid = ") + boost::str( boost::format("%d") % static_cast<uint>(mgid2_valid_)) + "\n";
      r += indent_string + std::string("mgid2 = ") + "0x" + boost::str( boost::format("%x") % static_cast<uint>(mgid2_)) + "\n";
      r += indent_string + std::string("hash1 = ") + "0x" + boost::str( boost::format("%x") % static_cast<uint>(hash1_)) + "\n";
      r += indent_string + std::string("hash2 = ") + "0x" + boost::str( boost::format("%x") % static_cast<uint>(hash2_)) + "\n";
      r += indent_string + std::string("physical_ingress_port = ") + boost::str( boost::format("%d") % static_cast<uint>(physical_ingress_port_)) + "\n";
      r += indent_string + std::string("ct_disable_mode = ") + boost::str( boost::format("%d") % static_cast<uint>(ct_disable_mode_)) + "\n";
      r += indent_string + std::string("ct_mcast_mode = ") + boost::str( boost::format("%d") % static_cast<uint>(ct_mcast_mode_)) + "\n";
      r += indent_string + std::string("dod = ") + boost::str( boost::format("%d") % static_cast<uint>(dod_)) + "\n";
      r += indent_string + std::string("meter_color = ") + boost::str( boost::format("%d") % static_cast<uint>(meter_color_)) + "\n";
      r += indent_string + std::string("multicast_pipe_vector = ") + "0x" + boost::str( boost::format("%x") % static_cast<uint>(multicast_pipe_vector_)) + "\n";
      r += indent_string + std::string("tm_vec = ") + "0x" + boost::str( boost::format("%x") % static_cast<uint>(tm_vec_)) + "\n";
      r += indent_string + std::string("qid = ") + boost::str( boost::format("%d") % static_cast<uint>(qid_)) + "\n";
      r += indent_string + std::string("use_yid_tbl = ") + boost::str( boost::format("%d") % static_cast<uint>(use_yid_tbl_)) + "\n";
      r += indent_string + std::string("bypass_egress_mode = ") + boost::str( boost::format("%d") % static_cast<uint>(bypass_egress_mode_)) + "\n";
      r += indent_string + std::string("xid = ") + "0x" + boost::str( boost::format("%x") % static_cast<uint>(xid_)) + "\n";
      r += indent_string + std::string("yid = ") + "0x" + boost::str( boost::format("%x") % static_cast<uint>(yid_)) + "\n";
      r += indent_string + std::string("rid = ") + "0x" + boost::str( boost::format("%x") % static_cast<uint>(rid_)) + "\n";
      r += indent_string + std::string("c2c_qid = ") + "0x" + boost::str( boost::format("%x") % static_cast<uint>(c2c_qid_)) + "\n";
      r += indent_string + std::string("mirror_bmp = ") + "0x" + boost::str( boost::format("%x") % static_cast<uint>(mirror_bmp_)) + "\n";
      r += indent_string + std::string("mirror_cos = ") + "0x" + boost::str( boost::format("%x") % static_cast<uint>(mirror_cos_)) + "\n";
      r += indent_string + std::string("afc port_id = ") + (afc_ ? boost::str( boost::format("%d") % static_cast<uint>(afc_->getPortId())):"invalid") + "\n";
      r += indent_string + std::string("afc q_id = ") + (afc_ ? boost::str( boost::format("%d") % static_cast<uint>(afc_->getQid())):"invalid") + "\n";
      r += indent_string + std::string("afc credit = ") + (afc_ ? boost::str( boost::format("%d") % static_cast<uint>(afc_->getCredit())):"invalid") + "\n";
      r += indent_string + std::string("afc adv_qfc = ") + (afc_ ? boost::str( boost::format("%d") % static_cast<uint>(afc_->getAdvQfc())):"invalid") + "\n";
      r += indent_string + std::string("mtu_trunc_len = ") + (mtu_trunc_len_ ? boost::str( boost::format("%d") % static_cast<uint>(*mtu_trunc_len_)):"invalid") + "\n";
      r += indent_string + std::string("mtu_trunc_err_f = ") + boost::str( boost::format("%d") % static_cast<uint>(mtu_trunc_err_f_)) + "\n";
      return r;
    }

    // Hash metadata
    inline uint32_t hash() {
      uint32_t hash  = 0u;
      uint32_t c2cc  = (copy_to_cpu_)          ?copy_to_cpu_cos_     :0u;
      uint32_t eup   = (egress_unicast_valid_) ?egress_unicast_port_ :0u;
      uint32_t mgid1 = (mgid1_valid_)          ?mgid1_               :0u;
      uint32_t mgid2 = (mgid2_valid_)          ?mgid2_               :0u;
      uint32_t color = (meter_color_);
      uint32_t mcast = (multicast_pipe_vector_);
      uint32_t tm_vec = (tm_vec_);
      uint32_t mirr_cos = (mirror_cos_);
      hash ^= (version_ << 24) | (version_ << 16) | (version_ << 8) | (version_ << 0);
      hash ^= (icos_    << 24) | (icos_    << 16) | (icos_    << 8) | (icos_    << 0);
      hash ^= (c2cc     << 24) | (c2cc     << 16) | (c2cc     << 8) | (c2cc     << 0);
      hash ^= (eup      << 16) | (eup      <<  0);
      hash ^= (mgid1    << 16) | (mgid1    <<  0);
      hash ^= (mgid2    << 16) | (mgid2    <<  0);
      hash ^= (hash1_   << 16) | (hash1_   <<  0);
      hash ^= (hash2_   << 16) | (hash2_   <<  0);
      hash ^= (physical_ingress_port_      << 16) | (physical_ingress_port_     << 0);
      hash ^= (ct_disable_mode_)    ?0x00010001u :0u;
      hash ^= (ct_mcast_mode_)      ?0x00020002u :0u;
      hash ^= (dod_)                ?0x00040004u :0u;
      hash ^= (color    << 24) | (color    << 16) | (color    << 8) | (color    << 0);
      hash ^= (mcast    << 24) | (mcast    << 16) | (mcast    << 8) | (mcast    << 0);
      hash ^= (tm_vec   << 24) | (tm_vec   << 16) | (tm_vec   << 8) | (tm_vec   << 0);
      hash ^= (qid_     << 24) | (qid_     << 16) | (qid_     << 8) | (qid_     << 0);
      hash ^= (use_yid_tbl_)        ?0x01000100u :0u;
      hash ^= (bypass_egress_mode_) ?0x02000200u :0u;
      hash ^= (xid_                        << 16) | (xid_                       << 0);
      hash ^= (yid_                        << 16) | (yid_                       << 0);
      hash ^= (rid_                        << 16) | (rid_                       << 0);
      hash ^= (c2c_qid_ << 24) | (c2c_qid_ << 16) | (c2c_qid_ << 8) | (c2c_qid_ << 0);
      hash ^= (mirror_bmp_                 << 16) | (mirror_bmp_                << 0);
      hash ^= (mirr_cos << 24) | (mirr_cos << 16) | (mirr_cos << 8) | (mirr_cos << 0);
      return hash;
    }


  private:
    uint8_t        version_ = 0;
    uint8_t        icos_ = 0;
    bool           copy_to_cpu_ = false;
    uint8_t        copy_to_cpu_cos_ = 0;
    bool           egress_unicast_valid_ = false;
    uint16_t       egress_unicast_port_ = 0;
    bool           mgid1_valid_ = false;
    uint16_t       mgid1_       = 0;
    bool           mgid2_valid_ = false;
    uint16_t       mgid2_       = 0;
    uint16_t       hash1_ = 0;
    uint16_t       hash2_ = 0;
    uint16_t       physical_ingress_port_ = 0;
    bool           ct_disable_mode_ = false;
    bool           ct_mcast_mode_ = false;
    bool           dod_ = false; // Deflect-on-Drop
    uint8_t        meter_color_ = 0; // Meter Color
    // jbay has 5 bits pipe vector, WIP has 9 bits
    uint16_t       multicast_pipe_vector_ = 0;
    uint8_t        tm_vec_ = 0;
    uint8_t        qid_ = 0; // Queue ID
    bool           use_yid_tbl_ = false;
    bool           bypass_egress_mode_ = false;
    uint16_t       xid_ = 0;
    uint16_t       yid_ = 0;
    uint16_t       rid_ = 0;
    uint8_t        c2c_qid_ = 0;
    uint16_t       mirror_bmp_ = 0;
    uint8_t        mirror_cos_ = 0;
    // new for JBay
    boost::optional<AFCMetadata> afc_;
    boost::optional<int>      mtu_trunc_len_{};
    int            mtu_trunc_err_f_ = 0;
  };
}
#endif // _SHARED_I2QUEUEING_METADATA_
