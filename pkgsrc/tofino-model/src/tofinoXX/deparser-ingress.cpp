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

#include <deparser-ingress.h>

#include <inttypes.h>
#include <deparser-reg.h>
#include <packet.h>
#include <phv.h>
#include <port.h>
#include <rmt-object-manager.h>
#include <event.h>

namespace MODEL_CHIP_NAMESPACE {

  DeparserIngress::DeparserIngress(RmtObjectManager *om, int pipeIndex,
                                   int ioIndex, DeparserReg &deparser_reg)
      : Deparser(om, pipeIndex, ioIndex, false, deparser_reg) {
  }

  Packet *
  DeparserIngress::Deparse(const Phv &phv, Packet **mirror_packet,
                           Packet **resubmit_pkt,
                           PacketGenMetadata **packet_gen_metadata) {

    phv.print("PHV Ingress deparse start \n", false);
    const auto objmgr = get_object_manager();

    // In the ingress deparser, first mirror and then deparse.
    // check resubmit before mirroring since it overrides mirror

    Packet *pkt = phv.ingress_packet();
    RMT_ASSERT(nullptr != pkt);

    RMT_ASSERT(nullptr != resubmit_pkt);
    (*resubmit_pkt) = nullptr;

    deparser_reg_.increment_phv_counter(false);
    deparser_reg_.increment_tphv_counter(false);
    Port *port = pkt->port();
    RMT_ASSERT(port);
    IpbCounters *ipb_counters = get_object_manager()->ipb_counters_lookup(
        pipe_index(),
        port->ipb_index(),
        port->ipb_chan());
    if (nullptr != ipb_counters) ipb_counters->increment_chnl_deparser_send_pkt();

    const auto drop_ctl = GetDropControl(phv);


    pkt = HandleResubmit(pkt, phv, drop_ctl, resubmit_pkt);

    // Called only when packet is not being resubmitted.
    if (nullptr != pkt) {
      // Check for mirroring.
      RMT_ASSERT(nullptr != mirror_packet);
      if ((drop_ctl & 0x04) == 0) {
        (*mirror_packet) = GetMirrorPacket(phv, *pkt);
      }
      else {
        (*mirror_packet) = nullptr;
      }

      pkt = Deparser::GetNewPacket(phv, pkt);
      if (nullptr == pkt) {
        // Packet is being dropped, probably because new header was > 480B.
        // Drop the mirrored packet too.
        get_object_manager()->pkt_delete(*mirror_packet);
        (*mirror_packet) = nullptr;
      }
      else {
        pkt = SetMetadata(phv, pkt);
      }
      if (nullptr == pkt) {
        if (nullptr != ipb_counters) ipb_counters->increment_chnl_deparser_drop_pkt();
        deparser_reg_.increment_discard_counter();
        deparser_reg_.increment_disc_pkts(false);
      }
    }
    // log events
    if (nullptr != pkt) {
      // not dropped, not resubmitted
      objmgr->log_packet(RmtTypes::kRmtTypeDeparser, *pkt);
      deparser_reg_.increment_read_counter();
      deparser_reg_.increment_cnt_pkts(false);
      deparser_reg_.increment_fwd_pkts(false);
    }
    if (*resubmit_pkt != nullptr) {
      deparser_reg_.increment_resubmit_counter();
      objmgr->log_packet(RmtTypes::kRmtTypeDeparser, **resubmit_pkt);
    }
    if (*mirror_packet != nullptr) {
      objmgr->log_packet(RmtTypes::kRmtTypeDeparser, **mirror_packet);
      deparser_reg_.increment_mirr_pkts(false);
    }
    return pkt;
  }

  uint8_t
  DeparserIngress::GetPktVersion(Packet *pkt) {
    return pkt->ingress_info()->version();
  }

