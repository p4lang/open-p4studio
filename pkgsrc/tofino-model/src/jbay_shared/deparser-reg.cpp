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
#include <rmt-object-manager.h>
#include <mcn_test.h>

// cog data is set up in include/jbay/deparser_metadata.py see include/jbay/deparser-reg.h running for details
//[[[cog
//   import sys
//   sys.path.append('../../include/jbay')
//   import deparser_metadata as metadata
//]]]
//[[[end]]] (checksum: d41d8cd98f00b204e9800998ecf8427e)

namespace MODEL_CHIP_NAMESPACE {

using Dfre = register_classes::DprsrFullcsumRowEntryArray;

FdChunkReg::FdChunkReg(int chip, int pipe, int slice, bool egress, int word) :
    word_index_(word),
    // this is no longer per slice, but keep it here for checking purposes
    fd_chunk_info_r_{{
                         {deparser_in_non_pp_adapter(fd_chunk_info_r_[0], chip, pipe, egress, (word*8) + 0 )},
                         {deparser_in_non_pp_adapter(fd_chunk_info_r_[1], chip, pipe, egress, (word*8) + 1 )},
                         {deparser_in_non_pp_adapter(fd_chunk_info_r_[2], chip, pipe, egress, (word*8) + 2 )},
                         {deparser_in_non_pp_adapter(fd_chunk_info_r_[3], chip, pipe, egress, (word*8) + 3 )},
                         {deparser_in_non_pp_adapter(fd_chunk_info_r_[4], chip, pipe, egress, (word*8) + 4 )},
                         {deparser_in_non_pp_adapter(fd_chunk_info_r_[5], chip, pipe, egress, (word*8) + 5 )},
                         {deparser_in_non_pp_adapter(fd_chunk_info_r_[6], chip, pipe, egress, (word*8) + 6 )},
                         {deparser_in_non_pp_adapter(fd_chunk_info_r_[7], chip, pipe, egress, (word*8) + 7 )} }},
    fd_byte_is_phv_r_{{
                          {deparser_in_hdr_adapter(fd_byte_is_phv_r_[0], chip, pipe, slice, egress, (word*8) + 0 )},
                          {deparser_in_hdr_adapter(fd_byte_is_phv_r_[1], chip, pipe, slice, egress, (word*8) + 1 )},
                          {deparser_in_hdr_adapter(fd_byte_is_phv_r_[2], chip, pipe, slice, egress, (word*8) + 2 )},
                          {deparser_in_hdr_adapter(fd_byte_is_phv_r_[3], chip, pipe, slice, egress, (word*8) + 3 )},
                          {deparser_in_hdr_adapter(fd_byte_is_phv_r_[4], chip, pipe, slice, egress, (word*8) + 4 )},
                          {deparser_in_hdr_adapter(fd_byte_is_phv_r_[5], chip, pipe, slice, egress, (word*8) + 5 )},
                          {deparser_in_hdr_adapter(fd_byte_is_phv_r_[6], chip, pipe, slice, egress, (word*8) + 6 )},
                          {deparser_in_hdr_adapter(fd_byte_is_phv_r_[7], chip, pipe, slice, egress, (word*8) + 7 )} }},
    fd_byte_off_info_r_{{
                            {deparser_in_hdr_adapter(fd_byte_off_info_r_[0], chip, pipe, slice, egress, (word*8) + 0 )},
                            {deparser_in_hdr_adapter(fd_byte_off_info_r_[1], chip, pipe, slice, egress, (word*8) + 1 )},
                            {deparser_in_hdr_adapter(fd_byte_off_info_r_[2], chip, pipe, slice, egress, (word*8) + 2 )},
                            {deparser_in_hdr_adapter(fd_byte_off_info_r_[3], chip, pipe, slice, egress, (word*8) + 3 )},
                            {deparser_in_hdr_adapter(fd_byte_off_info_r_[4], chip, pipe, slice, egress, (word*8) + 4 )},
                            {deparser_in_hdr_adapter(fd_byte_off_info_r_[5], chip, pipe, slice, egress, (word*8) + 5 )},
                            {deparser_in_hdr_adapter(fd_byte_off_info_r_[6], chip, pipe, slice, egress, (word*8) + 6 )},
                            {deparser_in_hdr_adapter(fd_byte_off_info_r_[7], chip, pipe, slice, egress, (word*8) + 7 )} }},
    fd_chunk_info_r_duplicate_{{
                                   {deparser_in_hdr_adapter(fd_chunk_info_r_duplicate_[0], chip, pipe, slice, egress, (word*8) + 0 )},
                                   {deparser_in_hdr_adapter(fd_chunk_info_r_duplicate_[1], chip, pipe, slice, egress, (word*8) + 1 )},
                                   {deparser_in_hdr_adapter(fd_chunk_info_r_duplicate_[2], chip, pipe, slice, egress, (word*8) + 2 )},
                                   {deparser_in_hdr_adapter(fd_chunk_info_r_duplicate_[3], chip, pipe, slice, egress, (word*8) + 3 )},
                                   {deparser_in_hdr_adapter(fd_chunk_info_r_duplicate_[4], chip, pipe, slice, egress, (word*8) + 4 )},
                                   {deparser_in_hdr_adapter(fd_chunk_info_r_duplicate_[5], chip, pipe, slice, egress, (word*8) + 5 )},
                                   {deparser_in_hdr_adapter(fd_chunk_info_r_duplicate_[6], chip, pipe, slice, egress, (word*8) + 6 )},
                                   {deparser_in_hdr_adapter(fd_chunk_info_r_duplicate_[7], chip, pipe, slice, egress, (word*8) + 7 )} }},

    // this is no longer per slice, but keep it here for checking purposes
    fd_word_clot_sel_info_r_{
        deparser_in_non_pp_adapter( fd_word_clot_sel_info_r_, chip, pipe, egress,  word  ) },

