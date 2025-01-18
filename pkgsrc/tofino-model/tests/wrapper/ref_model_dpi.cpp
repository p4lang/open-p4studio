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

#include "ref_model_wrapper.hpp"
#include "register_includes/reg.h"
#include "ref_model_dpi.hpp"

#ifdef __cplusplus
extern "C" {
#endif

  void dpi_model_wrapper_create() {
    printf("Creating REF MODEL WRAPPER\n");
    MODEL_CHIP_NAMESPACE::ref_model_wrapper::getInstance();
  }

  void dpi_model_wrapper_destroy() {
    printf("Deleting REF MODEL WRAPPER\n");
    delete MODEL_CHIP_NAMESPACE::ref_model_wrapper::getInstance();
  }

  void dpi_model_wrapper_host_if_reset() {
    printf("Reset host if\n");
    MODEL_CHIP_NAMESPACE::ref_model_wrapper::getInstance()->host_if_reset();
  }

  void dpi_parser_model_process_packet(uint32_t pipe, uint32_t prsr_inst, uint32_t prsr_type, uint32_t chan, uint32_t version, 
                                       uint32_t congested, const char* pktstr, 
                                       uint32_t* phv_data, uint32_t* phv_vld, 
                                       uint32_t* phvt_data, uint32_t* phvt_vld, 
                                       uint32_t* payload_offset, uint32_t* drop, 
                                       uint32_t* pri, uint32_t* err_flags) {
    MODEL_CHIP_NAMESPACE::ref_model_wrapper::getInstance()->parser_model_process_packet(pipe, prsr_inst, prsr_type, chan, version & 0xFF, 
                                                                  congested, pktstr, phv_data, phv_vld, 
                                                                  phvt_data, phvt_vld, payload_offset, 
                                                                  drop, pri, err_flags);
  }

  void dpi_parser_model_process_packet_clot(uint32_t chip, uint32_t pipe, uint32_t prsr_inst, uint32_t prsr_type, uint32_t chan, uint32_t version, 
					    uint32_t congested, const char* pktstr, 
					    uint32_t* phv_data, uint32_t* phv_vld, 
					    uint32_t* clot_data, uint32_t* clot_vld, 
					    uint32_t* payload_offset, uint32_t* drop, 
					    uint32_t* pri, uint32_t* err_flags, uint32_t* counter_data, uint8_t* ref_version) {
    MODEL_CHIP_NAMESPACE::ref_model_wrapper::getInstance()->parser_model_process_packet_clot(chip, pipe, prsr_inst, prsr_type, chan, version & 0xFF, 
											     congested, pktstr, phv_data, phv_vld, 
											     clot_data, clot_vld, payload_offset, 
											     drop, pri, err_flags, counter_data, ref_version);
  }

  void dpi_deparser_model_process_ing_packet(uint32_t* phv_data, uint32_t* phv_vld, 
                                                                     uint32_t* phvt_data, uint32_t* phvt_vld,
                                             uint32_t* ing_mau_err,
					     const char* input_pkt_data, uint32_t orig_hdr_len, uint32_t version,
                                             uint32_t* i_pkt_length, char* i_pkt_data,
                                             uint32_t* i_mirror_pkt_length, char* i_mirror_pkt_data,
                                             uint32_t input_port_num, uint32_t* meta_data,
                                             uint32_t* lfltr_pkt_valid, char* lfltr_pkt_data,
                                             uint32_t* resubmit_meta_data_valid, char* resubmit_meta_data,
                                             uint32_t* pkt_dropped
                                            ) {
    uint32_t ing_pkt_length = 0;
    uint32_t mirror_pkt_length = 0;
    uint32_t lq_pkt_valid = 0;
    uint32_t rs_meta_data_valid = 0;
    uint32_t ing_pkt_dropped = 0;

    std::unique_ptr<uint8_t[]> output_pkt_data;
    std::unique_ptr<uint8_t[]> mirror_pkt_data;
    std::unique_ptr<uint8_t[]> lq_pkt_data;
    std::unique_ptr<uint8_t[]> rs_meta_data;

    MODEL_CHIP_NAMESPACE::ref_model_wrapper::getInstance()->deparser_model_process_packet(1, phv_data, phv_vld, phvt_data, phvt_vld, ing_mau_err, input_pkt_data, orig_hdr_len, version, ing_pkt_length, output_pkt_data, mirror_pkt_length, mirror_pkt_data, input_port_num, meta_data, lq_pkt_valid, lq_pkt_data, rs_meta_data_valid, rs_meta_data, ing_pkt_dropped);

    printf("orig_hdr_len:%d model_pkt_length:%d\n", orig_hdr_len, ing_pkt_length);
    for (uint32_t i=0; i<ing_pkt_length; i++) {
      i_pkt_data[i*4] = static_cast<char>(output_pkt_data[i]); 
    }
    (*i_pkt_length) = ing_pkt_length;
    (*pkt_dropped)  = ing_pkt_dropped;

    for (uint32_t i=0; i<mirror_pkt_length; i++) {
      i_mirror_pkt_data[i*4] = static_cast<char>(mirror_pkt_data[i]); 
    }
    (*i_mirror_pkt_length) = mirror_pkt_length;

    if (lq_pkt_valid) {
      for (uint32_t i=0; i<48; i++) {
        //lfltr_pkt_data[(47-i)*4] = static_cast<char>(lq_pkt_data[i]); 
        lfltr_pkt_data[i*4] = static_cast<char>(lq_pkt_data[i]); 
      }
    }
    (*lfltr_pkt_valid) = lq_pkt_valid;

    if (rs_meta_data_valid) {
      for (uint32_t i=0; i<8; i++) {
        resubmit_meta_data[(7-i)*4] = static_cast<char>(rs_meta_data[i]); 
      }
    }
    (*resubmit_meta_data_valid) = rs_meta_data_valid;


  }

  void dpi_deparser_model_process_ing_packet_clot(uint32_t chip, uint32_t pipe, uint32_t* phv_data, uint32_t* phv_vld, 
                                                                     uint32_t* clot_data, uint32_t* clot_vld,
                                             uint32_t* ing_mau_err,
					     const char* input_pkt_data, uint32_t orig_hdr_len, uint32_t version,
                                             uint32_t* i_pkt_length, char* i_pkt_data,
                                             uint32_t* i_mirror_pkt_length, char* i_mirror_pkt_data,
                                             uint32_t input_port_num, uint32_t* meta_data,
                                             uint32_t* lfltr_pkt_valid, char* lfltr_pkt_data,
                                             uint32_t* resubmit_meta_data_valid, char* resubmit_meta_data,
                                             uint32_t* pgen_meta_data_valid, char* pgen_meta_data, uint32_t* pgen_address, uint32_t* pgen_length,
                                             uint32_t* pkt_dropped
                                            ) {
    uint32_t ing_pkt_length = 0;
    uint32_t mirror_pkt_length = 0;
    uint32_t lq_pkt_valid = 0;
    uint32_t rs_meta_data_valid = 0;
    uint32_t pg_meta_data_valid = 0;
    uint32_t pg_address = 0;
    uint32_t pg_length = 0;
    uint32_t ing_pkt_dropped = 0;

    std::unique_ptr<uint8_t[]> output_pkt_data;
    std::unique_ptr<uint8_t[]> mirror_pkt_data;
    std::unique_ptr<uint8_t[]> lq_pkt_data;
    std::unique_ptr<uint8_t[]> rs_meta_data;
    std::unique_ptr<uint8_t[]> pg_meta_data;

    MODEL_CHIP_NAMESPACE::ref_model_wrapper::getInstance()->deparser_model_process_packet_clot(chip, pipe, 1, phv_data, phv_vld, clot_data, clot_vld, ing_mau_err, input_pkt_data, orig_hdr_len, version, ing_pkt_length, output_pkt_data, mirror_pkt_length, mirror_pkt_data, input_port_num, meta_data, lq_pkt_valid, lq_pkt_data, rs_meta_data_valid, rs_meta_data, pg_meta_data_valid, pg_meta_data, pg_address, pg_length, ing_pkt_dropped);

    printf("orig_hdr_len:%d model_pkt_length:%d\n", orig_hdr_len, ing_pkt_length);
    for (uint32_t i=0; i<ing_pkt_length; i++) {
      i_pkt_data[i*4] = static_cast<char>(output_pkt_data[i]); 
    }
    (*i_pkt_length) = ing_pkt_length;
    (*pkt_dropped)  = ing_pkt_dropped;

    for (uint32_t i=0; i<mirror_pkt_length; i++) {
      i_mirror_pkt_data[i*4] = static_cast<char>(mirror_pkt_data[i]); 
    }
    (*i_mirror_pkt_length) = mirror_pkt_length;

    if (lq_pkt_valid) {
      for (uint32_t i=0; i<48; i++) {
        //lfltr_pkt_data[(47-i)*4] = static_cast<char>(lq_pkt_data[i]); 
        lfltr_pkt_data[i*4] = static_cast<char>(lq_pkt_data[i]); 
      }
    }
    (*lfltr_pkt_valid) = lq_pkt_valid;

    if (rs_meta_data_valid) {
      for (uint32_t i=0; i<16; i++) {
        resubmit_meta_data[i*4] = static_cast<char>(rs_meta_data[i]); 
      }
    }
    (*resubmit_meta_data_valid) = rs_meta_data_valid;

    if (pg_meta_data_valid) {
      for (uint32_t i=0; i<16; i++) {
        pgen_meta_data[i*4] = static_cast<char>(pg_meta_data[i]); 
      }
      (*pgen_address) = pg_address;
      (*pgen_length) = pg_length;
    }
    (*pgen_meta_data_valid) = pg_meta_data_valid;


  }

  void dpi_deparser_model_process_egr_packet(uint32_t* phv_data, uint32_t* phv_vld, 
                                                                     uint32_t* phvt_data, uint32_t* phvt_vld,
					     uint32_t* egr_mau_err,
                                             const char* input_pkt_data, uint32_t orig_hdr_len, uint32_t version,
                                             uint32_t* e_pkt_length, char* e_pkt_data, 
                                             uint32_t* e_mirror_pkt_length, char* e_mirror_pkt_data, 
                                             uint32_t input_port_num, uint32_t* meta_data,
                                             uint32_t* pkt_dropped
                                            ) {
    uint32_t egr_pkt_length = 0;
    uint32_t mirror_pkt_length = 0;
    uint32_t lfltr_pkt_valid = 0;
    uint32_t rs_meta_data_valid = 0;
    uint32_t egr_pkt_dropped = 0;

    std::unique_ptr<uint8_t[]> output_pkt_data;
    std::unique_ptr<uint8_t[]> mirror_pkt_data;
    std::unique_ptr<uint8_t[]> lfltr_pkt_data;
    std::unique_ptr<uint8_t[]> rs_meta_data;

    MODEL_CHIP_NAMESPACE::ref_model_wrapper::getInstance()->deparser_model_process_packet(0, phv_data, phv_vld, phvt_data, phvt_vld, egr_mau_err, input_pkt_data, orig_hdr_len, version, egr_pkt_length, output_pkt_data, mirror_pkt_length, mirror_pkt_data, input_port_num, meta_data, lfltr_pkt_valid, lfltr_pkt_data, rs_meta_data_valid, rs_meta_data, egr_pkt_dropped);

    printf("orig_hdr_len:%d model_pkt_length:%d\n", orig_hdr_len, egr_pkt_length);
    for (uint32_t i=0; i<egr_pkt_length; i++) {
      e_pkt_data[(i*4)] = static_cast<char>(output_pkt_data[i]); 
    }
    (*e_pkt_length) = egr_pkt_length;
    (*pkt_dropped)  = egr_pkt_dropped;

    for (uint32_t i=0; i<mirror_pkt_length; i++) {
      e_mirror_pkt_data[(i*4)] = static_cast<char>(mirror_pkt_data[i]); 
    }
    (*e_mirror_pkt_length) = mirror_pkt_length;

  }

  void dpi_deparser_model_process_egr_packet_clot(uint32_t chip, uint32_t pipe, uint32_t* phv_data, uint32_t* phv_vld, 
                                                                     uint32_t* clot_data, uint32_t* clot_vld,
					     uint32_t* egr_mau_err,
                                             const char* input_pkt_data, uint32_t orig_hdr_len, uint32_t version,
                                             uint32_t* e_pkt_length, char* e_pkt_data, 
                                             uint32_t* e_mirror_pkt_length, char* e_mirror_pkt_data, 
                                             uint32_t input_port_num, uint32_t* meta_data,
                                             uint32_t* pkt_dropped
                                            ) {
    uint32_t egr_pkt_length = 0;
    uint32_t mirror_pkt_length = 0;
    uint32_t lfltr_pkt_valid = 0;
    uint32_t rs_meta_data_valid = 0;
    uint32_t pg_meta_data_valid = 0;
    uint32_t pg_address = 0;
    uint32_t pg_length = 0;
    uint32_t egr_pkt_dropped = 0;

    std::unique_ptr<uint8_t[]> output_pkt_data;
    std::unique_ptr<uint8_t[]> mirror_pkt_data;
    std::unique_ptr<uint8_t[]> lfltr_pkt_data;
    std::unique_ptr<uint8_t[]> rs_meta_data;
    std::unique_ptr<uint8_t[]> pg_meta_data;

    MODEL_CHIP_NAMESPACE::ref_model_wrapper::getInstance()->deparser_model_process_packet_clot(chip, pipe, 0, phv_data, phv_vld, clot_data, clot_vld, egr_mau_err, input_pkt_data, orig_hdr_len, version, egr_pkt_length, output_pkt_data, mirror_pkt_length, mirror_pkt_data, input_port_num, meta_data, lfltr_pkt_valid, lfltr_pkt_data, rs_meta_data_valid, rs_meta_data, pg_meta_data_valid, pg_meta_data, pg_address, pg_length, egr_pkt_dropped);

    printf("orig_hdr_len:%d model_pkt_length:%d\n", orig_hdr_len, egr_pkt_length);
    for (uint32_t i=0; i<egr_pkt_length; i++) {
      e_pkt_data[(i*4)] = static_cast<char>(output_pkt_data[i]); 
    }
    (*e_pkt_length) = egr_pkt_length;
    (*pkt_dropped)  = egr_pkt_dropped;

    for (uint32_t i=0; i<mirror_pkt_length; i++) {
      e_mirror_pkt_data[(i*4)] = static_cast<char>(mirror_pkt_data[i]); 
    }
    (*e_mirror_pkt_length) = mirror_pkt_length;

  }


  /* Ref Model Address DPI */
  void dpi_mau_model_make_phys_address(uint32_t* addr_hi, uint32_t* addr_lo, 
                                       int pipe, int stage, int memType,
                                       int row, int col, int index) {
     uint64_t baddr = 0;
     baddr = MODEL_CHIP_NAMESPACE::MauMemory::make_phys_address(pipe, stage,memType,
                                                                row, col, index);
     *addr_lo = (uint32_t)(baddr & 0xFFFFFFFF);
     *addr_hi = (uint32_t)((baddr >> 32) & 0xFFFFFFFF);
  }
  
  void dpi_mau_model_get_pipe_stage_csr_base_addr(uint32_t* addr, 
                                                  int pipe, int stage) {
    *addr = MODEL_CHIP_NAMESPACE::RegisterUtils::dpi_addr_mau_first(pipe,stage);
    if (*addr == 0xFFFFFFFF) {
      printf("UVM_ERROR ERROR dpi_mau_model_get_pipe_stage_csr_base_addr() "
             "Bad pipe/stage number = %d/%d\n", pipe, stage);
      *addr = MODEL_CHIP_NAMESPACE::RegisterUtils::dpi_addr_mau_first(pipe,0);
    }
  }
  
  void dpi_mau_model_get_pipe_stage_csr_last_addr(uint32_t* addr, 
                                                  int pipe, int stage) {
    *addr = MODEL_CHIP_NAMESPACE::RegisterUtils::dpi_addr_mau_last(pipe,stage);
    if (*addr == 0xFFFFFFFF) {
      printf("UVM_ERROR ERROR dpi_mau_model_get_pipe_stage_csr_last_addr() "
             "Bad pipe/stage number = %d/%d\n", pipe, stage);
      *addr = MODEL_CHIP_NAMESPACE::RegisterUtils::dpi_addr_mau_last(pipe,0);
    }
  }
  
  /* Mau DPI */
  void dpi_mau_model_reset_resources(uint32_t pipe, uint32_t stage) {
          //printf("Mau model reset resources for next PHV\n");
    MODEL_CHIP_NAMESPACE::ref_model_wrapper::getInstance()->mau_model_reset_resources(pipe, stage);
  }
  
  void dpi_mau_model_handle_sweep(uint32_t pipe, uint32_t stage, uint32_t m_id,uint32_t m_cycle,uint64_t meter_sweep_time, uint32_t sweep_ltid,uint32_t home_row,uint32_t sweep_indx,uint32_t *sweep_addr,uint32_t *sweep_dout) {
    MODEL_CHIP_NAMESPACE::ref_model_wrapper::getInstance()->mau_model_handle_sweep(pipe, stage, m_id,m_cycle,meter_sweep_time, sweep_ltid,home_row,sweep_indx,sweep_addr,sweep_dout);
  }  
  void dpi_mau_model_handle_eop(uint32_t pipe, uint32_t stage, uint32_t phvside, uint64_t *ts, uint64_t *rng, uint32_t active, uint32_t *data_in, uint32_t *color, uint32_t eopid,uint32_t eop_cycle, int *retval) {

   MODEL_CHIP_NAMESPACE::ref_model_wrapper::getInstance()->mau_model_handle_eop(pipe, stage, phvside, ts, rng, active, data_in, color,eopid,eop_cycle, retval);
  }

   void dpi_mau_model_handle_dteop(uint32_t pipe, uint32_t stage, uint32_t phvside, uint32_t *dteop, uint64_t *ts, uint64_t *rng, uint32_t active, uint32_t *data_in, uint32_t *color, uint32_t eopid,uint32_t eop_cycle, int *retval) {

         MODEL_CHIP_NAMESPACE::ref_model_wrapper::getInstance()->mau_model_handle_dteop(pipe, stage, phvside, dteop, ts, rng, active, data_in, color,eopid,eop_cycle, retval);
   }


  void dpi_mau_model_disable_stateful_alu_checks(uint32_t pipe, uint32_t stage) {
    MODEL_CHIP_NAMESPACE::ref_model_wrapper::getInstance()->mau_model_disable_stateful_alu_checks(pipe, stage);
  }

  void dpi_mau_model_relax_mau_tcam_array( ) {
    MODEL_CHIP_NAMESPACE::ref_model_wrapper::getInstance()->mau_model_relax_mau_tcam_array( );
  }

  void dpi_mau_model_run_mau_tcam_array( uint32_t  pipe, uint32_t stage,
                                          uint32_t *phvside,
                                          int      *next_table,
					  uint64_t *kIxbarTmData,
                                          uint32_t *tcam_match_addr,
                                          int      *tcam_match_hit,
                                          uint32_t iphv_cycle, uint32_t ophv_cycle,
                                          int      *retval ) {
    MODEL_CHIP_NAMESPACE::ref_model_wrapper::getInstance()->mau_model_run_mau_tcam_array(  pipe, stage,
                                                                                           phvside,
                                                                                           next_table,
											   kIxbarTmData,
                                                                                           tcam_match_addr,
											   tcam_match_hit,
                                                                                           iphv_cycle, ophv_cycle,
                                                                                           retval );
  }

  void dpi_mau_model_run_mau_memory_core( uint32_t pipe, uint32_t stage,
                                          uint32_t *phvside,
                                          int      *next_table,
                                          uint32_t *tcam_match_addr,
                                          uint32_t iphv_cycle, uint32_t ophv_cycle,
                                          int      *retval ) {
    MODEL_CHIP_NAMESPACE::ref_model_wrapper::getInstance()->mau_model_run_mau_memory_core( pipe, stage,
                                                                                           phvside,
                                                                                           next_table,
                                                                                           tcam_match_addr,
                                                                                           iphv_cycle, ophv_cycle,
                                                                                           retval );
  }

  void dpi_mau_model_get_table_active( uint32_t pipe, uint32_t stage, uint32_t *table_active ) {
    MODEL_CHIP_NAMESPACE::ref_model_wrapper::getInstance()->get_table_active( pipe, stage, table_active );
  }

  void dpi_mau_model_process_match_n_action(uint32_t pipe, uint32_t stage,
                                            uint32_t* next_iphv_data, uint32_t* next_iphv_vld, 
                                            uint32_t* next_ophv_data, uint32_t* next_ophv_vld, 
                                            uint32_t* mresult, uint32_t *actdatabus, 
                                            uint32_t *hashgen, uint32_t *gateway,
                                            uint32_t *ehit_raw, uint32_t *em_resbus,
                                            uint32_t *ltcam,
                                            uint32_t* iphv_data, uint32_t* iphv_vld, 
                                            uint32_t* ophv_data, uint32_t* ophv_vld, 
                                            uint32_t phvside, uint32_t ltcamIndex, uint32_t *table_active,
                                            int *ingress_next_table, int *egress_next_table, uint32_t *action_cnt, uint32_t *hashdist, uint32_t *alu_io,
                                            uint32_t lpf_active, uint64_t *lpf_ts, uint64_t *lpf_rng,uint32_t lpfid,uint32_t iphv_cycle,uint32_t ophv_cycle, int *retval) {
    // printf("Tofino Mau model process match & action PHV (at once)\n");
    uint32_t  *newphvside = new uint32_t[4]();
    uint32_t  *snapshot_data = new uint32_t();
    uint32_t  *teop_result = new uint32_t[4]();
    uint32_t  *mpr_result = new uint32_t();

    newphvside[0] = phvside; newphvside[1] = 0; newphvside[2] = 0; newphvside[3] = 0;
    snapshot_data[0] = 0;
    teop_result[0] = 0; teop_result[1] = 0; teop_result[2] = 0 ; teop_result[3] = 0 ;
    mpr_result[0] = 0;
    MODEL_CHIP_NAMESPACE::ref_model_wrapper::getInstance()->mau_model_process_match_n_action(pipe, stage,
                                                                       next_iphv_data, next_iphv_vld, 
                                                                       next_ophv_data, next_ophv_vld, 
                                                                       mresult, actdatabus, 
                                                                       hashgen, gateway,
                                                                       ehit_raw, em_resbus,
                                                                       ltcam,
                                                                       iphv_data, iphv_vld, 
                                                                       ophv_data, ophv_vld, 
                                                                       newphvside, ltcamIndex, table_active,
                                                                       ingress_next_table, egress_next_table, 
                                                                       action_cnt, hashdist, alu_io,
                                                                       lpf_active, lpf_ts, lpf_rng, lpfid,
                                                                       iphv_cycle, ophv_cycle, snapshot_data, teop_result, mpr_result, retval);
  }

  void dpi_jbay_mau_model_process_match_n_action(uint32_t pipe, uint32_t stage,
                                            uint32_t *next_iphv_data, uint32_t *next_iphv_vld, 
                                            uint32_t *next_ophv_data, uint32_t *next_ophv_vld, 
                                            uint32_t *mresult, uint32_t *actdatabus, 
                                            uint32_t *hashgen, uint32_t *gateway,
                                            uint32_t *ehit_raw, uint32_t *em_resbus,
                                            uint32_t *ltcam,
                                            uint32_t *iphv_data, uint32_t *iphv_vld, 
                                            uint32_t *ophv_data, uint32_t *ophv_vld, 
                                            uint32_t *phvside, uint32_t ltcamIndex, uint32_t *table_active,
                                            int *ingress_next_table, int *egress_next_table, uint32_t *action_cnt, 
                                            uint32_t *hashdist, uint32_t *alu_io,
                                            uint32_t lpf_active, uint64_t *lpf_ts, uint64_t *lpf_rng,
                                            uint32_t lpfid, uint32_t iphv_cycle, uint32_t ophv_cycle, 
                                            uint32_t *snapshot_data, uint32_t *teop_result, uint32_t *mpr_result,  int *retval) {
    // printf("Jbay Mau model process match & action PHV (at once)\n");
    MODEL_CHIP_NAMESPACE::ref_model_wrapper::getInstance()->mau_model_process_match_n_action(pipe, stage,
                                                                       next_iphv_data, next_iphv_vld, 
                                                                       next_ophv_data, next_ophv_vld, 
                                                                       mresult, actdatabus, 
                                                                       hashgen, gateway,
                                                                       ehit_raw, em_resbus,
                                                                       ltcam,
                                                                       iphv_data, iphv_vld, 
                                                                       ophv_data, ophv_vld, 
                                                                       phvside, ltcamIndex, table_active,
                                                                       ingress_next_table, egress_next_table, 
                                                                       action_cnt, hashdist, alu_io,
                                                                       lpf_active, lpf_ts, lpf_rng, lpfid, 
                                                                       iphv_cycle, ophv_cycle, snapshot_data, teop_result, mpr_result, retval);
  }


  void dpi_twlvpk_model_process_match_n_action(uint32_t pipe, uint32_t stage_start, uint32_t stage_end,
                                               uint32_t* next_iphv_data, uint32_t* next_iphv_vld, 
                                               uint32_t* next_ophv_data, uint32_t* next_ophv_vld,
                                               uint32_t* iphv_data, uint32_t* iphv_vld, 
                                               uint32_t* ophv_data, uint32_t* ophv_vld, 
                                               uint32_t phvside, 
                                               int *ingress_next_table, int *egress_next_table, 
                                               uint32_t *action_cnt,uint32_t lpfid, int *retval) { 
    uint32_t  *newphvside = new uint32_t[4]();
    newphvside[0] = phvside; newphvside[1] = 0; newphvside[2] = 0; newphvside[3] = 0;
    printf("Twlvpk model process match & action PHV (at once)\n");
    MODEL_CHIP_NAMESPACE::ref_model_wrapper::getInstance()->multipk_model_process_match_n_action(pipe, stage_start, stage_end,
                                                                          next_iphv_data, next_iphv_vld, 
                                                                          next_ophv_data, next_ophv_vld, 
                                                                          iphv_data, iphv_vld, 
                                                                          ophv_data, ophv_vld, 
                                                                          newphvside, 
                                                                          ingress_next_table, egress_next_table, 
                                                                          action_cnt,lpfid, retval);
  }


  void dpi_jbay_multipk_model_process_match_n_action(uint32_t pipe, uint32_t stage_start, uint32_t stage_end,
                                               uint32_t *next_iphv_data, uint32_t *next_iphv_vld, 
                                               uint32_t *next_ophv_data, uint32_t *next_ophv_vld,
                                               uint32_t *iphv_data, uint32_t *iphv_vld, 
                                               uint32_t *ophv_data, uint32_t *ophv_vld, 
                                               uint32_t *phvside, 
                                               int *ingress_next_table, int *egress_next_table, 
                                               uint32_t *action_cnt,uint32_t lpfid, int *retval) { 
    printf("Multipk model process match & action PHV (at once)\n");
    MODEL_CHIP_NAMESPACE::ref_model_wrapper::getInstance()->multipk_model_process_match_n_action(pipe, stage_start, stage_end,
                                                                          next_iphv_data, next_iphv_vld, 
                                                                          next_ophv_data, next_ophv_vld, 
                                                                          iphv_data, iphv_vld, 
                                                                          ophv_data, ophv_vld, 
                                                                          phvside, 
                                                                          ingress_next_table, egress_next_table, action_cnt,lpfid, retval);
  }


  void dpi_mau_model_get_match_hash(uint32_t pipe, uint32_t stage,
                                    uint32_t *hashgen,
                                    uint32_t *iphv_data, uint32_t *iphv_vld, 
                                    uint32_t *ophv_data, uint32_t *ophv_vld, 
                                    uint32_t phvside,
                                    int *ingress_next_table, int *egress_next_table, int *retval) {
    MODEL_CHIP_NAMESPACE::ref_model_wrapper::getInstance()->mau_model_get_match_hash(pipe, stage,
                                                               hashgen,
                                                               iphv_data, iphv_vld, 
                                                               ophv_data, ophv_vld, 
                                                               phvside,
                                                               ingress_next_table, egress_next_table, retval);
  }



  void dpi_mau_model_tcam_lookup(uint32_t pipe, uint32_t stage,
                                 int * hit_pri, uint32_t phvside,
                                 uint32_t *phv_data, uint32_t* phv_vld, 
                                 uint32_t row, uint32_t col) {
    printf("Mau model process MODEL_CHIP_NAMESPACE::ref_model_wrapper::getInstance()-> tcam_lookup\n");
    MODEL_CHIP_NAMESPACE::ref_model_wrapper::getInstance()->mau_model_tcam_lookup(pipe, stage,
                                                            hit_pri, phvside,
                                                            phv_data, phv_vld, 
                                                            row, col);
  }

  void dpi_mau_model_ltcam_lookup_ternary_match(uint32_t pipe, uint32_t stage,
                                                uint32_t * mresult, 
                                                int ltcamVec, uint32_t phvside,
                                                uint32_t *phv_data, uint32_t* phv_vld) {
    printf("Mau model process MODEL_CHIP_NAMESPACE::ref_model_wrapper::getInstance()-> tcam_lookup\n");
    MODEL_CHIP_NAMESPACE::ref_model_wrapper::getInstance()->mau_model_ltcam_lookup_ternary_match(pipe, stage,
                                                                           mresult,  
                                                                           ltcamVec, phvside,
                                                                           phv_data, phv_vld);
  }

  void dpi_model_update_dependencies(void) {
    printf("Pipe Model update Mau dependecies due to programming\n");
    MODEL_CHIP_NAMESPACE::ref_model_wrapper::getInstance()->pipe_model_update_dependencies();
  }

  void dpi_model_run_stateful_alu(uint32_t  pipe, uint32_t stage,
                                  uint32_t  which_alu,        /* 0 to 3 */
                                  uint32_t  addr,
                                  uint32_t* phv_data_in,     /* [4] */
                                  uint32_t* data_in,         /* [4] */
                                  uint32_t* data_out,        /* [4] */
                                  uint64_t  present_time,
                                  uint32_t* action_out       /* [4] */
                                ) {

    char     match_out[1];
    char     learn_out[1];
    uint32_t match_in[4];
    uint32_t lmatch_in[4];
    uint32_t minmax_out[1];

    match_in[0] = 0; match_in[1] = 0; match_in[2] = 0; match_in[3] = 0;
    lmatch_in[0] = 0; lmatch_in[1] = 0; lmatch_in[2] = 0; lmatch_in[3] = 0;

    MODEL_CHIP_NAMESPACE::ref_model_wrapper::getInstance()->run_stateful_alu(
        pipe, stage, which_alu, addr,
        phv_data_in,  /* [4] */
        data_in,      /* [4] */
        data_out,     /* [4] */
        present_time,
        action_out,   /* [4] */
        match_in,     /* [4] */
        lmatch_in,    /* [4] */
        match_out,    /* match_out */
        learn_out,    /* learn_out */
        0,            /* stateful_rng */
        minmax_out    /* minmax_out */
        );
  }

  void dpi_jbay_model_run_stateful_alu( uint32_t  pipe, uint32_t stage,
                                        uint32_t  which_alu,        /* 0 to 3 */
                                        uint32_t  addr,
                                        uint32_t* phv_data_in,     /* [4] */
                                        uint32_t* data_in,         /* [4] */
                                        uint32_t* data_out,        /* [4] */
                                        uint64_t  present_time,
                                        uint32_t* action_out,      /* [4] */
                                        uint32_t* match_in,        /* [4] */
                                        uint32_t* lmatch_in,       /* [4] */
                                        char*     match_out,
                                        char*     learn_out,
                                        uint32_t  stateful_rng,
                                        uint32_t* minmax_out
                                      ) {
    MODEL_CHIP_NAMESPACE::ref_model_wrapper::getInstance()->run_stateful_alu(
        pipe, stage, which_alu, addr,
        phv_data_in,  /* [4] */
        data_in,      /* [4] */
        data_out,     /* [4] */
        present_time,
        action_out,   /* [4] */
        match_in,     /* [4] */
        lmatch_in,    /* [4] */
        match_out,
        learn_out,
        stateful_rng,
        minmax_out
        );
  }


  void dpi_model_mau_movereg_op(uint32_t pipe, uint32_t stage,
                                int ltid, /* 0  to 15 */
                                int addr,
                                uint32_t ops_mask) {
    MODEL_CHIP_NAMESPACE::ref_model_wrapper::getInstance()->mau_movereg_op(pipe, stage, ltid, addr, ops_mask);
  }
  
  void dpi_model_mau_read_full_res_stats(uint32_t pipe, uint32_t stage,
                                         int ltid, /* 0  to 15 */
                                         int addr,
                                         uint32_t* data_out /* [4] 128b data byte[3:2]/pkt[1:0] */ ) {
    MODEL_CHIP_NAMESPACE::ref_model_wrapper::getInstance()->mau_read_full_res_stats(pipe, stage, ltid, addr, data_out);  
  }
  
  void dpi_model_mau_dump_full_res_stats(uint32_t pipe, uint32_t stage) {
    MODEL_CHIP_NAMESPACE::ref_model_wrapper::getInstance()->mau_dump_full_res_stats(pipe, stage);
  }
  void dpi_model_get_mau_counter(uint32_t pipe, uint32_t stage, char* name, uint32_t* value_out) {
    MODEL_CHIP_NAMESPACE::ref_model_wrapper::getInstance()->get_mau_counter(pipe,stage,name,value_out);
  } 
  void dpi_model_mau_flush_queues(uint32_t pipe, uint32_t stage){
    MODEL_CHIP_NAMESPACE::ref_model_wrapper::getInstance()->mau_flush_queues(pipe,stage);
  }  
  
  void dpi_model_tm_init_pre_mit(uint32_t chip, uint32_t pipe, uint32_t mg_id, uint32_t l1_node_ptr){
    MODEL_CHIP_NAMESPACE::ref_model_wrapper::getInstance()->tm_init_pre_mit(chip, pipe, mg_id, l1_node_ptr);
  }
  
  void dpi_model_tm_pre_enqueue(uint32_t chip, uint32_t pipe, uint32_t fifo, uint32_t mg_id1, uint32_t mg_id2, uint32_t rid, 
				uint32_t xid,  uint32_t yid, uint32_t tbl_id, uint32_t hash1, uint32_t hash2, 
				uint32_t c2c_vld,  uint32_t mg_id1_vld, uint32_t mg_id2_vld, uint32_t pkt_id) {
    MODEL_CHIP_NAMESPACE::ref_model_wrapper::getInstance()->tm_pre_enqueue(chip, pipe, fifo, mg_id1, mg_id2, rid, 
									   xid, yid, tbl_id, hash1, hash2, 
									   c2c_vld, mg_id1_vld, mg_id2_vld, 
									   pkt_id);
  }

  void dpi_model_tm_pre_dequeue(uint32_t chip, uint32_t pipe, uint32_t* rep_id, uint32_t* rid_first, uint32_t* egress_port, 
				uint32_t* deq_vld, uint32_t* pkt_id) {
    MODEL_CHIP_NAMESPACE::ref_model_wrapper::getInstance()->tm_pre_dequeue(chip, pipe, rep_id, rid_first, egress_port, deq_vld, pkt_id);
  }

  void dpi_stop_rmt_packet_coordinator(uint32_t chip) {
    MODEL_CHIP_NAMESPACE::ref_model_wrapper::getInstance()->stop_rmt_packet_coordinator(chip);
  }

  void dpi_model_tm_run_sch_model(uint32_t chip, uint32_t pipe) {
    MODEL_CHIP_NAMESPACE::ref_model_wrapper::getInstance()->tm_run_sch_model(chip, pipe);
  }

#ifdef __cplusplus
}
#endif