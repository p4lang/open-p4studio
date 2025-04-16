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

#include <epb-common.h>
#include <string>
#include <common/rmt-assert.h>
#include <rmt-log.h>
#include <rmt-object-manager.h>
#include <port.h>
#include <register_adapters.h>


namespace MODEL_CHIP_NAMESPACE {

  EpbCommon::EpbCommon(RmtObjectManager *om, int pipeIndex, int epbIndex)
      : PipeObject(om, pipeIndex, epbIndex, kType) {
    RMT_LOG_VERBOSE("EPB::create pipeIndex:%d epbIndex:%d\n",pipeIndex,epbIndex);
  }
  EpbCommon::~EpbCommon() {
    RMT_LOG_VERBOSE("EPB::delete pipeIndex:%d epbIndex:%d\n",pipe_index(),s_index());
  }

  uint64_t EpbCommon::get_global_timestamp(int chan) {
    return (get_object_manager() != NULL) ? get_object_manager()->timestamp_get() : UINT64_C(0);
  }


  Packet *EpbCommon::prepend_metadata_hdr(Packet *packet, int chan) {
    RMT_ASSERT ((packet != NULL) && (packet->is_egress()));
    Port *port = packet->port();
    if ((chan < 0) && (port != NULL)) chan = port->epb_chan();
    // XXX - emit a log rather than just assert here
    if (!is_chan_enabled(chan)) {
      RMT_LOG(RmtDebug::warn(),
              "Epb::prepend_metadata_hdr - chan %d not enabled for port\n",
              chan);
      return packet;
    }

    // Default certain vals
    //uint64_t epb_epb_global_timestamp = get_global_timestamp(chan);
    //uint32_t epb_global_version = get_global_version(chan);

    // Figure out what metadata we need to add

    bool add_ingress_q_depth = is_ctrl_flag_set(chan, kFlagIngressQueueDepth);
    bool add_ingress_congestion = is_ctrl_flag_set(chan, kFlagIngressCongestion);
    bool add_ingress_q_timestamp = is_ctrl_flag_set(chan, kFlagIngressQueueTimestamp);
    bool add_egress_q_depth = is_ctrl_flag_set(chan, kFlagEgressQueueDepth);
    bool add_egress_congestion = is_ctrl_flag_set(chan, kFlagEgressCongestion);
    bool add_app_pool_congestion = is_ctrl_flag_set(chan, kFlagAppPoolCongestion);
    bool add_queue_delay = is_ctrl_flag_set(chan, kFlagQueueDelay);
    bool add_multicast_replication_id = is_ctrl_flag_set(chan, kFlagMulticastReplicationID);
    bool add_replication_id_first = is_ctrl_flag_set(chan, kFlagReplicationIDFirst);
    bool add_qid = is_ctrl_flag_set(chan, kFlagQID);
    bool add_ecos = is_ctrl_flag_set(chan, kFlageCos);
    bool add_redir_or_recirc = is_ctrl_flag_set(chan, kFlagRedirOrRecirc);
    bool add_length = is_ctrl_flag_set(chan, kFlagLength);
    bool add_tbd1 = is_ctrl_flag_set(chan, kFlagTBD1);
    bool add_tbd2 = is_ctrl_flag_set(chan, kFlagTBD2);
    bool add_tbd3 = is_ctrl_flag_set(chan, kFlagTBD3);
    if (add_tbd1 || add_tbd2 || add_tbd3) {
      RMT_LOG(RmtDebug::warn(),
              "Epb::prepend_metadata_hdr - adding UNDEFINED fields to hdr\n");
    }
    // Then build hdr
    uint8_t hdr[kMetadataMaxHdrBytes];
    uint32_t undef = 0x0DEF0DEFu;
    uint64_t zeros = UINT64_C(0);
    int pos = 0;

    uint16_t logical_egress_port = RemapPhyToLogical(packet->qing2e_metadata()->egress_port(), chan);
    pos = fill_hdr(hdr, pos, 2, logical_egress_port);
    if (add_ingress_q_depth) pos = fill_hdr(hdr, pos, 3, packet->qing2e_metadata()->ing_q_depth());
    if (add_ingress_congestion) pos = fill_hdr(hdr, pos, 1, packet->qing2e_metadata()->ing_congested() ?0x1 :0x0);
    if (add_ingress_q_timestamp) pos = fill_hdr(hdr, pos, 4, packet->qing2e_metadata()->ing_q_ts());
    if (add_egress_q_depth) pos = fill_hdr(hdr, pos, 3, packet->qing2e_metadata()->egr_q_depth());
    if (add_egress_congestion) pos = fill_hdr(hdr, pos, 1, packet->qing2e_metadata()->egr_congested() ?0x1 :0x0);

    if (add_app_pool_congestion) {
      pos = fill_hdr(hdr, pos, 1, packet->qing2e_metadata()->app_pool_status());
      RMT_LOG(RmtDebug::warn(),
            "Epb::prepend_metadata_hdr - kFlagAppPoolCongestion currently unmodeled\n");
    }

    if (add_queue_delay) pos = fill_hdr(hdr, pos, 4, packet->qing2e_metadata()->delay());
    if (add_multicast_replication_id) pos = fill_hdr(hdr, pos, 2, packet->qing2e_metadata()->erid());

    if (add_replication_id_first) {
      pos = fill_hdr(hdr, pos, 1, packet->qing2e_metadata()->rid_first());
    }


    if (add_qid) pos = fill_hdr(hdr, pos, 1, packet->qing2e_metadata()->eqid());
    if (add_ecos) pos = fill_hdr(hdr, pos, 1, packet->qing2e_metadata()->ecos());

    if (add_redir_or_recirc) {
      pos = fill_hdr(hdr, pos, 1, packet->qing2e_metadata()->redc());
    }

    if (add_length) pos = fill_hdr(hdr, pos, 2, get_packet_len(packet));

    if (add_tbd1) pos = fill_hdr(hdr, pos, 4, undef);
    if (add_tbd2) pos = fill_hdr(hdr, pos, 4, undef);
    if (add_tbd3) pos = fill_hdr(hdr, pos, 4, undef);

    // On JBay pad out metadata to be 4B multiple - XXX
    uint16_t pad = get_padding(pos);
    RMT_ASSERT(pad < 8);
    if (pad > 0) pos = fill_hdr(hdr, pos, pad, zeros);

    // XXX:
    // Find max packet *data* byte that will be given to Parser.
    // This determined by the sun of EBP prsr_dph_max CSR.
    // Add on the EBP header_size to determine parsable len.
    // Store this inside the packet for the Parser to access.
    packet->set_parsable_len(get_max_byte() + pos);

    packet->prepend_metadata_hdr(hdr, pos);
    packet->set_metadata_added();

    if (!is_chip1_or_later()) {
      Parser *parser = (port != NULL) ?port->parser()->egress() :NULL;
      int hdr_len_adj = (parser != NULL) ?parser->hdr_len_adj() :0;
      if (pos != hdr_len_adj) {
        RMT_LOG(RmtDebug::warn(),
                "Epb::prepend_metadata_hdr: EgressParser HdrLenAdj(%d) != EpbHeaderSize(%d)\n",
                hdr_len_adj, pos);
      }
    }
    return packet;
  }
  Packet *EpbCommon::add_metadata_hdr(Packet *packet, int chan) {
    if (packet->metadata_added()) return packet;
    return prepend_metadata_hdr(packet, chan);
  }

}
