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

#include <deparser-egress.h>

#include <deparser-reg.h>
#include <e2mac-metadata.h>
#include <packet.h>
#include <phv.h>
#include <rmt-object-manager.h>
#include <event.h>

namespace MODEL_CHIP_NAMESPACE {

  DeparserEgress::DeparserEgress(RmtObjectManager *om, int pipeIndex,
                                 int ioIndex, DeparserReg &deparser_reg)
      : Deparser(om, pipeIndex, ioIndex, true, deparser_reg) {
  }

  Packet *DeparserEgress::Deparse(const Phv &phv, Packet **mirror_packet) {
    phv.print_d("PHV Egress deparse start\n", false);
    deparser_reg_.increment_phv_counter(true);
    deparser_reg_.increment_tphv_counter(true);

    Packet *pkt = Deparser::GetNewPacket(phv, phv.egress_packet());

    if (nullptr != pkt) {
      pkt = SetMetadata(phv, pkt, mirror_packet);
    }

    if (nullptr == pkt) {
      deparser_reg_.increment_disc_pkts(true);
    } else {
      get_object_manager()->log_packet(RmtTypes::kRmtTypeDeparser, *pkt);
      deparser_reg_.increment_cnt_pkts(true);
      deparser_reg_.increment_fwd_pkts(true);
    }
    if (nullptr != *mirror_packet) {
      deparser_reg_.increment_mirr_pkts(true);
    }
    return pkt;
  }

  Packet *
  DeparserEgress::SetMetadata(const Phv &phv, Packet *pkt, Packet **mirror_packet) {
    // In the egress deparser, first deparse and then mirror.
    RMT_ASSERT(nullptr != mirror_packet);
    const auto drop_ctl = GetDropControl(phv);
    if ((drop_ctl & 0x04) == 0) {
      (*mirror_packet) = GetMirrorPacket(phv, *pkt);
    }
    else {
      (*mirror_packet) = nullptr;
    }

    pkt = SetMetadataChip(phv,pkt,mirror_packet);

    auto pkt_id = pkt->pkt_id();
    E2MacMetadata *e2mac_metadata = pkt->e2mac_metadata();
    RMT_ASSERT(e2mac_metadata);

    bool valid = GLOBAL_FALSE;
    uint64_t value = 0;

    {
      std::tie(valid, value) = ExtractMetadata(phv, deparser_reg_.get_capture_tx_ts_info());
      RMT_ASSERT(valid);
      RMT_LOG_VERBOSE("DeparserEgress::DeparseEgress capture TS on TX is %" PRIu64 "\n", value);
      e2mac_metadata->set_capture_tx_ts(value);
    }

    {
      std::tie(valid, value) = ExtractMetadata(phv, deparser_reg_.get_ecos_info());
      RMT_ASSERT(valid);
      RMT_LOG_VERBOSE("DeparserEgress::DeparseEgress ecos is %" PRIu64 "\n", value);
      e2mac_metadata->set_ecos(value);
    }

    {
      std::tie(valid, value) = ExtractMetadata(phv, deparser_reg_.get_force_tx_err_info());
      RMT_ASSERT(valid);
      RMT_LOG_VERBOSE("DeparserEgress::DeparseEgress force TX error is %" PRIu64 "\n", value);
      e2mac_metadata->set_force_tx_error(value);
    }

    {
      std::tie(valid, value) = ExtractMetadata(phv, deparser_reg_.get_tx_pkt_has_ts_offsets_info());
      RMT_ASSERT(valid);
      RMT_LOG_VERBOSE("DeparserEgress::DeparseEgress update delay on TX is %" PRIu64 "\n", value);
      e2mac_metadata->set_update_delay_on_tx(value);
    }

    {
      std::tie(valid, value) = ExtractMetadata(phv, deparser_reg_.get_egress_unicast_port_info(DeparserReg::IngressEgressIndexEnum::kEgress));
      RMT_LOG_VERBOSE("DeparserEgress::DeparseEgress egress unicast port is %s\n",
                      (valid ? (boost::lexical_cast<std::string>(value)).c_str() : "invalid"));

      if (valid) {
        e2mac_metadata->set_egress_unicast_port(value);
      }
      // Log complete metadata (before packet drop)
      if ( !valid || !Port::is_valid_pipe_local_port_index(e2mac_metadata->egress_unicast_port() ) ||
          ((drop_ctl & 0x01) == 0x01)) {
        get_object_manager()->log_deparser_pkt_drop(pkt->pkt_id(), pipe_index(), Gress::egress);
        get_object_manager()->pkt_delete(pkt);
        pkt = nullptr;
      }
      get_object_manager()->log_deparser_metadata(pkt_id, pipe_index(), *e2mac_metadata);

    }

    return pkt;
  }

