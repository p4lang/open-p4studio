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

#ifndef _TOFINOXX_DEPARSER_REG_
#define _TOFINOXX_DEPARSER_REG_

#include <cstdint>
#include <array>
#include <vector>
#include <boost/integer/static_log2.hpp>
#include <deparser-metadata.h>
#include <rmt-log.h>
#include <deparser-chip-reg.h>
#include <common/rmt-util.h>
#include <register_includes/dprsr_phv_csum_cfg_entry_g_array.h>
#include <register_includes/dprsr_tphv_csum_cfg_entry_g_array.h>
#include <register_includes/dprsr_pov_position_r.h>
#include <register_includes/dprsr_egress_unicast_port.h>
#include <register_includes/dprsr_capture_tx_ts_r.h>
#include <register_includes/dprsr_copy_to_c_p_u_r.h>
#include <register_includes/dprsr_copy_to_cpu_cos_r.h>
#include <register_includes/dprsr_copy_to_c_p_u_p_v_r.h>
#include <register_includes/dprsr_ct_disable_mode_r.h>
#include <register_includes/dprsr_ct_mcast_mode_r.h>
#include <register_includes/dprsr_deflect_on_drop_r.h>
#include <register_includes/dprsr_drop_ctl_r.h>
#include <register_includes/dprsr_ecos_r.h>
#include <register_includes/dprsr_egress_multicast_group_r.h>
#include <register_includes/dprsr_force_tx_err_r.h>
#include <register_includes/dprsr_hash_l_a_g_e_c_m_p_mcast_r.h>
#include <register_includes/dprsr_fde_pov_r_array.h>
#include <register_includes/dprsr_fde_phv_r_array.h>
#include <register_includes/dprsr_icos_r.h>
#include <register_includes/dprsr_input_egress_only_g.h>
#include <register_includes/dprsr_input_ingress_only_g.h>
#include <register_includes/dprsr_meter_color_r.h>
#include <register_includes/dprsr_mirror_cfg_r.h>
#include <register_includes/dprsr_mirror_table_entry_r_array.h>
#include <register_includes/dprsr_pvt_r_array.h>
#include <register_includes/dprsr_physical_ingress_port_r.h>
#include <register_includes/dprsr_resubmit_cfg_r.h>
#include <register_includes/dprsr_resubmit_table_entry_r_array.h>
#include <register_includes/dprsr_qid_r.h>
#include <register_includes/dprsr_tx_pkt_has_ts_offsets_r.h>
#include <register_includes/dprsr_rid_r.h>
#include <register_includes/dprsr_use_yid_tbl_r.h>
#include <register_includes/dprsr_bypass_egr_mode_r.h>
#include <register_includes/dprsr_xid_r.h>
#include <register_includes/dprsr_yid_r.h>
#include <register_includes/dprsr_e_coal_r.h>
#include <register_includes/dprsr_cnt_vld_i_phv_mutable.h>
#include <register_includes/dprsr_cnt_vld_i_tphv_mutable.h>
#include <register_includes/dprsr_cnt_i_dec_resubmit_mutable.h>
#include <register_includes/dprsr_cnt_i_dec_read_mutable.h>
#include <register_includes/dprsr_cnt_i_dec_discard_mutable.h>
#include <register_includes/dprsr_cnt_i_dec_learn_mutable.h>
#include <register_includes/dprsr_ii_regs_hdr_too_long_i_mutable.h>
#include <register_includes/dprsr_ie_regs_hdr_too_long_e_mutable.h>
#include <register_includes/dprsr_cnt_oi_pkts_mutable.h>
#include <register_includes/dprsr_cnt_oe_pkts_mutable.h>
#include <register_includes/dprsr_oi_regs_i_egr_pkt_err.h>
#include <register_includes/dprsr_oe_regs_e_egr_pkt_err.h>
#include <register_includes/dprsr_oi_regs_i_ctm_pkt_err.h>
#include <register_includes/dprsr_oe_regs_e_ctm_pkt_err.h>
#include <register_includes/dprsr_oi_regs_i_fwd_pkts_mutable.h>
#include <register_includes/dprsr_oe_regs_e_fwd_pkts_mutable.h>
#include <register_includes/dprsr_oi_regs_i_disc_pkts_mutable.h>
#include <register_includes/dprsr_oe_regs_e_disc_pkts_mutable.h>
#include <register_includes/dprsr_oi_regs_i_mirr_pkts_mutable.h>
#include <register_includes/dprsr_oe_regs_e_mirr_pkts_mutable.h>
#include <register_includes/dprsr_oi_regs_ctm_crc_err_array.h>
#include <register_includes/dprsr_oe_regs_ctm_crc_err_array.h>

