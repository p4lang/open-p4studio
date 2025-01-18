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

#include <functional>
#include <deparser-reg.h>
#include <deparser.h>
#include <phv.h>
#include <register_adapters.h>


namespace MODEL_CHIP_NAMESPACE {

// Registers must use the correct adapter:
//  deparser_in_hdr_adapter() for registers in the Input and Header phase
//  deparser_out_adapter() for registers in the Output phase (not shared)

DeparserReg::DeparserReg(int chip, int pipe) :
    deparser_chip_reg_(chip, pipe),
    phv_csum_cfg_entry_array_{{
    {deparser_in_hdr_adapter(phv_csum_cfg_entry_array_[0], chip, pipe, register_classes::DprsrPhvCsumCfgEntryGArray::kIim,
                      [this](uint32_t a0){this->PhvCsumCfgWriteCallback(0,1,a0);} )},
    {deparser_in_hdr_adapter(phv_csum_cfg_entry_array_[1], chip, pipe, register_classes::DprsrPhvCsumCfgEntryGArray::kIem,
                      [this](uint32_t a0){this->PhvCsumCfgWriteCallback(1,0,a0);} )} }},
    tphv_csum_cfg_entry_array_{{
    {deparser_in_hdr_adapter(tphv_csum_cfg_entry_array_[0], chip, pipe, register_classes::DprsrTphvCsumCfgEntryGArray::kHim,
                      [this](uint32_t a0){this->TphvCsumCfgWriteCallback(0,1,a0);} )},
    {deparser_in_hdr_adapter(tphv_csum_cfg_entry_array_[1], chip, pipe, register_classes::DprsrTphvCsumCfgEntryGArray::kHem,
                      [this](uint32_t a0){this->TphvCsumCfgWriteCallback(1,0,a0);} )} }} ,
    pov_position_{{
    {deparser_in_hdr_adapter(pov_position_[0], chip, pipe, register_classes::DprsrPovPositionR::kIir )},
    {deparser_in_hdr_adapter(pov_position_[1], chip, pipe, register_classes::DprsrPovPositionR::kIer )}  }},
    egress_unicast_port_{{
    {deparser_in_hdr_adapter(egress_unicast_port_[0], chip, pipe, register_classes::DprsrEgressUnicastPort::kIir )},
    {deparser_in_hdr_adapter(egress_unicast_port_[1], chip, pipe, register_classes::DprsrEgressUnicastPort::kIer )}  }},
    // counters
    cnt_vld_i_phv_{{
      {deparser_in_hdr_adapter(cnt_vld_i_phv_[0], chip, pipe, register_classes::DprsrCntVldIPhvMutable::kIir )},
      {deparser_in_hdr_adapter(cnt_vld_i_phv_[1], chip, pipe, register_classes::DprsrCntVldIPhvMutable::kIer )}  }},
    cnt_vld_i_tphv_{{
      {deparser_in_hdr_adapter(cnt_vld_i_tphv_[0], chip, pipe, register_classes::DprsrCntVldITphvMutable::kIir )},
      {deparser_in_hdr_adapter(cnt_vld_i_tphv_[1], chip, pipe, register_classes::DprsrCntVldITphvMutable::kIer )}  }},
    cnt_i_dec_resubmit_{deparser_in_hdr_adapter(cnt_i_dec_resubmit_, chip, pipe)},
    cnt_i_dec_read_{deparser_in_hdr_adapter(cnt_i_dec_read_, chip, pipe)},
    cnt_i_dec_discard_{deparser_in_hdr_adapter(cnt_i_dec_discard_, chip, pipe)},
    cnt_i_dec_learn_{deparser_in_hdr_adapter(cnt_i_dec_learn_, chip, pipe)},
    hdr_too_long_i_{deparser_in_hdr_adapter(hdr_too_long_i_, chip, pipe)},
    hdr_too_long_e_{deparser_in_hdr_adapter(hdr_too_long_e_, chip, pipe)},
    cnt_pkts_i_{deparser_in_hdr_adapter(cnt_pkts_i_, chip, pipe)},
    cnt_pkts_e_{deparser_in_hdr_adapter(cnt_pkts_e_, chip, pipe)},
    i_fwd_pkts_{deparser_in_hdr_adapter(i_fwd_pkts_, chip, pipe)},
    e_fwd_pkts_{deparser_in_hdr_adapter(e_fwd_pkts_, chip, pipe)},
    i_disc_pkts_{deparser_in_hdr_adapter(i_disc_pkts_, chip, pipe)},
    e_disc_pkts_{deparser_in_hdr_adapter(e_disc_pkts_, chip, pipe)},
    i_mirr_pkts_{deparser_in_hdr_adapter(i_mirr_pkts_, chip, pipe)},
    e_mirr_pkts_{deparser_in_hdr_adapter(e_mirr_pkts_, chip, pipe)},
    i_egr_pkt_err_{deparser_in_hdr_adapter(i_egr_pkt_err_, chip, pipe)},
    e_egr_pkt_err_{deparser_in_hdr_adapter(e_egr_pkt_err_, chip, pipe)},
    i_ctm_pkt_err_{deparser_in_hdr_adapter(i_ctm_pkt_err_, chip, pipe)},
    e_ctm_pkt_err_{deparser_in_hdr_adapter(e_ctm_pkt_err_, chip, pipe)},
    ctm_crc_err_i_{deparser_in_hdr_adapter(ctm_crc_err_i_, chip, pipe)},
    ctm_crc_err_e_{deparser_in_hdr_adapter(ctm_crc_err_e_, chip, pipe)},
    // Ingress deparser registers.
    copy_to_cpu_{deparser_in_hdr_adapter(copy_to_cpu_, chip, pipe )},
    copy_to_cpu_cos_{deparser_in_hdr_adapter(copy_to_cpu_cos_, chip, pipe )},
    copy_to_cpu_pv_{deparser_in_hdr_adapter(copy_to_cpu_pv_, chip, pipe )},
    ct_disable_mode_{deparser_in_hdr_adapter(ct_disable_mode_, chip, pipe )},
    ct_mcast_mode_{deparser_in_hdr_adapter(ct_mcast_mode_, chip, pipe )},
    deflect_on_drop_{deparser_in_hdr_adapter(deflect_on_drop_, chip, pipe )},
    drop_ctl_ingress_{deparser_in_hdr_adapter(drop_ctl_ingress_, chip, pipe, register_classes::DprsrDropCtlR::kIir )},
    hash_lag_emcp_mcast_1_{deparser_in_hdr_adapter(hash_lag_emcp_mcast_1_, chip, pipe, 0 )},
    hash_lag_emcp_mcast_2_{deparser_in_hdr_adapter(hash_lag_emcp_mcast_2_, chip, pipe, 1 )},
    icos_{deparser_in_hdr_adapter(icos_, chip, pipe )},
    meter_color_{deparser_in_hdr_adapter(meter_color_, chip, pipe )},
    egress_multicast_group_1_{deparser_in_hdr_adapter(egress_multicast_group_1_, chip, pipe, 0 )},
    egress_multicast_group_2_{deparser_in_hdr_adapter(egress_multicast_group_2_, chip, pipe, 1 )},
    pvt_r_array_{{
    {deparser_in_hdr_adapter(pvt_r_array_[0], chip, pipe, register_classes::DprsrPvtRArray::kTbl0 )},
    {deparser_in_hdr_adapter(pvt_r_array_[1], chip, pipe, register_classes::DprsrPvtRArray::kTbl1 )}  }},
    physical_ingress_port_{deparser_in_hdr_adapter(physical_ingress_port_, chip, pipe )},
    qid_{deparser_in_hdr_adapter(qid_, chip, pipe )},
    rid_{deparser_in_hdr_adapter(rid_, chip, pipe )},
    use_yid_tbl_{deparser_in_hdr_adapter(use_yid_tbl_, chip, pipe )},
    bypass_egr_mode_{deparser_in_hdr_adapter(bypass_egr_mode_, chip, pipe )},
    xid_{deparser_in_hdr_adapter(xid_, chip, pipe )},
    yid_{deparser_in_hdr_adapter(yid_, chip, pipe )},
    // Resubmit registers
    resubmit_cfg_{deparser_in_hdr_adapter(resubmit_cfg_, chip, pipe )},
    resubmit_table_entry_r_array_{deparser_in_hdr_adapter(resubmit_table_entry_r_array_, chip, pipe )},
    // Egress deparser registers.
    capture_tx_ts_{deparser_in_hdr_adapter(capture_tx_ts_, chip, pipe )},
    drop_ctl_egress_{deparser_in_hdr_adapter(drop_ctl_egress_, chip, pipe, register_classes::DprsrDropCtlR::kIer )},
    ecos_{deparser_in_hdr_adapter(ecos_, chip, pipe )},
    force_tx_err_{deparser_in_hdr_adapter(force_tx_err_, chip, pipe )},
    tx_pkt_has_ts_offsets_{deparser_in_hdr_adapter(tx_pkt_has_ts_offsets_, chip, pipe )},
    // Mirroring registers.
    mirror_cfg_{{
    {deparser_in_hdr_adapter(mirror_cfg_[0], chip, pipe, register_classes::DprsrMirrorCfgR::kHir )},
    {deparser_in_hdr_adapter(mirror_cfg_[1], chip, pipe, register_classes::DprsrMirrorCfgR::kHer )}  }},
    mirror_table_entry_r_array_{{
    {deparser_in_hdr_adapter(mirror_table_entry_r_array_[0], chip, pipe, register_classes::DprsrMirrorTableEntryRArray::kHir )},
    {deparser_in_hdr_adapter(mirror_table_entry_r_array_[1], chip, pipe, register_classes::DprsrMirrorTableEntryRArray::kHer )}  }},
    e_coal_(deparser_in_hdr_adapter(e_coal_,chip, pipe)),
    fde_pov_r_array_{{
    {deparser_in_hdr_adapter(fde_pov_r_array_[0], chip, pipe, register_classes::DprsrFdePovRArray::kIim )},
    {deparser_in_hdr_adapter(fde_pov_r_array_[1], chip, pipe, register_classes::DprsrFdePovRArray::kIem )}  }},
    fde_phv_r_array_{{
    {deparser_in_hdr_adapter(fde_phv_r_array_[0], chip, pipe, register_classes::DprsrFdePhvRArray::kHim )},
    {deparser_in_hdr_adapter(fde_phv_r_array_[1], chip, pipe, register_classes::DprsrFdePhvRArray::kHem )}  }},
    // Deparser PHV group registers.
    egress_only_phv8_group_{deparser_in_hdr_adapter(egress_only_phv8_group_, chip, pipe )},
    egress_only_phv8_split_{deparser_in_hdr_adapter(egress_only_phv8_split_, chip, pipe )},
    egress_only_phv16_group_{deparser_in_hdr_adapter(egress_only_phv16_group_, chip, pipe )},
    egress_only_phv16_split_{deparser_in_hdr_adapter(egress_only_phv16_split_, chip, pipe )},
    egress_only_phv32_group_{deparser_in_hdr_adapter(egress_only_phv32_group_, chip, pipe )},
    egress_only_phv32_split_{deparser_in_hdr_adapter(egress_only_phv32_split_, chip, pipe )},
    ingress_only_phv8_group_{deparser_in_hdr_adapter(ingress_only_phv8_group_, chip, pipe )},
    ingress_only_phv8_split_{deparser_in_hdr_adapter(ingress_only_phv8_split_, chip, pipe )},
    ingress_only_phv16_group_{deparser_in_hdr_adapter(ingress_only_phv16_group_, chip, pipe )},
    ingress_only_phv16_split_{deparser_in_hdr_adapter(ingress_only_phv16_split_, chip, pipe )},
    ingress_only_phv32_group_{deparser_in_hdr_adapter(ingress_only_phv32_group_, chip, pipe )},
    ingress_only_phv32_split_{deparser_in_hdr_adapter(ingress_only_phv32_split_, chip, pipe )},
    csum_idx_to_phv_idx{ 64+63, 64+31, 64+62, 64+30, 64+61, 64+29, 64+60, 64+28,
                         128+95, 128+47, 128+94, 128+46, 128+93, 128+45, 128+92, 128+44,
                         128+91, 128+43, 128+90, 128+42, 64+59, 64+27, 64+58, 64+26,
                         64+57, 64+25, 64+56, 64+24, 128+89, 128+41, 128+88, 128+40,
                         128+87, 128+39, 128+86, 128+38, 128+85, 128+37, 128+84, 128+36,
                         64+55, 64+23, 64+54, 64+22, 64+53, 64+21, 64+52, 64+20,
                         128+83, 128+35, 128+82, 128+34, 128+81, 128+33, 128+80, 128+32,
                         128+79, 128+31, 128+78, 128+30, 64+51, 64+19, 64+50, 64+18,
                         64+49, 64+17, 64+48, 64+16, 128+77, 128+29, 128+76, 128+28,
                         128+75, 128+27, 128+74, 128+26, 128+73, 128+25, 128+72, 128+24,
                         64+47, 64+15, 64+46, 64+14, 64+45, 64+13, 64+44, 64+12,
                         128+71, 128+23, 128+70, 128+22, 128+69, 128+21, 128+68, 128+20,
                         128+67, 128+19, 128+66, 128+18, 64+43, 64+11, 64+42, 64+10,
                         64+41, 64+9, 64+40, 64+8, 128+65, 128+17, 128+64, 128+16,
                         128+63, 128+15, 128+62, 128+14, 128+61, 128+13, 128+60, 128+12,
                         64+39, 64+7, 64+38, 64+6, 64+37, 64+5, 64+36, 64+4,
                         128+59, 128+11, 128+58, 128+10, 128+57, 128+9, 128+56, 128+8,
                         128+55, 128+7, 128+54, 128+6, 64+35, 64+3, 64+34, 64+2,
                         64+33, 64+1, 64+32, 64+0, 128+53, 128+5, 128+52, 128+4,
                         128+51, 128+3, 128+50, 128+2, 128+49, 128+1, 128+48, 128+0, 63,
                         63, 31, 31, 62, 62, 30, 30, 61, 61, 29, 29, 60, 60, 28,
                         28, 59, 59, 27, 27, 58, 58, 26, 26, 57, 57, 25, 25, 56,
                         56, 24, 24, 55, 55, 23, 23, 54, 54, 22, 22, 53, 53, 21,
                         21, 52, 52, 20, 20, 51, 51, 19, 19, 50, 50, 18, 18, 49,
                         49, 17, 17, 48, 48, 16, 16, 47, 47, 15, 15, 46, 46, 14,
                         14, 45, 45, 13, 13, 44, 44, 12, 12, 43, 43, 11, 11, 42,
                         42, 10, 10, 41, 41, 9, 9, 40, 40, 8, 8, 39, 39, 7, 7,
                         38, 38, 6, 6, 37, 37, 5, 5, 36, 36, 4, 4, 35, 35, 3, 3,
                         34, 34, 2, 2, 33, 33, 1, 1, 32, 32, 0, 0 },
    csum_idx_to_shift { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 16, 0, 16, 0, 16, 0, 16,
                        0, 16, 0, 16, 0, 16, 0, 16, 0, 16, 0, 16, 0, 16, 0, 16,
                        0, 16, 0, 16, 0, 16, 0, 16, 0, 16, 0, 16, 0, 16, 0, 16,
                        0, 16, 0, 16, 0, 16, 0, 16, 0, 16, 0, 16, 0, 16, 0, 16,
                        0, 16, 0, 16, 0, 16, 0, 16, 0, 16, 0, 16, 0, 16, 0, 16,
                        0, 16, 0, 16, 0, 16, 0, 16, 0, 16, 0, 16, 0, 16, 0, 16,
                        0, 16, 0, 16, 0, 16, 0, 16, 0, 16, 0, 16, 0, 16, 0, 16,
                        0, 16, 0, 16, 0, 16, 0, 16, 0, 16, 0, 16, 0, 16, 0, 16,
                        0, 16, 0, 16, 0, 16, 0 } {
    Reset();
  }

