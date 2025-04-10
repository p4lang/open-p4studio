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

#ifndef _SHARED_MIRROR_BUFFER_REG_COMMON_
#define _SHARED_MIRROR_BUFFER_REG_COMMON_

#include <cstdint>
#include <array>
#include <vector>
#include <memory>
#include <rmt-log.h>
#include <common/rmt-util.h>


#include <register_includes/mir_buf_regs_glb_ctrl.h>
#include <register_includes/mir_buf_regs_coalescing_baseid.h>
#include <register_includes/mir_buf_regs_coalescing_basetime.h>
#include <register_includes/mir_buf_regs_neg_mirr_ctrl.h>
#include <register_includes/mir_buf_desc_norm_desc_grp_array.h>
#include <register_includes/mir_buf_regs_coal_desc_grp_array.h>


namespace MODEL_CHIP_NAMESPACE {

class MirrorBuffer;

class MirrorRegsCommon {
  public:
    MirrorRegsCommon(int chip, int pipe, MirrorBuffer *mb);
    virtual ~MirrorRegsCommon();

    // Accessor functions to registers or some abstraction of it
    bool mirror_global_enable(bool ingress) {
        return ingress ? !!glb_ctrl_.ingr_ena() : !!glb_ctrl_.egr_ena();
    }

    bool mirror_session_enable(uint16_t sid, bool ingress) {
        return ingress ?
                !! norm_desc_grp_.norm_desc_grp_session_ctrl(sid).norm_ingr_ena() :
                !! norm_desc_grp_.norm_desc_grp_session_ctrl(sid).norm_egr_ena();
    }
    bool        mirror_session_is_coalescing(uint16_t sess_id);
    uint8_t     mirror_sesion_icos(uint16_t sess_id);
    uint8_t     mirror_sesion_copy_to_cpu(uint16_t sess_id);
    uint8_t     mirror_sesion_copy_to_cpu_cos(uint16_t sess_id);
    uint16_t    mirror_sesion_egress_unicast_port(uint16_t sess_id);
    uint32_t    mirror_sesion_mgid1(uint16_t sess_id);
    uint32_t    mirror_sesion_mgid2(uint16_t sess_id);
    uint32_t    mirror_sesion_hash1(uint16_t sess_id);
    uint32_t    mirror_sesion_hash2(uint16_t sess_id);
    uint8_t     mirror_sesion_pipe_mask(uint16_t sess_id);
    uint8_t     mirror_sesion_meter_color(uint16_t sess_id);
    bool        mirror_sesion_egress_unicast_port_valid(uint16_t sess_id);
    bool        mirror_sesion_mgid1_valid(uint16_t sess_id);
    bool        mirror_sesion_mgid2_valid(uint16_t sess_id);
    uint32_t    mirror_session_pkt_size(uint16_t sess_id);
    uint32_t    mirror_sesion_qid(uint16_t sess_id);
    uint32_t    mirror_sesion_xid(uint16_t sess_id);
    uint32_t    mirror_sesion_yid(uint16_t sess_id);
    uint32_t    mirror_sesion_rid(uint16_t sess_id);
    uint8_t     mirror_coal_session_idx(uint16_t sess_id);  // session id to coalescing idx
    bool        mirror_session_coal_ena(uint8_t coal_idx);
    uint16_t    mirror_coal_idx_to_sid(uint8_t  coal_idx);
    uint32_t    mirror_session_coal_pkt_size(uint8_t coal_idx);
    bool        mirror_session_coal_len_from_input(uint8_t coal_idx);
    uint32_t    mirror_session_coal_len(uint8_t coal_idx);
    uint32_t    mirror_session_coal_version(uint8_t coal_idx);
    uint32_t    mirror_session_coal_timeout(uint8_t coal_idx);
    uint32_t    mirror_session_coal_header_len(uint8_t coal_idx);
    uint32_t    mirror_session_coal_header0(uint8_t coal_idx);
    uint32_t    mirror_session_coal_header1(uint8_t coal_idx);
    uint32_t    mirror_session_coal_header2(uint8_t coal_idx);
    uint32_t    mirror_session_coal_header3(uint8_t coal_idx);

    uint32_t    mirror_session_coal_basetime() { return coal_basetime_.coal_basetime(); }

  private:
    // global control registers
    register_classes::MirBufRegsGlbCtrl             glb_ctrl_;
    register_classes::MirBufRegsNegMirrCtrl         neg_mir_ctrl_;
    register_classes::MirBufRegsCoalescingBaseid    coal_baseid_;
    register_classes::MirBufRegsCoalescingBasetime  coal_basetime_;

    // XXX add other global controls as needed

    // instantiate the registers manually. Need this due to StartOffset problems in auto generated
    // group class
    class NormDescGrp_ {
     public:
        NormDescGrp_(int chip=0, int pipe=0, int grp_idx=0);
        virtual ~NormDescGrp_() {}

        // move constructor is required for emplace_back(), but should never be used
        //  as registers are not movable
        NormDescGrp_(NormDescGrp_&&) { RMT_ASSERT(0); }
        void reset();

