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

#include <array>
#include <iostream>
#include <cstdint>
#include <common/rmt-assert.h>
#include <cstdlib>
#include <vector>
#include <utility>
#include <algorithm>
#include <ctime>
#include <pktgen.h>
#include <rmt-object-manager.h>
#include <pktgen.h>
#include <pktgen-reg.h>
#include <packet.h>

namespace MODEL_CHIP_NAMESPACE {


PktGen::PktGen (RmtObjectManager* om, PacketEnqueuer* pe,
                int pipe_id) :
    PipeObject(om,pipe_id),
    g_config_drop_cnt_(0),
    g_config_recirc_drop_cnt_(0),
    g_recirc_cnt_(0),
    g_pgen_cnt_(0),
    g_mxbar_cnt_(0),
    g_drop_cnt_(0),
    g_recirc_drop_cnt_(0),
    g_triggers_(0),
    om_(om),
    m_pipe(pipe_id),
    tdm_en_(false),
    testing_(false),
    avail_buffer_(RmtDefs::kIpbSz),
    //limited_buffer_(false),
    def_recir_cnt_(RmtDefs::kDefaultRecirCount),
    pgen_reg_{{
    new P16_regs(chip_index(), m_pipe, om_, this, pe) ,
    new P17_regs(chip_index(), m_pipe, om_, this, pe)  }}

{
}
// Dtor
PktGen::~PktGen () {
  for (int i=0;i<2;i++) {
    if (pgen_reg_[i] != nullptr) {
      delete pgen_reg_[i];
      pgen_reg_[i] = nullptr;
    }
  }
}

// TOOD: where does this comment go? Not here I think!
// Called on egress packet arrival. Path is not used
// by packet generator.
// For recirc packets to P17, the packet is simply queued
// and a null pointer returned.
// P17 ingress path picks up the packet and pushes it into
// ingress buffer for processing.

void PktGen::handle_port_down(uint16_t port) {
    pgen_reg_[0]->port_down(port);
    pgen_reg_[1]->port_down(port);
}

void PktGen::handle_port_up(uint16_t port) {
  // Tofino doesn't do anything with port up
}

void PktGen::stop(void) {
    pgen_reg_[0]->stop();
    pgen_reg_[1]->stop();
}

bool PktGen::maybe_recirculate(Packet** r_packet) {
    RMT_ASSERT((*r_packet)->is_egress());

    uint8_t pi = (*r_packet)->port()->parser_index();
    int which;
    switch (pi) {
      case RmtDefs::kPktGen_P16:
        which=0;
        break;
      case RmtDefs::kPktGen_P17:
        which=1;
        break;
      default:
        return false;
        break;
    }

    uint8_t ch = (*r_packet)->port()->parser_chan();
    if (!pgen_reg_[which]->is_ch_enabled(ch) || pgen_reg_[which]->drop()) {
        ++g_config_drop_cnt_;
        ++g_drop_cnt_;
        om_->pkt_delete(*r_packet);
        *r_packet = nullptr;
        return false;
    } else if (pgen_reg_[which]->recirc()) {
        // Recirculation path
        bool recir_done = false;
        if (!process_recirc(which,r_packet, &recir_done)) {
            if (recir_done) {
                // this is only set if get_test() is true (ie in test mode)
                return false;
            }
            if (*r_packet) {
                om_->pkt_delete(*r_packet);
                *r_packet = nullptr;
            }
            g_recirc_drop_cnt_++;
            g_drop_cnt_++;
            return false;
        } else {
          // recirculating case has always queued the packet somewhere
          //  and nulled out the packet pointer.
          RMT_ASSERT(*r_packet == nullptr);
          return true;
        }
    } else if (pgen_reg_[which]->mxbar()) {
        // Normal processing, no enqueue
        g_mxbar_cnt_++;
        return false;
    }
    g_drop_cnt_++;
    return false;
}

bool PktGen::process_recirc(int which, Packet** r_packet, bool* recir_done) {
    uint32_t count = 0;
    if (get_test() && ((count = (*r_packet)->recirc_cnt()) == def_recir_cnt_)) {
        *recir_done = true;
        return false;
    }

    return pgen_reg_[which]->recirc_packet(r_packet);
}
void PktGen::update_recirc_packet(Packet* r_packet) {
    uint32_t count;
    uint16_t curr_port;
    curr_port = r_packet->port()->port_index();
    if (get_test()) {
        r_packet->i2qing_metadata()->set_egress_unicast_port(curr_port);
        count = r_packet->recirc_cnt();
        r_packet->set_recirc_cnt(++count);
    } else {
        r_packet->reset_meta();
        r_packet->reset_i2q();
        r_packet->reset_q2e();
        r_packet->reset_qing_data();
    }

    /* Return to ingress */
    r_packet->set_ingress();
    r_packet->set_port(om_->port_get(curr_port));
    g_recirc_cnt_++;
}



}
