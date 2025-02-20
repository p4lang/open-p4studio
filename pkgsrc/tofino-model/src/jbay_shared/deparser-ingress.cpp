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
#include <chip.h>
#include <deparser-reg.h>
#include <packet.h>
#include <phv.h>
#include <port.h>
#include <rmt-object-manager.h>
#include <event.h>

namespace MODEL_CHIP_NAMESPACE {

  DeparserIngress::DeparserIngress(RmtObjectManager *om, int pipeIndex,
                                   int ioIndex, DeparserReg &deparser_reg)
      : Deparser(om, pipeIndex, ioIndex, false, deparser_reg),
        learning_(om,pipeIndex,deparser_reg)
  {
  }

  Packet *
  DeparserIngress::Deparse(const Phv &phv,
                           LearnQuantumType* learn_quantum,
                           Packet **mirror_packet,
                           Packet **resubmit_pkt,
                           PacketGenMetadata **packet_gen_metadata) {
    // note: this goes through the whole phv even if logging is off!
    //phv.print_d("PHV Ingress deparse start \n", false);

    Packet *pkt = phv.ingress_packet();
    RMT_ASSERT(nullptr != pkt);

    RMT_ASSERT(nullptr != resubmit_pkt);
    (*resubmit_pkt) = nullptr;

    deparser_reg_.increment_phv_counter( false /*ingress*/ );
    deparser_reg_.increment_configurable_counter( false /*ingress*/, GetPortNumber(pkt) );
    Port *port = pkt->port();

    IpbCounters *ipb_counters = get_object_manager()->ipb_counters_lookup(
        pipe_index(),
        port->ipb_index(),
        port->ipb_chan());
    if (nullptr != ipb_counters) ipb_counters->increment_chnl_deparser_send_pkt();

    // Extract POV from incoming PHV
    const BitVector<kPovWidth> pov = ExtractPov(phv);

    // Work out the learn quantam
    learning_.CalculateLearningQuantum(phv,pov,learn_quantum);

    uint64_t drop_ctl;
    // drop_ctl_valid is ignored, if not valid drop_ctl will be set to 0 and not do anything
    /*bool drop_ctl_valid = */ deparser_reg_.get_i_drop_ctl_info(phv,pov,&drop_ctl);

    pkt = HandleResubmit(pkt, phv, pov, drop_ctl, resubmit_pkt, packet_gen_metadata);

    RMT_ASSERT(nullptr != mirror_packet);
    (*mirror_packet) = nullptr;    // set to null, might be set to something else later

    if (nullptr != pkt) {  // no resubmit so we still have a pkt
      // Check for mirroring.

      int slice = GetSliceNumber(pkt);
      // if mirr_io_sel is not valid will default to 0 (at input)
      uint64_t mirr_io_sel;
      /* bool mirr_io_sel_valid = */ deparser_reg_.get_i_mirr_io_sel_info(slice,phv,pov,&mirr_io_sel);

      RMT_LOG_VERBOSE("DEPARSER::deparse drop_ctl:%d mirr_io_sel:%d\n", drop_ctl, mirr_io_sel );
      if (((drop_ctl & 0x04) == 0) && (mirr_io_sel==0)) {
        (*mirror_packet) = GetMirrorPacket(phv, *pkt,pov);
      }

      pkt = GetNewPacket(phv, pkt, pov);
      if (nullptr == pkt) {
        // Packet is being dropped, probably because new header was too big
        // Drop the mirrored packet too.
        RMT_LOG_VERBOSE("DEPARSER::deparse packet drop after GetNewPacket\n" );
        get_object_manager()->pkt_delete(*mirror_packet);
        (*mirror_packet) = nullptr;
      }
      else {
        pkt = SetMetadata(phv, pkt,pov);

        if (nullptr != pkt) (void)MaybeTruncate(pkt, true /*ingr*/); // Do truncation before mirroring

        // do the mirroring before deciding whether to drop
        if (((drop_ctl & 0x04) == 0) && (mirr_io_sel==1)) {
          (*mirror_packet) = GetMirrorPacket(phv, *pkt,pov);
        }
        // XXX: zeroise last 4B of packet (maybe <4B for v.short packets)
        // XXX: but *after* packet goes to Mirror
        (void)ZeroiseCRC(pkt);

        I2QueueingMetadata *i2q_md = pkt->i2qing_metadata();

        bool packet_dropped_by_drop_ctl=false;
        if ((drop_ctl & 0x01) == 0x01) {
          RMT_LOG_VERBOSE("DEPARSER::deparse clearing egress unicast port, mgid1 and mgid2 as drop_ctl=%d\n",drop_ctl );
          packet_dropped_by_drop_ctl = true;
          i2q_md->clr_egress_unicast_port();
          i2q_md->clr_mgid1();
          i2q_md->clr_mgid2();
        }

        if (((drop_ctl & 0x02) == 0x02) && i2q_md->cpu_needs_copy()) {
          RMT_LOG_VERBOSE("DEPARSER::deparse clearing copy_to_cpu as drop_ctl=%d\n",drop_ctl );
          i2q_md->set_copy_to_cpu(0);
        }

        SetTmVector(pkt);
        SetPipeVector(pkt);

        if (pkt) {
          RMT_LOG_VERBOSE("DEPARSER::deparse i2q metadata:\n%s",  pkt->i2qing_metadata()->to_string("    ").c_str());
        }

        // Work out whether to drop the packet ie not send it to TM
        bool is_unicast   = (
            i2q_md->is_egress_uc() &&
            Port::is_valid_pipe_local_port_index(i2q_md->egress_uc_port())) &&
                (! packet_dropped_by_drop_ctl);
        bool is_multicast = i2q_md->has_mgid1() || i2q_md->has_mgid2();
        bool is_to_cpu    = i2q_md->cpu_needs_copy();
        bool going_somewhere = is_unicast || is_multicast || is_to_cpu;
        RMT_LOG_VERBOSE("DEPARSER::deparse packet is %s%s%s%s\n",
                        is_unicast?"unicast":"",is_multicast?" multicast":"",
                        is_to_cpu?" going to cpu":"",
                        going_somewhere?"":" going nowhere, dropped");
        if ( ! going_somewhere ) {
          get_object_manager()->log_deparser_pkt_drop(pkt->pkt_id(), pipe_index(), Gress::ingress);
          get_object_manager()->pkt_delete(pkt);
          pkt = nullptr;
        }

        if (nullptr != pkt) (void)MaybePad(pkt, true /*ingr*/); // Do padding (WIP only) after mirroring
      }


      if ((nullptr == pkt) && (nullptr != ipb_counters)) {
        ipb_counters->increment_chnl_deparser_drop_pkt();
      }
      if (nullptr != pkt) {
        // these counters do not include resubmit packets (XXX)
        int slice_channel = GetChannelWithinSlice(pkt);
        deparser_reg_.increment_perf_pkt_counter(false, slice, slice_channel);
        deparser_reg_.increment_perf_byte_counter(
            false, slice, slice_channel, static_cast<uint64_t>(pkt->len()));
      }
    }

    if ((*resubmit_pkt) != nullptr) {
      RMT_LOG_VERBOSE("DEPARSER::deparse resubmitting\n" );
      // Clots should be discarded and reformed on resubmit. Do it last as
      //   (*resumbit_pkt) == pkt and we need the clots in pkt for deparsing
      (*resubmit_pkt)->reset_clots();
      return nullptr;
    }

    return pkt;
  }