namespace MODEL_CHIP_NAMESPACE {

using namespace model_common;

class DeparserReg {

 public:
  enum IngressEgressIndexEnum {
    kIngress = 0,
    kEgress };

  DeparserReg(int chip, int pipe);
  virtual ~DeparserReg();
  void Reset();

  uint32_t get_pov_pos(bool egress, int index) {
    // RMT_ASSERT(index < );
    return pov_position_[egress_flag_to_index(egress)].phvs(index);
  }

  std::tuple<bool, bool, bool>
  GetChecksumEntry(const bool &tagalong, const int &engine, const int &index) {
    auto entry = get_csum_entry(tagalong, engine, index);
    RMT_ASSERT(entry);
    const bool swap_bytes = to_bool_(entry->swap());
    const bool zero_msb = to_bool_(entry->zero_m_s_b());
    const bool zero_lsb = to_bool_(entry->zero_l_s_b());
    return std::make_tuple(swap_bytes, zero_msb, zero_lsb);
  }

  bool
  zeros_as_ones(int engine_idx) {
    int i, engine_idx_offset;
    get_csum_array_index(engine_idx, &i, &engine_idx_offset);
    return (0 != tphv_csum_cfg_entry_array_.at(i).csum_cfg_zeros_as_ones(engine_idx_offset).en());
  }

  uint8_t get_fde_pov_sel(bool egress, int index) {
    // TODO RMT_ASSERT(index < );
    return fde_pov_r_array_[egress_flag_to_index(egress)].pov_sel(index);
  }
  uint32_t get_fde_version(bool egress, int index) {
    // TODO RMT_ASSERT(index < );
    return fde_pov_r_array_[egress_flag_to_index(egress)].version(index);
  }
  int get_fde_num_bytes(bool egress, int index, bool *is_consistent) {
    // TODO RMT_ASSERT(index < );
    if ( fde_pov_r_array_[egress_flag_to_index(egress)].num_bytes(index) !=
         fde_phv_r_array_[egress_flag_to_index(egress)].num_bytes(index) ) {
      (*is_consistent) = false;
    }
    else {
      (*is_consistent) = true;
    }
    switch ( fde_pov_r_array_[egress_flag_to_index(egress)].num_bytes(index) ) {
      case 0: return 4;
      case 1: return 1;
      case 2: return 2;
      case 3: return 3;
      default: RMT_ASSERT(0);
    }
  }
  uint32_t get_fde_valid(bool egress, int index) {
    // TODO RMT_ASSERT(index < );
    return fde_pov_r_array_[egress_flag_to_index(egress)].valid(index);
  }

  uint32_t get_fde_phv_pointer(bool egress, int index, int which);

  // These functions return true if the PHV is assigned to the corresponding
  // gress. They also check that the PHV is not assigned to the other gress.
  bool
  CheckEgressDeparserPhvGroupConfig(const int &phv_idx);
  bool
  CheckIngressDeparserPhvGroupConfig(const int &phv_idx);

  void get_egress_unicast_port_info (bool egress, uint8_t *phv_valid, uint32_t *phv_value, uint8_t *default_valid, uint32_t *default_value) {
    // TODO RMT_ASSERT(index < );
    auto& reg = egress_unicast_port_[egress_flag_to_index(egress)];
    *default_valid = reg.dflt_vld();
    *default_value = reg.dflt_value();
    *phv_valid = reg.valid();
    *phv_value = reg.phv();
  }