  DeparserReg::~DeparserReg() {
  }

  void DeparserReg::Reset() {
    deparser_chip_reg_.Reset();
    phv_csum_cfg_entry_array_[kIngress].reset();
    phv_csum_cfg_entry_array_[kEgress].reset();
    tphv_csum_cfg_entry_array_[kIngress].reset();
    tphv_csum_cfg_entry_array_[kEgress].reset();
    pov_position_[kIngress].reset();
    pov_position_[kEgress].reset();

    // Counters
    cnt_vld_i_phv_[kIngress].reset();
    cnt_vld_i_phv_[kEgress].reset();
    cnt_vld_i_tphv_[kIngress].reset();
    cnt_vld_i_tphv_[kEgress].reset();
    cnt_i_dec_resubmit_.reset();
    cnt_i_dec_read_.reset();
    cnt_i_dec_discard_.reset();
    cnt_i_dec_learn_.reset();
    hdr_too_long_i_.reset();
    hdr_too_long_e_.reset();
    cnt_pkts_i_.reset();
    cnt_pkts_e_.reset();
    i_fwd_pkts_.reset();
    e_fwd_pkts_.reset();
    i_disc_pkts_.reset();
    e_disc_pkts_.reset();
    i_mirr_pkts_.reset();
    e_mirr_pkts_.reset();
    i_egr_pkt_err_.reset();
    e_egr_pkt_err_.reset();
    i_ctm_pkt_err_.reset();
    e_ctm_pkt_err_.reset();
    ctm_crc_err_i_.reset();
    ctm_crc_err_e_.reset();

    egress_unicast_port_[kIngress].reset();
    egress_unicast_port_[kEgress].reset();
    fde_pov_r_array_[kIngress].reset();
    fde_pov_r_array_[kEgress].reset();
    fde_phv_r_array_[kIngress].reset();
    fde_phv_r_array_[kEgress].reset();

    // Ingress-only registers.
    copy_to_cpu_.reset();
    copy_to_cpu_cos_.reset();
    copy_to_cpu_pv_.reset();
    ct_disable_mode_.reset();
    ct_mcast_mode_.reset();
    deflect_on_drop_.reset();
    drop_ctl_ingress_.reset();
    hash_lag_emcp_mcast_1_.reset();
    hash_lag_emcp_mcast_2_.reset();
    icos_.reset();
    meter_color_.reset();
    egress_multicast_group_1_.reset();
    egress_multicast_group_2_.reset();
    pvt_r_array_[0].reset();
    pvt_r_array_[1].reset();
    physical_ingress_port_.reset();
    qid_.reset();
    rid_.reset();
    use_yid_tbl_.reset();
    bypass_egr_mode_.reset();
    xid_.reset();
    xid_.reset();
    yid_.reset();

    // Resubmit registers.
    resubmit_cfg_.reset();
    resubmit_table_entry_r_array_.reset();

    // Egress-only registers.
    capture_tx_ts_.reset();
    drop_ctl_egress_.reset();
    ecos_.reset();
    force_tx_err_.reset();
    tx_pkt_has_ts_offsets_.reset();

    // Mirroring registers.
    mirror_cfg_[0].reset();
    mirror_cfg_[1].reset();
    mirror_table_entry_r_array_[0].reset();
    mirror_table_entry_r_array_[1].reset();
    e_coal_.reset();

    // Deparser PHV group registers.
    egress_only_phv8_group_.reset();
    egress_only_phv8_split_.reset();
    egress_only_phv16_group_.reset();
    egress_only_phv16_split_.reset();
    egress_only_phv32_group_.reset();
    egress_only_phv32_split_.reset();
    ingress_only_phv8_group_.reset();
    ingress_only_phv8_split_.reset();
    ingress_only_phv16_group_.reset();
    ingress_only_phv16_split_.reset();
    ingress_only_phv32_group_.reset();
    ingress_only_phv32_split_.reset();
  }