    fd_word_clot_sel_info_r_duplicate_{
        deparser_slice_r_adapter(fd_word_clot_sel_info_r_duplicate_,chip,pipe,slice,egress,word) }
{
  reset();
}

void FdChunkReg::reset() {
  for (int i=0;i<8;i++) {
    fd_chunk_info_r_[i].reset();
    fd_byte_is_phv_r_[i].reset();
    fd_byte_off_info_r_[i].reset();
    fd_chunk_info_r_duplicate_[i].reset();
  }
  fd_word_clot_sel_info_r_.reset();
  fd_word_clot_sel_info_r_duplicate_.reset();
}

Deparser::FdChunk FdChunkReg::get_chunk(int chunk) {
  RMT_ASSERT(chunk < 8);
  // these fields no longer exist in the duplicate
  //RMT_ASSERT( fd_chunk_info_r_duplicate_[chunk].chunk_vld() == fd_chunk_info_r_[chunk].chunk_vld() );
  //RMT_ASSERT( fd_chunk_info_r_duplicate_[chunk].pov()       == fd_chunk_info_r_[chunk].pov() );
  RMT_ASSERT( fd_chunk_info_r_duplicate_[chunk].seg_vld()   == fd_chunk_info_r_[chunk].seg_vld() );
  RMT_ASSERT( fd_chunk_info_r_duplicate_[chunk].seg_sel()   == fd_chunk_info_r_[chunk].seg_sel() );
  RMT_ASSERT( fd_chunk_info_r_duplicate_[chunk].seg_slice() == fd_chunk_info_r_[chunk].seg_slice() );

  Deparser::FdChunk fdc;
  fdc.valid     = fd_chunk_info_r_[chunk].chunk_vld();
  fdc.pov       = fd_chunk_info_r_[chunk].pov();
  fdc.seg_vld   = fd_chunk_info_r_[chunk].seg_vld();
  fdc.seg_sel   = fd_chunk_info_r_[chunk].seg_sel();
  fdc.seg_slice = fd_chunk_info_r_[chunk].seg_slice();
  // Note: if a chunk is used for PHV only (i.e. seg_vld == 0) then the length of the chunk is
  //   in {seg_sel[0], seg_slice[2:0]}.
  fdc.len = (fdc.seg_vld == 0) ? ((( (fdc.seg_sel) & 0x1 ) << 3) | ( (fdc.seg_slice) & 0x7 ) ) : 0;

  uint32_t is_phv =  fd_byte_is_phv_r_[chunk].is_phv();
  static_assert( Deparser::kChunksPerWord <= 32 , "kChunksPerWord must be less than 32" );
  for (int i=0; i<Deparser::kChunksPerWord; ++i) {
    fdc.is_phv[i]   = (is_phv>>i) & 1;
    fdc.byte_off[i] = fd_byte_off_info_r_[chunk].phv_offset(i);
  }
  fdc.index = (word_index_ * Deparser::kChunksPerWord) + chunk;
  return fdc;
}

uint8_t FdChunkReg::get_clot_sel_segment_tag(int index) {
  int tag = fd_word_clot_sel_info_r_.segment_tag(index);
  int tag_copy = fd_word_clot_sel_info_r_duplicate_.segment_tag(index);
  RMT_ASSERT( tag == tag_copy );
  return tag;
}

DeparserReg::EnginesCsumRows::EnginesCsumRows(int chip,int pipe) :  // for the one in inp
    csum_row_entry_array_{{
                              {deparser_in_hdr_adapter(csum_row_entry_array_[0], chip, pipe, 0 )},
                              {deparser_in_hdr_adapter(csum_row_entry_array_[1], chip, pipe, 1 )},
                              {deparser_in_hdr_adapter(csum_row_entry_array_[2], chip, pipe, 2 )},
                              {deparser_in_hdr_adapter(csum_row_entry_array_[3], chip, pipe, 3 )},
                              {deparser_in_hdr_adapter(csum_row_entry_array_[4], chip, pipe, 4 )},
                              {deparser_in_hdr_adapter(csum_row_entry_array_[5], chip, pipe, 5 )},
                              {deparser_in_hdr_adapter(csum_row_entry_array_[6], chip, pipe, 6 )},
                              {deparser_in_hdr_adapter(csum_row_entry_array_[7], chip, pipe, 7 )} }}
{
  reset();
}

void DeparserReg::EnginesCsumRows::reset() {
  for (int i=0;i<8;++i) {
    csum_row_entry_array_[i].reset();
  }
}

DeparserReg::DeparserReg(RmtObjectManager *om, int chip, int pipe) :
    PipeObject(om, pipe),
    ctor_running_(true),
    deparser_chip_reg_(chip, pipe),
    pov_position_{{
      {deparser_in_hdr_adapter(pov_position_[0], chip, pipe, register_classes::DprsrPovPositionR::kMainI )},
      {deparser_in_hdr_adapter(pov_position_[1], chip, pipe, register_classes::DprsrPovPositionR::kMainE )}  }},
    cnt_vld_i_phv_{{
      {deparser_in_hdr_adapter(cnt_vld_i_phv_[0], chip, pipe, register_classes::DprsrCntVldIPhvMutable::kMainI )},
      {deparser_in_hdr_adapter(cnt_vld_i_phv_[1], chip, pipe, register_classes::DprsrCntVldIPhvMutable::kMainE )}  }},

    pp_ctr_cfg_data_{{
      {deparser_in_hdr_adapter(pp_ctr_cfg_data_[0], chip, pipe, register_classes::DprsrInputIngAndEgrGPpCtrCfgData::kMainI )},
      {deparser_in_hdr_adapter(pp_ctr_cfg_data_[1], chip, pipe, register_classes::DprsrInputIngAndEgrGPpCtrCfgData::kMainE )}  }},
    pp_ctr_cfg_mask_{{
      {deparser_in_hdr_adapter(pp_ctr_cfg_mask_[0], chip, pipe, register_classes::DprsrInputIngAndEgrGPpCtrCfgMask::kMainI )},
      {deparser_in_hdr_adapter(pp_ctr_cfg_mask_[1], chip, pipe, register_classes::DprsrInputIngAndEgrGPpCtrCfgMask::kMainE )}  }},
    pp_ctr_cfg48_r_{{
      {deparser_in_hdr_adapter(pp_ctr_cfg48_r_[0], chip, pipe, register_classes::PpCtrCfg48RMutable::kMainI )},
      {deparser_in_hdr_adapter(pp_ctr_cfg48_r_[1], chip, pipe, register_classes::PpCtrCfg48RMutable::kMainE )}  }},