  DeparserMetadataInfo<1> get_copy_to_cpu_info() {
    auto metadata_info = get_metadata_info<register_classes::DprsrCopyToCPUR, 1> (copy_to_cpu_);
    metadata_info.set_shift(copy_to_cpu_.shft());
    return metadata_info;
  }

  DeparserMetadataInfo< RmtDefs::kDeparserCopyToCpuCosWidth > get_copy_to_cpu_cos_info() {
    auto metadata_info = get_metadata_info<register_classes::DprsrCopyToCpuCosR, RmtDefs::kDeparserCopyToCpuCosWidth> (copy_to_cpu_cos_);
    metadata_info.set_shift(copy_to_cpu_cos_.shft());
    return metadata_info;
  }

  uint8_t get_copy_to_cpu_pipe_vector() {
    return copy_to_cpu_pv_.pipe_vec();
  }

  DeparserMetadataInfo<1> get_ct_disable_mode_info() {
    auto metadata_info = get_metadata_info<register_classes::DprsrCtDisableModeR, 1> (ct_disable_mode_);
    metadata_info.set_shift(ct_disable_mode_.shft());
    return metadata_info;
  }

  DeparserMetadataInfo<1> get_ct_mcast_mode_info() {
    auto metadata_info = get_metadata_info<register_classes::DprsrCtMcastModeR, 1> (ct_mcast_mode_);
    metadata_info.set_shift(ct_mcast_mode_.shft());
    return metadata_info;
  }

  DeparserMetadataInfo<1> get_deflect_on_drop_info() {
    auto metadata_info = get_metadata_info<register_classes::DprsrDeflectOnDropR, 1> (deflect_on_drop_);
    metadata_info.set_shift(deflect_on_drop_.shft());
    return metadata_info;
  }

  DeparserMetadataInfo< RmtDefs::kDeparserDropCtlWidth > get_ingress_drop_ctl_info() {
    auto metadata_info = get_metadata_info<register_classes::DprsrDropCtlR, RmtDefs::kDeparserDropCtlWidth> (drop_ctl_ingress_);
    metadata_info.set_shift(drop_ctl_ingress_.shft());
    return metadata_info;
  }

  DeparserMetadataInfo< RmtDefs::kDeparserEgressUnicastPortWidth > get_egress_unicast_port_info(IngressEgressIndexEnum index) {
    return get_metadata_info_with_default_valid<register_classes::DprsrEgressUnicastPort, RmtDefs::kDeparserEgressUnicastPortWidth> (egress_unicast_port_[index]);
  }

  DeparserMetadataInfo< RmtDefs::kDeparserHashLAGECMPMcastWidth > get_hash_lag_emcp_mcast_1_info() {
    return get_metadata_info<register_classes::DprsrHashLAGECMPMcastR, RmtDefs::kDeparserHashLAGECMPMcastWidth> (hash_lag_emcp_mcast_1_);
  }

  DeparserMetadataInfo< RmtDefs::kDeparserHashLAGECMPMcastWidth > get_hash_lag_emcp_mcast_2_info() {
    return get_metadata_info<register_classes::DprsrHashLAGECMPMcastR, RmtDefs::kDeparserHashLAGECMPMcastWidth> (hash_lag_emcp_mcast_2_);
  }

  DeparserMetadataInfo< RmtDefs::kDeparserIcosWidth > get_icos_info() {
    auto metadata_info = get_metadata_info<register_classes::DprsrIcosR, RmtDefs::kDeparserIcosWidth> (icos_);
    metadata_info.set_shift(icos_.shft());
    return metadata_info;
  }

  DeparserMetadataInfo< RmtDefs::kDeparserMeterColorWidth > get_meter_color_info() {
    auto metadata_info = get_metadata_info<register_classes::DprsrMeterColorR, RmtDefs::kDeparserMeterColorWidth> (meter_color_);
    metadata_info.set_shift(meter_color_.shft());
    return metadata_info;
  }

  DeparserMetadataInfo< RmtDefs::kDeparserEgressMulticastGroupWidth > get_egress_multicast_group_1_info() {
    return get_metadata_info_with_default_valid<register_classes::DprsrEgressMulticastGroupR, RmtDefs::kDeparserEgressMulticastGroupWidth> (egress_multicast_group_1_);
  }

