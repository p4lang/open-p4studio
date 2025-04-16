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

#ifndef _SHARED_MIRROR_METADATA_
#define _SHARED_MIRROR_METADATA_

namespace MODEL_CHIP_NAMESPACE {

  class MirrorMetadata {

 public:
    uint16_t mirror_id()                   const { return mirror_id_; }
    uint8_t  version()                     const { return version_;   }
    uint8_t  coal_len()                    const { return coal_len_;   }

    void set_mirror_id(uint16_t x)               { mirror_id_ = (x & 0x3FF); }
    void set_version(uint8_t x)                  { version_ = (x & 0x3); }
    void set_coal_len(uint8_t x)                 { coal_len_ = x; }


    // new for JBay
    int  mirr_io_sel()                     const { return io_sel_; }
    int  mirr_hash()                       const { return hash_; }
    int  mirr_mc_ctrl()                    const { return mc_ctrl_; }
    int  mirr_c2c_ctrl()                   const { return c2c_ctrl_; }
    int  mirr_sel()                        const { return mirr_sel_; }
    boost::optional<int> mirr_epipe_port() const { return mirr_epipe_port_; }
    int  mirr_qid()                        const { return mirr_qid_; }
    bool mirr_dond_ctrl()                  const { return mirr_dond_ctrl_; }
    int  mirr_icos()                       const { return mirr_icos_; }

    void set_mirr_io_sel(int x)                  { io_sel_ = x; }
    void set_mirr_hash(int x)                    { hash_ = x; }
    void set_mirr_mc_ctrl(int x)                 { mc_ctrl_ = x; }
    void set_mirr_c2c_ctrl(int x)                { c2c_ctrl_ = x; }
    void set_mirr_sel(int x)                     { mirr_sel_ = x; }
    void set_mirr_epipe_port(int x)              {
      mirr_epipe_port_ = (x & ((1 << RmtDefs::kDeparserMirrEpipePortWidth) - 1)); }
    void set_mirr_qid(int x)                     { mirr_qid_ = x; }
    void set_mirr_dond_ctrl(bool x)              { mirr_dond_ctrl_ = x; }
    void set_mirr_icos(int x)                    { mirr_icos_ = x; }


    void reset() {
      set_mirror_id(0xFFFF);
      set_version(0xFF);
      set_coal_len(0x00);
      set_mirr_io_sel(0);
      set_mirr_hash(0);
      set_mirr_mc_ctrl(0);
      set_mirr_c2c_ctrl(0);
      set_mirr_sel(0);
      mirr_epipe_port_ = boost::none;
      set_mirr_qid(0);
      set_mirr_dond_ctrl(false);
      set_mirr_icos(0);
    }

    void copy_from(const MirrorMetadata s) {
      mirror_id_       = s.mirror_id_;
      version_         = s.version_;
      coal_len_        = s.coal_len_;
      io_sel_          = s.io_sel_;
      hash_            = s.hash_;
      mc_ctrl_         = s.mc_ctrl_;
      c2c_ctrl_        = s.c2c_ctrl_;
      mirr_sel_        = s.mirr_sel_;
      mirr_epipe_port_ = s.mirr_epipe_port_;
      mirr_qid_        = s.mirr_qid_;
      mirr_dond_ctrl_  = s.mirr_dond_ctrl_;
      mirr_icos_       = s.mirr_icos_;
    }

 private:
    uint16_t             mirror_id_      = 0xFFFF;
    uint8_t              version_        = 0xFF;
    uint8_t              coal_len_       = 0x00;
    int                  io_sel_         = 0;
    int                  hash_           = 0;
    int                  mc_ctrl_        = 0;
    int                  c2c_ctrl_       = 0;
    int                  mirr_sel_       = 0;
    boost::optional<int> mirr_epipe_port_{};
    int                  mirr_qid_       = 0;
    bool                 mirr_dond_ctrl_ = false;
    int                  mirr_icos_      = 0;

  };
}
#endif // _SHARED_MIRROR_METADATA_