  uint8_t
  DeparserIngress::GetDropControl(const Phv &phv) {
    uint64_t drop_ctl = GLOBAL_ZERO;
    bool valid = GLOBAL_FALSE;
    std::tie(valid, drop_ctl) = ExtractMetadata(
                                    phv,
                                    deparser_reg_.get_ingress_drop_ctl_info());
    RMT_ASSERT(valid);
    RMT_LOG_VERBOSE("DEPARSER::update_metadata ingress drop_ctl is %" PRIu64 "\n",
                drop_ctl);
    return drop_ctl;
  }


  Packet *
  DeparserIngress::SetMetadata(const Phv &phv, Packet *pkt) {
    RMT_ASSERT(nullptr != pkt);

    pkt = SetMetadataChip(phv, pkt);

    bool valid = GLOBAL_FALSE;
    uint64_t value = GLOBAL_ZERO;
    RMT_ASSERT(pkt);
    I2QueueingMetadata *i2q_md = pkt->i2qing_metadata();

    i2q_md->set_version(pkt->ingress_info()->version());

    std::tie(valid, value) = ExtractMetadata(
                               phv, deparser_reg_.get_copy_to_cpu_info());
    RMT_ASSERT(valid);
    RMT_LOG_VERBOSE("DEPARSER::update_metadata copy-to-cpu is %" PRIu64 "\n", value);
    i2q_md->set_copy_to_cpu(value);

    std::tie(valid, value) = ExtractMetadata(
                               phv, deparser_reg_.get_copy_to_cpu_cos_info());
    RMT_ASSERT(valid);
    RMT_LOG_VERBOSE("DEPARSER::update_metadata copy-to-cpu_cos is %" PRIu64 "\n", value);
    i2q_md->set_copy_to_cpu_cos(value);

    std::tie(valid, value) = ExtractMetadata(
                               phv, deparser_reg_.get_ct_disable_mode_info());
    RMT_ASSERT(valid);
    RMT_LOG_VERBOSE("DEPARSER::update_metadata ct_disable_mode is %" PRIu64 "\n", value);
    i2q_md->set_ct_disable_mode(value);

    std::tie(valid, value) = ExtractMetadata(
                               phv, deparser_reg_.get_ct_mcast_mode_info());
    RMT_ASSERT(valid);
    RMT_LOG_VERBOSE("DEPARSER::update_metadata ct_mcast_mode is %" PRIu64 "\n", value);
    i2q_md->set_ct_mcast_mode(value);

    std::tie(valid, value) = ExtractMetadata(phv, deparser_reg_.get_deflect_on_drop_info());
    RMT_ASSERT(valid);
    RMT_LOG_VERBOSE("DEPARSER::update_metadata deflect_on_drop is %" PRIu64 "\n", value);
    i2q_md->set_dod(value);

    std::tie(valid, value) = ExtractMetadata(phv, deparser_reg_.get_egress_unicast_port_info(DeparserReg::IngressEgressIndexEnum::kIngress));
    RMT_LOG_VERBOSE("DEPARSER::update_metadata egress unicast port is %s\n",
                    (valid ? (boost::lexical_cast<std::string>(value)).c_str() : "invalid"));

    if (valid && ((value & 0x7F) < 72)) {
      value = RemapLogicalToPhy(value, false);
      i2q_md->set_egress_unicast_port(value);
    }
    if (!valid || ((value & 0x7F) >= 72)) {
      i2q_md->clr_egress_unicast_port();
    }

    std::tie(valid, value) = ExtractMetadata(phv, deparser_reg_.get_hash_lag_emcp_mcast_1_info());
    RMT_ASSERT(valid);
    RMT_LOG_VERBOSE("DEPARSER::update_metadata hash1 is %" PRIu64 "\n", value);
    i2q_md->set_hash1(value);

    std::tie(valid, value) = ExtractMetadata(phv, deparser_reg_.get_hash_lag_emcp_mcast_2_info());
    RMT_ASSERT(valid);
    RMT_LOG_VERBOSE("DEPARSER::update_metadata hash2 is %" PRIu64 "\n", value);
    i2q_md->set_hash2(value);

    std::tie(valid, value) = ExtractMetadata(phv, deparser_reg_.get_icos_info());
    RMT_ASSERT(valid);
    RMT_LOG_VERBOSE("DEPARSER::update_metadata icos is %" PRIu64 "\n", value);
    i2q_md->set_icos(value);

    std::tie(valid, value) = ExtractMetadata(phv, deparser_reg_.get_meter_color_info());
    RMT_ASSERT(valid);
    RMT_LOG_VERBOSE("DEPARSER::update_metadata meter color is %" PRIu64 "\n", value);
    i2q_md->set_meter_color(value);

    std::tie(valid, value) = ExtractMetadata(phv, deparser_reg_.get_egress_multicast_group_1_info());
    RMT_LOG_VERBOSE("DEPARSER::update_metadata mgid1 is %s\n", (valid ? (boost::lexical_cast<std::string>(value)).c_str() : "invalid"));
    i2q_md->clr_mgid1();
    if (valid) {
      i2q_md->set_mgid1(value);
    }

    std::tie(valid, value) = ExtractMetadata(phv, deparser_reg_.get_egress_multicast_group_2_info());
    RMT_LOG_VERBOSE("DEPARSER::update_metadata mgid2 is %s\n", (valid ? boost::lexical_cast<std::string>(value).c_str() : "invalid"));
    i2q_md->clr_mgid2();
    if (valid) {
      i2q_md->set_mgid2(value);
    }

    // Match RTL behavior
    const auto drop_ctl = GetDropControl(phv);
    if ((drop_ctl & 0x01) == 0x01) {
      i2q_md->clr_egress_unicast_port();
      i2q_md->clr_mgid1();
      i2q_md->clr_mgid2();
    }

    if ((drop_ctl & 0x02) == 0x02) {
      i2q_md->set_copy_to_cpu(0);
    }

    uint8_t multicast_pipe_vector = 0;
    if (i2q_md->has_mgid1()) {
      multicast_pipe_vector |= deparser_reg_.get_multicast_pipe_vector(0, i2q_md->mgid1());
    }
    if (i2q_md->has_mgid2()) {
      multicast_pipe_vector |= deparser_reg_.get_multicast_pipe_vector(1, i2q_md->mgid2());
    }
    if (i2q_md->cpu_needs_copy()) {
       multicast_pipe_vector |= deparser_reg_.get_copy_to_cpu_pipe_vector();
    }
    i2q_md->set_multicast_pipe_vector(multicast_pipe_vector);

    {
      uint16_t physical_ingress_port = pkt->ingress_info()->port()->port_index();

      uint8_t phv_idx = 0;
      bool sel = false;
      std::tie(phv_idx, sel) = deparser_reg_.get_physical_ingress_port_info();
      if (!sel && phv.is_valid_d(phv_idx)) {
        RMT_ASSERT(phv.which_width_d(phv_idx) >= 9);
        physical_ingress_port = phv.get_d(phv_idx);
        if ((physical_ingress_port & 0x7F) < 72)
          physical_ingress_port = RemapLogicalToPhy(physical_ingress_port, true);
      }

      i2q_md->set_physical_ingress_port(physical_ingress_port);
    }

    std::tie(valid, value) = ExtractMetadata(phv, deparser_reg_.get_qid_info());
    RMT_ASSERT(valid);
    RMT_LOG_VERBOSE("DEPARSER::update_metadata qid is %" PRIu64 "\n", value);
    i2q_md->set_qid(value);

    std::tie(valid, value) = ExtractMetadata(phv, deparser_reg_.get_rid_info());
    RMT_ASSERT(valid);
    RMT_LOG_VERBOSE("DEPARSER::update_metadata is %" PRIu64 "\n", value);
    i2q_md->set_irid(value);

    value = deparser_reg_.get_use_yid_tbl();
    RMT_LOG_VERBOSE("DEPARSER::update_metadata use_yid_tbl is %" PRIu64 "\n", value);
    i2q_md->set_use_yid_tbl(value);

    std::tie(valid, value) = ExtractMetadata(phv, deparser_reg_.get_bypass_egr_mode_info());
    RMT_ASSERT(valid);
    RMT_LOG_VERBOSE("DEPARSER::update_metadata bypass_egress is %" PRIu64 "\n", value);
    i2q_md->set_bypass_egr_mode(value);

    std::tie(valid, value) = ExtractMetadata(phv, deparser_reg_.get_xid_info());
    RMT_ASSERT(valid);
    RMT_LOG_VERBOSE("DEPARSER::update_metadata xid is %" PRIu64 "\n", value);
    i2q_md->set_xid(value);

    std::tie(valid, value) = ExtractMetadata(phv, deparser_reg_.get_yid_info());
    RMT_ASSERT(valid);
    RMT_LOG_VERBOSE("DEPARSER::update_metadata yid is %" PRIu64 "\n", value);
    i2q_md->set_yid(value);


    get_object_manager()->log_deparser_metadata(pkt->pkt_id(), pipe_index(), *i2q_md);

    // The if-statement below determines if the deparser drops the packet or
    // sends it to TM. The condition is as follows: cond1 && cond2.
    // cond1 checks if the packet needs to be unicast.
    // cond2 checks if the packet needs to be copied to CPU.
    // Both cond1 and cond2 have the following structure:
    // (check i2q_md fields) || (check drop_ctl bit).
    if ((!i2q_md->is_egress_uc() || ((i2q_md->egress_uc_port() & 0x7F) >= 72)) &&
        (!i2q_md->has_mgid1()) && (!i2q_md->has_mgid2()) &&
        (!i2q_md->cpu_needs_copy())) {
      get_object_manager()->log_deparser_pkt_drop(pkt->pkt_id(), pipe_index(), Gress::ingress);
      get_object_manager()->pkt_delete(pkt);
      pkt = nullptr;
    }

    return pkt;
  }