  DeparserMetadataInfo< RmtDefs::kDeparserEgressMulticastGroupWidth > get_egress_multicast_group_2_info() {
    return get_metadata_info_with_default_valid<register_classes::DprsrEgressMulticastGroupR, RmtDefs::kDeparserEgressMulticastGroupWidth> (egress_multicast_group_2_);
  }

  uint8_t get_multicast_pipe_vector(uint32_t table_num, uint16_t index) {
    constexpr int kPortVectorWidth = RmtDefs::kDeparserPortVectorWidth;
    static_assert( kPortVectorWidth <= 8, "Port vector width max is 8 bits");
    constexpr int kWordIndexBits   = 5 - boost::static_log2< kPortVectorWidth >::value;
    constexpr int kWordIndexMask   = (1 << kWordIndexBits) - 1;
    constexpr int kEntryMask       = (1 << kPortVectorWidth) - 1;
    // Mask off the top of the index.  Eg in tofino the pipe vector table is actually
    // only 32768 entries and "wraps around" so that both index 0 and 0x8000
    // refer to the same entry.
    // Futhermore the base address of table 1 is wrong.  When the two tables were
    // halved the base address of table 1 shifted down to where the upper half of
    // table 0 was.  Since none of these changes are reflected in the CSRs the base
    // address and size of these register arrays are in correct in the model.  To
    // work around that, when attempting to read table 1, just look in the top half
    // of table 0 instead.
    index &= (RmtDefs::kDeparserMulticastGroupIds - 1);
    uint32_t tbl_index = index >> kWordIndexBits;
    if (table_num) tbl_index += 4096;
    uint32_t entry = pvt_r_array_[0].entry(tbl_index);
    return (entry >> ((index & kWordIndexMask) * kPortVectorWidth )) & kEntryMask;
  }

  std::tuple<uint8_t, bool> get_physical_ingress_port_info() {
    return std::make_tuple(physical_ingress_port_.phv(), (physical_ingress_port_.sel() != 0) ? true : false);
  }

  DeparserMetadataInfo< RmtDefs::kDeparserQidWidth > get_qid_info() {
    auto metadata_info = get_metadata_info<register_classes::DprsrQidR, RmtDefs::kDeparserQidWidth> (qid_);
    metadata_info.set_shift(qid_.shft());
    return metadata_info;
  }

  DeparserMetadataInfo< RmtDefs::kDeparserRidWidth > get_rid_info() {
    return get_metadata_info<register_classes::DprsrRidR, RmtDefs::kDeparserRidWidth> (rid_);
  }

  bool get_use_yid_tbl() {
    return (use_yid_tbl_.sel() != 0) ? true : false;
  }

  DeparserMetadataInfo<1> get_bypass_egr_mode_info() {
    auto metadata_info = get_metadata_info<register_classes::DprsrBypassEgrModeR, 1> (bypass_egr_mode_);
    metadata_info.set_shift(bypass_egr_mode_.shft());
    return metadata_info;
  }

  DeparserMetadataInfo< RmtDefs::kDeparserXidWidth > get_xid_info() {
    return get_metadata_info<register_classes::DprsrXidR, RmtDefs::kDeparserXidWidth> (xid_);
  }

  DeparserMetadataInfo< RmtDefs::kDeparserYidWidth > get_yid_info() {
    return get_metadata_info<register_classes::DprsrYidR, RmtDefs::kDeparserYidWidth> (yid_);
  }

  uint8_t get_ipipe_remap() {
    return deparser_chip_reg_.get_ipipe_remap();
  }

  uint8_t get_epipe_remap() {
    return deparser_chip_reg_.get_epipe_remap();
  }

  // Getters for egress metadata configuration.
  DeparserMetadataInfo<1> get_capture_tx_ts_info() {
    auto metadata_info = get_metadata_info<register_classes::DprsrCaptureTxTsR, 1> (capture_tx_ts_);
    metadata_info.set_shift(capture_tx_ts_.shft());
    return metadata_info;
  }