    //[[[cog cog.out(metadata.init_list) ]]]
    m_learn_sel_{deparser_in_hdr_adapter(m_learn_sel_, chip, pipe )},
    m_resub_sel_{deparser_in_hdr_adapter(m_resub_sel_, chip, pipe )},
    m_pgen_{deparser_in_hdr_adapter(m_pgen_, chip, pipe )},
    m_pgen_len_{deparser_in_hdr_adapter(m_pgen_len_, chip, pipe )},
    m_pgen_addr_{deparser_in_hdr_adapter(m_pgen_addr_, chip, pipe )},
    m_i_egress_unicast_port_{deparser_in_hdr_adapter(m_i_egress_unicast_port_, chip, pipe )},
    m_mgid1_{deparser_in_hdr_adapter(m_mgid1_, chip, pipe )},
    m_mgid2_{deparser_in_hdr_adapter(m_mgid2_, chip, pipe )},
    m_copy_to_cpu_{deparser_in_hdr_adapter(m_copy_to_cpu_, chip, pipe )},
    m_i_mirr_sel_{deparser_in_hdr_adapter(m_i_mirr_sel_, chip, pipe )},
    m_i_drop_ctl_{deparser_in_hdr_adapter(m_i_drop_ctl_, chip, pipe )},
    m_e_egress_unicast_port_{deparser_in_hdr_adapter(m_e_egress_unicast_port_, chip, pipe )},
    m_e_mirr_sel_{deparser_in_hdr_adapter(m_e_mirr_sel_, chip, pipe )},
    m_e_drop_ctl_{deparser_in_hdr_adapter(m_e_drop_ctl_, chip, pipe )},
    m_hash1_pov_reg_{deparser_in_hdr_adapter(m_hash1_pov_reg_, chip, pipe )},
    m_hash2_pov_reg_{deparser_in_hdr_adapter(m_hash2_pov_reg_, chip, pipe )},
    m_copy_to_cpu_cos_pov_reg_{deparser_in_hdr_adapter(m_copy_to_cpu_cos_pov_reg_, chip, pipe )},
    m_deflect_on_drop_pov_reg_{deparser_in_hdr_adapter(m_deflect_on_drop_pov_reg_, chip, pipe )},
    m_icos_pov_reg_{deparser_in_hdr_adapter(m_icos_pov_reg_, chip, pipe )},
    m_pkt_color_pov_reg_{deparser_in_hdr_adapter(m_pkt_color_pov_reg_, chip, pipe )},
    m_qid_pov_reg_{deparser_in_hdr_adapter(m_qid_pov_reg_, chip, pipe )},
    m_xid_l1_pov_reg_{deparser_in_hdr_adapter(m_xid_l1_pov_reg_, chip, pipe )},
    m_xid_l2_pov_reg_{deparser_in_hdr_adapter(m_xid_l2_pov_reg_, chip, pipe )},
    m_rid_pov_reg_{deparser_in_hdr_adapter(m_rid_pov_reg_, chip, pipe )},
    m_bypss_egr_pov_reg_{deparser_in_hdr_adapter(m_bypss_egr_pov_reg_, chip, pipe )},
    m_ct_disable_pov_reg_{deparser_in_hdr_adapter(m_ct_disable_pov_reg_, chip, pipe )},
    m_ct_mcast_pov_reg_{deparser_in_hdr_adapter(m_ct_mcast_pov_reg_, chip, pipe )},
    m_i_mirr_io_sel_pov_reg_{deparser_in_hdr_adapter(m_i_mirr_io_sel_pov_reg_, chip, pipe )},
    m_i_mirr_hash_pov_reg_{deparser_in_hdr_adapter(m_i_mirr_hash_pov_reg_, chip, pipe )},
    m_i_mirr_epipe_port_pov_reg_{deparser_in_hdr_adapter(m_i_mirr_epipe_port_pov_reg_, chip, pipe )},
    m_i_mirr_qid_pov_reg_{deparser_in_hdr_adapter(m_i_mirr_qid_pov_reg_, chip, pipe )},
    m_i_mirr_dond_ctrl_pov_reg_{deparser_in_hdr_adapter(m_i_mirr_dond_ctrl_pov_reg_, chip, pipe )},
    m_i_mirr_icos_pov_reg_{deparser_in_hdr_adapter(m_i_mirr_icos_pov_reg_, chip, pipe )},
    m_i_mirr_mc_ctrl_pov_reg_{deparser_in_hdr_adapter(m_i_mirr_mc_ctrl_pov_reg_, chip, pipe )},
    m_i_mirr_c2c_ctrl_pov_reg_{deparser_in_hdr_adapter(m_i_mirr_c2c_ctrl_pov_reg_, chip, pipe )},
    m_i_mirr_coal_smpl_len_pov_reg_{deparser_in_hdr_adapter(m_i_mirr_coal_smpl_len_pov_reg_, chip, pipe )},
    m_i_afc_pov_reg_{deparser_in_hdr_adapter(m_i_afc_pov_reg_, chip, pipe )},
    m_i_mtu_trunc_len_pov_reg_{deparser_in_hdr_adapter(m_i_mtu_trunc_len_pov_reg_, chip, pipe )},
    m_i_mtu_trunc_err_f_pov_reg_{deparser_in_hdr_adapter(m_i_mtu_trunc_err_f_pov_reg_, chip, pipe )},
    m_force_tx_err_pov_reg_{deparser_in_hdr_adapter(m_force_tx_err_pov_reg_, chip, pipe )},
    m_capture_tx_ts_pov_reg_{deparser_in_hdr_adapter(m_capture_tx_ts_pov_reg_, chip, pipe )},
    m_tx_pkt_has_offsets_pov_reg_{deparser_in_hdr_adapter(m_tx_pkt_has_offsets_pov_reg_, chip, pipe )},
    m_e_mirr_io_sel_pov_reg_{deparser_in_hdr_adapter(m_e_mirr_io_sel_pov_reg_, chip, pipe )},
    m_e_mirr_hash_pov_reg_{deparser_in_hdr_adapter(m_e_mirr_hash_pov_reg_, chip, pipe )},
    m_e_mirr_epipe_port_pov_reg_{deparser_in_hdr_adapter(m_e_mirr_epipe_port_pov_reg_, chip, pipe )},
    m_e_mirr_qid_pov_reg_{deparser_in_hdr_adapter(m_e_mirr_qid_pov_reg_, chip, pipe )},
    m_e_mirr_dond_ctrl_pov_reg_{deparser_in_hdr_adapter(m_e_mirr_dond_ctrl_pov_reg_, chip, pipe )},
    m_e_mirr_icos_pov_reg_{deparser_in_hdr_adapter(m_e_mirr_icos_pov_reg_, chip, pipe )},
    m_e_mirr_mc_ctrl_pov_reg_{deparser_in_hdr_adapter(m_e_mirr_mc_ctrl_pov_reg_, chip, pipe )},
    m_e_mirr_c2c_ctrl_pov_reg_{deparser_in_hdr_adapter(m_e_mirr_c2c_ctrl_pov_reg_, chip, pipe )},
    m_e_mirr_coal_smpl_len_pov_reg_{deparser_in_hdr_adapter(m_e_mirr_coal_smpl_len_pov_reg_, chip, pipe )},
    m_e_afc_pov_reg_{deparser_in_hdr_adapter(m_e_afc_pov_reg_, chip, pipe )},
    m_e_mtu_trunc_len_pov_reg_{deparser_in_hdr_adapter(m_e_mtu_trunc_len_pov_reg_, chip, pipe )},
    m_e_mtu_trunc_err_f_pov_reg_{deparser_in_hdr_adapter(m_e_mtu_trunc_err_f_pov_reg_, chip, pipe )},
    m_hash1_{ chip,pipe },
    m_hash2_{ chip,pipe },
    m_copy_to_cpu_cos_{ chip,pipe },
    m_deflect_on_drop_{ chip,pipe },
    m_icos_{ chip,pipe },
    m_pkt_color_{ chip,pipe },
    m_qid_{ chip,pipe },
    m_xid_l1_{ chip,pipe },
    m_xid_l2_{ chip,pipe },
    m_rid_{ chip,pipe },
    m_bypss_egr_{ chip,pipe },
    m_ct_disable_{ chip,pipe },
    m_ct_mcast_{ chip,pipe },
    m_i_mirr_io_sel_{ chip,pipe },
    m_i_mirr_hash_{ chip,pipe },
    m_i_mirr_epipe_port_{ chip,pipe },
    m_i_mirr_qid_{ chip,pipe },
    m_i_mirr_dond_ctrl_{ chip,pipe },
    m_i_mirr_icos_{ chip,pipe },
    m_i_mirr_mc_ctrl_{ chip,pipe },
    m_i_mirr_c2c_ctrl_{ chip,pipe },
    m_i_mirr_coal_smpl_len_{ chip,pipe },
    m_i_afc_{ chip,pipe },
    m_i_mtu_trunc_len_{ chip,pipe },
    m_i_mtu_trunc_err_f_{ chip,pipe },
    m_force_tx_err_{ chip,pipe },
    m_capture_tx_ts_{ chip,pipe },
    m_tx_pkt_has_offsets_{ chip,pipe },
    m_e_mirr_io_sel_{ chip,pipe },
    m_e_mirr_hash_{ chip,pipe },
    m_e_mirr_epipe_port_{ chip,pipe },
    m_e_mirr_qid_{ chip,pipe },
    m_e_mirr_dond_ctrl_{ chip,pipe },
    m_e_mirr_icos_{ chip,pipe },
    m_e_mirr_mc_ctrl_{ chip,pipe },
    m_e_mirr_c2c_ctrl_{ chip,pipe },
    m_e_mirr_coal_smpl_len_{ chip,pipe },
    m_e_afc_{ chip,pipe },
    m_e_mtu_trunc_len_{ chip,pipe },
    m_e_mtu_trunc_err_f_{ chip,pipe },
    //[[[end]]] (checksum: 9521d1e4808758360417ec35cbe1ad7e)

