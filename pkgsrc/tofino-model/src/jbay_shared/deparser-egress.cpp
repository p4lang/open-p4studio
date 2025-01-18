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

  Packet *DeparserEgress::Deparse(Phv &phv, Packet **mirror_packet) {
    // note: this goes through the whole phv even if logging is off!
    //phv.print_d("PHV Egress deparse start\n", false);

    // Extract POV from incoming PHV
    const BitVector<kPovWidth> pov = ExtractPov(phv);
    Packet *pkt = phv.egress_packet();
    Teop *teop = phv.teop();
    RMT_ASSERT(nullptr != pkt);

    deparser_reg_.increment_phv_counter( true /*egress*/ );
    deparser_reg_.increment_configurable_counter( true /*egress*/, GetPortNumber(pkt) );
    Port *port = pkt->port();
    auto *epb_counters = get_object_manager()->epb_counters_lookup(pipe_index(),
                                                                   port->epb_index(),
                                                                   port->epb_chan());
    if (nullptr != epb_counters) epb_counters->increment_chnl_deparser_send_pkt();

    uint64_t drop_ctl;
    // drop_ctl_valid is ignored, if not valid drop_ctl will be set to 0 and not do anything
    /*bool drop_ctl_valid = */ deparser_reg_.get_e_drop_ctl_info(phv,pov,&drop_ctl);


    RMT_ASSERT(nullptr != mirror_packet);
    (*mirror_packet) = nullptr;    // set to null, might be set to something else later


    int slice = GetSliceNumber(pkt);
    // if mirr_io_sel is not valid will default to 0 (at input)
    uint64_t mirr_io_sel;
    /* bool mirr_io_sel_valid = */ deparser_reg_.get_e_mirr_io_sel_info(slice,phv,pov,&mirr_io_sel);

    // Check for mirroring.
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
      pkt = SetMetadata(phv, pkt, pov);

      if (((drop_ctl & 0x04) == 0) && (mirr_io_sel==1)) {
        (*mirror_packet) = GetMirrorPacket(phv, *pkt,pov);
      }
      // XXX: zeroise last 4B of packet (maybe <4B for v.short packets)
      // XXX: but *after* packet goes to Mirror
      (void)ZeroiseCRC(pkt);

      if ( teop != nullptr ) {
        RMT_ASSERT( teop->hdr_time() );
        teop->set_byte_len(pkt->len());
        //  Can indicate ECC/parity errors within the IBUF as well as FCS errors from the MAC
        //teop->set_error_fcs();
        if (pkt->truncated()) {
          teop->set_error_trunc();
        }
      }

      E2MacMetadata *e2mac_metadata = pkt->e2mac_metadata();
      bool valid = e2mac_metadata->is_egress_uc();

      // Only drop becuase of metadata if the check register is set and the metadata is not valid or out of range
      bool drop_because_of_metadata = deparser_reg_.EgressUnicastNeedsCheck() &&
        ( !valid || !Port::is_valid_pipe_local_port_index(e2mac_metadata->egress_unicast_port()) );

      if ( drop_because_of_metadata ||
           ((drop_ctl & 0x01) == 0x01)) {
        RMT_LOG_VERBOSE("DEPARSER::deparse dropping. valid=%s port=%d drop_ctl=%d\n", valid?"true":"false", e2mac_metadata->egress_unicast_port(), drop_ctl );
        get_object_manager()->log_deparser_pkt_drop(pkt->pkt_id(), pipe_index(), Gress::egress);
        get_object_manager()->pkt_delete(pkt);
        pkt = nullptr;

        if ( deparser_reg_.teop_inhibit(slice) &&
             teop != nullptr ) {
          teop->clear_stats_en();
          teop->clear_meter_en();
        }

      }
    }

    if (nullptr != pkt) {
      // XXX/XXX: Maybe pad to a minimum length (WIP only)
      if (MaybePad(pkt, false /*egress*/)) {
        // If we padded pkt fixup teop
        if (nullptr != teop) teop->set_byte_len(pkt->len());
      }
    }

    if (nullptr != pkt) {
      int slice_channel = GetChannelWithinSlice(pkt);
      deparser_reg_.increment_perf_pkt_counter(true, slice, slice_channel);
      deparser_reg_.increment_perf_byte_counter(
          true, slice, slice_channel, static_cast<uint64_t>(pkt->len()));
    }
    return pkt;
  }

  Packet *DeparserEgress::SetMetadata(const Phv &phv, Packet *pkt,const BitVector<kPovWidth>& pov) {
    int slice = GetSliceNumber(pkt);

    pkt = SetMetadataChip(phv,pkt,nullptr);

    E2MacMetadata *e2mac_metadata = pkt->e2mac_metadata();
    RMT_ASSERT(e2mac_metadata);
    e2mac_metadata->reset();

    uint64_t ret_value;
    // cog data is set up in include/jbay/deparser_metadata.py see include/jbay/deparser-reg.h running for details
    //[[[cog import deparser_metadata as metadata ]]]
    //[[[end]]] (checksum: d41d8cd98f00b204e9800998ecf8427e)
    //[[[cog cog.out(metadata.egress_extract_list) ]]]
    if ( deparser_reg_.get_e_egress_unicast_port_info(phv, pov, &ret_value) ) e2mac_metadata->set_egress_unicast_port( ret_value );
    if ( deparser_reg_.get_force_tx_err_info(slice, phv, pov, &ret_value) ) e2mac_metadata->set_force_tx_error( ret_value );
    if ( deparser_reg_.get_capture_tx_ts_info(slice, phv, pov, &ret_value) ) e2mac_metadata->set_capture_tx_ts( ret_value );
    if ( deparser_reg_.get_tx_pkt_has_offsets_info(slice, phv, pov, &ret_value) ) e2mac_metadata->set_update_delay_on_tx( ret_value );
    if ( deparser_reg_.get_e_afc_info(slice, phv, pov, &ret_value) ) e2mac_metadata->set_afc(AFCMetadata(ret_value));
    if ( deparser_reg_.get_e_mtu_trunc_len_info(slice, phv, pov, &ret_value) ) e2mac_metadata->set_mtu_trunc_len( ret_value );
    if ( deparser_reg_.get_e_mtu_trunc_err_f_info(slice, phv, pov, &ret_value) ) e2mac_metadata->set_mtu_trunc_err_f( ret_value );
    //[[[end]]] (checksum: 2d66c203b8c8be3561bd33b8e3bf69b7)
    RMT_LOG_VERBOSE("DEPARSER::deparse egress unicast port is %d\n", e2mac_metadata->egress_unicast_port() );

    auto mtu_trunc_len = pkt->e2mac_metadata()->mtu_trunc_len();
    if ( mtu_trunc_len && ( pkt->len() > *mtu_trunc_len ) ) {
      RMT_LOG_VERBOSE("DEPARSER::deparse truncating to %d\n", *mtu_trunc_len );
      pkt->trim_back(pkt->len() - *mtu_trunc_len );
      pkt->set_truncated(true);
    }

    get_object_manager()->log_deparser_metadata(pkt->pkt_id(), pipe_index(), *e2mac_metadata);

    return pkt;
  }

  uint8_t
  DeparserEgress::GetPktVersion(Packet *pkt) {
    return pkt->egress_info()->version();
  }

  bool
  DeparserEgress::CheckDeparserPhvGroupConfig(const int &phv_idx, int slice) {
    return deparser_reg_.CheckEgressDeparserPhvGroupConfig(phv_idx) &&
        (!deparser_reg_.CheckIngressDeparserPhvGroupConfig(phv_idx));
  }

  Packet *
  DeparserEgress::SetMetadataChip(const Phv &phv, Packet *pkt, Packet **mirror_packet) {
    return pkt;
  }
}