  uint32_t
  DeparserReg::get_fde_phv_pointer(bool egress, int index, int which) {
    RMT_ASSERT(index < Deparser::kNumFieldDictionaryEntries);
    const uint32_t phv_idx = fde_phv_r_array_[egress_flag_to_index(egress)].phv(
                               index, which);
    return phv_idx;
  }

  bool
  DeparserReg::CheckIngressDeparserPhvGroupConfig(const int &phv_idx) {
    RMT_ASSERT(phv_idx < Phv::phv_max_d());
    bool config = true;
    switch (Phv::which_width_d(phv_idx)) {
      case 8:
        if (is_split_phv_group(phv_idx)) {
          const auto split_offset = 1 << (phv_idx & 0x07);
          config = ((ingress_only_phv8_split_.val() & split_offset) != 0);
        }
        else {
          config = CheckDeparserPhvGroupConfig(phv_idx,
                                               ingress_only_phv8_group_.val());
        }
        break;
      case 16:
        if (is_split_phv_group(phv_idx)) {
          const auto split_offset = 1 << (phv_idx & 0x07);
          config = ((ingress_only_phv16_split_.val() & split_offset) != 0);
        }
        else {
          config = CheckDeparserPhvGroupConfig(phv_idx,
                                               ingress_only_phv16_group_.val());
        }
        break;
      case 32:
        if (is_split_phv_group(phv_idx)) {
          const auto split_offset = 1 << (phv_idx & 0x03);
          config = ((ingress_only_phv32_split_.val() & split_offset) != 0);
        }
        else {
          config = CheckDeparserPhvGroupConfig(phv_idx,
                                               ingress_only_phv32_group_.val());
        }
        break;
      default:
        RMT_ASSERT(false);
    }
    return config;
  }