  DeparserMetadataInfo< RmtDefs::kDeparserEcosWidth > get_ecos_info() {
    auto metadata_info = get_metadata_info<register_classes::DprsrEcosR, RmtDefs::kDeparserEcosWidth> (ecos_);
    metadata_info.set_shift(ecos_.shft());
    return metadata_info;
  }

  DeparserMetadataInfo< RmtDefs::kDeparserDropCtlWidth > get_egress_drop_ctl_info() {
    auto metadata_info = get_metadata_info<register_classes::DprsrDropCtlR, RmtDefs::kDeparserDropCtlWidth> (drop_ctl_egress_);
    metadata_info.set_shift(drop_ctl_egress_.shft());
    return metadata_info;
  }

  DeparserMetadataInfo<1> get_force_tx_err_info() {
    auto metadata_info = get_metadata_info<register_classes::DprsrForceTxErrR, 1> (force_tx_err_);
    metadata_info.set_shift(force_tx_err_.shft());
    return metadata_info;
  }

  DeparserMetadataInfo<1> get_tx_pkt_has_ts_offsets_info() {
    auto metadata_info = get_metadata_info<register_classes::DprsrTxPktHasTsOffsetsR, 1> (tx_pkt_has_ts_offsets_);
    metadata_info.set_shift(tx_pkt_has_ts_offsets_.shft());
    return metadata_info;
  }

  // Getters for mirror registers.
  void get_mirror_cfg(const bool &egress, uint8_t *phv, bool *valid, uint8_t *shift) {
    (*phv) = mirror_cfg_[egress_flag_to_index(egress)].phv();
    (*valid) = mirror_cfg_[egress_flag_to_index(egress)].valid();
    (*shift) = mirror_cfg_[egress_flag_to_index(egress)].shft();
  }

  void get_mirror_table_entry(const bool &egress,
                              const uint8_t mirror_table_idx, bool *valid,
                              std::vector<uint8_t> *phv_idx_list,
                              uint8_t *mirror_id_phv_idx) {
    RMT_ASSERT(mirror_table_idx < 8);
    register_classes::DprsrMirrorTableEntryRArray &mirror_table_entry_array = mirror_table_entry_r_array_[egress_flag_to_index(egress)];
    (*valid) = (mirror_table_entry_array.valid(mirror_table_idx) == 0 ? false : true);
    for (int i = 0; i < mirror_table_entry_array.len(mirror_table_idx); ++i) {
      phv_idx_list->push_back(mirror_table_entry_array.phvs(mirror_table_idx, i));
    }
    (*mirror_id_phv_idx) = mirror_table_entry_array.id_phv(mirror_table_idx);
  }

  void get_resubmit_cfg(uint8_t *phv_idx, bool *valid, uint8_t *shift) {
    (*valid) = (resubmit_cfg_.valid() == 0 ? false : true);
    (*phv_idx) = resubmit_cfg_.phv();
    (*shift) = resubmit_cfg_.shft();
  }

  void get_resubmit_table_entry(const uint8_t resubmit_table_idx, bool *valid,
                                std::vector<uint8_t> *phv_idx_list) {
    RMT_ASSERT(resubmit_table_idx < 8);
    (*valid) = (resubmit_table_entry_r_array_.valid(resubmit_table_idx) == 0 ? false : true);
    for (int i = 0; i < resubmit_table_entry_r_array_.len(resubmit_table_idx); ++i) {
      phv_idx_list->push_back(resubmit_table_entry_r_array_.phvs(resubmit_table_idx, i));
    }
  }

  // This function takes the checksum index and returns the PHV index. Due to
  // PD constraints, there is a weird mapping from checksum index to PHV
  // index.
  int phv_idx(const int &csum_idx, int *shift);

  uint8_t e_coal_len() { return e_coal_.dflt_value(); }
  uint8_t e_coal_phv() { return e_coal_.phv(); }
  bool    e_coal_phv_valid() { return e_coal_.valid(); }