    dprsr_pre_version_r_{ chip, pipe },

    phv_csum_row_entry_array_{ deparser_in_hdr_adapter(phv_csum_row_entry_array_, chip, pipe) },

    csum_thread_reg_{ deparser_in_hdr_adapter(csum_thread_reg_,chip, pipe) },

    csum_pov_one_engine_{{
        {deparser_in_hdr_adapter(csum_pov_one_engine_[0], chip, pipe, 0 )},
        {deparser_in_hdr_adapter(csum_pov_one_engine_[1], chip, pipe, 1 )},
        {deparser_in_hdr_adapter(csum_pov_one_engine_[2], chip, pipe, 2 )},
        {deparser_in_hdr_adapter(csum_pov_one_engine_[3], chip, pipe, 3 )},
        {deparser_in_hdr_adapter(csum_pov_one_engine_[4], chip, pipe, 4 )},
        {deparser_in_hdr_adapter(csum_pov_one_engine_[5], chip, pipe, 5 )},
        {deparser_in_hdr_adapter(csum_pov_one_engine_[6], chip, pipe, 6 )},
        {deparser_in_hdr_adapter(csum_pov_one_engine_[7], chip, pipe, 7 )} }},

    csum_pov_invert_engine_{{
        {deparser_in_hdr_adapter(csum_pov_invert_engine_[0], chip, pipe, 0 )},
        {deparser_in_hdr_adapter(csum_pov_invert_engine_[1], chip, pipe, 1 )},
        {deparser_in_hdr_adapter(csum_pov_invert_engine_[2], chip, pipe, 2 )},
        {deparser_in_hdr_adapter(csum_pov_invert_engine_[3], chip, pipe, 3 )},
        {deparser_in_hdr_adapter(csum_pov_invert_engine_[4], chip, pipe, 4 )},
        {deparser_in_hdr_adapter(csum_pov_invert_engine_[5], chip, pipe, 5 )},
        {deparser_in_hdr_adapter(csum_pov_invert_engine_[6], chip, pipe, 6 )},
        {deparser_in_hdr_adapter(csum_pov_invert_engine_[7], chip, pipe, 7 )} }},

    csum_constant_{{
        { deparser_in_hdr_adapter(csum_constant_[0], chip, pipe, 0) },
        { deparser_in_hdr_adapter(csum_constant_[1], chip, pipe, 1) },
        { deparser_in_hdr_adapter(csum_constant_[2], chip, pipe, 2) },
        { deparser_in_hdr_adapter(csum_constant_[3], chip, pipe, 3) },
        { deparser_in_hdr_adapter(csum_constant_[4], chip, pipe, 4) },
        { deparser_in_hdr_adapter(csum_constant_[5], chip, pipe, 5) },
        { deparser_in_hdr_adapter(csum_constant_[6], chip, pipe, 6) },
        { deparser_in_hdr_adapter(csum_constant_[7], chip, pipe, 7) } }},
    zeros_as_ones_{{
        { deparser_in_hdr_adapter(zeros_as_ones_[0], chip, pipe, 0) },
        { deparser_in_hdr_adapter(zeros_as_ones_[1], chip, pipe, 1) },
        { deparser_in_hdr_adapter(zeros_as_ones_[2], chip, pipe, 2) },
        { deparser_in_hdr_adapter(zeros_as_ones_[3], chip, pipe, 3) },
        { deparser_in_hdr_adapter(zeros_as_ones_[4], chip, pipe, 4) },
        { deparser_in_hdr_adapter(zeros_as_ones_[5], chip, pipe, 5) },
        { deparser_in_hdr_adapter(zeros_as_ones_[6], chip, pipe, 6) },
        { deparser_in_hdr_adapter(zeros_as_ones_[7], chip, pipe, 7) } }},
    tags_array_{{
        { deparser_in_hdr_adapter(tags_array_[0], chip, pipe, 0) },
        { deparser_in_hdr_adapter(tags_array_[1], chip, pipe, 1) },
        { deparser_in_hdr_adapter(tags_array_[2], chip, pipe, 2) },
        { deparser_in_hdr_adapter(tags_array_[3], chip, pipe, 3) },
        { deparser_in_hdr_adapter(tags_array_[4], chip, pipe, 4) },
        { deparser_in_hdr_adapter(tags_array_[5], chip, pipe, 5) },
        { deparser_in_hdr_adapter(tags_array_[6], chip, pipe, 6) },
        { deparser_in_hdr_adapter(tags_array_[7], chip, pipe, 7) } }},
    csum_row_entry_array_phv_{{
        { deparser_in_hdr_adapter(csum_row_entry_array_phv_[0], chip, pipe, 0, Dfre::kCsumEnginePhvEntry) },
        { deparser_in_hdr_adapter(csum_row_entry_array_phv_[1], chip, pipe, 1, Dfre::kCsumEnginePhvEntry) },
        { deparser_in_hdr_adapter(csum_row_entry_array_phv_[2], chip, pipe, 2, Dfre::kCsumEnginePhvEntry) },
        { deparser_in_hdr_adapter(csum_row_entry_array_phv_[3], chip, pipe, 3, Dfre::kCsumEnginePhvEntry) },
        { deparser_in_hdr_adapter(csum_row_entry_array_phv_[4], chip, pipe, 4, Dfre::kCsumEnginePhvEntry) },
        { deparser_in_hdr_adapter(csum_row_entry_array_phv_[5], chip, pipe, 5, Dfre::kCsumEnginePhvEntry) },
        { deparser_in_hdr_adapter(csum_row_entry_array_phv_[6], chip, pipe, 6, Dfre::kCsumEnginePhvEntry) },
        { deparser_in_hdr_adapter(csum_row_entry_array_phv_[7], chip, pipe, 7, Dfre::kCsumEnginePhvEntry) } }},
    csum_row_entry_array_clot_{{
        { deparser_in_hdr_adapter(csum_row_entry_array_clot_[0], chip, pipe, 0, Dfre::kCsumEngineClotEntry) },
        { deparser_in_hdr_adapter(csum_row_entry_array_clot_[1], chip, pipe, 1, Dfre::kCsumEngineClotEntry) },
        { deparser_in_hdr_adapter(csum_row_entry_array_clot_[2], chip, pipe, 2, Dfre::kCsumEngineClotEntry) },
        { deparser_in_hdr_adapter(csum_row_entry_array_clot_[3], chip, pipe, 3, Dfre::kCsumEngineClotEntry) },
        { deparser_in_hdr_adapter(csum_row_entry_array_clot_[4], chip, pipe, 4, Dfre::kCsumEngineClotEntry) },
        { deparser_in_hdr_adapter(csum_row_entry_array_clot_[5], chip, pipe, 5, Dfre::kCsumEngineClotEntry) },
        { deparser_in_hdr_adapter(csum_row_entry_array_clot_[6], chip, pipe, 6, Dfre::kCsumEngineClotEntry) },
        { deparser_in_hdr_adapter(csum_row_entry_array_clot_[7], chip, pipe, 7, Dfre::kCsumEngineClotEntry) } }},

