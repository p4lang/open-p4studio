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

#ifndef __REF_MODEL_WRAPPER_HPP__
#define __REF_MODEL_WRAPPER_HPP__
#ifdef INCLUDED_FROM_TEST_WRAPPER
#include <utests/gtest.h>
#include <rmt-object-manager.h>
#include <parser.h>
#include <parser-block.h>
#include <mau.h>
#include <mau-lookup-result.h>
#include <tcam3.h>
#include <deparser.h>
#include <deparser-block.h>
#include <register_utils.h>
#else
//#include "gtest.h"
#include "rmt-object-manager.h"
#include "parser.h"
#include "parser-block.h"
#include "mau.h"
#include "mau-lookup-result.h"
#include "tcam3.h"
#include "deparser.h"
#include "deparser-block.h"
#include "register_utils.h"
#endif

#include <tuple>
#include "dprsr_meta_data_enum.hpp"
#include "other-tm-objects.h"

namespace MODEL_CHIP_NAMESPACE {

using namespace std;

class ref_model_wrapper
{
private:
  unsigned long long           m_log_level;
  static bool                  m_wrapper_created;
  static ref_model_wrapper*    m_wrapper_instance_p;

  RmtObjectManager* m_rmt_obj_mgr_p[2];
  Parser*           m_ing_parser_inst_p[2][4][RmtDefs::kParsers];
  Parser*           m_egr_parser_inst_p[2][4][RmtDefs::kParsers];
  DeparserBlock*    m_deparser_p[2][4];
  Mau*              m_mau_inst_ary[2][RmtDefs::kStagesMax];
  unsigned int      m_n_dies;
  unsigned int      m_mau_stages;
  unsigned int      m_mau_first_stage;
  int               m_mau_first_pipe;
  Pipe*             m_pipe_inst_p[2];
  PacketReplicationEngine* m_pre_inst[2][RmtDefs::kPresTotal];
  Queueing* qing_blk[2];
  TmSchModelWrapper* m_sch_model_wrap[2][RmtDefs::kPipesMax];

  unsigned long long m_gbl_timestamp;
  unsigned long long m_gbl_version;


  ref_model_wrapper();

  void infer_my_die_id(uint32_t port_index);
  void set_my_die_id(uint32_t my_die_id);
  void do_deparser_model_process_packet(
      uint32_t chip,
      uint32_t pipe,
      Phv *phv,
      Packet *input_pkt,
      uint32_t is_ingr,
      const char *input_pkt_data,
      uint32_t orig_hdr_len,
      uint32_t &output_pkt_length,
      std::unique_ptr<uint8_t[]> &output_pkt_data,
      uint32_t &mirror_pkt_length,
      std::unique_ptr<uint8_t[]> &mirror_pkt_data,
      uint32_t input_port_num,
      uint32_t *meta_data,
      uint32_t &lfltr_pkt_valid,
      std::unique_ptr<uint8_t[]> &lfltr_pkt_data,
      uint32_t &resubmit_meta_data_valid,
      std::unique_ptr<uint8_t[]> &resubmit_meta_data,
      uint32_t &pgen_meta_data_valid,
      std::unique_ptr<uint8_t[]> &pgen_meta_data,
      uint32_t &pgen_address,
      uint32_t &pgen_length,
      uint32_t &pkt_dropped
  );

public:
  RmtObjectManager* get_rmtobjmanager_inst(uint32_t chip) { return m_rmt_obj_mgr_p[chip]; }
  Parser* get_parser_inst(uint32_t chip,uint32_t pipe, uint32_t prsr_inst, uint32_t prsr_type);
  Mau *   get_mau_stage(uint32_t pipe, uint32_t stage);
  Pipe*   get_pipe_int(uint32_t chip)    { return m_pipe_inst_p[chip]; }

  static ref_model_wrapper* getInstance(bool force=false);
  static void deleteInstance();

  void parser_model_process_packet(uint32_t pipe, uint32_t prsr_inst, uint32_t prsr_type, uint32_t chan, uint8_t version,
                                   uint32_t congested, const char* packet,
                                   uint32_t* phv_data, uint32_t* phv_vld,
                                   uint32_t* phvt_data, uint32_t* phvt_vld,
                                   uint32_t* payload_offset, uint32_t* drop,
                                   uint32_t* pri, uint32_t* err_flags);

  void parser_model_process_packet_clot(uint32_t chip, uint32_t pipe, uint32_t prsr_inst, uint32_t prsr_type, uint32_t chan, uint8_t version,
					uint32_t congested, const char* packet,
					uint32_t* phv_data, uint32_t* phv_vld,
					uint32_t* clot_data, uint32_t* clot_vld,
					uint32_t* payload_offset, uint32_t* drop,
					uint32_t* pri, uint32_t* err_flags, uint32_t* counter_data, uint8_t* ref_version);