  // Checksum engines are numbered [0, 11]. This function decodes the pipe
  // (ingress or egress) and checksum engine index within the pipe from the
  // engine index.
  void
  get_csum_array_index(const int &engine_idx, int *array_index,
                       int *engine_idx_offset) const {
    *engine_idx_offset = engine_idx;
    // Each checksum cfg array has 6 elements.
    const int num_engines_in_array_element = 6;
    (*array_index) = 0;
    if (engine_idx >= num_engines_in_array_element) {
      (*engine_idx_offset) -= num_engines_in_array_element;
      ++(*array_index);
    }
  }

  DeparserChipReg& chip_reg() { return deparser_chip_reg_; }

  void increment_phv_counter(bool egress) {
    auto& counter = egress ? cnt_vld_i_phv_[1] : cnt_vld_i_phv_[0];
    counter.ctr48(Util::increment_and_wrap(counter.ctr48(), 48));
  }
  void increment_tphv_counter(bool egress) {
    auto& counter = egress ? cnt_vld_i_tphv_[1] : cnt_vld_i_tphv_[0];
    counter.ctr48(Util::increment_and_wrap(counter.ctr48(), 48));
  }
  void increment_resubmit_counter() {
    cnt_i_dec_resubmit_.ctr48(
        Util::increment_and_wrap(cnt_i_dec_resubmit_.ctr48(), 48));
  }
  void increment_read_counter() {
    cnt_i_dec_read_.ctr48(
        Util::increment_and_wrap(cnt_i_dec_read_.ctr48(), 48));
  }
  void increment_discard_counter() {
    cnt_i_dec_discard_.ctr48(
        Util::increment_and_wrap(cnt_i_dec_discard_.ctr48(), 48));
  }
  void increment_learn_counter() {
    cnt_i_dec_learn_.ctr48(
        Util::increment_and_wrap(cnt_i_dec_learn_.ctr48(), 48));
  }
  void increment_hdr_too_long(bool egress) {
    egress ?
    hdr_too_long_e_.ctr32(Util::increment_and_wrap(hdr_too_long_e_.ctr32(), 32)) :
    hdr_too_long_i_.ctr32(Util::increment_and_wrap(hdr_too_long_i_.ctr32(), 32));
  }
  void increment_cnt_pkts(bool egress) {
    egress ?
    cnt_pkts_e_.ctr48(Util::increment_and_wrap(cnt_pkts_e_.ctr48(), 48)) :
    cnt_pkts_i_.ctr48(Util::increment_and_wrap(cnt_pkts_i_.ctr48(), 48));
  }
  void increment_fwd_pkts(bool egress) {
    egress ?
    e_fwd_pkts_.ctr48(Util::increment_and_wrap(e_fwd_pkts_.ctr48(), 48)) :
    i_fwd_pkts_.ctr48(Util::increment_and_wrap(i_fwd_pkts_.ctr48(), 48));
  }
  void increment_disc_pkts(bool egress) {
    egress ?
    e_disc_pkts_.ctr48(Util::increment_and_wrap(e_disc_pkts_.ctr48(), 48)) :
    i_disc_pkts_.ctr48(Util::increment_and_wrap(i_disc_pkts_.ctr48(), 48));
  }
  void increment_mirr_pkts(bool egress) {
    egress ?
    e_mirr_pkts_.ctr48(Util::increment_and_wrap(e_mirr_pkts_.ctr48(), 48)) :
    i_mirr_pkts_.ctr48(Util::increment_and_wrap(i_mirr_pkts_.ctr48(), 48));
  }

 private:
  DeparserChipReg              deparser_chip_reg_; // chip specific registers


  int egress_flag_to_index(bool egress) { return egress ? kEgress : kIngress; }
  std::array< register_classes::DprsrPhvCsumCfgEntryGArray, 2 >  phv_csum_cfg_entry_array_;
  std::array< register_classes::DprsrTphvCsumCfgEntryGArray, 2 > tphv_csum_cfg_entry_array_;

  std::array< register_classes::DprsrPovPositionR, 2 > pov_position_;
  std::array< register_classes::DprsrEgressUnicastPort, 2 > egress_unicast_port_;