        // Use shared pointers so that class registered with the host interface
        // cannot be deleted (after copy etc)
        register_classes::MirBufDescSessionMeta0 norm_desc_grp_session_meta0_;
        register_classes::MirBufDescSessionMeta1 norm_desc_grp_session_meta1_;
        register_classes::MirBufDescSessionMeta2 norm_desc_grp_session_meta2_;
        register_classes::MirBufDescSessionMeta3 norm_desc_grp_session_meta3_;
        register_classes::MirBufDescSessionMeta4 norm_desc_grp_session_meta4_;
        register_classes::MirBufDescSessionCtrl  norm_desc_grp_session_ctrl_;

     private:
      DISALLOW_COPY_AND_ASSIGN(NormDescGrp_);
    };
    // to reuse the rest of the code (similar to auto generated files) create another class
    class NormDescGrpArr_ {
        public:
        NormDescGrpArr_(int chip=0, int pipe=0);
        virtual ~NormDescGrpArr_() {}

        void reset();

        register_classes::MirBufDescSessionMeta0 & norm_desc_grp_session_meta0(uint16_t sid) {
            return norm_desc_grp_arr_.at(sid).norm_desc_grp_session_meta0_;
        }
        register_classes::MirBufDescSessionMeta1 & norm_desc_grp_session_meta1(uint16_t sid) {
            return norm_desc_grp_arr_.at(sid).norm_desc_grp_session_meta1_;
        }
        register_classes::MirBufDescSessionMeta2 & norm_desc_grp_session_meta2(uint16_t sid) {
            return norm_desc_grp_arr_.at(sid).norm_desc_grp_session_meta2_;
        }
        register_classes::MirBufDescSessionMeta3 & norm_desc_grp_session_meta3(uint16_t sid) {
            return norm_desc_grp_arr_.at(sid).norm_desc_grp_session_meta3_;
        }
        register_classes::MirBufDescSessionMeta4 & norm_desc_grp_session_meta4(uint16_t sid) {
            return norm_desc_grp_arr_.at(sid).norm_desc_grp_session_meta4_;
        }
        register_classes::MirBufDescSessionCtrl & norm_desc_grp_session_ctrl(uint16_t sid) {
            return norm_desc_grp_arr_.at(sid).norm_desc_grp_session_ctrl_;
        }

        std::vector<NormDescGrp_> norm_desc_grp_arr_;
    };

    NormDescGrpArr_     norm_desc_grp_;

    class CoalDescGrp_ {
    public:
        CoalDescGrp_(int chip, int pipe, int grp_idx);
        virtual ~CoalDescGrp_() {}

        // move constructor is required for emplace_back(), but should never be used
        //  as registers are not movable
        CoalDescGrp_(CoalDescGrp_&&) { RMT_ASSERT(0); }

        void reset();

        register_classes::MirBufRegsCoalCtrl0      coal_desc_grp_coal_ctrl0_;
        register_classes::MirBufRegsCoalCtrl1      coal_desc_grp_coal_ctrl1_;
        register_classes::MirBufRegsCoalPktHeader0 coal_desc_grp_coal_pkt_header0_;
        register_classes::MirBufRegsCoalPktHeader1 coal_desc_grp_coal_pkt_header1_;
        register_classes::MirBufRegsCoalPktHeader2 coal_desc_grp_coal_pkt_header2_;
        register_classes::MirBufRegsCoalPktHeader3 coal_desc_grp_coal_pkt_header3_;
    private:
        DISALLOW_COPY_AND_ASSIGN(CoalDescGrp_);
    };

    class CoalDescGrpArr_ {
        public:
        CoalDescGrpArr_(int chip, int pipe);
        virtual ~CoalDescGrpArr_() {}
        void reset();

        register_classes::MirBufRegsCoalCtrl0 & coal_desc_grp_coal_ctrl0(uint16_t sid) {
            return coal_desc_grp_arr_.at(sid).coal_desc_grp_coal_ctrl0_;
        }
        register_classes::MirBufRegsCoalCtrl1 & coal_desc_grp_coal_ctrl1(uint16_t sid) {
            return coal_desc_grp_arr_.at(sid).coal_desc_grp_coal_ctrl1_;
        }
        register_classes::MirBufRegsCoalPktHeader0 & coal_desc_grp_coal_pkt_header0(uint16_t sid) {
            return coal_desc_grp_arr_.at(sid).coal_desc_grp_coal_pkt_header0_;
        }
        register_classes::MirBufRegsCoalPktHeader1 & coal_desc_grp_coal_pkt_header1(uint16_t sid) {
            return coal_desc_grp_arr_.at(sid).coal_desc_grp_coal_pkt_header1_;
        }
        register_classes::MirBufRegsCoalPktHeader2 & coal_desc_grp_coal_pkt_header2(uint16_t sid) {
            return coal_desc_grp_arr_.at(sid).coal_desc_grp_coal_pkt_header2_;
        }
        register_classes::MirBufRegsCoalPktHeader3 & coal_desc_grp_coal_pkt_header3(uint16_t sid) {
            return coal_desc_grp_arr_.at(sid).coal_desc_grp_coal_pkt_header3_;
        }

        std::vector<CoalDescGrp_> coal_desc_grp_arr_;
    };

    CoalDescGrpArr_     coal_desc_grp_;

    // back pointer
    MirrorBuffer    *mb_;
};

}
#endif // _SHARED_MIRROR_BUFFER_REG_COMMON_