  bool
  DeparserReg::CheckEgressDeparserPhvGroupConfig(const int &phv_idx) {
    RMT_ASSERT(phv_idx < Phv::phv_max_d());
    bool config = true;
    switch (Phv::which_width_d(phv_idx)) {
      case 8:
        if (is_split_phv_group(phv_idx)) {
          const auto split_offset = 1 << (phv_idx & 0x07);
          config = ((egress_only_phv8_split_.val() & split_offset) != 0);
        }
        else {
          config = CheckDeparserPhvGroupConfig(phv_idx,
                                               egress_only_phv8_group_.val());
        }
        break;
      case 16:
        if (is_split_phv_group(phv_idx)) {
          const auto split_offset = 1 << (phv_idx & 0x07);
          config = ((egress_only_phv16_split_.val() & split_offset) != 0);
        }
        else {
          config = CheckDeparserPhvGroupConfig(phv_idx,
                                               egress_only_phv16_group_.val());
        }
        break;
      case 32:
        if (is_split_phv_group(phv_idx)) {
          const auto split_offset = 1 << (phv_idx & 0x03);
          config = ((egress_only_phv32_split_.val() & split_offset) != 0);
        }
        else {
          config = CheckDeparserPhvGroupConfig(phv_idx,
                                               egress_only_phv32_group_.val());
        }
        break;
      default:
        RMT_ASSERT(false);
    }
    return config;
  }

