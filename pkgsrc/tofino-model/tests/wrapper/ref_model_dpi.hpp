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

#ifndef __REF_MODEL_DPI_H__
#define __REF_MODEL_DPI_H__

#include <stdint.h>
#include <tuple>

// Following routines are imported to SV through DPI.  Thus, C linkage.
#ifdef __cplusplus
extern "C" {
#endif
  extern void dpi_model_wrapper_create();
  extern void dpi_model_wrapper_destroy();
  extern void dpi_parser_model_process_packet(uint32_t pipe, uint32_t prsr_inst, uint32_t prsr_type, uint32_t chan, uint32_t version, 
                                              uint32_t congested, const char* data, 
                                              uint32_t* phv_data, uint32_t* phv_vld, 
                                              uint32_t* phvt_data, uint32_t* phvt_vld, 
                                              uint32_t* payload_offset, uint32_t* drop, 
                                              uint32_t* pri, uint32_t* err_flags);
  extern void dpi_parser_model_process_packet_clot(uint32_t chip, uint32_t pipe, uint32_t prsr_inst, uint32_t prsr_type, uint32_t chan, uint32_t version, 
						   uint32_t congested, const char* data, 
						   uint32_t* phv_data, uint32_t* phv_vld, 
						   uint32_t* clot_data, uint32_t* clot_vld, 
						   uint32_t* payload_offset, uint32_t* drop, 
						   uint32_t* pri, uint32_t* err_flags, uint32_t* counter_data, uint8_t* ref_version);
  extern void dpi_deparser_model_process_ing_packet(uint32_t* phv_data, uint32_t* phv_vld, 
                                                                            uint32_t* phvt_data, uint32_t* phvt_vld,
						    uint32_t* ing_mau_err,
						    const char* input_pkt_data, uint32_t orig_hdr_len, uint32_t version,
                                                    uint32_t* i_pkt_length, char* i_pkt_data,
                                                    uint32_t* i_mirror_pkt_length, char* i_mirror_pkt_data,
                                                    uint32_t input_port_num, uint32_t* meta_data,
                                                    uint32_t* lfltr_pkt_valid, char* lfltr_pkt_data,
                                                    uint32_t* resubmit_data_valid, char* resubmit_meta_data,
                                                    uint32_t* pkt_dropped
                                                   );
  extern void dpi_deparser_model_process_egr_packet(uint32_t* phv_data, uint32_t* phv_vld, 
                                                                            uint32_t* phvt_data, uint32_t* phvt_vld,
						    uint32_t* egr_mau_err,
						    const char* input_pkt_data, uint32_t orig_hdr_len, uint32_t version,
                                                    uint32_t* e_pkt_length, char* e_pkt_data,
                                                    uint32_t* e_mirror_pkt_length, char* e_mirror_pkt_data,
                                                    uint32_t input_port_num, uint32_t* meta_data,
                                                    uint32_t* pkt_dropped
                                                   );
  extern void dpi_deparser_model_process_ing_packet_clot(uint32_t chip, uint32_t pipe, uint32_t* phv_data, uint32_t* phv_vld, 
                                                                            uint32_t* clot_data, uint32_t* clot_vld,
						    uint32_t* ing_mau_err,
						    const char* input_pkt_data, uint32_t orig_hdr_len, uint32_t version,
                                                    uint32_t* i_pkt_length, char* i_pkt_data,
                                                    uint32_t* i_mirror_pkt_length, char* i_mirror_pkt_data,
                                                    uint32_t input_port_num, uint32_t* meta_data,
                                                    uint32_t* lfltr_pkt_valid, char* lfltr_pkt_data,
                                                    uint32_t* resubmit_data_valid, char* resubmit_meta_data,
                                                    uint32_t* pgen_meta_data_valid, char* pgen_meta_data, uint32_t* pgen_address, uint32_t* pgen_length,
                                                    uint32_t* pkt_dropped
                                                   );
  extern void dpi_deparser_model_process_egr_packet_clot(uint32_t chip, uint32_t pipe, uint32_t* phv_data, uint32_t* phv_vld, 
                                                                            uint32_t* clot_data, uint32_t* clot_vld,
						    uint32_t* egr_mau_err,
						    const char* input_pkt_data, uint32_t orig_hdr_len, uint32_t version,
                                                    uint32_t* e_pkt_length, char* e_pkt_data,
                                                    uint32_t* e_mirror_pkt_length, char* e_mirror_pkt_data,
                                                    uint32_t input_port_num, uint32_t* meta_data,
                                                    uint32_t* pkt_dropped
                                                   );


  /* Mau memory address functions */
  extern void dpi_mau_model_make_phys_address(uint32_t* addr_hi, uint32_t* addr_lo, 
                                              int pipe, int stage, int memType,
                                              int row, int col, int index);
  extern void dpi_mau_model_get_pipe_stage_csr_base_addr(uint32_t* addr, 
                                                         int pipe, int stage);
  extern void dpi_mau_model_get_pipe_stage_csr_last_addr(uint32_t* addr, 
                                                         int pipe, int stage);
  /* Mau ones */
  extern void dpi_mau_model_reset_resources(uint32_t pipe, uint32_t stage);
  extern void dpi_mau_model_handle_eop(uint32_t pipe, uint32_t stage, uint32_t phvside, uint64_t *ts, uint64_t *rng, uint32_t active, uint32_t *data_in, uint32_t *color, uint32_t eopid,uint32_t eop_cycle, int *retval);
  extern void dpi_mau_model_handle_dteop(uint32_t pipe, uint32_t stage, uint32_t phvside, uint32_t *dteop, uint64_t *ts, uint64_t *rng, uint32_t active, uint32_t *data_in, uint32_t *color, uint32_t eopid,uint32_t eop_cycle, int *retval);
  extern void dpi_mau_model_handle_sweep(uint32_t pipe, uint32_t stage, uint32_t m_id,uint32_t m_cycle,uint64_t meter_sweep_time, uint32_t sweep_ltid,uint32_t home_row, uint32_t sweep_indx, uint32_t *sweep_addr,uint32_t *sweep_dout);


  extern void dpi_mau_model_disable_stateful_alu_checks(uint32_t pipe, uint32_t stage);

  extern void dpi_mau_model_relax_mau_tcam_array( );

  extern void dpi_mau_model_run_mau_tcam_array( uint32_t pipe, uint32_t stage,
                                                 uint32_t *phvside,
                                                 int      *next_table,
						 uint64_t *kIxbarTmData,
                                                 uint32_t *tcam_match_addr,
                                                 int      *tcam_match_hit,
                                                 uint32_t iphv_cycle, uint32_t ophv_cycle,
                                                 int      *retval );

  extern void dpi_mau_model_run_mau_memory_core( uint32_t pipe, uint32_t stage,
                                                 uint32_t *phvside,
                                                 int      *next_table,
                                                 uint32_t *tcam_match_addr,
                                                 uint32_t iphv_cycle, uint32_t ophv_cycle,
                                                 int      *retval );

  /* get functions */
  extern void dpi_mau_model_get_table_active( uint32_t pipe, uint32_t stage, uint32_t *table_active ); 

  extern void dpi_mau_model_process_match_n_action(uint32_t pipe, uint32_t stage,
                                                   uint32_t *next_iphv_data, uint32_t *next_iphv_vld, 
                                                   uint32_t *next_ophv_data, uint32_t *next_ophv_vld, 
                                                   uint32_t *mresult, uint32_t *actdatabus, 
                                                   uint32_t *hashgen, uint32_t *gateway,
                                                   uint32_t *ehit_raw, uint32_t *em_resbus,
                                                   uint32_t *ltcam,
                                                   uint32_t *iphv_data, uint32_t *iphv_vld, 
                                                   uint32_t *ophv_data, uint32_t *ophv_vld, 
                                                   uint32_t phvside, uint32_t ltcamIndex, uint32_t *table_active,
                                                   int *ingress_next_table, int *egress_next_table, 
                                                   uint32_t *action_cnt, uint32_t *hashdist, uint32_t *alu_io,
                                                   uint32_t lpf_active, uint64_t *lpf_ts, uint64_t *lpf_rng, 
                                                   uint32_t lpfid,uint32_t iphv_cycle, uint32_t ophv_cycle, 
                                                   int *retval);
  extern void dpi_jbay_mau_model_process_match_n_action(uint32_t pipe, uint32_t stage,
                                                   uint32_t *next_iphv_data, uint32_t *next_iphv_vld, 
                                                   uint32_t *next_ophv_data, uint32_t *next_ophv_vld, 
                                                   uint32_t *mresult, uint32_t *actdatabus, 
                                                   uint32_t *hashgen, uint32_t *gateway,
                                                   uint32_t *ehit_raw, uint32_t *em_resbus,
                                                   uint32_t *ltcam,
                                                   uint32_t *iphv_data, uint32_t *iphv_vld, 
                                                   uint32_t *ophv_data, uint32_t *ophv_vld, 
                                                   uint32_t *phvside, uint32_t ltcamIndex, uint32_t *table_active,
                                                   int *ingress_next_table, int *egress_next_table, 
                                                   uint32_t *action_cnt, uint32_t *hashdist, uint32_t *alu_io,
                                                   uint32_t lpf_active, uint64_t *lpf_ts, uint64_t *lpf_rng, 
                                                   uint32_t lpfid,uint32_t iphv_cycle, uint32_t ophv_cycle,
                                                   uint32_t *snapshot_data, uint32_t *teop_result, uint32_t *mpr_result,  int *retval);

  extern void dpi_twlvpk_model_process_match_n_action(uint32_t pipe, uint32_t stage_start, uint32_t stage_end,
                                                      uint32_t* next_iphv_data, uint32_t* next_iphv_vld, 
                                                      uint32_t* next_ophv_data, uint32_t* next_ophv_vld,
                                                      uint32_t* iphv_data, uint32_t* iphv_vld, 
                                                      uint32_t* ophv_data, uint32_t* ophv_vld, 
                                                      uint32_t phvside, 
                                                      int *ingress_next_table, int *egress_next_table, 
                                                      uint32_t *action_cnt, uint32_t lpfid, int *retval);
  extern void dpi_jbay_multipk_model_process_match_n_action(uint32_t pipe, uint32_t stage_start, uint32_t stage_end,
                                                            uint32_t* next_iphv_data, uint32_t* next_iphv_vld, 
                                                            uint32_t* next_ophv_data, uint32_t* next_ophv_vld,
                                                            uint32_t* iphv_data, uint32_t* iphv_vld, 
                                                            uint32_t* ophv_data, uint32_t* ophv_vld, 
                                                            uint32_t* phvside, 
                                                            int *ingress_next_table, int *egress_next_table, 
                                                            uint32_t *action_cnt, uint32_t lpfid, int *retval);
  
  extern void dpi_mau_model_get_match_hash(uint32_t pipe, uint32_t stage,
                                           uint32_t *hashgen,
                                           uint32_t *iphv_data, uint32_t *iphv_vld, 
                                           uint32_t *ophv_data, uint32_t *ophv_vld, 
                                           uint32_t phvside,
                                           int *ingress_next_table, int *egress_next_table, int *retval);
  extern void dpi_mau_model_tcam_lookup(uint32_t pipe, uint32_t stage,
                                        int *hit_pri,  uint32_t phvside,
                                        uint32_t *phv_data, uint32_t *phv_vld, 
                                        uint32_t row, uint32_t col);
  extern void dpi_mau_model_ltcam_lookup_ternary_match(uint32_t pipe, uint32_t stage,
                                                       uint32_t *mresult, 
                                                       int ltcamVec, uint32_t phvside,
                                                       uint32_t *phv_data, uint32_t *phv_vld);
  extern void dpi_model_update_dependencies(void);
  extern void dpi_model_run_stateful_alu( uint32_t pipe, uint32_t stage,
                                          uint32_t which_alu,        /* 0 to 3 */
                                          uint32_t addr,
                                          uint32_t* phv_data_in,     /* [4] */
                                          uint32_t* data_in,         /* [4] */
                                          uint32_t* data_out,        /* [4] */
					                                uint64_t  present_time,
                                          uint32_t* action_out       /* [4] */
                                         );
  extern void dpi_jbay_model_run_stateful_alu( uint32_t pipe, uint32_t stage,
                                               uint32_t which_alu,        /* 0 to 3 */
                                               uint32_t addr,
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
                                              );
  extern void dpi_model_mau_movereg_op(uint32_t pipe, uint32_t stage,
                                       int ltid, /* 0  to 15 */
                                       int addr,
                                       uint32_t ops_mask);
  extern void dpi_model_mau_read_full_res_stats(uint32_t pipe, uint32_t stage,
                                                int ltid, /* 0  to 15 */
                                                int addr,
                                                uint32_t* data_out    /* [4] 128b data byte[3:2]/pkt[1:0] */
          );
  extern void dpi_model_mau_dump_full_res_stats(uint32_t pipe, uint32_t stage);
  extern void dpi_model_get_mau_counter(uint32_t pipe, uint32_t stage, char* name, uint32_t* value_out);
  extern void dpi_model_mau_flush_queues(uint32_t pipe, uint32_t stage);

  extern void dpi_model_tm_init_pre_mit(uint32_t chip, uint32_t pipe, uint32_t mg_id, uint32_t l1_node_ptr);
  extern void dpi_model_tm_pre_enqueue(uint32_t chip, uint32_t pipe, uint32_t fifo, uint32_t mg_id1, uint32_t mg_id2, uint32_t rid, 
				       uint32_t xid,  uint32_t yid, uint32_t tbl_id, uint32_t hash1, uint32_t hash2, 
				       uint32_t c2c_vld, uint32_t mg_id1_vld, uint32_t mg_id2_vld, uint32_t pkt_id);
  extern void dpi_model_tm_pre_dequeue(uint32_t chip, uint32_t pipe, uint32_t* rep_id, uint32_t* rid_first, uint32_t* egress_port, 
				       uint32_t* deq_vld, uint32_t* pkt_id);
  extern void dpi_stop_rmt_packet_coordinator(uint32_t chip);
  extern void dpi_model_tm_run_sch_model(uint32_t chip, uint32_t pipe); 
  
#ifdef __cplusplus
}
#endif

#endif