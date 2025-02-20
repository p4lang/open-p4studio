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

#include <string>
#include <rmt-log.h>
#include <rmt-object-manager.h>
#include <port.h>
#include <register_includes/pgr_port_down_dis.h>


namespace MODEL_CHIP_NAMESPACE {

  uint64_t Port::zeroVal = UINT64_C(0);

  Port::Port(RmtObjectManager *om, int portIndex)
      : RmtObject(om), port_index_(portIndex), enabled_(true) {
    // Derive config from portIndex
    config_.speed_bits_per_sec = kInitialSpeed;
    config_.hdr_word0 = 0u;
    config_.hdr_word1 = 0u;
    config_.enabled = true;
    config_.logical_port_index = portIndex;
    config_.pipe_index = get_pipe_num(portIndex);
    config_.mac_index = -1;
    config_.mac_chan = -1;
    config_.parser_index = get_parser_num(portIndex);
    config_.parser_chan = get_parser_chan(portIndex);
    config_.ipb_index = get_ipb_num(portIndex);
    config_.epb_index = get_epb_num(portIndex);
    config_.ipb_chan = get_ipb_chan(portIndex);
    config_.epb_chan = get_epb_chan(portIndex);
    reset();
  }
  Port::Port(RmtObjectManager *om, int portIndex, int macIndex, int macChan)
      : Port(om, portIndex) {
    config_.mac_index = macIndex; // May be -1 if no mac
    config_.mac_chan = macChan;
  }
  Port::~Port() { }

  void Port::reset() {
    version_ = kInitialVersion;
    mac_ = nullptr;
    parser_ = nullptr;
    pipe_ = nullptr;
    deparser_ = nullptr;
    ipb_ = nullptr;
    epb_ = nullptr;
    mirror_ = nullptr;
    port_en.reset();
  }
  Phv *Port::parse(Packet *packet) {
    // TODO: prepend pseudo-header to packet
    if (!enabled()) return NULL;
    // Install port and port version info into packet
    packet->set_port(this);
    packet->set_version(version());
    return parser()->parse(packet, parser_chan());
  }
  Phv *Port::matchaction(Phv *phv) {
    if (!enabled()) return NULL;
    return pipe()->run_maus(phv);
  }
  Phv *Port::matchaction2(Phv *phv, Phv *ophv) {
    if (!enabled()) return NULL;
    return pipe()->run_maus2(phv, ophv);
  }
  Phv *Port::parse_matchaction(Packet *packet) {
    Phv *phv = parse(packet);
    if (phv != NULL) phv = matchaction(phv);
    return phv;
  }
  void Port::handle_eop(const Eop &eop) {
    if (!enabled()) return;
    pipe()->handle_eop(eop);
  }


  Packet *Port::process_inbound(Packet *packet, uint64_t& recirc) {
    // Install port and port version info into packet
    packet->set_port(this);
    packet->set_version(version());
    Packet *queued_packet = nullptr, *resubmit_pkt = packet;
    PacketGenMetadata *packet_gen_metadata = nullptr; // not used on Tofino
    do {
      packet = resubmit_pkt;
      resubmit_pkt = nullptr;
      RMT_ASSERT(nullptr == queued_packet);
      pipe()->process(packet, NULL, &queued_packet, NULL, &resubmit_pkt, NULL, NULL,
                      &packet_gen_metadata);
      delete packet_gen_metadata;
      packet_gen_metadata=nullptr;
    } while (nullptr != resubmit_pkt);

    return queued_packet;
  }

  Packet *Port::process_outbound(Packet *packet) {
    // Install port and port version info into packet
    packet->set_port(this);
    packet->set_version(version());
    packet->set_egress();
    Packet *sent_packet = nullptr, *resubmit_pkt = nullptr;
    PacketGenMetadata *packet_gen_metadata = nullptr; // not used on Tofino
    pipe()->process(NULL, packet, NULL, &sent_packet, &resubmit_pkt, NULL, NULL,
                    &packet_gen_metadata);
    delete packet_gen_metadata;
    RMT_ASSERT(nullptr == resubmit_pkt);
    return sent_packet;
  }

  Packet *Port::process(Packet *packet, uint64_t& recirc) {
    Packet *sent_packet = NULL;
    do {
      Packet *queued_packet = process_inbound(packet, recirc);
      if (queued_packet != NULL) {
        uint16_t port = queued_packet->i2qing_metadata()->egress_uc_port();
        RMT_ASSERT (RmtDefs::kPortsTotal > port);
        Port* eg_port = get_object_manager()->port_lookup(port);
        queued_packet->set_port(eg_port);
        queued_packet->qing2e_metadata()->set_egress_port(port);
        sent_packet = process_outbound(queued_packet);
        //sent_packet = process_recirc(sent_packet, recirc);
      }
      recirc = 0;
      //sent_packet->i2qing_metadata()->set_egress_unicast_port(port);
    } while (recirc != 0);
    return sent_packet;
  }
}