  Packet *DeparserIngress::SetMetadataChip(const Phv &phv, Packet *pkt) {
    RMT_ASSERT(nullptr != pkt);
    return pkt;
  }

  bool
  DeparserIngress::CheckDeparserPhvGroupConfig(const int &phv_idx) {
    return deparser_reg_.CheckIngressDeparserPhvGroupConfig(phv_idx) &&
           (!deparser_reg_.CheckEgressDeparserPhvGroupConfig(phv_idx));
  }

  std::set<int>
  DeparserIngress::GetInvalidPhvs() {
    return std::set<int> {
      Phv::make_word_d(0,16), Phv::make_word_d(0,17), Phv::make_word_d(0,18),
      Phv::make_word_d(0,19), Phv::make_word_d(0,20), Phv::make_word_d(0,21),
      Phv::make_word_d(0,22), Phv::make_word_d(0,23), Phv::make_word_d(0,24),
      Phv::make_word_d(0,25), Phv::make_word_d(0,26), Phv::make_word_d(0,27),
      Phv::make_word_d(0,28), Phv::make_word_d(0,29), Phv::make_word_d(0,30),
      Phv::make_word_d(0,31),
      Phv::make_word_d(2,16), Phv::make_word_d(2,17), Phv::make_word_d(2,18),
      Phv::make_word_d(2,19), Phv::make_word_d(2,20), Phv::make_word_d(2,21),
      Phv::make_word_d(2,22), Phv::make_word_d(2,23), Phv::make_word_d(2,24),
      Phv::make_word_d(2,25), Phv::make_word_d(2,26), Phv::make_word_d(2,27),
      Phv::make_word_d(2,28), Phv::make_word_d(2,29), Phv::make_word_d(2,30),
      Phv::make_word_d(2,31),
      Phv::make_word_d(4,16), Phv::make_word_d(4,17), Phv::make_word_d(4,18),
      Phv::make_word_d(4,19), Phv::make_word_d(4,20), Phv::make_word_d(4,21),
      Phv::make_word_d(4,22), Phv::make_word_d(4,23), Phv::make_word_d(4,24),
      Phv::make_word_d(4,25), Phv::make_word_d(4,26), Phv::make_word_d(4,27),
      Phv::make_word_d(4,28), Phv::make_word_d(4,29), Phv::make_word_d(4,30),
      Phv::make_word_d(4,31) };
  }
}