  uint8_t
  DeparserIngress::GetPktVersion(Packet *pkt) {
    return pkt->ingress_info()->version();
  }

  Packet *
  DeparserIngress::SetMetadata(const Phv &phv, Packet *pkt,const BitVector<kPovWidth>& pov) {
    RMT_ASSERT(nullptr != pkt);

    I2QueueingMetadata *i2q_md = pkt->i2qing_metadata();
    i2q_md->reset(); // clear all the metadata before we start

    pkt = SetMetadataChip(phv, pkt);

    int slice = GetSliceNumber(pkt);

    RMT_ASSERT(pkt);

    i2q_md->set_version(pkt->ingress_info()->version());


    uint64_t ret_value;
    // cog data is set up in include/jbay/deparser_metadata.py see include/jbay/deparser-reg.h running for details
    //[[[cog import deparser_metadata as metadata ]]]
    //[[[end]]] (checksum: d41d8cd98f00b204e9800998ecf8427e)
    //[[[cog cog.out(metadata.ingress_extract_list) ]]]
    if ( deparser_reg_.get_i_egress_unicast_port_info(phv, pov, &ret_value) ) i2q_md->set_egress_unicast_port( ret_value );
    if ( deparser_reg_.get_mgid1_info(phv, pov, &ret_value) ) i2q_md->set_mgid1( ret_value );
    if ( deparser_reg_.get_mgid2_info(phv, pov, &ret_value) ) i2q_md->set_mgid2( ret_value );
    if ( deparser_reg_.get_copy_to_cpu_info(phv, pov, &ret_value) ) i2q_md->set_copy_to_cpu( ret_value );
    if ( deparser_reg_.get_hash1_info(slice, phv, pov, &ret_value) ) i2q_md->set_hash1( ret_value );
    if ( deparser_reg_.get_hash2_info(slice, phv, pov, &ret_value) ) i2q_md->set_hash2( ret_value );
    if ( deparser_reg_.get_copy_to_cpu_cos_info(slice, phv, pov, &ret_value) ) i2q_md->set_copy_to_cpu_cos( ret_value );
    if ( deparser_reg_.get_deflect_on_drop_info(slice, phv, pov, &ret_value) ) i2q_md->set_dod( ret_value );
    if ( deparser_reg_.get_icos_info(slice, phv, pov, &ret_value) ) i2q_md->set_icos( ret_value );
    if ( deparser_reg_.get_pkt_color_info(slice, phv, pov, &ret_value) ) i2q_md->set_meter_color( ret_value );
    if ( deparser_reg_.get_qid_info(slice, phv, pov, &ret_value) ) i2q_md->set_qid( ret_value );
    if ( deparser_reg_.get_xid_l1_info(slice, phv, pov, &ret_value) ) i2q_md->set_xid( ret_value );
    if ( deparser_reg_.get_xid_l2_info(slice, phv, pov, &ret_value) ) i2q_md->set_yid( ret_value );
    if ( deparser_reg_.get_rid_info(slice, phv, pov, &ret_value) ) i2q_md->set_irid( ret_value );
    if ( deparser_reg_.get_bypss_egr_info(slice, phv, pov, &ret_value) ) i2q_md->set_bypass_egr_mode( ret_value );
    if ( deparser_reg_.get_ct_disable_info(slice, phv, pov, &ret_value) ) i2q_md->set_ct_disable_mode( ret_value );
    if ( deparser_reg_.get_ct_mcast_info(slice, phv, pov, &ret_value) ) i2q_md->set_ct_mcast_mode( ret_value );
    if ( deparser_reg_.get_i_afc_info(slice, phv, pov, &ret_value) ) {
      // XXX: no longer any afc_mode or afc8_info on WIP - also dieId param ignored on WIP (already in ret_value)
      AFCMetadata afc(ret_value, get_object_manager()->chip()->GetMyDieId());
      i2q_md->set_afc(afc);
    }
    if ( deparser_reg_.get_i_mtu_trunc_len_info(slice, phv, pov, &ret_value) ) i2q_md->set_mtu_trunc_len( ret_value );
    if ( deparser_reg_.get_i_mtu_trunc_err_f_info(slice, phv, pov, &ret_value) ) i2q_md->set_mtu_trunc_err_f( ret_value );
    //[[[end]]] (checksum: 8779dbc0dec7de15b4f9caf2ce0c8834)

    if (i2q_md->is_egress_uc()) {
      uint16_t port = i2q_md->egress_uc_port();
      RMT_LOG_VERBOSE("DEPARSER::update_metadata egress unicast port is %d\n", port);
      if (Port::get_port_num(port) >= Port::kPortsPerPipe) {
        // XXX: Clear unicast flag if invalid port
        RMT_LOG_VERBOSE("DEPARSER::update_metadata egress unicast port >= %d; clearing unicast flag\n",
                        Port::kPortsPerPipe);
        i2q_md->clr_egress_unicast_port();
      }
    } else {
      RMT_LOG_VERBOSE("DEPARSER::update_metadata egress unicast port is invalid\n");
    }

    i2q_md->set_physical_ingress_port(pkt->ingress_info()->port()->port_index());

    //dprsr_pre_version_r.h
    //value = deparser_reg_.get_use_yid_tbl(); Tofino
    uint64_t pre_version = deparser_reg_.get_pre_version(slice);
    RMT_LOG_VERBOSE("DEPARSER::update_metadata pre_version (was use_yid_tbl) is %" PRIu64 "\n", pre_version);
    i2q_md->set_use_yid_tbl(pre_version);

    get_object_manager()->log_deparser_metadata(pkt->pkt_id(), pipe_index(), *i2q_md);

    return pkt;
  }

