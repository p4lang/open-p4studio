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
                int pipe_index) :
    PipeObject(om,pipe_index),
    g_config_drop_cnt_(0),
    g_config_recirc_drop_cnt_(0),
    g_recirc_cnt_(0),
    g_pgen_cnt_(0),
    g_mxbar_cnt_(0),
    g_drop_cnt_(0),
    g_recirc_drop_cnt_(0),
    g_triggers_(0),
    om_(om),
    testing_(false),
    pgen_reg_{ chip_index(), pipe_index, om_, this, pe }
{
}

PktGen::~PktGen () {
}

void PktGen::handle_port_down(uint16_t port) {
    pgen_reg_.port_down(port);
}

void PktGen::handle_port_up(uint16_t port) {
    pgen_reg_.port_up(port);
}

void PktGen::stop(void) {
    pgen_reg_.stop();
}

bool PktGen::maybe_recirculate(Packet** r_packet) {
    RMT_ASSERT((*r_packet)->is_egress());

    int port_index =  (*r_packet)->port()->port_index();
    uint8_t ch = Port::get_pipe_local_port_index(port_index);
    bool recirc = false;
    bool drop   = false;
    pgen_reg_.check_channel(ch,&recirc,&drop);
    if ( drop ) {
      ++g_config_drop_cnt_;
      ++g_drop_cnt_;
      om_->pkt_delete(*r_packet);
      *r_packet = nullptr;
      RMT_LOG_VERBOSE("PktGen::maybe_recirculate port=%d ch=%d drop\n",port_index,ch);
      return false;
    }
    else if ( recirc ) {
      // Recirculation path
      bool recir_done = false;
      if (!process_recirc(ch,r_packet, &recir_done)) {
        RMT_LOG_VERBOSE("PktGen::maybe_recirculate port=%d ch=%d not recirc\n",port_index,ch);
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
        RMT_LOG_VERBOSE("PktGen::maybe_recirculate port=%d ch=%d recirc\n",port_index,ch);
        RMT_ASSERT(*r_packet == nullptr);
        return true;
      }
    }
    else {
      RMT_LOG_VERBOSE("PktGen::maybe_recirculate port=%d ch=%d normal\n",port_index,ch);
      // Normal processing, no enqueue
      g_mxbar_cnt_++;
      return false;
    }
}

void PktGen::maybe_trigger(PacketGenMetadata* metadata) {
  RMT_ASSERT(metadata);
  PacketBuffer* trigger = metadata->get_trigger();
  if (trigger) {
    RMT_ASSERT( metadata->address_valid() );
    RMT_ASSERT( metadata->length_valid() );
    pgen_reg_.deparser_snoop( trigger , metadata->address(), metadata->length());
  }

}

bool PktGen::process_recirc(int channel, Packet** r_packet, bool* recir_done) {
    uint32_t count = 0;
    if (get_test() && ((count = (*r_packet)->recirc_cnt()) == kMaxRecirculations)) {
        *recir_done = true;
        return false;
    }
    update_recirc_packet( *r_packet );

    // Snoop the packet now we know recirculation is actually happening
    pgen_reg_.recirc_snoop(*r_packet, channel);

    return pgen_reg_.recirc_packet(channel,r_packet);
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
      // XXX: Reset most metadata - but not pktID, recirc_cnt
      r_packet->reset_except_bufs();
      r_packet->set_recirc_cnt( r_packet->recirc_cnt() + 1 );
    }

    /* Return to ingress */
    r_packet->set_ingress();
    r_packet->set_port(om_->port_get(curr_port));
    g_recirc_cnt_++;
}



}