  void host_if_reset();

  ~ref_model_wrapper();

  /* Data passing functions */
Phv * create_phv_from_data(const uint8_t  egress,
                           const uint8_t  pktver,
                           const uint8_t  hdrport,
                           const uint8_t  hrecirc,
                           const uint32_t *phv_data,
                           const uint32_t *phv_vld );

void extract_data_from_phv(Phv *phv,
                           uint8_t  egress,
                           uint32_t *phv_data,
                           uint32_t *phv_vld);

/* Mau functions */
Phv* create_mau_phv( const uint32_t *phvside );

Phv * create_mau_phv_from_data(const uint32_t *phvside,
                               const uint32_t* phv_data,
                               const uint32_t* phv_vld );

void extract_data_from_mau_phv(Phv *phv,
                               uint8_t  egress,
                               uint32_t* phv_data,
                               uint32_t* phv_vld);

void set_mauio_via_phvside(MauIO *mauio, const uint32_t *phvside);
void get_mauio_into_phvside(const MauIO *mauio, uint32_t *phvside);

void extract_mau_lookup_result(const MauLookupResult *mres,
                               uint32_t* matchd);

  void mau_model_reset_resources(uint32_t pipe, uint32_t stage) {
          Mau *   mau_p = get_mau_stage(pipe, stage);
          mau_p->reset_resources(); }

  void mau_model_handle_sweep(uint32_t pipe, uint32_t stage, uint32_t m_id,
                              uint32_t m_cycle, uint64_t meter_sweep_time,
                              uint32_t sweep_ltid, uint32_t home_row,
                              uint32_t sweep_indx, uint32_t *sweep_addr, uint32_t *sweep_dout);

  void mau_model_handle_eop(uint32_t pipe, uint32_t stage,
                            uint32_t phvside, uint64_t *ts, uint64_t *rng,
                            uint32_t active, uint32_t *data_in, uint32_t *color,
                            uint32_t eopid, uint32_t eop_cycle, int *retval);
  void mau_model_handle_dteop(uint32_t pipe, uint32_t stage,
                              uint32_t phvside, uint32_t *dteop, uint64_t *ts, uint64_t *rng,
                              uint32_t active, uint32_t *data_in, uint32_t *color,
                              uint32_t eopid, uint32_t eop_cycle, int *retval);

  void mau_model_disable_stateful_alu_checks(uint32_t pipe, uint32_t stage);

  void mau_model_relax_mau_tcam_array( );

  void mau_model_run_mau_tcam_array( uint32_t pipe, uint32_t stage,
                                      uint32_t *phvside,
                                      int      *next_table,
				      uint64_t *kIxbarTmData,
                                      uint32_t *tcam_match_addr,
                                      int      *tcam_match_hit,
                                      uint32_t iphv_cycle, uint32_t ophv_cycle,
                                      int      *retval );

  void mau_model_run_mau_memory_core( uint32_t pipe, uint32_t stage,
                                      uint32_t *phvside,
                                      int      *next_table,
                                      uint32_t *tcam_match_addr,
                                      uint32_t iphv_cycle, uint32_t ophv_cycle,
                                      int      *retval );

  void get_table_active( uint32_t pipe, uint32_t stage, uint32_t *table_active );

  void mau_model_process_match_n_action(uint32_t pipe, uint32_t stage,
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
                                        uint32_t lpf_active, uint64_t *lpf_ts, uint64_t *lpf_rng,uint32_t lpfid,
                                        uint32_t iphv_cycle, uint32_t ophv_cycle,
                                        uint32_t *snapshot_data, uint32_t *teop_result, uint32_t *mpr_result,  int *retval);

  void multipk_model_process_match_n_action(uint32_t pipe, uint32_t stage_start, uint32_t stage_end,
                                           uint32_t *next_iphv_data, uint32_t *next_iphv_vld,
                                           uint32_t *next_ophv_data, uint32_t *next_ophv_vld,
                                           uint32_t *iphv_data, uint32_t *iphv_vld,
                                           uint32_t *ophv_data, uint32_t *ophv_vld,
                                           uint32_t *phvside,
                                           int *ingress_next_table, int *egress_next_table,
                                           uint32_t *action_cnt, uint32_t lpfid, int *retval);