  // counters
  std::array< register_classes::DprsrCntVldIPhvMutable, 2 >  cnt_vld_i_phv_;  // ingress, egress
  std::array< register_classes::DprsrCntVldITphvMutable, 2 > cnt_vld_i_tphv_;  // ingress, egress
  register_classes::DprsrCntIDecResubmitMutable    cnt_i_dec_resubmit_;  // ingress
  register_classes::DprsrCntIDecReadMutable        cnt_i_dec_read_;      // ingress
  register_classes::DprsrCntIDecDiscardMutable     cnt_i_dec_discard_;   // ingress
  register_classes::DprsrCntIDecLearnMutable       cnt_i_dec_learn_;     // ingress
  register_classes::DprsrIiRegsHdrTooLongIMutable  hdr_too_long_i_;      // ingress
  register_classes::DprsrIeRegsHdrTooLongEMutable  hdr_too_long_e_;      // egress
  register_classes::DprsrCntOiPktsMutable          cnt_pkts_i_;          // ingress
  register_classes::DprsrCntOePktsMutable          cnt_pkts_e_;          // egress
  register_classes::DprsrOiRegsIFwdPktsMutable     i_fwd_pkts_;          // ingress
  register_classes::DprsrOeRegsEFwdPktsMutable     e_fwd_pkts_;          // egress
  register_classes::DprsrOiRegsIDiscPktsMutable    i_disc_pkts_;         // ingress
  register_classes::DprsrOeRegsEDiscPktsMutable    e_disc_pkts_;         // egress
  register_classes::DprsrOiRegsIMirrPktsMutable    i_mirr_pkts_;         // ingress
  register_classes::DprsrOeRegsEMirrPktsMutable    e_mirr_pkts_;         // egress
  register_classes::DprsrOiRegsIEgrPktErr          i_egr_pkt_err_;       // ingress
  register_classes::DprsrOeRegsEEgrPktErr          e_egr_pkt_err_;       // egress
  register_classes::DprsrOiRegsICtmPktErr          i_ctm_pkt_err_;       // ingress
  register_classes::DprsrOeRegsECtmPktErr          e_ctm_pkt_err_;       // egress
  register_classes::DprsrOiRegsCtmCrcErrArray      ctm_crc_err_i_;       // ingress
  register_classes::DprsrOeRegsCtmCrcErrArray      ctm_crc_err_e_;       // egress

  // Ingress deparser registers.
  register_classes::DprsrCopyToCPUR             copy_to_cpu_;
  register_classes::DprsrCopyToCpuCosR          copy_to_cpu_cos_;
  register_classes::DprsrCopyToCPUPVR           copy_to_cpu_pv_;
  register_classes::DprsrCtDisableModeR         ct_disable_mode_;
  register_classes::DprsrCtMcastModeR           ct_mcast_mode_;
  register_classes::DprsrDeflectOnDropR         deflect_on_drop_;
  register_classes::DprsrDropCtlR               drop_ctl_ingress_;
  register_classes::DprsrHashLAGECMPMcastR      hash_lag_emcp_mcast_1_;
  register_classes::DprsrHashLAGECMPMcastR      hash_lag_emcp_mcast_2_;
  register_classes::DprsrIcosR                  icos_;
  register_classes::DprsrMeterColorR            meter_color_;
  register_classes::DprsrEgressMulticastGroupR  egress_multicast_group_1_;
  register_classes::DprsrEgressMulticastGroupR  egress_multicast_group_2_;
  std::array<register_classes::DprsrPvtRArray, 2> pvt_r_array_;
  register_classes::DprsrPhysicalIngressPortR   physical_ingress_port_;
  register_classes::DprsrQidR                   qid_;
  register_classes::DprsrRidR                   rid_;
  register_classes::DprsrUseYidTblR             use_yid_tbl_;
  register_classes::DprsrBypassEgrModeR         bypass_egr_mode_;
  register_classes::DprsrXidR                   xid_;
  register_classes::DprsrYidR                   yid_;

  // Resubmit registers.
  register_classes::DprsrResubmitCfgR             resubmit_cfg_;
  register_classes::DprsrResubmitTableEntryRArray resubmit_table_entry_r_array_;