    hdr_xbar_const_defs_r_{{
        { chip, pipe, false/*ingress*/  },
        { chip, pipe, true /*egress */ } }},

    //[[[cog
    //       for i_ in range(0,9) :
    //         for r_ in ["En","Rates"] :
    //            cog.outl("ic_regs_mac{0}_{2}_{{ deparser_in_hdr_adapter(ic_regs_mac{0}_{2}_,chip,pipe) }},".format(i_,r_,r_.lower()) )
    // ]]]
    ic_regs_mac0_en_{ deparser_in_hdr_adapter(ic_regs_mac0_en_,chip,pipe) },
    ic_regs_mac0_rates_{ deparser_in_hdr_adapter(ic_regs_mac0_rates_,chip,pipe) },
    ic_regs_mac1_en_{ deparser_in_hdr_adapter(ic_regs_mac1_en_,chip,pipe) },
    ic_regs_mac1_rates_{ deparser_in_hdr_adapter(ic_regs_mac1_rates_,chip,pipe) },
    ic_regs_mac2_en_{ deparser_in_hdr_adapter(ic_regs_mac2_en_,chip,pipe) },
    ic_regs_mac2_rates_{ deparser_in_hdr_adapter(ic_regs_mac2_rates_,chip,pipe) },
    ic_regs_mac3_en_{ deparser_in_hdr_adapter(ic_regs_mac3_en_,chip,pipe) },
    ic_regs_mac3_rates_{ deparser_in_hdr_adapter(ic_regs_mac3_rates_,chip,pipe) },
    ic_regs_mac4_en_{ deparser_in_hdr_adapter(ic_regs_mac4_en_,chip,pipe) },
    ic_regs_mac4_rates_{ deparser_in_hdr_adapter(ic_regs_mac4_rates_,chip,pipe) },
    ic_regs_mac5_en_{ deparser_in_hdr_adapter(ic_regs_mac5_en_,chip,pipe) },
    ic_regs_mac5_rates_{ deparser_in_hdr_adapter(ic_regs_mac5_rates_,chip,pipe) },
    ic_regs_mac6_en_{ deparser_in_hdr_adapter(ic_regs_mac6_en_,chip,pipe) },
    ic_regs_mac6_rates_{ deparser_in_hdr_adapter(ic_regs_mac6_rates_,chip,pipe) },
    ic_regs_mac7_en_{ deparser_in_hdr_adapter(ic_regs_mac7_en_,chip,pipe) },
    ic_regs_mac7_rates_{ deparser_in_hdr_adapter(ic_regs_mac7_rates_,chip,pipe) },
    ic_regs_mac8_en_{ deparser_in_hdr_adapter(ic_regs_mac8_en_,chip,pipe) },
    ic_regs_mac8_rates_{ deparser_in_hdr_adapter(ic_regs_mac8_rates_,chip,pipe) },
    //[[[end]]] (checksum: 47fa51cf8376359cdd3fbba3bea7246f)

    ic_regs_i_phv_8_grp_{ deparser_in_hdr_adapter(ic_regs_i_phv_8_grp_, chip, pipe) },
    ic_regs_i_phv_16_grp_{ deparser_in_hdr_adapter(ic_regs_i_phv_16_grp_, chip, pipe) },
    ic_regs_i_phv_32_grp_{ deparser_in_hdr_adapter(ic_regs_i_phv_32_grp_, chip, pipe) },
    ic_regs_e_phv_8_grp_{ deparser_in_hdr_adapter(ic_regs_e_phv_8_grp_, chip, pipe) },
    ic_regs_e_phv_16_grp_{ deparser_in_hdr_adapter(ic_regs_e_phv_16_grp_, chip, pipe) },
    ic_regs_e_phv_32_grp_{ deparser_in_hdr_adapter(ic_regs_e_phv_32_grp_, chip, pipe) },

    inp_egr_unicast_check_r_{ deparser_in_hdr_adapter(inp_egr_unicast_check_r_, chip, pipe) },
    teop_inhibit_r_{ chip, pipe },