  Packet *DeparserEgress::SetMetadataChip(const Phv &phv, Packet *pkt, Packet **mirror_packet) {
    return pkt;
  }

  uint8_t
  DeparserEgress::GetDropControl(const Phv &phv) {
    uint64_t drop_ctl = GLOBAL_ZERO;
    bool valid = GLOBAL_FALSE;
    std::tie(valid, drop_ctl) = ExtractMetadata(
                                    phv,
                                    deparser_reg_.get_egress_drop_ctl_info());
    RMT_ASSERT(valid);
    RMT_LOG_VERBOSE("DEPARSER::update_metadata ingress drop_ctl is %" PRIu64 "\n",
                    drop_ctl);
    return drop_ctl;
  }

  uint8_t
  DeparserEgress::GetPktVersion(Packet *pkt) {
    return pkt->egress_info()->version();
  }

  bool
  DeparserEgress::CheckDeparserPhvGroupConfig(const int &phv_idx) {
    return deparser_reg_.CheckEgressDeparserPhvGroupConfig(phv_idx) &&
           (!deparser_reg_.CheckIngressDeparserPhvGroupConfig(phv_idx));
  }

  std::set<int>
  DeparserEgress::GetInvalidPhvs() {
    return std::set<int> {
      Phv::make_word_d(0,0), Phv::make_word_d(0,1), Phv::make_word_d(0,2),
      Phv::make_word_d(0,3), Phv::make_word_d(0,4), Phv::make_word_d(0,5),
      Phv::make_word_d(0,6), Phv::make_word_d(0,7), Phv::make_word_d(0,8),
      Phv::make_word_d(0,9), Phv::make_word_d(0,10), Phv::make_word_d(0,11),
      Phv::make_word_d(0,12), Phv::make_word_d(0,13), Phv::make_word_d(0,14),
      Phv::make_word_d(0,15),
      Phv::make_word_d(2,0), Phv::make_word_d(2,1), Phv::make_word_d(2,2),
      Phv::make_word_d(2,3), Phv::make_word_d(2,4), Phv::make_word_d(2,5),
      Phv::make_word_d(2,6), Phv::make_word_d(2,7), Phv::make_word_d(2,8),
      Phv::make_word_d(2,9), Phv::make_word_d(2,10), Phv::make_word_d(2,11),
      Phv::make_word_d(2,12), Phv::make_word_d(2,13), Phv::make_word_d(2,14),
      Phv::make_word_d(2,15),
      Phv::make_word_d(4,0), Phv::make_word_d(4,1), Phv::make_word_d(4,2),
      Phv::make_word_d(4,3), Phv::make_word_d(4,4), Phv::make_word_d(4,5),
      Phv::make_word_d(4,6), Phv::make_word_d(4,7), Phv::make_word_d(4,8),
      Phv::make_word_d(4,9), Phv::make_word_d(4,10), Phv::make_word_d(4,11),
      Phv::make_word_d(4,12), Phv::make_word_d(4,13), Phv::make_word_d(4,14),
      Phv::make_word_d(4,15) };
  }
}