  // Egress deparser registers.
  register_classes::DprsrCaptureTxTsR           capture_tx_ts_;
  register_classes::DprsrDropCtlR               drop_ctl_egress_;
  register_classes::DprsrEcosR                  ecos_;
  register_classes::DprsrForceTxErrR            force_tx_err_;
  register_classes::DprsrTxPktHasTsOffsetsR     tx_pkt_has_ts_offsets_;

  // Mirroring registers.
  std::array< register_classes::DprsrMirrorCfgR, 2 >             mirror_cfg_;
  std::array< register_classes::DprsrMirrorTableEntryRArray, 2 > mirror_table_entry_r_array_;
  register_classes::DprsrECoalR                 e_coal_;    // Length control

  std::array< register_classes::DprsrFdePovRArray, 2 > fde_pov_r_array_;
  std::array< register_classes::DprsrFdePhvRArray, 2 > fde_phv_r_array_;

  // Deparser PHV group registers.
  register_classes::DprsrInputEgressOnlyGPhv8Grp     egress_only_phv8_group_;
  register_classes::DprsrInputEgressOnlyGPhv8Split   egress_only_phv8_split_;
  register_classes::DprsrInputEgressOnlyGPhv16Grp    egress_only_phv16_group_;
  register_classes::DprsrInputEgressOnlyGPhv16Split  egress_only_phv16_split_;
  register_classes::DprsrInputEgressOnlyGPhv32Grp    egress_only_phv32_group_;
  register_classes::DprsrInputEgressOnlyGPhv32Split  egress_only_phv32_split_;
  register_classes::DprsrInputIngressOnlyGPhv8Grp    ingress_only_phv8_group_;
  register_classes::DprsrInputIngressOnlyGPhv8Split  ingress_only_phv8_split_;
  register_classes::DprsrInputIngressOnlyGPhv16Grp   ingress_only_phv16_group_;
  register_classes::DprsrInputIngressOnlyGPhv16Split ingress_only_phv16_split_;
  register_classes::DprsrInputIngressOnlyGPhv32Grp   ingress_only_phv32_group_;
  register_classes::DprsrInputIngressOnlyGPhv32Split ingress_only_phv32_split_;

  // TODO: all the new registers for the new parser
  // These array store the checksum index to PHV index mapping. Element i of
  // csum_idx_to_phv_idx indicates the PHV index corresponding to checksum
  // index i.
  const std::vector<int> csum_idx_to_phv_idx;
  std::vector<int> csum_idx_to_shift;

  // The pipe (ingress or egress) is actually encoded in the engine_idx.
  // Checksum engines in ingress are numbered [0, 5] and egress are numbered
  // [6, 11].
  register_classes::DprsrCsumRowEntry*
  get_csum_entry(const bool &tagalong, const int &engine_idx,
                 const int &index) {
    int i, engine_idx_offset;
    get_csum_array_index(engine_idx, &i, &engine_idx_offset);
    if (tagalong) {
      return &tphv_csum_cfg_entry_array_.at(i).csum_cfg_csum_cfg_entry(engine_idx_offset, index);
    }
    else {
      return &phv_csum_cfg_entry_array_.at(i).csum_cfg_csum_cfg_entry(engine_idx_offset, index);
    }
  }

  // The following functions are needed to check deparser PHV group
  // configuration.
  bool
  is_split_phv_group(const int &phv_idx);
  int
  phv_group(const int &phv_idx);
  bool
  CheckDeparserPhvGroupConfig(const int &phv_idx, const uint16_t &val);

  bool
  to_bool_(const uint8_t &i) {
    return ((i == 0) ? false : true);
  }

  // In hardware, the some checksum configuration registers can be accessed
  // from two addresses. Those registers appear two times in the CSR. In the
  // model, we keep their values in sync by copying from one to the another
  // when either is updated.
  void
  TphvCsumCfgWriteCallback(int src_idx, int dst_idx, int csum_engine_idx);
  void
  PhvCsumCfgWriteCallback(int src_idx, int dst_idx, int csum_engine_idx);
};
}

#endif // _TOFINOXX_DEPARSER_REG_