  bool
  DeparserReg::CheckDeparserPhvGroupConfig(const int &phv_idx,
                                           const uint16_t &val) {
    const auto group = phv_group(phv_idx);
    return ((val & (1 << group)) == 0) ? false : true;
  }

  bool
  DeparserReg::is_split_phv_group(const int &phv_idx) {
    bool is_split = false;
    if ((phv_idx >= 216 && phv_idx < 224) || // 16b split PHV group range
        (phv_idx >= 120 && phv_idx < 128) || // 8b split PHV group range
        (phv_idx >= 60 && phv_idx < 64)) // 32b split PHV group range
      is_split = true;
    return is_split;
  }

  int
  DeparserReg::phv_group(const int &phv_idx) {
    int group = -1;
    switch (Phv::which_width_d(phv_idx)) {
      case 8:
        group = (phv_idx - 64) >> 3;
        break;
      case 16:
        group = (phv_idx - 128) >> 3;
        break;
      case 32:
        group = phv_idx >> 2;
        break;
    }
    RMT_ASSERT (group >= 0);
    return group;
  }

  int DeparserReg::phv_idx(const int &csum_idx, int *shift) {
    (*shift) = csum_idx_to_shift[csum_idx];
    return csum_idx_to_phv_idx[csum_idx];
  }

// TphvCsumCfgWriteCallback and PhvCsumCfgWriteCallback are needed because
//   although it appears in from the registers that there are 12 checksum
//   units (6 ingress and 6 egress) there are only 8 in the hardware,
//   2 ingress, 2 egress and 4 shared. The 2 ingress and 2 egress
//   appear as index 0 and 1, while the 4 shared are index 2,3,4 and 5.
//  In the model there still are 12 units, but if you write to one of the
//   shared ones' ingress cfg we copy it to egress (and vice versa)
// Note: these checksum registers are write only, so we can't use
//  the read() function on them, so have to copy each field across
//  individually.
  void
  DeparserReg::TphvCsumCfgWriteCallback(int src_idx, int dst_idx,
                                        int csum_engine_idx) {
    RMT_ASSERT(csum_engine_idx < Deparser::kNumChecksumEngines);
    if (csum_engine_idx > 1) {
      for (int csum_cfg_entry = 0;
           csum_cfg_entry < Deparser::kNumTagalongPHVChecksumCfgEntries;
           ++csum_cfg_entry) {
        auto& src = tphv_csum_cfg_entry_array_[src_idx].csum_cfg_csum_cfg_entry(
            csum_engine_idx, csum_cfg_entry);
        auto& dst = tphv_csum_cfg_entry_array_[dst_idx].csum_cfg_csum_cfg_entry(
            csum_engine_idx, csum_cfg_entry);
        dst.zero_m_s_b() = src.zero_m_s_b();
        dst.zero_l_s_b() = src.zero_l_s_b();
        dst.swap()       = src.swap();
      }
    }
  }

  void
  DeparserReg::PhvCsumCfgWriteCallback(int src_idx, int dst_idx,
                                       int csum_engine_idx) {
    RMT_ASSERT(csum_engine_idx < Deparser::kNumChecksumEngines);
    if (csum_engine_idx > 1) {
      for (int csum_cfg_entry = 0;
           csum_cfg_entry < Deparser::kNumPHVChecksumCfgEntries;
           ++csum_cfg_entry) {
        auto& src = phv_csum_cfg_entry_array_[src_idx].csum_cfg_csum_cfg_entry(
            csum_engine_idx, csum_cfg_entry);
        auto& dst = phv_csum_cfg_entry_array_[dst_idx].csum_cfg_csum_cfg_entry(
            csum_engine_idx, csum_cfg_entry);
        dst.zero_m_s_b() = src.zero_m_s_b();
        dst.zero_l_s_b() = src.zero_l_s_b();
        dst.swap()       = src.swap();
      }
    }
  }

}