  void mau_model_tcam_lookup(uint32_t pipe, uint32_t stage,
                             int *hit_pri,  uint32_t phvside,
                             uint32_t *phv_data, uint32_t *phv_vld,
                             uint32_t rowI, uint32_t colI);
  void mau_model_ltcam_lookup_ternary_match(uint32_t pipe, uint32_t stage,
                                            uint32_t *mresult,
                                            int ltcamVec, uint32_t phvside,
                                            const uint32_t *phv_data, const uint32_t *phv_vld);
  void mau_model_get_match_hash(uint32_t pipe, uint32_t stage,
                                uint32_t *hashgen,
                                uint32_t *iphv_data, uint32_t *iphv_vld,
                                uint32_t *ophv_data, uint32_t *ophv_vld,
                                uint32_t phvside,
                                int *ingress_next_table, int *egress_next_table, int *retval);

  /* Pipe Model for Mau */
  void pipe_model_update_dependencies();

  void  deparser_model_process_packet(uint32_t is_ingr,
				      uint32_t* phv_data, uint32_t* phv_vld,
				      uint32_t* phvt_data, uint32_t* phvt_vld,
				      uint32_t* mau_err,
				      const char* input_pkt_data, uint32_t orig_hdr_len, uint32_t version,
                                      uint32_t &output_pkt_length, std::unique_ptr<uint8_t[]> &output_pkt_data,
                                      uint32_t &mirror_pkt_length, std::unique_ptr<uint8_t[]> &mirror_pkt_data,
                                      uint32_t input_port_num, uint32_t* meta_data,
                                      uint32_t &lfltr_pkt_valid, std::unique_ptr<uint8_t[]> &lfltr_pkt_data,
                                      uint32_t &resubmit_meta_data_valid, std::unique_ptr<uint8_t[]> &resubmit_meta_data,
                                      uint32_t &pkt_dropped
                                     );

  void  deparser_model_process_packet_clot(uint32_t chip, uint32_t pipe, uint32_t is_ingr,
					   uint32_t* phv_data, uint32_t* phv_vld,
					   uint32_t* clot_data, uint32_t* clot_vld,
					   uint32_t* mau_err,
					   const char* input_pkt_data, uint32_t orig_hdr_len, uint32_t version,
					   uint32_t &output_pkt_length, std::unique_ptr<uint8_t[]> &output_pkt_data,
					   uint32_t &mirror_pkt_length, std::unique_ptr<uint8_t[]> &mirror_pkt_data,
					   uint32_t input_port_num, uint32_t* meta_data,
					   uint32_t &lfltr_pkt_valid, std::unique_ptr<uint8_t[]> &lfltr_pkt_data,
					   uint32_t &resubmit_meta_data_valid, std::unique_ptr<uint8_t[]> &resubmit_meta_data,
						uint32_t &pgen_meta_data_valid, std::unique_ptr<uint8_t[]> &pgen_meta_data, uint32_t &pgen_address, uint32_t &pgen_length,
					   uint32_t &pkt_dropped
					   );

  void run_stateful_alu(
      int pipe, int stage,
      int which_alu              /* 0  to 3 */,
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

  void mau_movereg_op(uint32_t pipe, uint32_t stage,
                      int ltid, /* 0  to 15 */
                      int addr,
                      uint32_t ops_mask);

  void mau_read_full_res_stats(uint32_t pipe, uint32_t stage,
                               int ltid, /* 0  to 15 */
                               int addr,
                               uint32_t* data_out  /* [4] 128b data byte[3:2]/pkt[1:0] */ );
  void mau_dump_full_res_stats(uint32_t pipe, uint32_t stage);

  void get_mau_counter(uint32_t pipe, uint32_t stage, char* name, uint32_t* value_out);

  void set_relax_checks(uint32_t chip);
  int set_option(uint32_t chip, char* name, char *option);
  void mau_flush_queues(uint32_t pipe, uint32_t stage);

  PacketReplicationEngine* get_tm_pre_inst(uint32_t chip,uint32_t pipe);
  void stop_rmt_packet_coordinator(uint32_t chip);
  void tm_init_pre_mit(uint32_t chip, uint32_t pipe, uint32_t mg_id, uint32_t l1_node_ptr);
  void tm_pre_enqueue(uint32_t chip, uint32_t pipe, uint32_t fifo, uint32_t mg_id1, uint32_t mg_id2, uint32_t rid,
		      uint32_t xid, uint32_t yid, uint32_t tbl_id, uint32_t hash1, uint32_t hash2,
		      uint32_t c2c_vld, uint32_t mg_id1_vld, uint32_t mg_id2_vld, uint32_t pkt_id);
  void tm_pre_dequeue(uint32_t chip, uint32_t pipe, uint32_t* rep_id, uint32_t* rid_first, uint32_t* egress_port,
		      uint32_t* deq_vld, uint32_t* pkt_id);
  void tm_run_sch_model(uint32_t chip, uint32_t pipe);

}; // endclass

}
#endif