  bool
  DeparserIngress::CheckDeparserPhvGroupConfig(const int &phv_idx,int slice) {
    return deparser_reg_.CheckIngressDeparserPhvGroupConfig(phv_idx) &&
        (!deparser_reg_.CheckEgressDeparserPhvGroupConfig(phv_idx));
  }

// Chip specific metadata
Packet *
DeparserIngress::SetMetadataChip(const Phv &phv, Packet *pkt) {
  RMT_ASSERT(nullptr != pkt);

  return pkt;
}

// in JBay this always returns pkt unchanged as we might need to mirror it
Packet * DeparserIngress::HandleResubmit(Packet* const pkt, const Phv &phv,
                                         const BitVector<kPovWidth>& pov,
                                         uint8_t drop_ctl,
                                         Packet **resubmit_pkt,
                                         PacketGenMetadata **packet_gen_metadata) {
  uint64_t table_index;
  bool resubmit_valid = deparser_reg_.get_resub_sel_info(phv,pov,&table_index);

  uint64_t pgen_flag;
  bool pgen_valid = deparser_reg_.get_pgen_info(phv,pov,&pgen_flag) && (pgen_flag != 0);

  RMT_ASSERT(nullptr != resubmit_pkt);
  RMT_ASSERT(nullptr == (*resubmit_pkt));

  RMT_ASSERT(nullptr != packet_gen_metadata);
  RMT_ASSERT(nullptr == (*packet_gen_metadata));

  if (resubmit_valid && ((drop_ctl & 0x01) == 0)) {

    std::vector<uint8_t> phv_idx_list;
    bool table_entry_valid;
    deparser_reg_.get_resubmit_table_entry(table_index, &table_entry_valid,
                                           &phv_idx_list);
    if (table_entry_valid) {
      if (pkt->is_resubmit()) {
        RMT_LOG_WARN("Can not resubmit a pkt that has already been resubmitted\n");
      } else {
        (*resubmit_pkt) = pkt;
        // Do not add resubmit header to the packet. Just stash it somewhere
        // so ingress buffer module can add to the packet correctly.
        PacketBuffer *meta_hdr = GetMetadataHeader(phv, phv_idx_list);
        // XXX: if JBayA0 byte-swap buffer to match RTL
        if (RmtObject::is_jbayA0()) meta_hdr->swap_byte_order();
        (*resubmit_pkt)->set_resubmit_header(meta_hdr);
        (*resubmit_pkt)->mark_for_resubmit();
      }
      return nullptr;
    }
  }

  if ( pgen_valid && (nullptr == (*resubmit_pkt)) ) {
    // PGEN and Resub share the same bus, and resub has priority.
    *packet_gen_metadata = new PacketGenMetadata;
    uint64_t ret_value;
    if ( deparser_reg_.get_pgen_len_info(phv, pov, &ret_value) )
      (*packet_gen_metadata)->set_length( ret_value );
    else
      (*packet_gen_metadata)->clr_length( );

    if ( deparser_reg_.get_pgen_addr_info(phv, pov, &ret_value) )
      (*packet_gen_metadata)->set_address( ret_value );
    else
      (*packet_gen_metadata)->clr_address( );

    std::vector<uint8_t> phv_idx_list;
    bool table_entry_valid;
    deparser_reg_.get_packet_gen_table_entry(&table_entry_valid,&phv_idx_list);
    if (table_entry_valid) {
      PacketBuffer *trigger_data = GetMetadataHeader(phv, phv_idx_list);
      // XXX: if JBayA0 byte-swap trigger data to match RTL
      if (RmtObject::is_jbayA0()) trigger_data->swap_byte_order();
      (*packet_gen_metadata)->set_trigger(trigger_data);
    }

    RMT_LOG_VERBOSE("DEPARSER::deparse packet gen metadata:\n%s",
                    (*packet_gen_metadata)->to_string("    ").c_str());
  }
  return pkt;
}

void DeparserIngress::SetTmVector(Packet *pkt) {
  if (!is_chip1()) return;  // jbay does not have tm vec
  // XXX: calculate two tm_vec bits
  I2QueueingMetadata *i2q_md = pkt->i2qing_metadata();
  uint8_t tm_vec = 0;
  Chip *chip = get_object_manager()->chip();
  uint8_t tm_vec_0_mask = static_cast<uint8_t>(
      (0x1 << chip->GetMyDieId()) |
      (0x1 << chip->GetReadDieId()));
  uint8_t tm_vec_1_mask = static_cast<uint8_t>(
      (0x1 << chip->GetWriteDieId()) |
      (0x1 << chip->GetDiagonalDieId()));
  if (i2q_md->is_egress_uc()) {
    // The egress_die_id is extracted by the ingress Deparser from packet meta
    // data (the egress_die_ie is the two high order bits of the 11-bit egress
    // port_id for WIP).  The following equations are then used to
    // calculate the unicast tm_vec[1:0] value based on the egress_die_id value
    // and the *_die_id inputs to Deparser:
    //
    // uc_tm_local = (egress_die_id==my_die_id) || (egress_die_id==rd_die_id);
    // tm_vec_uc[0] = uc_tm_local & unicast_valid;
    // tm_vec_uc[1] = ~(uc_tm_local) & unicast_valid;
    uint8_t egress_die_id = Port::get_die_num(i2q_md->egress_uc_port());
    bool uc_tm_local = (
        (egress_die_id == chip->GetMyDieId()) ||
        (egress_die_id == chip->GetReadDieId()));
    tm_vec = uc_tm_local ? 0x1u : 0x2u;
    RMT_LOG_VERBOSE("DEPARSER::deparse tm_vec after unicast=%x\n", tm_vec );
  }
  if (i2q_md->cpu_needs_copy()) {
    // The following equations are used to generate the Copy-to-CPU tm_vec[1:0]
    // value based on the Copy-to-CPU TM vector CSR, the TM-Vector-Table &
    // Copy-to-CPU Mask CSR, and the *_die_id inputs to Deparser:
    //
    // Copy-to-CPU-tv-masked[3:0] = copy-to-cpu-tv[3:0] & tvt_c2c_mask[3:0];
    //
    // tm_vec_c2c[0] = (Copy-to-CPU-tv-masked[my_die_id] ||
    //                  Copy-to-CPU-tv-masked[rd_die_id]) & copy-to-cpu-valid;
    // tm_vec_c2c[1] = (Copy-to-CPU-tv-masked[wr_die_id] ||
    //                  Copy-to-CPU-tv-maksed[dg_die_id]) & copy-to-cpu-valid;
    uint8_t c2c_tv_masked = deparser_reg_.chip_reg().get_copy_to_cpu_die_vector();
    // set bit 0 if copy to cpu on my die or read die
    tm_vec |= (((tm_vec_0_mask & c2c_tv_masked) ? 0x1u : 0x0u) << 0u);
    // set bit 1 if copy to cpu on write die or diagonal die
    tm_vec |= (((tm_vec_1_mask & c2c_tv_masked) ? 0x1u : 0x0u) << 1u);
    RMT_LOG_VERBOSE("DEPARSER::deparse tm_vec after copy to cpu=%x\n", tm_vec );
  }
  // The ingress Deparser uses multicast group IDs (MGID1, MGID2) to perform a
  // lookup in the TM select Vector Table (TVT) to obtain 4-bit vectors
  // indicating which of the four Die are destinations for the multicast
  // packet.  Bit[0] is for die=0, bit[1] is for die=1, etc.  The following
  // equations are used by ingress Deparser to generate the multicast tm_vec
  // using the output of the TVT lookup, the TM-Vector-Table & Copy-to-CPU Mask
  // CSR, and the *_die_id inputs:
  //
  // TVT_1_masked = TVT[mgid1] & tvt_c2c_mask[3:0];
  // TVT_2_masked = TVT[mgid2] & tvt_c2c_mask[3:0];
  //
  // tm_vec_mc[0] = (TVT_1_masked[my_die_id] || TVT_1_masked[rd_die_id]
  //              || TVT_2_masked[my_die_id] || TVT_2_masked[rd_die_id])
  //              & multicast_valid;
  //  tm_vec_mc[1] = (TVT_1_masked[wr_die_id] || TVT_1_masked[dg_die_id]
  //               || TVT_2_masked[wr_die_id] || TVT_2_masked[dg_die_id])
  //               & multicast_valid;
  if (i2q_md->has_mgid1()) {
    uint8_t tvt_1_masked = deparser_reg_.chip_reg().get_multicast_die_vector(0, i2q_md->mgid1());
    tm_vec |= (((tm_vec_0_mask & tvt_1_masked) ? 0x1u : 0x0u) << 0u);
    tm_vec |= (((tm_vec_1_mask & tvt_1_masked) ? 0x1u : 0x0u) << 1u);
    RMT_LOG_VERBOSE("DEPARSER::deparse tm_vec after mgid1=%x\n", tm_vec );
  }
  if (i2q_md->has_mgid2()) {
    uint8_t tvt_2_masked = deparser_reg_.chip_reg().get_multicast_die_vector(1, i2q_md->mgid2());
    tm_vec |= (((tm_vec_0_mask & tvt_2_masked) ? 0x1u : 0x0u) << 0u);
    tm_vec |= (((tm_vec_1_mask & tvt_2_masked) ? 0x1u : 0x0u) << 1u);
    RMT_LOG_VERBOSE("DEPARSER::deparse tm_vec after mgid2=%x\n", tm_vec );
  }
  i2q_md->set_tm_vec(tm_vec);
  RMT_LOG_VERBOSE("DEPARSER::deparse tm_vec final=%x\n", tm_vec );
}


}