    mirror_table_entry_r_array_{{
        { chip, pipe, false /*ingress*/ },
        { chip, pipe, true  /*egress */ } }},
    perf_pkt_array_{{
        { chip, pipe, false /*ingress*/ },
        { chip, pipe, true  /*egress */ } }},
    perf_byte_array_{{
        { chip, pipe, false /*ingress*/ },
        { chip, pipe, true  /*egress */ } }},
    perf_pkt_time_{{
        { chip, pipe, false /*ingress*/ },
        { chip, pipe, true  /*egress */ } }},
    perf_byte_time_{{
        { chip, pipe, false /*ingress*/ },
        { chip, pipe, true  /*egress */ } }},
    perf_probe_{{
        { chip, pipe, false /*ingress*/ , this },
        { chip, pipe, true  /*egress*/  , this } }},
    shadow_perf_pkt_array_{{
        { 36, &perf_pkt_array_[0] },
        { 36, &perf_pkt_array_[1] } }},
    shadow_perf_byte_array_{{
        { 48, &perf_byte_array_[0] },
        { 48, &perf_byte_array_[1] } }},
    resubmit_table_entry_r_array_{ chip, pipe },
    pktgen_table_entry_r_{ chip, pipe },
    fd_{ chip, pipe }
{
  Reset();
  ctor_running_ = false;
}

DeparserReg::~DeparserReg() {
}

void DeparserReg::Reset() {
    pov_position_[kIngress].reset();
    pov_position_[kEgress].reset();
    cnt_vld_i_phv_[kIngress].reset();
    cnt_vld_i_phv_[kEgress].reset();

    pp_ctr_cfg_data_[kIngress].reset();
    pp_ctr_cfg_data_[kEgress].reset();
    pp_ctr_cfg_mask_[kIngress].reset();
    pp_ctr_cfg_mask_[kEgress].reset();
    pp_ctr_cfg48_r_[kIngress].reset();
    pp_ctr_cfg48_r_[kEgress].reset();

    //[[[cog cog.out(metadata.reset_list) ]]]
    m_learn_sel_.reset();
    m_resub_sel_.reset();
    m_pgen_.reset();
    m_pgen_len_.reset();
    m_pgen_addr_.reset();
    m_i_egress_unicast_port_.reset();
    m_mgid1_.reset();
    m_mgid2_.reset();
    m_copy_to_cpu_.reset();
    m_i_mirr_sel_.reset();
    m_i_drop_ctl_.reset();
    m_e_egress_unicast_port_.reset();
    m_e_mirr_sel_.reset();
    m_e_drop_ctl_.reset();
    m_hash1_pov_reg_.reset();
    m_hash2_pov_reg_.reset();
    m_copy_to_cpu_cos_pov_reg_.reset();
    m_deflect_on_drop_pov_reg_.reset();
    m_icos_pov_reg_.reset();
    m_pkt_color_pov_reg_.reset();
    m_qid_pov_reg_.reset();
    m_xid_l1_pov_reg_.reset();
    m_xid_l2_pov_reg_.reset();
    m_rid_pov_reg_.reset();
    m_bypss_egr_pov_reg_.reset();
    m_ct_disable_pov_reg_.reset();
    m_ct_mcast_pov_reg_.reset();
    m_i_mirr_io_sel_pov_reg_.reset();
    m_i_mirr_hash_pov_reg_.reset();
    m_i_mirr_epipe_port_pov_reg_.reset();
    m_i_mirr_qid_pov_reg_.reset();
    m_i_mirr_dond_ctrl_pov_reg_.reset();
    m_i_mirr_icos_pov_reg_.reset();
    m_i_mirr_mc_ctrl_pov_reg_.reset();
    m_i_mirr_c2c_ctrl_pov_reg_.reset();
    m_i_mirr_coal_smpl_len_pov_reg_.reset();
    m_i_afc_pov_reg_.reset();
    m_i_mtu_trunc_len_pov_reg_.reset();
    m_i_mtu_trunc_err_f_pov_reg_.reset();
    m_force_tx_err_pov_reg_.reset();
    m_capture_tx_ts_pov_reg_.reset();
    m_tx_pkt_has_offsets_pov_reg_.reset();
    m_e_mirr_io_sel_pov_reg_.reset();
    m_e_mirr_hash_pov_reg_.reset();
    m_e_mirr_epipe_port_pov_reg_.reset();
    m_e_mirr_qid_pov_reg_.reset();
    m_e_mirr_dond_ctrl_pov_reg_.reset();
    m_e_mirr_icos_pov_reg_.reset();
    m_e_mirr_mc_ctrl_pov_reg_.reset();
    m_e_mirr_c2c_ctrl_pov_reg_.reset();
    m_e_mirr_coal_smpl_len_pov_reg_.reset();
    m_e_afc_pov_reg_.reset();
    m_e_mtu_trunc_len_pov_reg_.reset();
    m_e_mtu_trunc_err_f_pov_reg_.reset();
    //[[[end]]] (checksum: bb4a20419ac2cfc10a2647c5e4751275)

    phv_csum_row_entry_array_.reset();
    csum_thread_reg_.reset();

    for (int i=0;i<8;++i) {
      csum_constant_[i].reset();
      zeros_as_ones_[i].reset();
      tags_array_[i].reset();
      csum_row_entry_array_phv_[i].reset();
      csum_row_entry_array_clot_[i].reset();
    }

    for (int engine=0;engine<8;++engine) {
      csum_pov_one_engine_[engine].reset();
      csum_pov_invert_engine_[engine].reset();
    }

    //[[[cog
    //       for i_ in range(0,9) :
    //         for r_ in ["En","Rates"] :
    //            cog.outl("ic_regs_mac{0}_{2}_.reset();".format(i_,r_,r_.lower()) )
    // ]]]
    ic_regs_mac0_en_.reset();
    ic_regs_mac0_rates_.reset();
    ic_regs_mac1_en_.reset();
    ic_regs_mac1_rates_.reset();
    ic_regs_mac2_en_.reset();
    ic_regs_mac2_rates_.reset();
    ic_regs_mac3_en_.reset();
    ic_regs_mac3_rates_.reset();
    ic_regs_mac4_en_.reset();
    ic_regs_mac4_rates_.reset();
    ic_regs_mac5_en_.reset();
    ic_regs_mac5_rates_.reset();
    ic_regs_mac6_en_.reset();
    ic_regs_mac6_rates_.reset();
    ic_regs_mac7_en_.reset();
    ic_regs_mac7_rates_.reset();
    ic_regs_mac8_en_.reset();
    ic_regs_mac8_rates_.reset();
    //[[[end]]] (checksum: c611eefd47bff81ea6730180c68f5c72)

    ic_regs_i_phv_8_grp_.reset();
    ic_regs_i_phv_16_grp_.reset();
    ic_regs_i_phv_32_grp_.reset();
    ic_regs_e_phv_8_grp_.reset();
    ic_regs_e_phv_16_grp_.reset();
    ic_regs_e_phv_32_grp_.reset();

    inp_egr_unicast_check_r_.reset();
    teop_inhibit_r_.reset();

    for (int gress=0; gress<=1; gress++) { // 0->ingress, 1->egress
      mirror_table_entry_r_array_[gress].reset();
      perf_pkt_array_[gress].reset();
      perf_pkt_time_[gress].reset();
      perf_byte_array_[gress].reset();
      perf_byte_time_[gress].reset();
      perf_probe_[gress].reset();
    }

    resubmit_table_entry_r_array_.reset();
    pktgen_table_entry_r_.reset();

    fd_.reset();
}

bool DeparserReg::CheckIngressDeparserPhvGroupConfig(const int &phv_idx) {
  RMT_ASSERT(phv_idx < Phv::phv_max_d());
  bool config = true;
  switch (Phv::which_width_d(phv_idx)) {
    case 8:
      config = CheckDeparserPhvGroupConfig(phv_idx, ic_regs_i_phv_8_grp_.val());
      break;
    case 16:
      config = CheckDeparserPhvGroupConfig(phv_idx, ic_regs_i_phv_16_grp_.val());
      break;
    case 32:
      config = CheckDeparserPhvGroupConfig(phv_idx, ic_regs_i_phv_32_grp_.val());
      break;
    default:
      RMT_ASSERT(false);
  }
  return config;
}

bool DeparserReg::CheckEgressDeparserPhvGroupConfig(const int &phv_idx) {
  RMT_ASSERT(phv_idx < Phv::phv_max_d());
  bool config = true;
  switch (Phv::which_width_d(phv_idx)) {
    case 8:
      config = CheckDeparserPhvGroupConfig(phv_idx, ic_regs_e_phv_8_grp_.val());
      break;
    case 16:
      config = CheckDeparserPhvGroupConfig(phv_idx, ic_regs_e_phv_16_grp_.val());
      break;
    case 32:
      config = CheckDeparserPhvGroupConfig(phv_idx, ic_regs_e_phv_32_grp_.val());
      break;
    default:
      RMT_ASSERT(false);
  }
  return config;
}

bool DeparserReg::CheckDeparserPhvGroupConfig(const int &phv_idx, const uint32_t &val) {
  const auto group = phv_group(phv_idx);
  return ((val & (1 << group)) == 0) ? false : true;
}

int DeparserReg::phv_group(const int &phv_idx) {
  int group = -1;
  switch (Phv::which_width_d(phv_idx)) {
    case 8:
      group = (phv_idx - 64) >> 2;
      break;
    case 16:
      group = (phv_idx - 128) >> 2;
      break;
    case 32:
      group = phv_idx >> 1;
      break;
  }
  RMT_ASSERT (group >= 0);
  return group;
}

void DeparserReg::sample_perf_counter(bool egress, int slice) {
  if (ctor_running_) return;
  RMT_ASSERT(is_valid_slice_index(slice));
  int gress = egress_flag_to_index(egress);
  const uint8_t match = 0x1;
  bool sample_pkt = ((perf_probe_[gress].reg_[slice].vld() == match) ||
                     (perf_probe_[gress].reg_[slice].pkt_vld() == match));
  bool sample_byt = ((perf_probe_[gress].reg_[slice].vld() ==  match) ||
                     (perf_probe_[gress].reg_[slice].byt_vld() == match));
  if (sample_pkt || sample_byt) {
    uint64_t sample_time = get_object_manager()->time_get_cycles();
    if (sample_pkt) {
      shadow_perf_pkt_array_[gress].ctr_[slice].sample();
      perf_pkt_time_[gress].reg_[slice].count(sample_time);
    }
    if (sample_byt) {
      shadow_perf_byte_array_[gress].ctr_[slice].sample();
      perf_byte_time_[gress].reg_[slice].count(sample_time);
    }
    // reset sample reg
    perf_probe_[gress].reg_[slice].vld(0x0);
    perf_probe_[gress].reg_[slice].byt_vld(0x0);
    perf_probe_[gress].reg_[slice].pkt_vld(0x0);
  }
}

bool DeparserReg::get_pre_version(int slice) {
  return dprsr_pre_version_r_.reg_[slice].sel();
}

uint8_t DeparserReg::get_copy_to_cpu_pipe_vector() {
  return chip_reg().get_copy_to_cpu_pipe_vector();
}

// 5 bits field on jbay, 9 bits on WIP: one bit per pipe plus an extra bit to
// indicate if there is a 400G port in the group
uint16_t DeparserReg::get_multicast_pipe_vector(int table_num, int mgid) {
  return chip_reg().get_multicast_pipe_vector(table_num, mgid);
}
void DeparserReg::set_multicast_pipe_vector(int table_num, int mgid, uint16_t val) {
  chip_reg().set_multicast_pipe_vector(table_num, mgid, val);
}

// Getters for mirror registers.
void DeparserReg::get_mirror_table_entry(const bool &egress,int slice,
                            const uint8_t mirror_table_idx,
                            std::vector<uint8_t> *phv_idx_list,
                            uint8_t *mirror_id_phv_idx) {
  RMT_ASSERT(mirror_table_idx < 16);
  register_classes::DprsrMirrorTableEntryRArray &mirror_table_entry_array = mirror_table_entry_r_array_[egress_flag_to_index(egress)].reg_[slice];
  RMT_LOG_VERBOSE("DEPARSER::get_mirror_table_entry %s slice=%d index:%d\n",egress?"egress":"ingress",slice, mirror_table_idx);
  for (int i = 0; i < mirror_table_entry_array.len(mirror_table_idx); ++i) {
    RMT_LOG_VERBOSE("DEPARSER::get_mirror_table_entry   %3d = 0x%x\n",i,mirror_table_entry_array.phvs(mirror_table_idx, i) );
    phv_idx_list->push_back(mirror_table_entry_array.phvs(mirror_table_idx, i));
  }
  (*mirror_id_phv_idx) = mirror_table_entry_array.id_phv(mirror_table_idx);
  RMT_LOG_VERBOSE("DEPARSER::get_mirror_table_entry id_phv = 0x%x\n", *mirror_id_phv_idx );
}

void DeparserReg::get_resubmit_table_entry(const uint8_t resubmit_table_idx, bool *valid,
                              std::vector<uint8_t> *phv_idx_list) {
  *valid = resubmit_table_entry_r_array_.valid(resubmit_table_idx);
  if (*valid) {
    for (int i = 0; i < resubmit_table_entry_r_array_.len(resubmit_table_idx); ++i) {
      phv_idx_list->push_back(resubmit_table_entry_r_array_.phvs(resubmit_table_idx, i));
    }
  }
}

void DeparserReg::get_packet_gen_table_entry(bool *valid,
                              std::vector<uint8_t> *phv_idx_list) {
  *valid = pktgen_table_entry_r_.valid();
  if (*valid) {
    for (int i = 0; i < pktgen_table_entry_r_.len(); ++i) {
      phv_idx_list->push_back(pktgen_table_entry_r_.phvs(i));
    }
  }
}

// This function takes the checksum index and returns the PHV index.
//  Assume phv order for now, so 32's then 8's then 16's
int DeparserReg::phv_idx(const int &csum_idx, int *shift) {
  RMT_ASSERT(csum_idx>=0)
  const int kN32bit = 64;
  const int kN8bit  = 64;
  const int kN16bit = 96;

  if ( csum_idx < (kN32bit*2) ) { // 32 bit groups
    int i = csum_idx - 0;
    *shift = (i&1) ? 0 : 16 ;  // row for top half comes first
    return i/2;
  }
  else if ( csum_idx < ((kN32bit*2)+kN8bit) ) { // 8 bit groups
    *shift = 0;
    return csum_idx - (kN32bit*2) + kN32bit;
  }
  else {
    RMT_ASSERT(csum_idx < ((kN32bit*2)+kN8bit+kN16bit)); // < 288
    *shift = 0;
    return csum_idx - ((kN32bit*2)+kN8bit) + kN32bit + kN8bit;
  }
}

Deparser::FdChunk DeparserReg::get_chunk(int slice, bool egress, int word, int chunk) {
  RMT_ASSERT(chunk < kChunksPerWord);
  RMT_ASSERT(word  < kWords);
  RMT_ASSERT(slice < kSlices);
  return fd_.slices_[slice].ie_[egress?1:0].chunks_[word].get_chunk( chunk );
}

uint8_t DeparserReg::get_constant(bool egress, int slice, int which) {
  RMT_ASSERT(which>=0 && which<kNumConstants);
  return hdr_xbar_const_defs_r_[egress?1:0].reg_[slice].value(which);
}

bool DeparserReg::port_enabled(bool egress, int slice, int slice_channel) {
  RMT_ASSERT(is_valid_slice_channel_index(slice_channel));
  RMT_ASSERT(is_valid_slice_index(slice));

  int v = 0;
  int shift = 0;
  if ( slice_channel < 2 ) {
     v    = ic_regs_mac0_en_.enbl();
    shift = slice_channel + (slice*2);
  }
  else if ( slice_channel < 10 ) {
    switch (slice) {
      case 0:  v = ic_regs_mac1_en_.enbl(); break;
      case 1:  v = ic_regs_mac3_en_.enbl(); break;
      case 2:  v = ic_regs_mac5_en_.enbl(); break;
      case 3:  v = ic_regs_mac7_en_.enbl(); break;
    }
    shift = slice_channel - 2;
  }
  else { // slice_channel < 18  asserted above by is_valid_slice_channel_index
    switch (slice) {
      case 0:  v = ic_regs_mac2_en_.enbl(); break;
      case 1:  v = ic_regs_mac4_en_.enbl(); break;
      case 2:  v = ic_regs_mac6_en_.enbl(); break;
      case 3:  v = ic_regs_mac8_en_.enbl(); break;
    }
    shift = slice_channel - 10;
  }
  RMT_LOG_VERBOSE("DEPARSER::port_enabled slice_channel=%d register:%02x bit index:%d result=%d\n",slice_channel, v, shift, (1 & ( v >> shift ) ));
  return (1 & ( v >> shift ) ) ;
}

DeparserChipReg& DeparserReg::chip_reg() { return deparser_chip_reg_; }

void DeparserReg::increment_phv_counter(bool egress) {
  auto& counter = egress ? cnt_vld_i_phv_[1] : cnt_vld_i_phv_[0];

  counter.ctr48( counter.ctr48() + 1 );
}

void DeparserReg::increment_learn_counter() {
  // stub for compatibility with Tofino interface, called from pipe.process()
}

void DeparserReg::increment_configurable_counter(bool egress, uint8_t port) {
  // live_data = {phv_id[6:0], phv_vld} - the model only runs when there is a valid phv
  uint8_t live_data = (port<<1) | 1;
  int index = egress ? 1 : 0;

  uint8_t cfg_mask = pp_ctr_cfg_mask_[index].cfg();
  uint8_t cfg_data = pp_ctr_cfg_data_[index].cfg();
  if ( (live_data & cfg_mask) == cfg_data ) {
    auto& counter = pp_ctr_cfg48_r_[index];
    counter.ctr48( counter.ctr48() + 1 );
  }
}

void DeparserReg::increment_perf_pkt_counter(bool egress,
                                int slice,
                                int slice_channel,
                                uint64_t amount) {
  int gress = egress_flag_to_index(egress);
  shadow_perf_pkt_array_[gress].ctr_[slice].increment(slice_channel, amount);
}

void DeparserReg::increment_perf_byte_counter(bool egress,
                                 int slice,
                                 int slice_channel,
                                 uint64_t amount) {
  int gress = egress_flag_to_index(egress);
  shadow_perf_byte_array_[gress].ctr_[slice].increment(slice_channel, amount);
}

bool DeparserReg::test_csum_pov(int engine, int which_bit, const BitVector<kPovWidth> &pov) {

  uint8_t which_byte_sel = which_bit / 8;
  uint8_t which_byte_bit = which_bit % 8;
  RMT_ASSERT( which_byte_sel < 4 );
  auto& pov_reg = csum_pov_one_engine_[engine];
  uint8_t byte_sel = pov_reg.byte_sel(which_byte_sel);
  RMT_ASSERT( byte_sel < 16 );
  uint8_t byte_val = pov.get_byte( byte_sel );

  auto&   invert_reg = csum_pov_invert_engine_[engine];
  RMT_ASSERT( which_bit < 32 );
  bool    invert = (invert_reg.mask() >> which_bit) & 1;

  bool    bit = (byte_val>>which_byte_bit) & 1;
  return bit ^ invert;
}

uint16_t DeparserReg::get_csum_constant(int slice, bool egress, int engine) {
  auto& reg = csum_constant_[engine];
  return reg.val();
}

uint8_t DeparserReg::get_clot_sel_segment_tag(int slice, bool egress, int word, int index) {
  return fd_.slices_[slice].ie_[egress?1:0].chunks_[word].get_clot_sel_segment_tag( index );
}


bool DeparserReg::EgressUnicastNeedsCheck() {
  return inp_egr_unicast_check_r_.need_check();
}
bool DeparserReg::teop_inhibit(int slice) {
  return teop_inhibit_r_.reg_[slice].inhibit();
}


bool DeparserReg::GetFullChecksumEntryValid(register_classes::DprsrFullcsumRowEntryArray& reg,int engine,int index,
                               const BitVector<kPovWidth>& pov) {
  if ( !reg.vld(index) ) return false;
  int pov_idx = reg.pov(index);
  return test_csum_pov( engine, pov_idx, pov );
}

int DeparserReg::egress_flag_to_index(bool egress) { return egress ? kEgress : kIngress; }

}
