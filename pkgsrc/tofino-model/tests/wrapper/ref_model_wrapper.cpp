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

#include <cinttypes>
#include "ref_model_wrapper.hpp"
#include "model_core/model.h"
#include "chip.h"
#include "clot.h"
#include "packet.h"
#include "phv.h"
#include "port.h"
#include "mau-meter-alu.h"
#include "mau-lookup-result.h"
#include "packet-replication-engine.h"
#include "rmt-packet-coordinator.h"
#include "packet-pipe-data.h"
#include "wrapper/sknobs.h"

#include "ref_model_wrapper_mau.cpp"

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;

namespace MODEL_CHIP_NAMESPACE {

uint64_t MODEL_ALL_DEBUG_FLAGS = UINT64_C(0xFFFFFFFFFFFFFFFF);

bool ref_model_wrapper::m_wrapper_created = false;
ref_model_wrapper* ref_model_wrapper::m_wrapper_instance_p = NULL;


ref_model_wrapper::ref_model_wrapper() {
  char name[] = "ref_model_wrapper.log_level";
  char name2[] = "ref_model_wrapper.model_debug_flags";
  char name3[] = "ref_model_wrapper.model_all_flags";
  char name4[] = "n_stages";
  char name5[] = "test.mau_first_stage";
  char name6[] = "test.mau_first_pipe";
  char name7[] = "ref_model_wrapper.n_dies";
  int chip_id_arr[4] = {-1, -1, -1, -1};
  m_mau_stages = sknobs_get_value(name4, 1);
  m_n_dies = 1;
  m_mau_first_stage = 0;
  m_mau_first_pipe  = 0;
  if ( m_mau_stages > RmtDefs::kStagesMax ) {
    printf("\nERROR ref_model_wrapper passed bad n_stages=%d  reset to %d.\n",
           m_mau_stages, RmtDefs::kStagesMax);
    m_mau_stages = RmtDefs::kStagesMax;
  } else {
    printf("\nref_model_wrapper n_stages=%d\n", m_mau_stages);
  }
  if (m_mau_stages == 1) {
     m_mau_first_stage = sknobs_get_value(name5, m_mau_first_stage);
     if (m_mau_first_stage >= RmtDefs::kStagesMax) {
         printf("ERROR ref_model_wrapper bad mau_first_stage=%0d > %0d for n_stages=%0d reset to 0\n",
                m_mau_first_stage, RmtDefs::kStagesMax, m_mau_stages);
         m_mau_first_stage = 0;
     } else {
         printf("ref_model_wrapper test.mau_first_stage=%d\n", m_mau_first_stage);
     }
  }
  m_mau_first_pipe = sknobs_get_value(name6, m_mau_first_pipe);
  m_n_dies = sknobs_get_value(name7, 1);
  assert(m_n_dies > 0);
  if (m_mau_first_pipe >= RmtDefs::kPipesMaxEver) {
     printf("ERROR ref_model_wrapper bad mau_first_pipe=%0d > %0d reset to 0\n",
            m_mau_first_pipe, RmtDefs::kPipesMaxEver);
     m_mau_first_pipe = 0;
  } else {
     printf("ref_model_wrapper  test.mau_first_pipe=%d\n", m_mau_first_pipe);
  }

  GLOBAL_MODEL->Reset();
  for (uint32_t chip=0; chip < m_n_dies; chip++) {
    GLOBAL_MODEL->DestroyChip(chip);
    GLOBAL_MODEL->CreateChip(chip, RmtDefs::kChipType);
    chip_id_arr[chip] = chip;
  }
  GLOBAL_MODEL->SetPackage(chip_id_arr[0], chip_id_arr[1], chip_id_arr[2], chip_id_arr[3]);
  for (uint32_t chip=0; chip < m_n_dies; chip++) {
    GLOBAL_MODEL->InitChip(chip);
    GLOBAL_MODEL->GetObjectManager(chip, &m_rmt_obj_mgr_p[chip]);
    assert(m_rmt_obj_mgr_p[chip] != NULL);
  }
  printf("ref_model_wrapper build %d dies\n", m_n_dies);

  for (uint32_t chip=0; chip < m_n_dies; chip++) {
    for (uint32_t pipe=0; pipe < 4; pipe++) {
      for (uint32_t prsr_inst=0; prsr_inst < RmtDefs::kParsers; prsr_inst++) {
        m_ing_parser_inst_p[chip][pipe][prsr_inst] = m_rmt_obj_mgr_p[chip]->parser_get(pipe,prsr_inst)->ingress();
        m_egr_parser_inst_p[chip][pipe][prsr_inst] = m_rmt_obj_mgr_p[chip]->parser_get(pipe,prsr_inst)->egress();
      }
    }
  }
  for (uint32_t chip=0; chip < m_n_dies; chip++) {
    m_pipe_inst_p[chip]  = m_rmt_obj_mgr_p[chip]->pipe_get(m_mau_first_pipe, m_rmt_obj_mgr_p[chip]->kMausPerPipe);
    for (uint32_t pipe=0; pipe < 4; pipe++) {
      m_deparser_p[chip][pipe]   = m_rmt_obj_mgr_p[chip]->deparser_get(pipe);
    }
  }

  for (uint32_t chip=0; chip < m_n_dies; chip++) {
    for (uint32_t i0 = m_mau_first_stage ; i0 < (m_mau_stages + m_mau_first_stage) ; i0++) {
      m_mau_inst_ary[chip][i0] = m_rmt_obj_mgr_p[chip]->mau_lookup(m_mau_first_pipe, i0);
      m_mau_inst_ary[chip][i0]->set_evaluate_all(true);
      printf("ref_model_wrapper mau pipe=%0d stage=%d to %0d enabled. total=%0d \n",
             m_mau_first_pipe, i0, (m_mau_stages + m_mau_first_stage), m_mau_stages);
    }
  }
  for (uint32_t chip=0; chip < m_n_dies; chip++) {
    for (uint8_t pre_inst=0; pre_inst < RmtDefs::kPresTotal; pre_inst++) {
      m_pre_inst[chip][pre_inst] = m_rmt_obj_mgr_p[chip]->pre_get(pre_inst);
      m_pre_inst[chip][pre_inst]->reset();
    }
    qing_blk[chip] = m_rmt_obj_mgr_p[chip]->queueing_get();
  }
  for (uint32_t chip=0; chip < m_n_dies; chip++) {
    for (uint8_t sch_inst = 0; sch_inst < RmtDefs::kPipesMax; sch_inst++)
      m_sch_model_wrap[chip][sch_inst] = m_rmt_obj_mgr_p[chip]->sch_model_wrap_get(sch_inst);
  }
  // Logging
  m_log_level = 0;
  /*
    typedef enum
    {
    UVM_NONE   = 0,
    UVM_LOW    = 100,
    UVM_MEDIUM = 200,
    UVM_HIGH   = 300,
    UVM_FULL   = 400,
    UVM_DEBUG  = 500
    } uvm_verbosity;
   */
  m_log_level = sknobs_get_value(name, 0);

  char gbl_ts[] = "top.gbl_timestamp";
  m_gbl_timestamp = sknobs_get_value(gbl_ts, 0);

  char gbl_ver[] = "top.gbl_version";
  m_gbl_version   = sknobs_get_value(gbl_ver, 0);

  uint64_t MODEL_DEBUG_FLAGS =
    RmtDebug::kRmtDebugParserExtractError|
    RmtDebug::kRmtDebugParserError|
    RmtDebug::kRmtDebugParserParseLoop|
    RmtDebug::kRmtDebugParserParseNoMatch|
    RmtDebug::kRmtDebugParserParseMatch |
    RmtDebug::kRmtDebugParserPriority|
    RmtDebug::kRmtDebugExit|
    RmtDebug::kRmtDebugCsumEngCalcFastB0B1|
    RmtDebug::kRmtDebugCsumEngCalcFin|
    RmtDebug::kRmtDebugCsumEngCalcBefore|
    RmtDebug::kRmtDebugDeparser1     |
    RmtDebug::kRmtDebugDeparser2     |
    RmtDebug::kRmtDebugDeparser4     |
    RmtDebug::kRmtDebugDeparser8     |
    RmtDebug::kRmtDebugDeparserBlock1|
    RmtDebug::kRmtDebugDeparserBlock2|
    RmtDebug::kRmtDebugDeparserBlock4|
    RmtDebug::kRmtDebugDeparserBlock8;

  if (m_log_level >= 200) {
    MODEL_DEBUG_FLAGS |= RmtDebug::kRmtDebugParserExtract;
  }
  if (m_log_level >= 901) {
    for (uint32_t chip=0; chip < m_n_dies; chip++) {
      GLOBAL_MODEL->SetPrintWrites(chip,true); // print the writes from the chip.cpp until DV  do it properly from SystemVerilog
    }
  }

  MODEL_DEBUG_FLAGS = sknobs_get_value(name2, MODEL_DEBUG_FLAGS);

  printf("ref_model_wrapper.log_level        : %lld\n",   m_log_level);
  printf("ref_model_wrapper.model_debug_flags: 0x%" PRIx64 "\n", MODEL_DEBUG_FLAGS);
  printf("\nREF_MODEL_WRAPPER gbl_timestamp=0x%llx, gbl_version=0x%llx\n", m_gbl_timestamp, m_gbl_version);

  for (uint32_t chip=0; chip < m_n_dies; chip++) {
    m_rmt_obj_mgr_p[chip]->set_log_flags(0,0,0,0,0,MODEL_DEBUG_FLAGS);
  }

  for (uint32_t chip=0; chip < m_n_dies; chip++) {
    for (uint32_t pipe=0; pipe < 4; pipe++) {
      for (uint32_t prsr_inst=0; prsr_inst < RmtDefs::kParsers; prsr_inst++) {
        m_ing_parser_inst_p[chip][pipe][prsr_inst]->timestamp_set(m_gbl_timestamp);
        m_ing_parser_inst_p[chip][pipe][prsr_inst]->version_set(m_gbl_version);

        m_egr_parser_inst_p[chip][pipe][prsr_inst]->timestamp_set(m_gbl_timestamp);
        m_egr_parser_inst_p[chip][pipe][prsr_inst]->version_set(m_gbl_version);
      }
    }
  }


  //Fatal/Error/Warning by default
  for (uint32_t chip=0; chip < m_n_dies; chip++) {
    m_rmt_obj_mgr_p[chip]->update_log_flags(MODEL_ALL_DEBUG_FLAGS, MODEL_ALL_DEBUG_FLAGS,
                                     MODEL_ALL_DEBUG_FLAGS, MODEL_ALL_DEBUG_FLAGS,
                                     MODEL_ALL_DEBUG_FLAGS, MODEL_DEBUG_FLAGS,
                                     MODEL_DEBUG_FLAGS);
    if ( sknobs_get_value(name3, 0) ) {
      printf("\nREF_MODEL_WRAPPER enable all model flags ....\n");
      m_rmt_obj_mgr_p[chip]->update_log_flags(MODEL_ALL_DEBUG_FLAGS, MODEL_ALL_DEBUG_FLAGS,
                                        MODEL_ALL_DEBUG_FLAGS, MODEL_ALL_DEBUG_FLAGS,
                                        MODEL_ALL_DEBUG_FLAGS, MODEL_ALL_DEBUG_FLAGS,
                                        MODEL_ALL_DEBUG_FLAGS);
    }
    set_relax_checks(chip);
  }

}

ref_model_wrapper::~ref_model_wrapper() {
  m_wrapper_created = false;
}

void ref_model_wrapper::infer_my_die_id(uint32_t port_index) {
  if (!RmtObject::is_chip1()) return;
  int my_die_id = Port::get_die_num(port_index);
  printf("Inferred chip my_die_id %d from port index %d\n", my_die_id, port_index);
  set_my_die_id(my_die_id);
}

void ref_model_wrapper::set_my_die_id(uint32_t my_die_id) {
  if (!RmtObject::is_chip1()) return;
  if (my_die_id >= m_n_dies) return;
  m_rmt_obj_mgr_p[my_die_id]->chip()->SetMyDieId(my_die_id);
  printf("Set chip my_die_id to %d\n", my_die_id);
}

Parser* ref_model_wrapper::get_parser_inst(uint32_t chip, uint32_t pipe, uint32_t prsr_inst, uint32_t prsr_type) {
  if (prsr_type == 0) {
    return m_ing_parser_inst_p[chip][pipe][prsr_inst];
  }
  else {
    return m_egr_parser_inst_p[chip][pipe][prsr_inst];
  }
}

ref_model_wrapper* ref_model_wrapper::getInstance(bool force) {
  if((!m_wrapper_created) || force) {
    printf("In getInstance m_wrapper_created=%1d", m_wrapper_created);
    deleteInstance();
    m_wrapper_instance_p = new ref_model_wrapper();
    m_wrapper_created    = true;
    return m_wrapper_instance_p;
  }
  else {
    // printf("In getInstance m_wrapper_created=%1d", m_wrapper_created);
    return m_wrapper_instance_p;
  }
}

void ref_model_wrapper::deleteInstance() {
  if (nullptr == m_wrapper_instance_p) return;
  delete m_wrapper_instance_p;
  m_wrapper_instance_p = nullptr;
  m_wrapper_created = false;
}

void ref_model_wrapper::host_if_reset() {
  GLOBAL_MODEL->Reset();
  for (uint32_t chip=0; chip < m_n_dies; chip++) {
    GLOBAL_MODEL->DestroyChip(chip);
    GLOBAL_MODEL->CreateChip(chip, RmtDefs::kChipType);
    GLOBAL_MODEL->InitChip(chip);
  }
  //assert(0);
}


void ref_model_wrapper::parser_model_process_packet(uint32_t pipe, uint32_t prsr_inst, uint32_t prsr_type,
                                                    uint32_t chan, uint8_t version,
                                                    uint32_t congested, const char* pktstr,
                                                    uint32_t* phv_data, uint32_t* phv_vld,
                                                    uint32_t* phvt_data, uint32_t* phvt_vld,
                                                    uint32_t* payload_offset, uint32_t* drop, uint32_t* pri,
                                                    uint32_t* err_flags) {

  printf("Packet from SV for %s parser inst %d, channel %d, version=0x%x:\n", prsr_type==0?"INGRESS":"EGRESS", prsr_inst, chan, version);
  if (m_log_level >= 300) {
    printf("%s\n", pktstr);
  }

  Packet *pkt = m_rmt_obj_mgr_p[0]->pkt_create(pktstr);
  assert(pkt != NULL);

  pkt->set_version(version);

  get_parser_inst(0, pipe, prsr_inst, prsr_type)->set_channel_congested(chan, congested & 1);



  Phv *phv = get_parser_inst(0, pipe, prsr_inst, prsr_type)->parse(pkt, chan);



  *payload_offset = pkt->orig_hdr_len();
  *pri            = pkt->priority();

  if (phv == NULL) {
    *drop = 1;
  }
  else {
    *drop = 0;
    if (m_log_level >= 300) {
      phv->print("PHV DUMP");
    }

    for (uint32_t i=0; i<224; i++) {
      phv_data[i] = phv->get(i);
      phv_vld[i/32]  |= (phv->is_valid(i) & 0x1) << (31 - i%32);
    }

    for (uint32_t i=0; i<112; i++) {
      phvt_data[i] = phv->get_x(256+i);
      phvt_vld[i/32]  |= (phv->is_valid_x(256+i) & 0x1) << (31 - i%32);
    }
  }

  *err_flags = get_parser_inst(0, pipe, prsr_inst, prsr_type)->last_err_flags();


  printf("ref_model_wrapper::parser_model_process_packet: Parser[%d][%s] payload_offset=%d, null_phv=%d, pri=%d, err_flags=0x%x\n", prsr_inst, prsr_type==0?"INGRESS":"EGRESS", *payload_offset, *drop, *pri, *err_flags);
  // Cleanup
  m_rmt_obj_mgr_p[0]->pkt_delete(pkt);
}

void ref_model_wrapper::parser_model_process_packet_clot(uint32_t chip, uint32_t pipe, uint32_t prsr_inst, uint32_t prsr_type,
							 uint32_t chan, uint8_t version,
							 uint32_t congested, const char* pktstr,
							 uint32_t* phv_data, uint32_t* phv_vld,
							 uint32_t* clot_data, uint32_t* clot_vld,
							 uint32_t* payload_offset, uint32_t* drop, uint32_t* pri,
							 uint32_t* err_flags, uint32_t* counter_data, uint8_t* ref_version) {
  uint64_t data;
  printf("Packet from SV for %s parser inst %d, channel %d, version=0x%x:\n", prsr_type==0?"INGRESS":"EGRESS", prsr_inst, chan, version);
  if (m_log_level >= 300) {
    printf("%s\n", pktstr);
  }

  Packet *pkt = m_rmt_obj_mgr_p[chip]->pkt_create(pktstr);
  assert(pkt != NULL);
  pkt->set_version(version);
  get_parser_inst(chip,pipe, prsr_inst, prsr_type)->set_channel_congested(chan, congested & 1);
  pkt->set_pipe_data_ctrl(!prsr_type, prsr_inst, PacketData::kPrsrCntr, PacketDataCtrl::kCalcAndStore);
  Phv *phv = get_parser_inst(chip,pipe, prsr_inst, prsr_type)->parse(pkt, chan);
  for (int cycle=0; cycle < 256; cycle++) {
    data = pkt->get_pipe_data(!prsr_type, prsr_inst, PacketData::kPrsrCntr, cycle * 64, 64);
    counter_data[(cycle*4)]  = uint32_t(data & 0xff);
    counter_data[(cycle*4)+1]  = uint32_t((data >> 8) & 0xff);
    counter_data[(cycle*4)+2]  = uint32_t((data >> 16) & 0xff);
    counter_data[(cycle*4)+3]  = uint32_t((data >> 24) & 0xff);
  }
  *payload_offset = pkt->orig_hdr_len();
  *pri            = pkt->priority();

  if (phv == NULL) {
    *drop = 1;
  }
  else {
    *drop = 0;
    *ref_version    = (prsr_type==0)?phv->ingress_version():phv->egress_version();

    if (m_log_level >= 300) {
      phv->print("PHV DUMP");
    }

    for (uint32_t i=0; i<224; i++) {
      phv_data[i] = phv->get_p(i);
      phv_vld[i/32]  |= (phv->is_valid(i) & 0x1) << (31 - i%32);
    }
    /*
    for (uint32_t i=0; i<112; i++) {
      phvt_data[i] = phv->get_x(256+i);
      phvt_vld[i/32]  |= (phv->is_valid_x(256+i) & 0x1) << (31 - i%32);
    }
    */

    // Copy out Clot info if there is any
    Clot *clot = pkt->clot();
    if (clot != NULL) {
      int n=0; // Put valid clots into consec pairs within clot_data array
      for (int i = 0; i < Clot::kMaxSimulTags; i++) {
        uint8_t  tag;
	uint16_t len, off, csum;
	bool valid = clot->read(i, &tag, &len, &off, &csum);
	if (valid) {
	  assert(n+1 < 32);
	  clot_data[n+0] = (static_cast<uint32_t>(len) << 16) | (static_cast<uint32_t>(off) << 0);
	  clot_data[n+1] = (static_cast<uint32_t>(tag) << 16) | (static_cast<uint32_t>(csum) << 0);
	  clot_data[n+1] |= 1u<<31;
	  n+=2;
	}
      }
    }
  }

  *err_flags = get_parser_inst(chip,pipe, prsr_inst, prsr_type)->last_err_flags();

  printf("ref_model_wrapper::parser_model_process_packet: Parser[%d][%s] payload_offset=%d, null_phv=%d, pri=%d, err_flags=0x%x\n", prsr_inst, prsr_type==0?"INGRESS":"EGRESS", *payload_offset, *drop, *pri, *err_flags);
  // Cleanup
  m_rmt_obj_mgr_p[chip]->pkt_delete(pkt);
}



/*  Prsr.Dprsr wrapper functions */
Phv * ref_model_wrapper::create_phv_from_data(
        const uint8_t  egress,
        const uint8_t  pktver,
        const uint8_t  hdrport,
        const uint8_t  hrecirc,
        const uint32_t* phv_data,
        const uint32_t* phv_vld ) {
    Phv * phv;
    uint8_t  eopnum = Eop::make_eopnum(hdrport, hrecirc);
    RmtObjectManager * rmtobjman = ref_model_wrapper::get_rmtobjmanager_inst(0);
    if (m_log_level >= 300) { printf("Create PHV from SV data \n"); }
    assert(rmtobjman != NULL);
    phv = rmtobjman->phv_create();
    assert(phv != NULL);
    if (egress) {
            phv->set_egress();
            phv->set_version(pktver, false);
            phv->set_eopnum(eopnum, false);
            if (m_log_level >= 300) { printf("Create PHV from SV data set_egress()  pktver=%d hdrport=%x resub=%d\n", pktver, hdrport, hrecirc); }
    }
    else {
            phv->set_ingress();
            phv->set_version(pktver, true);
            phv->set_eopnum(eopnum, true);
            if (m_log_level >= 300) { printf("Create PHV from SV data set_ingress() pktver=%d hdrport=0x%x resub=%d\n", pktver, hdrport, hrecirc); }
    }
    /* Fill in data and fields for Prsr/Dprsr - mau has different version of function */
    /* Note: phv_data is uint32_t [224] & phv_valid is uint32_t [7] */
    for (uint32_t i0=0; i0<RmtDefs::kPhvWordsMaxUnmapped; i0++) {
       if ( (bool)((phv_vld[i0/32] >> (31 - (i0 % 32))) & 0x1) ) {
               phv->set(i0, phv_data[i0]);
       }
    }
    return phv;
}

void ref_model_wrapper::extract_data_from_phv(Phv *phv,
                             uint8_t  egress,
                             uint32_t* phv_data,
                             uint32_t* phv_vld) {
    for (uint32_t i0=0; i0<RmtDefs::kPhvWordsMaxUnmapped; i0++) {
      if (phv->is_valid(i0)) {
         phv_data[i0]    = phv->get(i0);
      }
      else {
         phv_data[i0]    = 0;
      }
      phv_vld[i0/32] |= (phv->is_valid(i0) & 0x1) << (31 - (i0 % 32));
    }
}

void update_meta_data(Packet *output_pkt, uint32_t input_port_num, uint32_t * meta_data) {
    if (output_pkt != nullptr) {
      // Update meta data fields.
      //meta_data[ING_PKT_VER] = output_pkt->
      meta_data[ING_CT_MC                   ] = output_pkt->i2qing_metadata()->ct_mcast_mode();
      meta_data[ING_CT_DIS                  ] = output_pkt->i2qing_metadata()->ct_disable_mode();
      meta_data[ING_ICOS                    ] = output_pkt->i2qing_metadata()->icos();
      meta_data[ING_INPORT_PORT             ] = output_pkt->i2qing_metadata()->physical_ingress_port();
      //printf("physical_ingress_port:%0x\n", output_pkt->i2qing_metadata()->physical_ingress_port());
      meta_data[ING_INPORT_PORT_2L_PIPE_ID  ] = (output_pkt->i2qing_metadata()->physical_ingress_port()>>7);
      meta_data[ING_INPORT_PORT_2L_P_PORT_ID] = output_pkt->i2qing_metadata()->physical_ingress_port()&0x7f;
      meta_data[ING_EPIPE_PORT_VLD          ] = (output_pkt->i2qing_metadata()->is_egress_uc() && ((output_pkt->i2qing_metadata()->egress_uc_port()&0x7f) < 72));
      meta_data[ING_EPIPE_PORT              ] = output_pkt->i2qing_metadata()->egress_uc_port();
      meta_data[ING_EPORT_QID               ] = output_pkt->i2qing_metadata()->qid();
      meta_data[ING_COLOR                   ] = output_pkt->i2qing_metadata()->meter_color();
      meta_data[ING_PIPE_VEC                ] = output_pkt->i2qing_metadata()->pipe_mask();
      meta_data[ING_HASH1                   ] = output_pkt->i2qing_metadata()->hash1();
      meta_data[ING_HASH2                   ] = output_pkt->i2qing_metadata()->hash2();
      meta_data[ING_MCID1_VLD               ] = output_pkt->i2qing_metadata()->has_mgid1();
      meta_data[ING_MCID1_ID                ] = output_pkt->i2qing_metadata()->mgid1();
      meta_data[ING_MCID2_VLD               ] = output_pkt->i2qing_metadata()->has_mgid2();
      meta_data[ING_MCID2_ID                ] = output_pkt->i2qing_metadata()->mgid2();
      meta_data[ING_XID                     ] = output_pkt->i2qing_metadata()->xid();
      meta_data[ING_YID                     ] = output_pkt->i2qing_metadata()->yid();
      meta_data[ING_RID                     ] = output_pkt->i2qing_metadata()->irid();
      meta_data[ING_EGRESS_BYPASS           ] = output_pkt->i2qing_metadata()->bypass_egr_mode();
      meta_data[ING_TABLEID                 ] = output_pkt->i2qing_metadata()->use_yid_tbl();
      meta_data[ING_C2C_VLD                 ] = output_pkt->i2qing_metadata()->cpu_needs_copy();
      meta_data[ING_C2C_COS                 ] = output_pkt->i2qing_metadata()->cpu_cos();
      meta_data[ING_DEF_ON_DROP             ] = output_pkt->i2qing_metadata()->dod();
      meta_data[ING_TM_VEC                  ] = output_pkt->i2qing_metadata()->tm_vec();
      auto got_afc                            = output_pkt->i2qing_metadata()->afc();
      meta_data[ING_AFC_QID                 ] = (got_afc) ?output_pkt->i2qing_metadata()->afc()->getQid() :0xFFFFFFFF;
      meta_data[ING_AFC_PORT_ID             ] = (got_afc) ?output_pkt->i2qing_metadata()->afc()->getPortId() :0xFFFFFFFF;
      meta_data[ING_AFC_CREDIT              ] = (got_afc) ?output_pkt->i2qing_metadata()->afc()->getCredit() :0xFFFFFFFF;
      meta_data[ING_AFC_ADVQFC              ] = (got_afc) ?output_pkt->i2qing_metadata()->afc()->getAdvQfc() :0xFFFFFFFF;
    } else {
      meta_data[ING_INPORT_PORT             ] = input_port_num;
    }
}

void ref_model_wrapper::deparser_model_process_packet(
    uint32_t is_ingr,
    uint32_t *phv_data,
    uint32_t *phv_vld,
    uint32_t *phvt_data,
    uint32_t *phvt_vld,
    uint32_t *mau_err,
    const char *input_pkt_data,
    uint32_t orig_hdr_len,
    uint32_t version,
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
    uint32_t &pkt_dropped
) {
  infer_my_die_id(input_port_num);

  if (m_log_level >= 300) {
    printf("input_pkt_data:%s\n", input_pkt_data);
  }

  Packet *input_pkt = m_rmt_obj_mgr_p[0]->pkt_create(input_pkt_data);
  //assert(input_pkt != NULL);

  if (!is_ingr)
    input_pkt->set_egress();
  else
    input_pkt->set_ingress();
  input_pkt->set_version((uint8_t)version);

  Phv * phv = m_rmt_obj_mgr_p[0]->phv_create();

  if(mau_err) {
    phv->set_pkterr(1);
  }
  //assert(phv != NULL);
  phv->set_packet(input_pkt);


  /* Fill in data and fields*/
  /* Note: phv_data is uint32_t [224] & phv_valid is uint32_t [7] */
  for (uint32_t i0=0; i0<224; i0++) {
    if ((bool)((phv_vld[i0/32] >> (i0 % 32)) & 0x1)) {
      if (m_log_level >= 300)
        printf("phv_vld:%0d phv_data:%0x\n", i0, phv_data[i0]);
      phv->set_d(i0, phv_data[i0]);
    }
  }

  /* Fill in data and fields*/
  /* Note: phvt_data is uint32_t [112] & phvt_valid is uint32_t [4] */
  for (uint32_t i0=0; i0<112; i0++) {
    if ((bool)((phvt_vld[i0/32] >> (i0 % 32)) & 0x1)) {
      if (m_log_level >= 300)
        printf("phvt_vld:%0d phvt_data:%0x\n", i0, phvt_data[i0]);
      phv->set_x((i0+256), phvt_data[i0]);
    }
  }

  uint32_t pgen_meta_data_valid = 0, pgen_address = 0, pgen_length = 0;
  std::unique_ptr<uint8_t[]> pgen_meta_data;

  do_deparser_model_process_packet(
      0,
      0,
      phv,
      input_pkt,
      is_ingr,
      input_pkt_data,
      orig_hdr_len,
      output_pkt_length,
      output_pkt_data,
      mirror_pkt_length,
      mirror_pkt_data,
      input_port_num,
      meta_data,
      lfltr_pkt_valid,
      lfltr_pkt_data,
      resubmit_meta_data_valid,
      resubmit_meta_data,
      pgen_meta_data_valid,
      pgen_meta_data,
      pgen_address,
      pgen_length,
      pkt_dropped);

}

void ref_model_wrapper::deparser_model_process_packet_clot(
    uint32_t chip,
    uint32_t pipe,
    uint32_t is_ingr,
    uint32_t *phv_data,
    uint32_t *phv_vld,
    uint32_t *clot_data,
    uint32_t *clot_vld,
    uint32_t *mau_err,
    const char *input_pkt_data,
    uint32_t orig_hdr_len,
    uint32_t version,
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
) {
  if (m_log_level >= 300) {
    printf("M {\n");
    printf("M Packet *input_pkt = m_rmt_obj_mgr_p[%0d]->pkt_create(\"%s\");\n",chip,input_pkt_data);
    printf("M Clot *clot = new Clot();\n");
    printf("M Phv* phv = tu.phv_alloc();\n");
  }

  infer_my_die_id(input_port_num);

  Packet *input_pkt = m_rmt_obj_mgr_p[chip]->pkt_create(input_pkt_data);
  //assert(input_pkt != NULL);

  if (!is_ingr)
    input_pkt->set_egress();
  else
    input_pkt->set_ingress();
  input_pkt->set_version((uint8_t)version);

  Phv * phv = m_rmt_obj_mgr_p[chip]->phv_create();

  if(mau_err) {
    phv->set_pkterr(1);
  }
  //assert(phv != NULL);
  phv->set_packet(input_pkt);


  /* Fill in data and fields*/
  /* Note: phv_data is uint32_t [224] & phv_valid is uint32_t [7] */
  for (uint32_t i0=0; i0<224; i0++) {
    if (!(bool)((phv_vld[i0/32] >> (i0 % 32)) & 0x1)) { // all phv's are valid on JBay
      printf("ERROR: phv %i0 is not marked valid\n",i0); // TODO: remove this check
    }
    if (m_log_level >= 300)
      printf("M phv->set_p(%d, 0x%x);\n", i0, phv_data[i0]);
    phv->set_p(i0, phv_data[i0]);
  }

  // Setup Clot and copy data in
  Clot *clot = new Clot();
  for (uint8_t n=0; n<32; n+=2) {
    uint16_t  len = static_cast<uint16_t>((clot_data[n+0] >> 16) & 0xFFFF);
    uint16_t  off = static_cast<uint16_t>((clot_data[n+0] >>  0) & 0xFFFF);
    bool    valid = (((clot_data[n+1] >> 31) & 1) == 1);
    uint8_t   tag = static_cast<uint8_t> ((clot_data[n+1] >> 16) & 0x00FF);
    uint16_t csum = static_cast<uint16_t>((clot_data[n+1] >>  0) & 0xFFFF);
    if (valid) {
      int c_len = len+1;
      bool clot_ok = clot->set(tag, c_len, off, csum);
      if (!clot_ok) {
        if (!clot->is_valid_tag(tag)) {
          printf("ERROR:%s Bad CLOT tag %d\n",__func__,tag);
        }
        if (!clot->is_valid_length_offset(c_len, off)) {
          printf("ERROR:%s Bad length(%d) or offset(%d) or length+offset(%d)\n",
                 __func__,c_len,off, c_len+off);
        }
        if (clot->contains_overlap(tag, c_len, off)) {
          printf("ERROR:%s CLOT has pre-existing entry that overlaps with "
                 "length(%d) offset(%d) tag(%d) (Min inter-clot gap is %d)\n",
                 __func__,c_len, off, tag, Clot::kClotMinGap);
        }
        if (clot->is_full()) {
          printf("ERROR:%s CLOT is full, no room for tag %d\n",__func__, tag);
        }
        assert(0);
      }
      if (m_log_level >= 300)
        printf("M clot->set(0x%02x, %d, %d, 0x%04x);\n", tag, c_len, off, csum);
    }
  }
  input_pkt->set_clot(clot);
  do_deparser_model_process_packet(
      chip,
      pipe,
      phv,
      input_pkt,
      is_ingr,
      input_pkt_data,
      orig_hdr_len,
      output_pkt_length,
      output_pkt_data,
      mirror_pkt_length,
      mirror_pkt_data,
      input_port_num,
      meta_data,
      lfltr_pkt_valid,
      lfltr_pkt_data,
      resubmit_meta_data_valid,
      resubmit_meta_data,
      pgen_meta_data_valid,
      pgen_meta_data,
      pgen_address,
      pgen_length,
      pkt_dropped);
}

void ref_model_wrapper::do_deparser_model_process_packet(
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
) {
  Packet* output_pkt = nullptr;
  Port* port = new Port(m_rmt_obj_mgr_p[chip], input_port_num);

  Packet *mirror_pkt = nullptr;
  Packet *resubmit_pkt = nullptr;
  PacketGenMetadata *packet_gen_metadata = nullptr;
  LearnQuantumType lq;

  if (m_log_level >= 300)  {
    printf("M Port* port = new Port(m_rmt_obj_mgr_p[%0d], %d, port_config);\n",chip,input_port_num);
    printf("M input_pkt->ie()->set_port(port);\n");
    printf("M input_pkt->set_clot(clot);\n");
    printf("M input_pkt->set_orig_hdr_len(%d);\n",orig_hdr_len);
  }

  if (!is_ingr) {
    if (m_log_level >= 300)  {
      printf("M input_pkt->set_egress();\n");
      printf("M phv->set_packet(input_pkt);\n");
      printf("M output_pkt = m_deparser_p->DeparseEgress(*phv, &mirror_pkt);\n");
      printf("M }\n");
    }

    input_pkt->ie()->set_port(port);
    input_pkt->set_orig_hdr_len(orig_hdr_len);
    printf("before DeparseEgress\n");
    output_pkt = m_deparser_p[chip][pipe]->DeparseEgress(*phv, &mirror_pkt);
    printf("after DeparseEgress\n");
    meta_data[EGR_EPIPE_PORT] = input_port_num;

    if (  (output_pkt == nullptr)
       && (mirror_pkt == nullptr)
       ) {
      pkt_dropped = 1;
    }

  } else {
    if (m_log_level >= 300)  {
      printf("M input_pkt->set_ingress();\n");
      printf("M phv->set_packet(input_pkt);\n");
      printf("M PacketGenMetadata* packet_gen_metadata = nullptr;\n");
      printf("M output_pkt = m_deparser_p->DeparseIngress(*phv, &lq, &mirror_pkt, &resubmit_pkt, &packet_gen_metadata);\n");
      printf("M }\n");
    }
    input_pkt->ie()->set_port(port);
    input_pkt->set_orig_hdr_len(orig_hdr_len);
    if (m_log_level >= 900) printf("Before DeparseIngress\n");
    output_pkt = m_deparser_p[chip][pipe]->DeparseIngress(*phv, &lq, &mirror_pkt, &resubmit_pkt, &packet_gen_metadata);
    if (m_log_level >= 900) printf("After DeparseIngress\n");

    if (  (output_pkt == nullptr)
       && (mirror_pkt == nullptr)
       && (resubmit_pkt == nullptr)
       ) {
      pkt_dropped = 1;
    }

    update_meta_data(output_pkt, input_port_num, meta_data);

    // Update Learn quanta fields.
    lfltr_pkt_valid = lq.valid;
    //printf("ref_model_wrapper: lq.valid:%0d lq.length:%0d\n", lq.valid, lq.length);
    if (lq.valid) {
      lfltr_pkt_data = std::unique_ptr<uint8_t[]> (new uint8_t[48]);
      for (int32_t i=0; i<48; i++) {
        lfltr_pkt_data[i] = static_cast<char>(lq.data.get_byte(i));
        //printf("ref_model_wrapper: lq.data%0d %0x\n", i, lq.data.get_byte(i));
      }
      //lq.data.print_with_printf();
      printf("MODEL_LEARNED_PKT\n");
    }

    if (resubmit_pkt != nullptr) {
      resubmit_meta_data_valid = 1;
      resubmit_meta_data = std::unique_ptr<uint8_t[]> (new uint8_t[16]);
      for (uint32_t i=0; i<16; i++) {
        resubmit_meta_data[i] = 0x0;
      }
      resubmit_pkt->get_resubmit_header()->get_buf(
          resubmit_meta_data.get(), 0, resubmit_pkt->get_resubmit_header()->len());
      printf("MODEL_RESUBMITTED_PKT\n");
      //string tmp = "";
      //for (uint32_t i=0; i<8; i++) {
      //  printf("resubmit_meta_data:: %0x\n",(uint32_t)resubmit_meta_data[i]);
      //}

    } else {
      resubmit_meta_data_valid = 0x0;
    }

    pgen_meta_data_valid = 0;
    if (packet_gen_metadata != nullptr) {
      PacketBuffer* trigger = packet_gen_metadata->get_trigger();
      if ( trigger != nullptr ) {
        pgen_meta_data_valid = 1;
        pgen_meta_data = std::unique_ptr<uint8_t[]> (new uint8_t[16]);
        for (uint32_t i=0; i<16; i++) {
          pgen_meta_data[i] = 0x0;
        }
        trigger->get_buf(pgen_meta_data.get(),0,trigger->len());
        printf("MODEL_PGEN_PKT\n");
        string tmp = "";
        for (uint32_t i=0; i<16; i++) {
          printf("pgen_meta_data:: %0x\n",(uint32_t)pgen_meta_data[i]);
        }
        pgen_address = (uint32_t) packet_gen_metadata->address();
        pgen_length = (uint32_t)packet_gen_metadata->length();
      }
    }

  }

  if (output_pkt != nullptr) {
    output_pkt_length = (uint32_t)output_pkt->len();
    output_pkt_data = std::unique_ptr<uint8_t[]>(new uint8_t[output_pkt->len()]);
    output_pkt->get_buf(output_pkt_data.get(),0,output_pkt_length);
    printf("MODEL_OUTPUT_PKT\n");
  }

  if (mirror_pkt != nullptr) {
    mirror_pkt_length = (uint32_t)mirror_pkt->len();
    mirror_pkt_data = std::unique_ptr<uint8_t[]>(new uint8_t[mirror_pkt->len()]);
    mirror_pkt->get_buf(mirror_pkt_data.get(),0,mirror_pkt_length);
    meta_data[MIRROR_ID] = (mirror_pkt->mirror_metadata()->mirror_id() & 0x3ff);
    printf("MODEL_MIRRORED_PKT\n");
    meta_data[MIRROR_VERSION] = (mirror_pkt->mirror_metadata()->version() & 0x3);
    meta_data[MIRROR_COAL] = (mirror_pkt->mirror_metadata()->coal_len() & 0xff);
  } else {
    mirror_pkt_length = 0;
  }

  if (strlen(input_pkt_data) && (output_pkt == nullptr)) {
    pkt_dropped = 1;
    printf("MODEL_PKT_DROPPED\n");
  }

  // Cleanup
  // FIXME: This if condition might cause memory leaks
  // if (input_pkt)
  if ((output_pkt != nullptr))
    m_rmt_obj_mgr_p[chip]->pkt_delete(input_pkt);

  m_rmt_obj_mgr_p[chip]->phv_delete(phv);
  set_my_die_id(chip);  // reset
  delete port;

}


int ref_model_wrapper::set_option(uint32_t chip,char* name, char *option) {
  RmtObjectManager *om = m_rmt_obj_mgr_p[chip];
  return om->string_map_lookup(name,option);
}

void ref_model_wrapper::set_relax_checks(uint32_t chip) {
  char *kstr = new char[200];
  char *tfstr = new char[16];
  RmtObjectManager *om = m_rmt_obj_mgr_p[chip];
  std::vector<std::string> vs = om->string_map_get_all_keys();
  int sval;

  for (auto s: om->string_map_get_all_keys()) {
    strcpy(kstr, s.c_str());
    if(sknobs_exists(kstr)) {
      sval = sknobs_get_value(kstr, 0);
      if (sval > 1 ) sprintf(tfstr, "%d", sval);
      else if (sval == 0) strcpy(tfstr,"false");
      else strcpy(tfstr, "true");

      set_option(chip,kstr, tfstr);
      cout << "setting " << kstr << "to " << sval << " from commandline\n";
    }
  }
}

void ref_model_wrapper::stop_rmt_packet_coordinator(uint32_t chip) {
  m_rmt_obj_mgr_p[chip]->packet_coordinator_get()->stop();
}

PacketReplicationEngine* ref_model_wrapper::get_tm_pre_inst(uint32_t chip,uint32_t pipe) {
  return(m_pre_inst[chip][pipe]);
}

void ref_model_wrapper::tm_init_pre_mit(uint32_t chip, uint32_t pipe, uint32_t mg_id, uint32_t l1_node_ptr) {
  get_tm_pre_inst(chip,pipe)->mit(mg_id, l1_node_ptr);
}

void ref_model_wrapper::tm_pre_enqueue(uint32_t chip,uint32_t pipe, uint32_t fifo, uint32_t mg_id1, uint32_t mg_id2, uint32_t rid,
				       uint32_t xid,  uint32_t yid, uint32_t tbl_id, uint32_t hash1, uint32_t hash2,
				       uint32_t c2c_vld, uint32_t mg_id1_vld, uint32_t mg_id2_vld, uint32_t pkt_id) {
  const char *pkt_string = "123456789abcdeffedcba987654321";
  Packet *pkt = m_rmt_obj_mgr_p[chip]->pkt_create(pkt_string);
  if (mg_id1_vld)
    pkt->i2qing_metadata()->set_mgid1(mg_id1);
  if (mg_id2_vld)
    pkt->i2qing_metadata()->set_mgid2(mg_id2);
  pkt->i2qing_metadata()->set_irid(rid);
  pkt->i2qing_metadata()->set_xid(xid);
  pkt->i2qing_metadata()->set_yid(yid);
  pkt->i2qing_metadata()->set_hash1(hash1);
  pkt->i2qing_metadata()->set_hash2(hash2);
  pkt->set_ph_ver(tbl_id);
  pkt->i2qing_metadata()->set_copy_to_cpu((uint64_t) c2c_vld);
  pkt->pkt_id((uint64_t) pkt_id);
  // Use Chip Id as Qid to separate self & remote packets
  pkt->i2qing_metadata()->set_qid(chip);
  get_tm_pre_inst(chip,pipe)->enqueue(fifo, pkt);
}

void ref_model_wrapper::tm_pre_dequeue(uint32_t chip,uint32_t pipe, uint32_t* rep_id, uint32_t* rid_first,
				       uint32_t* egress_port, uint32_t* deq_vld, uint32_t* pkt_id) {
  Packet *pkt = nullptr;
  int read_chip;

  // Wait for all copies to be made.
  bool has_packets = false;
  do {
    has_packets = false;
    std::this_thread::yield();
    has_packets = !(get_tm_pre_inst(chip,pipe)->fifos_empty());
  } while(has_packets);

  if (pipe < RmtDefs::kPipesMax) {
    for (int portGrp=0; portGrp<RmtDefs::kPortGroupsPerPipe; portGrp++) {
      qing_blk[chip]->dequeue(pipe, portGrp, /*qid*/chip, pkt);
      if (pkt != nullptr) break;
    }
  } else {
    read_chip = static_cast<int>(m_rmt_obj_mgr_p[chip]->chip()->GetReadDieId());
    for (int portGrp=0; portGrp<RmtDefs::kPortGroupsPerPipe; portGrp++) {
      qing_blk[read_chip]->dequeue((pipe & RmtDefs::kPipeMask), portGrp, /*qid*/chip, pkt);
      if (pkt != nullptr) break;
    }
  }
  
  *deq_vld = (pkt != nullptr);
  if (pkt != nullptr) {
    *rep_id = (uint32_t) pkt->qing2e_metadata()->erid();
    *egress_port = (uint32_t) pkt->qing2e_metadata()->egress_port();
    *rid_first = (uint32_t) pkt->qing2e_metadata()->rid_first();
    *pkt_id = pkt->pkt_id();
    m_rmt_obj_mgr_p[chip]->pkt_delete(pkt);
  }
}

void ref_model_wrapper::tm_run_sch_model(uint32_t chip,uint32_t pipe) {
  m_rmt_obj_mgr_p[chip]->sch_model_wrap_run(pipe);
  m_rmt_obj_mgr_p[chip]->sch_model_wrap_delete(pipe);
}

}