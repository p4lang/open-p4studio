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

#ifndef __REF_MODEL_WRAPPER_MAU_CPP__
#define __REF_MODEL_WRAPPER_MAU_CPP__

#include <cinttypes>
#include "ref_model_wrapper.hpp"
#include "model_core/model.h"
#include "clot.h"
#include "packet.h"
#include "phv.h"
#include "port.h"
#include "mau-meter-alu.h"
#include "mau-lookup-result.h"


extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;

namespace MODEL_CHIP_NAMESPACE {

/*  Mau wrapper functions */
/* Phvside mapping - input and output...
    uint8_t  egress  = (phvside[0] & 0x1);
    uint8_t  ingress = (phvside[0] >> 1)  & 0x1; // Or if egr/ghost not set
    uint8_t  ghost   = (phvside[0] >> 2)  & 0x1;
    uint8_t  pktver  = (phvside[0] >> 4)  & 0x3;
    uint8_t  hdrport = (phvside[0] >> 8)  & 0x7F;
    uint8_t  hrecirc = (phvside[0] >> 15) & 0x1;
    bout[31:16]      = bytesize;
    bout[40:32]      = m_mpr_next_table[0];             // 9b
    bout[52:44]      = m_mpr_next_table[1];             // 9b
    bout[62:54]      = m_mpr_next_table[2];             // 9b
    bout[79:64]      = m_glob_exec;                     // 16b
    bout[95:80]      = m_mpr_glob_exec;                 // 16b
    bout[103:96]     = m_long_brch;                     // 8b
    bout[111:104]    = m_mpr_long_brch;                 // 8b
    bout[120:112]    = m_next_table[2]; // ghost        // 9b
 */
Phv * ref_model_wrapper::create_mau_phv( const uint32_t *phvside ) {
    Phv*     phv;
    char     prefix[256] = "REFM_CreatePHV";
    char     setLogPrefix[256] = "SetLogPrefix";

    uint8_t  egress  = (phvside[0] & 0x1);
    uint8_t  ingress = (phvside[0] >> 1)  & 0x1;
    uint8_t  ghost   = (phvside[0] >> 2)  & 0x1;
    uint8_t  pktver  = (phvside[0] >> 4)  & 0x3;
    uint8_t  hdrport = (phvside[0] >> 8)  & 0x7F;
    uint8_t  hrecirc = (phvside[0] >> 15) & 0x1;
    uint8_t  eopnum = Eop::make_eopnum(hdrport, hrecirc);

    //!!    ingress = (egress||ingress||ghost) ? ingress : 1; // fail safe in case no thread passed
    set_option(0, setLogPrefix, prefix);

    RmtObjectManager * rmtobjman = ref_model_wrapper::get_rmtobjmanager_inst(0);
    assert(rmtobjman != NULL);
    phv = rmtobjman->phv_create();
    assert(phv != NULL);

    if (egress) {
      phv->set_egress();
      phv->set_version(pktver, false);
      phv->set_eopnum(eopnum, false);

      if (m_log_level >= 300) {
	printf("ref_model_wrapper::create_mau_phv: Create PHV from SV egress=1 pktver=%d hdrport=%x resub=%d\n", pktver, hdrport, hrecirc);
      }
    }
    //!!    } else {
    if (ingress) {
      phv->set_ingress();
      phv->set_version(pktver, true);
      phv->set_eopnum(eopnum, true);
      if (m_log_level >= 300) {
	printf("ref_model_wrapper::create_mau_phv: Create PHV from SV ingress=%d pktver=%d hdrport=0x%x resub=%d\n", (static_cast<int>(ingress)), pktver, hdrport, hrecirc);
      }
    }

    if (ghost) {
      phv->set_ghost();
      if (m_log_level >= 300) {
	printf("ref_model_wrapper::create_mau_phv: Create PHV from SV ghost=%d pktver=%d hdrport=0x%x resub=%d\n", (static_cast<int>(ghost)), pktver, hdrport, hrecirc);
      }
    }

    //!!    }
    printf("ref_model_wrapper::create_mau_phv: Create PHV from SV NO-ACTIVE-THREADpktver=%d hdrport=0x%x resub=%d\n", pktver, hdrport, hrecirc);

    return phv;
} // create_mau_phv

Phv * ref_model_wrapper::create_mau_phv_from_data(const uint32_t *phvside,
                                                  const uint32_t *phv_data,
                                                  const uint32_t *phv_vld ) {
    Phv * phv;
    char     prefix[256] = "REFM_PhvGen";
    char     setLogPrefix[256] = "SetLogPrefix";
    uint8_t  egress  = phvside[0] & 0x1;
    uint8_t  ingress = (phvside[0] >> 1) & 0x1;
    uint8_t  ghost   = (phvside[0] >> 2) & 0x1;
    uint8_t  pktver  = (phvside[0] >> 4) & 0x3;
    uint8_t  hdrport = (phvside[0] >> 8) & 0x7F;
    uint8_t  hrecirc = (phvside[0] >> 15) & 0x1;
    ingress = (egress||ingress||ghost) ? ingress : 1; // fail safe in case no thread passed
    set_option(0,setLogPrefix, prefix);

    uint8_t  eopnum = Eop::make_eopnum(hdrport, hrecirc);
    RmtObjectManager * rmtobjman = ref_model_wrapper::get_rmtobjmanager_inst(0);
    assert(rmtobjman != NULL);
    phv = rmtobjman->phv_create();
    assert(phv != NULL);
    if (egress) {
            phv->set_egress();
            phv->set_version(pktver, false);
            phv->set_eopnum(eopnum, false);
            if (m_log_level >= 300) {
                    printf("ref_model_wrapper::create_mau_phv_from_data: Create PHV from SV data set_egress()  pktver=%d hdrport=%x resub=%d\n", pktver, hdrport, hrecirc);
            }
    }
    else {
            if (ingress) {
                    phv->set_ingress();
                    phv->set_version(pktver, true);
                    phv->set_eopnum(eopnum, true);
            }
            if (ghost) { phv->set_ghost(); }
            if (m_log_level >= 300) {
                    printf("ref_model_wrapper::create_mau_phv_from_data: Create PHV from SV data set_ing=%d ghost=%d pktver=%d hdrport=0x%x resub=%d\n", (static_cast<int>(ingress)),  (static_cast<int>(ghost)), pktver, hdrport, hrecirc);
            }
    }
    /* Note: phv_data uint32_t [224/280] & phv_valid is uint32_t [7/9] depending on tofino/jbay */
    for (uint32_t i0=0; i0 < RmtDefs::kPhvWordsMax; i0++) {
       if ( (bool)((phv_vld[i0/32] >> (31 - (i0 % 32))) & 0x1) ) {
               phv->set(i0, phv_data[i0]);
       }
    }
    return phv;
} //  create_mau_phv_from_data

void ref_model_wrapper::extract_data_from_mau_phv(Phv *phv,
                                                  uint8_t  egress,
                                                  uint32_t* phv_data,
                                                  uint32_t* phv_vld) {
    /* Note: phv_data uint32_t [224/280] & phv_valid is uint32_t [7/9] depending on tofino/jbay */
    for (uint32_t i0=0; i0 < RmtDefs::kPhvWordsMax; i0++) {
      if (phv->is_valid(i0)) {
         phv_data[i0]    = phv->get(i0);
      }
      else {
         phv_data[i0]    = 0;
      }
      phv_vld[i0/32] |= (phv->is_valid(i0) & 0x1) << (31 - (i0 % 32));
    }
}  // extract_data_from_mau_phv

void ref_model_wrapper::set_mauio_via_phvside(MauIO *mauio, const uint32_t *phvside) {
    mauio->set_ingress_mpr_nxt_tab( ((phvside[1] >>  0) & 0x1FF) );
    mauio->set_egress_mpr_nxt_tab(  ((phvside[1] >> 12) & 0x1FF) );
    mauio->set_ghost_mpr_nxt_tab(   ((phvside[1] >> 22) & 0x1FF) );
    mauio->set_global_exec(         ((phvside[2] >>  0) & 0xFFFF) );
    mauio->set_mpr_global_exec(     ((phvside[2] >> 16) & 0xFFFF) );
    mauio->set_long_branch(         ((phvside[3] >>  0) & 0xFF) );
    mauio->set_mpr_long_branch(     ((phvside[3] >>  8) & 0xFF) );
    mauio->set_ghost_nxt_tab(       ((phvside[3] >> 16) & 0x1FF) );
}

void ref_model_wrapper::get_mauio_into_phvside(const MauIO *mauio, uint32_t *phvside) {
    uint32_t foo;
    foo = mauio->ingress_mpr_nxt_tab();
    phvside[1] = (foo & 0x1FF);
    foo = mauio->egress_mpr_nxt_tab();
    phvside[1] |= ((foo & 0x1FF) << 12);
    foo = mauio->ghost_mpr_nxt_tab();
    phvside[1] |= ((foo & 0x1FF) << 22);
    foo = mauio->global_exec();
    phvside[2] = (foo & 0xFFFF);
    foo = mauio->mpr_global_exec();
    phvside[2] |= ((foo & 0xFFFF) << 16);
    foo = mauio->long_branch();
    phvside[3] = (foo & 0xFF);
    foo = mauio->mpr_long_branch();
    phvside[3] |= ((foo & 0xFF) << 8);
    foo = mauio->ghost_nxt_tab();
    phvside[3] |= ((foo & 0x1FF) << 16);
}


void ref_model_wrapper::extract_mau_lookup_result(const MauLookupResult *mres,
                                                  uint32_t * matchd) {

    char     prefix[256] = "REFM_Lup-Res";
    char     setLogPrefix[256] = "SetLogPrefix";

    set_option(0,setLogPrefix, prefix);


    // New: 8 words per Old: 7 words per
    matchd[0] = (0x1 & mres->valid() )
            | ((0x1 & mres->match() ) << 1 )
            | ((0x1 & mres->exactmatch() ) << 2 )
            | ((0x1 & mres->ternarymatch() ) << 3 )
            | ((0x1 & mres->ternaryindirect() ) << 4 )
            | ((0xFF & mres->payload() ) << 8);

    matchd[1] = mres->row();
    matchd[2] = mres->col();
    matchd[3] = mres->outbus();
    matchd[4] = mres->hitindex();
    matchd[5] = mres->hitentry();
    matchd[6] = (0xFFFF & mres->logical_tables() )
            | ((0xFF & mres->instr() ) << 16)
            | ((0xFF & mres->next_table() ) << 24);
    matchd[7] = (0x1FF & mres->next_table_orig() )
            | ((0x1FF & mres->next_table_form() ) << 9)
            | ((0x1FF & mres->next_table_pred() ) << 18)
            | ((0x1 & (mres->next_table() >> 8))  << 27);
    if (m_log_level >= 300) {  // LOW code
            cout << "ref_model_wrapper::extract_mau_lookup_result:" << std::dec
                 << " v="  <<  mres->valid()
                 << " m="  <<  mres->match()
                 << " em=" <<  mres->exactmatch()
                 << " tm=" <<  mres->ternarymatch()
                 << " ti=" <<  mres->ternaryindirect()
                 << std::hex
                 << " row=0x" << mres->row()
                 << " col=0x" << mres->col()
                 << " outbus=0x" << mres->outbus()
                 << " hitidx=0x" << mres->hitindex()
                 << " hentry=0x" << mres->hitentry()
                 << " ltable=0x" << mres->logical_tables()
                 << " pay=0x"    <<  (static_cast<int>  (mres->payload() ) )
                 << " nexttbl=0x" << (static_cast<int>  (mres->next_table() ) )
                 << " instr=0x"   << (static_cast<int>  (mres->instr() ) )
                 << " nexttbl_org =0x" << (static_cast<int>  (mres->next_table_orig() ))
                 << " nexttbl_form=0x" << (static_cast<int>  (mres->next_table_form() ))
                 << " nexttbl_pred=0x" << (static_cast<int>  (mres->next_table_pred() ))
                 << "\n" << std::dec << std::endl << std::flush;
         }
}  // extract_mau_lookup_result

Mau * ref_model_wrapper::get_mau_stage(uint32_t pipe, uint32_t stage) {

  char     prefix[256] = "REFM_Stage";
  char     setLogPrefix[256] = "SetLogPrefix";

  set_option(0,setLogPrefix, prefix);

  //if (pipe != 0)
  if ( (stage >= m_mau_first_stage) && (stage < (m_mau_first_stage + m_mau_stages) )) {
    if (m_mau_inst_ary[0][stage] != NULL) {
      return m_mau_inst_ary[0][stage];
    } else {
      printf("\nERROR ref_model_wrapper bad ptr for stage =%0d mau_first_stage=%0d endstage=%0d (total=%0d) for maxstages=%0d return 0\n",
             stage, m_mau_first_stage, (m_mau_first_stage + m_mau_stages -1), m_mau_stages, RmtDefs::kStagesMax);
      return m_mau_inst_ary[0][m_mau_first_stage];
    }
  } else {
    printf("\nERROR ref_model_wrapper bad stage =%0d mau_first_stage=%0d endstage=%0d (total=%0d) for maxstages=%0d return 0\n",
           stage, m_mau_first_stage, (m_mau_first_stage + m_mau_stages -1), m_mau_stages, RmtDefs::kStagesMax);

    return m_mau_inst_ary[0][m_mau_first_stage];
  }
}

void ref_model_wrapper::mau_model_tcam_lookup(uint32_t pipe, uint32_t stage,
                                              int * hit_pri, uint32_t phvside,
                                              uint32_t *phv_data, uint32_t* phv_vld,
                                              uint32_t rowI, uint32_t colI) {
    // Get a physical TCAM you then call tcam_lookup on the MAU
    // passing row/col - row in (0-15) col in (0-1)
    uint32_t  *newphvside = new uint32_t[4]();
    char     prefix[256] = "REFM_Tcam-Lup";
    char     setLogPrefix[256] = "SetLogPrefix";

    set_option(0,setLogPrefix, prefix);
    newphvside[0] = phvside; newphvside[1] = 0; newphvside[2] = 0; newphvside[3] = 0;

    Mau *   mau_p = get_mau_stage(pipe, stage);
    MauTcam *tcam =  mau_p->tcam_lookup( (rowI & 0xF), (colI & 0x1) );

    // Finally to lookup a PHV in a TCAM call ->lookup(phv) - that will
    // return the priority of the highest pri match (NOT the index)
    Phv *cur_phv = create_mau_phv_from_data(newphvside, phv_data, phv_vld);
    *hit_pri = tcam->lookup(cur_phv);
    // Cleanup
    m_rmt_obj_mgr_p[0]->phv_delete(cur_phv);
}

void ref_model_wrapper::mau_model_ltcam_lookup_ternary_match(uint32_t pipe, uint32_t stage,
                                                             uint32_t * mresult,
                                                             int ltcamVec, uint32_t phvside,
                                                             const uint32_t *phv_data, const uint32_t* phv_vld) {
    // From Mau get all the results from Logical TCAM [0-7]
    MauLookupResult *mlres = new MauLookupResult();
    MauLogicalTcam * ltcam;
    uint32_t  *newphvside = new uint32_t[4]();
    uint8_t  egress  = phvside & 0x1;
    char     prefix[256] = "REFM_Tcam-Match";
    char     setLogPrefix[256] = "SetLogPrefix";

    set_option(0,setLogPrefix, prefix);
    newphvside[0] = phvside; newphvside[1] = 0; newphvside[2] = 0; newphvside[3] = 0;

    Mau *   mau_p = get_mau_stage(pipe, stage);
    bool ingress = (egress) ? false : true;
    if (m_log_level >= 300) { // DEBUG code
            cout << "ref_model_wrapper::mau_model_ltcam_lookup_ternary_match: lrn_v=" << std::hex << ltcamVec
                 << "  phv data {"  << "\n"  << std::endl << std::flush;
       // DBG for (uint32_t i0 = 0 ; i0 < 224 ; i0 += 1) {
       // DBG   cout << std::dec <<  i0  << "{" << (i0/32) <<"," << (i0%32) << " }" << std::hex
       // DBG        << " v=" << ((phv_vld[i0/32] >> (31 - i0%32)) & 0x1)
       // DBG        << " d=0x" << phv_data[i0] << std::endl;
       // DBG }
       //cout << " }"  << "\n" << std::endl << std::flush;
    }

    // Lookup PHV  ->lookup_ternary_match() - that returns Match Object.
    //  lookup_ternary_match(Phv *match_phv, int logicalTableIndex,
    //            MauLookupResult *result, bool ingress, bool evalAll)
    //
    Phv * mphv = create_mau_phv_from_data(newphvside, phv_data, phv_vld);
    mphv->set_match_only(true);

    if (m_log_level >= 300) {  // DEBUG code
            cout << "ref_model_wrapper::mau_model_ltcam_lookup_ternary_match: lrn_v=" << std::hex <<  ltcamVec
                 << "  input phv {" << std::endl;
            // DBG  mphv->print(NULL, true);
            cout << "}" << "\\n" << std::endl << std::flush;
    }
    for (int i0 = 0 ; i0 < 8 ; i0++) {
      if ( (ltcamVec >> i0) & 0x1) {
         int ltid = -1;
         ltcam = mau_p->logical_tcam_get(i0);
         mlres->reset();
         ltid = ltcam->find_logical_table();
         if ( (ltid < 0) || (ltid > (MauDefs::kLogicalTablesPerMau - 1))) {
                 MauLogicalTcam *pair = ltcam->paired_ltcam_obj(ingress);
                 ltid = (pair != NULL) ?pair->get_logical_table() :-1;
         }
         if (m_log_level >= 300) {
                 cout << "ref_model_wrapper::mau_model_ltcam_lookup_ternary_match: LTcam start egr="<< static_cast<int>(egress)
                      <<  " ltn=" << i0 <<  " ltid=" << ltid << std::dec << "\n" << std::endl << std::flush;
         }
         ltcam->lookup_ternary_match(mphv, ltid, mlres, ingress, false);
         // Extract and pack match result into uint32_t array mresult
         extract_mau_lookup_result( mlres, &mresult[8*i0]);
      }
    }
    // Cleanup
    delete mlres;
    m_rmt_obj_mgr_p[0]->phv_delete(mphv);
} // ref_model_wrapper::mau_model_ltcam_lookup_ternary_match

void ref_model_wrapper::pipe_model_update_dependencies() {
    char     prefix[256] = "REFM_Upd-Dep";
    char     setLogPrefix[256] = "SetLogPrefix";

    set_option(0,setLogPrefix, prefix);

   // TBD pipe has functions to update dependecies - need something similar
   m_pipe_inst_p[0]->update_dependencies();
}

void ref_model_wrapper::mau_model_handle_sweep(uint32_t pipe, uint32_t stage, uint32_t m_id,uint32_t m_cycle,uint64_t meter_sweep_time, uint32_t sweep_ltid,uint32_t home_row,uint32_t sweep_indx,uint32_t *sweep_addr,uint32_t *sweep_dout) {
   MauLogicalRow *lrowp;
   MauAddrDist *mau_addr_dist;
   uint32_t meter_sweep_addr = 0u;
   uint64_t sweep_time = 0u;
   uint64_t relative_time = 0u;
   int      retval;
   int      sweep_subw = (*sweep_addr < 16) ?*sweep_addr :0;
   int      sweep_alu = 0;

   char     prefix[256] = "REFM_Sweep";
   char     setLogPrefix[256] = "SetLogPrefix";

   set_option(0,setLogPrefix, prefix);

   BitVector<MauDefs::kDataBusWidth> stats_meter_wr_data(UINT64_C(0));

   Mau *mau_p    = get_mau_stage(pipe, stage);
   mau_addr_dist = mau_p->mau_addr_dist();
   lrowp         = mau_p->logical_row_lookup(home_row);
   if (m_log_level >= 200) { printf("ref_model_wrapper::mau_model_handle_sweep: Called Meter sweep (id=%0d,home_row=%0d) for ltid=%0d received at MAU input in cycle=%0d. Sweep cycle time=0x%" PRIX64 " (cycles) sweep_index=%0d, sweep_subword=%d,",m_id,home_row,sweep_ltid,m_cycle,meter_sweep_time,sweep_indx,sweep_subw); }
   //mau_addr_dist->meter_sweep(sweep_ltid,UINT64_C(meter_sweep_time*800));//cycle_tiem * 800 to get ps
   sweep_time = meter_sweep_time;
   relative_time = m_cycle;
   sweep_alu  = home_row/4; //CRB09062015 , now sweeps happen in alu/physical space
   retval     = mau_addr_dist->meter_sweep_one_index_with_reltime(sweep_alu,sweep_indx,sweep_subw,
                                                                  sweep_time,true, relative_time);
   if (retval < 0) {
     printf("ref_model_wrapper::mau_model_handle_sweep: UVM_ERROR Retval=%0d (error) from RefModel when called Meter sweep (id=%0d,home_row=%0d) for ltid=%0d )\n",retval,m_id,home_row,sweep_ltid);
   }
   lrowp->meter_rd_addr(&meter_sweep_addr);
   lrowp->stats_wr_data(&stats_meter_wr_data);
   sweep_dout[0] = stats_meter_wr_data.get_word(0,  32);
   sweep_dout[1] = stats_meter_wr_data.get_word(32, 32);
   sweep_dout[2] = stats_meter_wr_data.get_word(64, 32);
   sweep_dout[3] = stats_meter_wr_data.get_word(96, 32);
   *sweep_addr   = meter_sweep_addr;
   if (m_log_level >= 300) { printf("ref_model_wrapper::mau_model_handle_sweep: After sweep (id=%0d,home_row=%0d alu=%0d) for ltid=%0d RefModel output sweep_addr=0x%08x, sweep_data_out=0x%08x_%08x_%08x_%08x \n",m_id,home_row,sweep_alu,sweep_ltid,*sweep_addr,sweep_dout[3],sweep_dout[2],sweep_dout[1],sweep_dout[0]); }

} // mau_model_handle_sweep

void ref_model_wrapper::mau_model_handle_dteop(uint32_t pipe, uint32_t stage,
                                               uint32_t phvside, uint32_t *dteop, uint64_t *ts,
                                               uint64_t *rng, uint32_t active, uint32_t *data_in,
                                               uint32_t *color, uint32_t eopid, uint32_t eop_cycle,
                                               int *retval ) {
   Teop   model_dteop;
   MauLogicalRow *lrowp;
   uint8_t  color_wr_data;
   Mau * mau_p = get_mau_stage(pipe, stage);
   uint32_t alu_n = 0;
   uint8_t  egress  =  phvside & 0x1;
   uint8_t  dteop_err_fcs   = (phvside >> 1) & 1;
   uint8_t  dteop_err_trunc = (phvside >> 2) & 1;
   //uint8_t  hdrport = (phvside >> 8) & 0x7F;
   //uint8_t  hrecirc = (phvside >> 15) & 0x1;
   uint16_t pktlen  = (phvside >> 16) & 0x3FFF;
   uint32_t addr[4];
   uint8_t color_in[4];
   uint8_t stats_en[4];
   uint8_t meter_en[4];
   //uint8_t  eopnum  = Eop::make_eopnum(hdrport, hrecirc);

   char     prefix[256] = "REFM_dTeop";
   char     setLogPrefix[256] = "SetLogPrefix";

   sprintf(prefix, "REFM_dTeop#%de%d#_EopCycle#%d#", eopid, egress?1:0,eop_cycle);
   set_option(0,setLogPrefix, prefix);


   for (int i0 = 0 ; i0 < 4 ; i0++) {
       addr[i0] = (dteop[i0] >> 4 ) & 0x3FFFF;
       color_in[i0] = (dteop[i0] >> 2 ) & 0x3;
       stats_en[i0] = (dteop[i0] >> 1 ) & 0x1;
       meter_en[i0] = (dteop[i0] >> 0 ) & 0x1;
       if (m_log_level >= 200) {
          printf("ref_model_wrapper::mau_model_handle_dteop:#dteop->set_raw_addr(%2d, 0x%x); /* RefModel Meter dTeop#%0de%01d# */\n", i0, dteop[i0],eopid,egress);
       }
   }

   for (int i0 = 0 ; i0 < 4 ; i0++) {
     model_dteop.set_addr(i0, addr[i0], stats_en[i0], meter_en[i0], color_in[i0]);
     if (active & (1<<((i0*4) + 3))) {
       if (m_log_level >= 200) {
               printf("ref_model_wrapper::mau_model_handle_dteop:#dteop->set_meter_tick_time(%0d, %2d, 0x%" PRIX64 "); /* RefModel Meter dTeop#%0de%01d# */\n", stage, i0, ts[i0],eopid,egress);
               printf("ref_model_wrapper::mau_model_handle_dteop:#dteop->set_meter_random_value(%0d, %2d, 0x%" PRIX64 "); /* RefModel Meter dTeop#%0de%01d# */\n", stage, i0, rng[i0],eopid,egress);
       }
       model_dteop.set_meter_tick_time(stage, i0, ts[i0]);
       model_dteop.set_meter_random_value(stage, i0, rng[i0]);
     }
   }
   model_dteop.set_byte_len(pktlen);
   if (dteop_err_fcs) model_dteop.set_error_fcs();
   if (dteop_err_trunc) model_dteop.set_error_trunc();
   if (m_log_level >= 200) {
     printf("ref_model_wrapper::mau_model_handle_dteop:#dteop->set_relative_time(%0d); /* RefModel Meter dTeop#%0de%01d# */\n", eop_cycle,eopid,egress);
     printf("ref_model_wrapper::mau_model_handle_dteop:#dteop->set_byte_len(%d); /* RefModel Meter dTeop#%0de%01d# */\n", pktlen,eopid,egress);
     printf("ref_model_wrapper::mau_model_handle_dteop:#dteop->set_error(0x%02x); /* RefModel Meter dTeop#%0de%01d# */\n", model_dteop.errors(),eopid,egress);
   }
   model_dteop.set_relative_time(eop_cycle);
   if (m_log_level >= 200) { printf("ref_model_wrapper::mau_model_handle_dteop:#handle_dp_teop(dteop); /* RefModel Meter dTeop#%0de%01d# */\n",eopid,egress); }
   *retval = mau_p->handle_dp_teop(model_dteop);

   for (int lrow = 3; lrow <16; lrow=lrow+4) {
     uint32_t meter_rd_addr = 0u;
     uint32_t offset;
     bool     meter_run=false;
     BitVector<MauDefs::kDataBusWidth> stats_meter_rd_data(UINT64_C(0));
     if (active & (1<<lrow)) {
       color_wr_data=0;
       lrowp = mau_p->logical_row_lookup(lrow);
       lrowp->meter_rd_addr(&meter_rd_addr); //1-32b
       //       lrowp->stats_alu_rd_data(&stats_meter_rd_data); //4-32b
       lrowp->get_meter_alu_input_data(&stats_meter_rd_data); //4-32b
       lrowp->color_write_data(&color_wr_data);
       meter_run    = lrowp->color_write_data_was_set();
       color[alu_n] = color_wr_data;
       if (meter_run) {
         color[alu_n] = (color[alu_n] | 0x100); // Set color_valid represented by second nibble or bit8(bit0-7 store color from meter)
       }
       offset = alu_n*4;
       data_in[offset+0] = stats_meter_rd_data.get_word(0,  32);
       data_in[offset+1] = stats_meter_rd_data.get_word(32, 32);
       data_in[offset+2] = stats_meter_rd_data.get_word(64, 32);
       data_in[offset+3] = stats_meter_rd_data.get_word(96, 32);
       if (m_log_level >= 200) { printf("ref_model_wrapper::mau_model_handle_dteop:In handle_dteop: meter_rd_data[lrn=%2d,alu=%2d] = 0x%08x_0x%08x_0x%08x_0x%08x, rd_addr=0x%08x, output=0x%x\n",
                                        lrow, alu_n, data_in[offset+3], data_in[offset+2], data_in[offset+1], data_in[offset+0], meter_rd_addr, static_cast<int>(color_wr_data) );
       }
     }
     alu_n++;
  }
} // mau_model_handle_dteop



void ref_model_wrapper::mau_model_handle_eop(uint32_t pipe, uint32_t stage,
                                             uint32_t phvside, uint64_t *ts, uint64_t *rng,
                                             uint32_t active, uint32_t *data_in, uint32_t *color,
                                             uint32_t eopid, uint32_t eop_cycle, int *retval) {
   Eop   eop;
   MauLogicalRow *lrowp;
   uint8_t  color_wr_data;
   Mau * mau_p = get_mau_stage(pipe, stage);
   uint32_t alu_n = 0;
   uint8_t  egress  =  phvside & 0x1;
   uint8_t  eoperr  = (phvside >> 4) & 0x1;
   //bool eoperr = ((phvside >> 4) & 0x1) ? true : false;
   uint8_t  hdrport = (phvside >> 8) & 0x7F;
   uint8_t  hrecirc = (phvside >> 15) & 0x1;
   uint16_t pktlen  = (phvside >> 16) & 0xFFFF;
   uint8_t  eopnum  = Eop::make_eopnum(hdrport, hrecirc);

   char     prefix[256] = "REFM_Eop";
   char     setLogPrefix[256] = "SetLogPrefix";

   sprintf(prefix, "REFM_Eop#%de%d#_EopCycle#%d#", eopid, egress,eop_cycle);
   set_option(0,setLogPrefix, prefix);

   if (egress) {
     eop.set_egress_eopinfo(pktlen, eopnum,eoperr);
     if (m_log_level >= 200) { printf("ref_model_wrapper::mau_model_handle_eop: #eop->set_egress_eopinfo(0x%04x, 0x%02x, 0x%02x); /* RefModel Meter Eop#%0de%01d# */\n",
                                      pktlen,eopnum,static_cast<int>(eoperr), eopid,egress); }
   } else {
     eop.set_ingress_eopinfo(pktlen, eopnum,eoperr);
     if (m_log_level >= 200) { printf("ref_model_wrapper::mau_model_handle_eop: #eop->set_ingress_eopinfo(0x%04x, 0x%02x, 0x%02x); /* RefModel Meter Eop#%0de%01d# */\n",
                                      pktlen,eopnum,static_cast<int>(eoperr), eopid, egress); }
   }

   for (int i0 = 0 ; i0 < 4 ; i0++) {
     if (active & (1<<((i0*4) + 3))) {
       if (m_log_level >= 200) {
               printf("ref_model_wrapper::mau_model_handle_eop:#eop->set_meter_tick_time(%0d, %2d, 0x%" PRIX64 "); /* RefModel Meter Eop#%0de%01d# */\n", stage, i0, ts[i0],eopid,egress);
               printf("ref_model_wrapper::mau_model_handle_eop:#eop->set_meter_random_value(%0d, %2d, 0x%" PRIX64 "); /* RefModel Meter Eop#%0de%01d# */\n", stage, i0, rng[i0],eopid,egress);
          printf("ref_model_wrapper::mau_model_handle_eop:#eop->set_relative_time(%0d); /* RefModel Meter Eop#%0de%01d# */\n", eop_cycle,eopid,egress);
       }
       eop.set_meter_tick_time(stage, i0, ts[i0]);
       eop.set_meter_random_value(stage, i0, rng[i0]);
       eop.set_relative_time(eop_cycle);
     }
   }
   if (m_log_level >= 200) { printf("ref_model_wrapper::mau_model_handle_eop:#handle_eop(eop); /* RefModel Meter Eop#%0de%01d# */\n",eopid,egress); }
   *retval = mau_p->handle_eop(eop);

   for (int lrow = 3; lrow <16; lrow=lrow+4) {
     uint32_t meter_rd_addr = 0u;
     uint32_t offset;
     bool     meter_run=false;
     BitVector<MauDefs::kDataBusWidth> stats_meter_rd_data(UINT64_C(0));
     if (active & (1<<lrow)) {
       color_wr_data=0;
       lrowp = mau_p->logical_row_lookup(lrow);
       lrowp->meter_rd_addr(&meter_rd_addr); //1-32b
       //       lrowp->stats_alu_rd_data(&stats_meter_rd_data); //4-32b
       lrowp->get_meter_alu_input_data(&stats_meter_rd_data); //4-32b
       lrowp->color_write_data(&color_wr_data);
           meter_run    = lrowp->color_write_data_was_set();
       color[alu_n] = color_wr_data;
           if (meter_run) {
         color[alu_n] = (color[alu_n] | 0x100); // Set color_valid represented by second nibble or bit8(bit0-7 store color from meter)
       }
       offset = alu_n*4;
       data_in[offset+0] = stats_meter_rd_data.get_word(0,  32);
       data_in[offset+1] = stats_meter_rd_data.get_word(32, 32);
       data_in[offset+2] = stats_meter_rd_data.get_word(64, 32);
       data_in[offset+3] = stats_meter_rd_data.get_word(96, 32);
       if (m_log_level >= 200) { printf("ref_model_wrapper::mau_model_handle_eop:In handle_eop: meter_rd_data[lrn=%2d,alu=%2d] = 0x%08x_0x%08x_0x%08x_0x%08x, rd_addr=0x%08x, output=0x%x\n",
                                        lrow, alu_n, data_in[offset+3], data_in[offset+2], data_in[offset+1], data_in[offset+0], meter_rd_addr, static_cast<int>(color_wr_data) );
       }
     }
     alu_n++;
  }
} // mau_model_handle_eop

void ref_model_wrapper::mau_model_disable_stateful_alu_checks(uint32_t pipe, uint32_t stage) {
  // don't check that the total is correct in Adjust Total instructions to avoid assertions when
  //  data is put in which hasn't got a correct total (eg some forwarding cases)
  RmtObjectManager *om = m_rmt_obj_mgr_p[0];

  char     prefix[256] = "REFM_Disable-Stat";
  char     setLogPrefix[256] = "SetLogPrefix";

  set_option(0,setLogPrefix, prefix);

  for (int which_alu=0; which_alu<4; which_alu++) {
    int logrow = MauMeterAlu::get_meter_alu_logrow_index(which_alu);
    MauStatefulAlu* salu = om->mau_get(pipe, stage)->logical_row_lookup(logrow)->mau_meter_alu()->get_salu();
    assert( salu );
    salu->set_check_total_correct(false);
  }
}

  void ref_model_wrapper::mau_model_relax_mau_tcam_array( ) {

    char     model_option[256]        = "RelaxThreadCheck";
    char     model_value[256]         = "true";

    set_option(0, model_option, model_value );
    //!!   set_option( "RelaxThreadCheck" , "true" );
  }

void ref_model_wrapper::mau_model_run_mau_tcam_array( uint32_t pipe, uint32_t stage,
                                                       uint32_t *phvside,
                                                       int      *next_table,
						       uint64_t *kIxbarTmData,
                                                       uint32_t *tcam_match_addr,
                                                       int      *tcam_match_hit,
                                                       uint32_t iphv_cycle, uint32_t ophv_cycle,
                                                       int      *retval ) {

   char     prefix[256]       = "REFM_TcamArray";
   char     setLogPrefix[256] = "SetLogPrefix";

   //!! added
   //char     model_option[256]        = "RelaxThreadCheck";
   //char     model_value[256]         = "true";

   set_option(0, setLogPrefix, prefix );

   //!! added
   //!!   set_option( model_option, model_value );
   //!!   set_option( "RelaxThreadCheck" , "true" );

   Mau      *mau_p;
   Phv      *iphv, *ophv;
   //Phv    *next_iphv, *next_ophv;
   MauIO    *mau_i;
   //MauIO  *mau_o;

   int      i;
   //int    j,k;
   int      search_data_words = 9;
   uint8_t  action_bit[8];
   uint8_t  tcam_hit[8];
   //!! int      tcam_hit2[8];

   mau_p = get_mau_stage( pipe, stage );

   iphv = create_mau_phv( phvside );
   ophv = create_mau_phv( phvside );

   // FIXME: do I really need to initialize to zero?
   for ( uint32_t i0=0; i0 < RmtDefs::kPhvWordsMax; i0++ ) {
      iphv->set( i0, 0 );
      ophv->set( i0, 0 );
   }

   iphv->set_pipe_data_ctrl(0, PhvData::kIxbarTmData, PhvDataCtrl::kLoadFromPhv);
   ophv->set_pipe_data_ctrl(0, PhvData::kIxbarTmData, PhvDataCtrl::kLoadFromPhv);
   iphv->set_pipe_data_ctrl(0, PhvData::kTcamMatchAddr, PhvDataCtrl::kCalcAndStore);
   ophv->set_pipe_data_ctrl(0, PhvData::kTcamMatchAddr, PhvDataCtrl::kCalcAndStore);

   //!!   iphv->set_relative_time( iphv_cycle );
   //!!   ophv->set_relative_time( ophv_cycle );

   ref_model_wrapper::getInstance()->mau_model_reset_resources( pipe, stage );

   mau_i = mau_p->mau_io_input();
   set_mauio_via_phvside( mau_i, phvside );

   if (m_log_level >= 200) {  // DEBUG code
      cout << "ref_model_wrapper::mau_model_run_mau_tcam_array (input2):" << std::endl;
      cout << "     START TABLE { 0x" << std::hex << next_table[2] << ", 0x" << next_table[1] <<
                    ", 0x" << next_table[0] << " } glob=0x" << mau_i->global_exec() <<
                    "  lbr=0x" << static_cast<int>(mau_i->long_branch()) << std::endl ;
      cout << " MPR START TABLE { 0x" << mau_i->ghost_mpr_nxt_tab() << ", 0x" << mau_i->egress_mpr_nxt_tab() <<
              ", 0x" <<  mau_i->ingress_mpr_nxt_tab() << " } glob=0x" << mau_i->mpr_global_exec() <<
              "  lbr=0x" << static_cast<int>(mau_i->mpr_long_branch()) <<
              "\n" << std::dec << std::endl << std::flush;
   }

   for(i = 0; i < search_data_words; i++) {
     iphv->set_pipe_data(0, PhvData::kIxbarTmData, 64*i, kIxbarTmData[i], 64);
   }

   //!! Don't set, but must later get tcam_match_addr
   /*
   for ( int i = 0; i < 8; i++ ) {
      iphv->set_pipe_data_tcam_match_addr( 0, i, tcam_match_addr[i], 0 );
      ophv->set_pipe_data_tcam_match_addr( 0, i, tcam_match_addr[i], 0 );
   }
   */

   //!!next_iphv = mau_p->process_match( iphv, ophv, &next_table[0], &next_table[1], &next_table[2] );
   mau_p->process_match( iphv, ophv, &next_table[0], &next_table[1], &next_table[2] );
   cout << "ref_model_wrapper::mau_model_run_mau_tcam_array after process match:" << std::endl << std::flush;

   //!!!!   *retval = *retval | ( next_iphv == NULL );


   //!! stop after process_match
   /*
   *retval = *retval | ( next_iphv == NULL );

   next_ophv = mau_p->process_action( iphv, ophv );

   *retval = *retval | ( next_ophv == NULL );

   mau_o = mau_p->mau_io_output();
   get_mauio_into_phvside( mau_o, phvside );

   //
   next_table[0] = mau_o->ingress_nxt_tab();
   next_table[1] = mau_o->egress_nxt_tab();
   next_table[2] = mau_o->ghost_nxt_tab();

   if (m_log_level >= 200) {  // DEBUG code
      cout << "ref_model_wrapper::mau_model_run_mau_tcam_array (output):" << std::endl;
      cout << "     START TABLE { 0x" << std::hex << next_table[2] << ", 0x" << next_table[1] <<
                    ", 0x" << next_table[0] << " } glob=0x" << mau_o->global_exec() <<
                    "  lbr=0x" << static_cast<int>(mau_o->long_branch()) << std::endl;
      cout << " MPR START TABLE { 0x" << mau_o->ghost_mpr_nxt_tab() << ", 0x" << mau_o->egress_mpr_nxt_tab() <<
              ", 0x" <<  mau_o->ingress_mpr_nxt_tab() << " } glob=0x" << mau_o->mpr_global_exec() <<
              "  lbr=0x" << static_cast<int>(mau_o->mpr_long_branch()) <<
              "\n" << std::dec << std::endl << std::flush;
   }

   */

   // -1 is the "error" return value
   *retval = 0;

   //!! Get tcam_match_addr
   cout << "ref_model_wrapper::mau_model_run_mau_tcam_array before get_pipe_data_tcam_match_addr\n" << std::endl << std::flush;
   for ( i = 0; i < 8; i++ ) {
     //!!      iphv->get_pipe_data_tcam_match_addr( 0, i, &tcam_match_addr[i], &action_bit[i] );
      cout << "ref_model_wrapper::mau_model_run_mau_tcam_array before get_pipe_data_tcam_match_addr == 0x " << std::hex << *(tcam_match_addr+i) << "\n" << std::endl << std::flush;
      //!!      iphv->get_pipe_data_tcam_match_addr( 0, i, tcam_match_addr+i, &tcam_hit[i], &action_bit[i] );
      tcam_hit[i] = 0; //!! zero out hit indicator before getting new value.
      //!! tcam_hit2[i] = 0; //!! zero out hit indicator before getting new value.
      //!!      cout << "ref_model_wrapper::mau_model_run_mau_tcam_array before tcam_hit[i] = : " << (static_cast<unsigned int>(tcam_hit[i]) & 0xFF) << std::endl << std::flush;
      //!!      cout << "ref_model_wrapper::mau_model_run_mau_tcam_array before tcam_hit2[i] = : " << std::dec << tcam_hit2[i] << std::endl << std::flush;
      cout << "ref_model_wrapper::mau_model_run_mau_tcam_array before tcam_hit[ " << std::dec << i << "] == : " << (static_cast<unsigned int>(tcam_hit[i]) & 0xFF) << " :\n" << std::endl << std::flush;
      //!!      cout << "ref_model_wrapper::mau_model_run_mau_tcam_array before tcam_hit2[ " << std::dec << i << "] == : " << std::hex << tcam_hit2[i] << " :\n" << std::endl << std::flush;
      iphv->get_pipe_data_tcam_match_addr( 0, i, tcam_match_addr+i, tcam_hit+i, action_bit+i );
      tcam_match_hit[i] = (static_cast<unsigned int>(tcam_hit[i]) & 0xFF);
      //!! iphv->get_pipe_data_tcam_match_addr( 0, i, tcam_match_addr+i, tcam_hit2+i, action_bit+i );
      cout << "ref_model_wrapper::mau_model_run_mau_tcam_array after get_pipe_data_tcam_match_addr == 0x " << std::hex << *(tcam_match_addr+i) << "\n" << std::endl << std::flush;
      cout << "ref_model_wrapper::mau_model_run_mau_tcam_array after tcam_hit[ " << std::dec << i << "] == : " << (static_cast<unsigned int>(tcam_hit[i]) & 0xFF) << " :\n" << std::endl << std::flush;
      //!!      // This is the hit status
      //!!      if((static_cast<unsigned int>(tcam_hit[i]) & 0xFF)) {
      //!!	//!!	*retval = (524288) | (static_cast<int>(tcam_match_addr[i]) & 0x7FFFF)  ;
      //!!	*retval |= 1;
      //!!      }
      //!!      else {
      //!!	*retval = 0;
      //!!      }
      *retval = *retval | (static_cast<int>(tcam_hit[i]) & 0xFF)  ;
      cout << "ref_model_wrapper::mau_model_run_mau_tcam_array after retval[ " << std::dec << i << "] == : " << std::dec << *retval << " :\n" << std::endl << std::flush;
      //!!      cout << "ref_model_wrapper::mau_model_run_mau_tcam_array after tcam_hit2[ " << std::dec << i << "] == : " << std::hex << tcam_hit2[i] << " :\n" << std::endl << std::flush;
   }
   /*
   if (m_log_level >= 200) {  // DEBUG code
      cout << "ref_model_wrapper::mau_model_run_mau_tcam_array (output):" << std::endl;
      cout << "     tcam_match_addr: " << *(tcam_match_addr)  << std::endl;
      cout << "     START TABLE { 0x" << std::hex << next_table[2] << ", 0x" << next_table[1] <<
                    ", 0x" << next_table[0] << " } glob=0x" << mau_o->global_exec() <<
                    "  lbr=0x" << static_cast<int>(mau_o->long_branch()) << std::endl;
      cout << " MPR START TABLE { 0x" << mau_o->ghost_mpr_nxt_tab() << ", 0x" << mau_o->egress_mpr_nxt_tab() <<
              ", 0x" <<  mau_o->ingress_mpr_nxt_tab() << " } glob=0x" << mau_o->mpr_global_exec() <<
              "  lbr=0x" << static_cast<int>(mau_o->mpr_long_branch()) <<
              "\n" << std::dec << std::endl << std::flush;
   }
   */

   m_rmt_obj_mgr_p[0]->phv_delete( iphv );
   m_rmt_obj_mgr_p[0]->phv_delete( ophv );

   //!!!!   retval = 0;

} //mau_model_run_mau_tcam_array

void ref_model_wrapper::mau_model_run_mau_memory_core( uint32_t pipe, uint32_t stage,
                                                       uint32_t *phvside,
                                                       int      *next_table,
                                                       uint32_t *tcam_match_addr,
                                                       uint32_t iphv_cycle, uint32_t ophv_cycle,
                                                       int      *retval ) {
   char     prefix[256]       = "REFM_MemCore";
   char     setLogPrefix[256] = "SetLogPrefix";

   set_option(0, setLogPrefix, prefix );

   Mau      *mau_p;
   Phv      *iphv, *ophv, *next_iphv, *next_ophv;
   MauIO    *mau_i, *mau_o;

   mau_p = get_mau_stage( pipe, stage );

   iphv = create_mau_phv( phvside );
   ophv = create_mau_phv( phvside );

   // FIXME: do I really need to initialize to zero?
   for ( uint32_t i0=0; i0 < RmtDefs::kPhvWordsMax; i0++ ) {
      iphv->set( i0, 0 );
      ophv->set( i0, 0 );
   }

   iphv->set_pipe_data_ctrl( stage, PhvData::kTcamMatchAddr, PhvDataCtrl::kLoadFromPhv );
   ophv->set_pipe_data_ctrl( stage, PhvData::kTcamMatchAddr, PhvDataCtrl::kLoadFromPhv );

   iphv->set_relative_time( iphv_cycle );
   ophv->set_relative_time( ophv_cycle );

   ref_model_wrapper::getInstance()->mau_model_reset_resources( pipe, stage );

   mau_i = mau_p->mau_io_input();
   set_mauio_via_phvside( mau_i, phvside );

   if (m_log_level >= 200) {  // DEBUG code
      cout << "ref_model_wrapper::mau_model_run_mau_memory_core (input):" << std::endl;
      cout << "     START TABLE { 0x" << std::hex << next_table[2] << ", 0x" << next_table[1] <<
                    ", 0x" << next_table[0] << " } glob=0x" << mau_i->global_exec() <<
                    "  lbr=0x" << static_cast<int>(mau_i->long_branch()) << std::endl ;
      cout << " MPR START TABLE { 0x" << mau_i->ghost_mpr_nxt_tab() << ", 0x" << mau_i->egress_mpr_nxt_tab() <<
              ", 0x" <<  mau_i->ingress_mpr_nxt_tab() << " } glob=0x" << mau_i->mpr_global_exec() <<
              "  lbr=0x" << static_cast<int>(mau_i->mpr_long_branch()) <<
              "\n" << std::dec << std::endl << std::flush;
   }

   for ( int i = 0; i < 8; i++ ) {
     //!!      iphv->set_pipe_data_tcam_match_addr( 0, i, tcam_match_addr[i], 0 );
     //!!      ophv->set_pipe_data_tcam_match_addr( 0, i, tcam_match_addr[i], 0 );
     iphv->set_pipe_data_tcam_match_addr( 0, i, tcam_match_addr[i], 1, 0 );
     ophv->set_pipe_data_tcam_match_addr( 0, i, tcam_match_addr[i], 1, 0 );
   }

   next_iphv = mau_p->process_match( iphv, ophv, &next_table[0], &next_table[1], &next_table[2] );

   *retval = *retval | ( next_iphv == NULL );

   next_ophv = mau_p->process_action( iphv, ophv );

   *retval = *retval | ( next_ophv == NULL );

   mau_o = mau_p->mau_io_output();
   get_mauio_into_phvside( mau_o, phvside );

   //
   next_table[0] = mau_o->ingress_nxt_tab();
   next_table[1] = mau_o->egress_nxt_tab();
   next_table[2] = mau_o->ghost_nxt_tab();

   if (m_log_level >= 200) {  // DEBUG code
      cout << "ref_model_wrapper::mau_model_run_mau_memory_core (output):" << std::endl;
      cout << "     START TABLE { 0x" << std::hex << next_table[2] << ", 0x" << next_table[1] <<
                    ", 0x" << next_table[0] << " } glob=0x" << mau_o->global_exec() <<
                    "  lbr=0x" << static_cast<int>(mau_o->long_branch()) << std::endl;
      cout << " MPR START TABLE { 0x" << mau_o->ghost_mpr_nxt_tab() << ", 0x" << mau_o->egress_mpr_nxt_tab() <<
              ", 0x" <<  mau_o->ingress_mpr_nxt_tab() << " } glob=0x" << mau_o->mpr_global_exec() <<
              "  lbr=0x" << static_cast<int>(mau_o->mpr_long_branch()) <<
              "\n" << std::dec << std::endl << std::flush;
   }

   m_rmt_obj_mgr_p[0]->phv_delete( iphv );
   m_rmt_obj_mgr_p[0]->phv_delete( ophv );

   retval = 0;
} //mau_model_run_mau_memory_core

void ref_model_wrapper::get_table_active( uint32_t pipe, uint32_t stage, uint32_t *table_active ) {
   Mau* mau_p = get_mau_stage( pipe, stage );

   BitVector<MauDefs::kLogicalTablesPerMau> tbl_act_vec;

   tbl_act_vec = mau_p->table_active();
   // Get Powered table vector in upper half of table_active
   *table_active  = ( ( ( ( uint32_t ) mau_p->pred_lt_info( Pred::kPowered ) ) << 16 ) & 0xFFFF0000 ) |
                      ( ( ( uint32_t ) tbl_act_vec.get_byte( 1 ) ) << 8 ) |
                        ( ( uint32_t ) tbl_act_vec.get_byte( 0 ) );
}

void ref_model_wrapper::mau_model_process_match_n_action(uint32_t pipe, uint32_t stage,
                                                         uint32_t *next_iphv_data, uint32_t *next_iphv_vld,
                                                         uint32_t *next_ophv_data, uint32_t *next_ophv_vld,
                                                         uint32_t *mresult, uint32_t *actdatabus,
                                                         uint32_t *hashgen, uint32_t *gateway,
                                                         uint32_t *ehit_raw, uint32_t *em_resbus,
                                                         uint32_t *ltcamres,
                                                         uint32_t *iphv_data, uint32_t *iphv_vld,
                                                         uint32_t *ophv_data, uint32_t *ophv_vld,
                                                         uint32_t *phvside, uint32_t ltcamIndex, uint32_t *table_active,
                                                         int *ingress_next_table, int *egress_next_table, uint32_t *action_cnt,
                                                         uint32_t *hashdist, uint32_t *alu_io, uint32_t lpf_active,
                                                         uint64_t *lpf_ts, uint64_t *lpf_rng, uint32_t lpfid,
                                                         uint32_t iphv_cycle, uint32_t ophv_cycle,
                                                         uint32_t *snapshot_data, uint32_t *teop_result, uint32_t *mpr_result, int *retval) {
   /* phvside: uint32 array[4] (tofino is input only and [1])
      tofino & jbay:
       array[0]
        [   0] egr  1=egr 0=ing
        [ 5:4] pktversion
        [14:8] hdr port
        [  16] hdrport recirc
      jbay only:
    bout[40:32] = m_mpr_next_table[0]; //9b
    bout[52:44] = m_mpr_next_table[1]; //9b
    bout[62:54] = m_mpr_next_table[2]; //9b
    bout[79:64] = m_glob_exec; // 16
    bout[95:80] = m_mpr_glob_exec; // 16
    bout[103:96]  = m_long_brch; // 8
    bout[111:104] = m_mpr_long_brch; // 8
    bout[120:112] = m_next_table[2]; // ghost 9b
 */
    static uint32_t iphv_cycle_last[2] = {0, 0};
    static uint32_t ophv_cycle_last[2] = {0, 0};

    const uint32_t MRSIZE = 17;
    uint8_t  egress  = phvside[0] & 0x1;
    uint8_t  ingress = (!egress) & 0x1;
    int      hashidx = 0;
    int      idx = 0;

   char     prefix[256] = "REFM_MatAct";
   char     setLogPrefix[256] = "SetLogPrefix";

   sprintf(prefix, "REFM_MatAct#%de%d#_OphvCycle#%d#", lpfid, egress,ophv_cycle);
   set_option(0,setLogPrefix, prefix);

    Phv *next_iphv;
    Phv *next_ophv;
    Phv *iphv;
    Phv *ophv;
    Phv *match_inpt_phv;

    BitVector<MauDefs::kLogicalTablesPerMau> tbl_act_vec;
    BitVector<MauDefs::kActionHVOutputBusWidth> *myadb;
    Mau *   mau_p = get_mau_stage(pipe, stage);

    MauLookupResult::kRelaxLookupShiftPfePosCheck = true; //FIXME:
    RmtObjectManager *om = m_rmt_obj_mgr_p[0];

    *retval = 0;

    if (m_log_level >= 200) {  // DEBUG code
            cout << "ref_model_wrapper::mau_model_process_match_n_action: pipe=" << pipe <<
                    " stage=" << stage << "\n" << std::endl << std::flush;
    }

    iphv = create_mau_phv_from_data(phvside, iphv_data, iphv_vld);
    ophv = create_mau_phv_from_data(phvside, ophv_data, ophv_vld);

    if (m_log_level >= 200) {
       printf("ref_model_wrapper::mau_model_process_match_n_action: Phv#%de%d#_iPhvCycle#%0d#_oPhvCycle#%0d#/\n",lpfid,egress, iphv_cycle,ophv_cycle);
       printf("ref_model_wrapper::mau_model_process_match_n_action: #iphv_in->set_relative_time(%0d) /* Meter #%0de%01d# */\n", iphv_cycle,lpfid,egress);
       printf("ref_model_wrapper::mau_model_process_match_n_action: #ophv_in->set_relative_time(%0d) /* Meter #%0de%01d# */\n", ophv_cycle,lpfid,egress);
    }
    iphv->set_relative_time(iphv_cycle);
    ophv->set_relative_time(ophv_cycle);

    if (iphv_cycle_last[egress] != iphv_cycle) {
      iphv_cycle_last[egress] = iphv_cycle;
      iphv->set_match_only(false);
    } else {
      iphv->set_match_only(true);
    }

    if (ophv_cycle_last[egress] != ophv_cycle) {
      ophv_cycle_last[egress] = ophv_cycle;
      ophv->set_match_only(false);
    } else {
      ophv->set_match_only(true);
    }

    match_inpt_phv = mau_p->make_match_phv(iphv, ophv); // Get effective iPhv after input mux_sel
    match_inpt_phv->set_match_only(true);

    MauIO *miop = mau_p->mau_io_input();
    set_mauio_via_phvside(miop, phvside);
    if (m_log_level >= 200) {  // DEBUG code
      cout << "ref_model_wrapper::mau_model_process_match_n_action: INPUT iphv=" << iphv <<
              " ophv=" << ophv << std::endl ;
      cout << " start_tbl{0x" << std::hex << miop->ghost_nxt_tab() << ", 0x" << *egress_next_table <<
                    ", 0x" << *ingress_next_table << "} glob=0x" << miop->global_exec() <<
                    "  lbr=0x" << static_cast<int>(miop->long_branch()) << std::endl ;
      cout << " MPR sttbl{0x" << miop->ghost_mpr_nxt_tab() << ", 0x" << miop->egress_mpr_nxt_tab() <<
              ", 0x" <<  miop->ingress_mpr_nxt_tab() << "} glob=0x" << miop->mpr_global_exec() <<
              "  lbr=0x" << static_cast<int>(miop->mpr_long_branch()) <<
              "\n" << std::dec << std::endl << std::flush;
    }

    ref_model_wrapper::getInstance()->mau_model_reset_resources(pipe, stage);
    if (m_log_level >= 300) {  // DEBUG code
            cout << "ref_model_wrapper::mau_model_process_match_n_action: extract LTcam results start egr="<< static_cast<int>(egress)
                 <<  " ltcam_en=" << std::hex << ltcamIndex << std::dec << "\n" << std::endl << std::flush;
    }
    // Set pred for ltcam lookup - needed Oct 2019 tcam changes (JHR)
    bool thread_active[3] = { match_inpt_phv->ingress(), match_inpt_phv->egress(), match_inpt_phv->ghost() };
    mau_p->pred_start(thread_active);
    
    // Extract Logical Tcam info
    MauLookupResult *ltres0 = new MauLookupResult();
    MauLogicalTcam * ltcam0;
    for (int i0 = 0 ; i0 < MauDefs::kLogicalTcamsPerMau ; i0++) {
      if ( (ltcamIndex >> i0) & 0x1) {
         int ltid = -1;
         ltcam0 = mau_p->logical_tcam_get(i0);
         ltres0->reset();
         ltid = ltcam0->find_logical_table();
         if ( (ltid < 0) || (ltid > (MauDefs::kLogicalTablesPerMau - 1))) {
                 MauLogicalTcam *pair = ltcam0->paired_ltcam_obj(ingress);
                 ltid = (pair != NULL) ?pair->get_logical_table() :-1;
         }
         if (m_log_level >= 300) {
                 cout << "ref_model_wrapper::mau_model_process_match_n_action: LTcam start egr="<< static_cast<int>(egress)
                      <<  " ltn=" << i0 <<  " ltid=" << ltid << std::dec << "\n" << std::endl << std::flush;
         }
         ltcam0->lookup_ternary_match(match_inpt_phv, ltid, ltres0, ingress, false);
         extract_mau_lookup_result( ltres0, &ltcamres[8*i0]);
      }
    }
    delete ltres0;
    if (m_log_level >= 300) {  // DEBUG code
            cout << "ref_model_wrapper::mau_model_process_match_n_action: extract LTcam results done egr="<<
                    static_cast<int>(egress)<< "\n" <<  std::endl << std::flush;
    }
    ref_model_wrapper::getInstance()->mau_model_reset_resources(pipe, stage);

    if (m_log_level >= 200) {  // DEBUG code
            cout << "ref_model_wrapper::mau_model_process_match_n_action: prev iPHV  #"<< lpfid << "e"<<
                    static_cast<int>(egress) << "# << \n" << std::endl << std::flush;
            // DBG if (m_log_level >= 900) {  // DEBUG code
            // DBG         iphv->print(NULL, true);
            // DBG         cout << ">> " << std::endl << "   prev oPHV << " << std::endl;
            // DBG         ophv->print(NULL, true);
            // DBG }
            //cout << "\n" << std::endl << std::flush;
    }
    // Note: does not seem to be needed
    //     Pipe.cpp uses this
    // ophv->copydata(iphv, mau_p->ingress_selector());
    // ophv->copydata(iphv, mau_p->egress_selector());

    // holders in pipe.h for next table id (get & set functions
    // int ipipe_table_id = ipipe_table_id_0();
    // int epipe_table_id = epipe_table_id_0();

    for (int i0 = 0 ; i0 < 4 ; i0++) {
      int lrow = (i0 << 2) + 3;
      if ((lpf_active & (1 << lrow))) {
        if (m_log_level >= 200) {
          printf("ref_model_wrapper::mau_model_process_match_n_action:Setting lpf for alu=%2d, vec=0x%x, ts=0x%" PRIX64 " , rng=0x%" PRIX64 " /* RefModel Meter Lpf#%0de%01d# */\n",
                 i0, lpf_active, lpf_ts[i0], lpf_rng[i0],lpfid,egress);
          printf("ref_model_wrapper::mau_model_process_match_n_action:#iphv_in->set_meter_tick_time(%0d, %2d, 0x%" PRIX64 ") /* RefModel Meter Lpf#%0de%01d# */\n", stage, i0, lpf_ts[i0],lpfid,egress);
          printf("ref_model_wrapper::mau_model_process_match_n_action:#iphv_in->set_meter_random_value(%0d, %2d, 0x%" PRIX64 ") /* RefModel Meter Lpf#%0de%01d# */\n", stage, i0, lpf_rng[i0],lpfid,egress);
          printf("ref_model_wrapper::mau_model_process_match_n_action:#ophv_in->set_meter_tick_time(%0d, %2d, 0x%" PRIX64 ") /* RefModel Meter Lpf#%0de%01d# */\n", stage, i0, lpf_ts[i0],lpfid,egress);
          printf("ref_model_wrapper::mau_model_process_match_n_action:#ophv_in->set_meter_random_value(%0d, %2d, 0x%" PRIX64 ") /* RefModel Meter Lpf#%0de%01d# */\n", stage, i0, lpf_rng[i0],lpfid,egress);
        }
        iphv->set_meter_tick_time(stage, i0, lpf_ts[i0]);
        iphv->set_meter_random_value(stage, i0, lpf_rng[i0]);

        ophv->set_meter_tick_time(stage, i0, lpf_ts[i0]);
        ophv->set_meter_random_value(stage, i0, lpf_rng[i0]);
      }
    }


/*
    if ( m_log_level >= 200 ) {
      printf( "ref_model_wrapper::mau_model_process_match_n_action:Setting snapshot triggered for ingress=%0x\n", ( snapshot_data[0] & 0x00000001 ) );
      printf( "ref_model_wrapper::mau_model_process_match_n_action:Setting snapshot triggered for egress=%0x\n",  ( ( snapshot_data[0] & 0x00000002 ) >> 1 ) );
    }
    if ( ( snapshot_data[0] & 0x00000001 ) != 0 )
      miop->set_ingress_snapshot_triggered( true );

    if ( ( snapshot_data[0] & 0x00000002 ) != 0 )
      miop->set_egress_snapshot_triggered( true );
*/
    if ( m_log_level >= 200 ) {
      printf( "ref_model_wrapper::mau_model_process_match_n_action:Setting snapshot_i_trigger=%0x\n", ( snapshot_data[0] & 0x00000001 ) );
    }
    if ( ingress )
      miop->set_ingress_snapshot_triggered( ( snapshot_data[0] & 0x00000001 ) != 0 );
    else
      miop->set_egress_snapshot_triggered( ( snapshot_data[0] & 0x00000001 ) != 0 );

    next_iphv = mau_p->process_match(iphv, ophv, ingress_next_table, egress_next_table);

    //assert(next_iphv != NULL);
    *retval = *retval | (next_iphv == NULL);

    if (m_log_level >= 300) {  // DEBUG code
            cout << "ref_model_wrapper::mau_model_process_match_n_action: next_iPHV egr="<<
                    static_cast<int>(egress) << "\n" << std::endl << std::flush;
            // DBG only next_iphv->print(NULL, true);
            //cout << "\n" << std::endl << std::flush;
    }
    // Extract iPhv
    extract_data_from_mau_phv(next_iphv, egress, next_iphv_data, next_iphv_vld); // iPhv

    // Extract hash gen values
    for (int grp0 = 0 ; grp0 < 8 ; grp0++) {
       BitVector<MauDefs::kHashOutputWidth> fooV;
       uint32_t val = 0;
       fooV = mau_p->get_hash_output(next_iphv, grp0);
       val = (fooV.get_byte(3) << 24) | (fooV.get_byte(2) << 16) |
               (fooV.get_byte(1) << 8) | fooV.get_byte(0);
       hashgen[grp0*2]   = val;
       val = (fooV.get_bit(51) << 19) |
               (fooV.get_bit(50) << 18) |
               (fooV.get_bit(49) << 17) |
               (fooV.get_bit(48) << 16) |
               (fooV.get_byte(5) << 8) | fooV.get_byte(4);
       hashgen[grp0*2 + 1]   = val;
    }

    hashidx=0;
    for (int ltid = 0 ; ltid < 16 ; ltid++) {
          uint32_t imm_hash_ls, imm_hash_ms;
          uint32_t meter_addr, stats_addr, action_addr, selector_addr, selector_action_addr;
          meter_addr = stats_addr = action_addr = selector_addr = selector_action_addr = 0u;


          //DO NOT CHANGE THE ORDER BELOW
          imm_hash_ls = mau_p->mau_hash_dist()->get_immediate_data_ls_hash(next_iphv, ltid);
          hashdist[hashidx++] = imm_hash_ls;

          imm_hash_ms = mau_p->mau_hash_dist()->get_immediate_data_ms_hash(next_iphv, ltid);
          hashdist[hashidx++] = imm_hash_ms;

          meter_addr = mau_p->mau_hash_dist()->get_meter_address(next_iphv, ltid);
          hashdist[hashidx++] = meter_addr;

          stats_addr = mau_p->mau_hash_dist()->get_stats_address(next_iphv, ltid);
          hashdist[hashidx++] = stats_addr;

          action_addr = mau_p->mau_hash_dist()->get_action_address(next_iphv, ltid);
          hashdist[hashidx++] = action_addr;

          selector_addr = mau_p->mau_hash_dist()->get_selector_address(next_iphv, ltid);
          hashdist[hashidx++] = selector_addr;

          selector_action_addr = mau_p->mau_hash_dist()->get_selector_action_address(next_iphv, ltid);
          hashdist[hashidx++] = selector_action_addr;

          if (m_log_level >= 300) {  // DEBUG code
            cout << "ref_model_wrapper::mau_model_process_match_n_action:immediate ls  hash[ltid=" << ltid << "] =" << std::hex << imm_hash_ls
            << " immediate ms  hash[ltid=" << ltid << "] =" << std::hex << imm_hash_ms
            << " meter_addr[ltid=" << ltid << "] =" << std::hex << meter_addr
            << " stats_addr[ltid=" << ltid << "] =" << std::hex << stats_addr
            << " action_addr[ltid=" << ltid << "] =" << std::hex << action_addr
            << " selector_addr[ltid=" << ltid << "] =" << std::hex << selector_addr
            << " selector_action_addr[ltid=" << ltid << "] =" << std::hex << selector_action_addr  << "\n" << std::endl << std::flush;
          }
    }

   // debug code snippet
   // Go thru LogicalRows in MAU and check selector_rd_addr
   /*
    for (int i = 0; i < 16; i++) {
      MauLogicalRow *lrow = mau_p->logical_row_lookup(i);
      uint32_t addr = 0u;
      lrow->selector_rd_addr(&addr);
      if (m_log_level >= 200) { printf("ref_model_wrapper::mau_model_process_match_n_action: "
      "LRow[%d]: selector_rd_addr=0x%08x\n", i, addr); }
    }
    */

    idx=0;
    for (int lrow = 0; lrow <16; lrow++) {
      uint8_t  color_wr_data;
      //uint64_t mss_alu_data;
      uint64_t mss_alu_data0;
      uint64_t mss_alu_data1;
      uint32_t stats_rd_addr, meter_rd_addr, sel_rd_addr = 0u;
      BitVector<MauDefs::kDataBusWidth> stats_meter_rd_data(UINT64_C(0)), action_rd_data(UINT64_C(0)), stats_wr_data(UINT64_C(0)), data0(UINT64_C(0)), sel_inp_data(UINT64_C(0));
      BitVector<MauDefs::kActionOutputBusWidth> salu_action_data(UINT64_C(0));
      stats_rd_addr = meter_rd_addr = sel_rd_addr = 0u;
      //BitVector<128> stats_meter_rd_data, action_rd_data, stats_wr_data;
      MauLogicalRow *lrowp;

      lrowp = mau_p->logical_row_lookup(lrow);


      //DO NOT CHANGE THE ORDER (START)
      //Each logical row consumes 18 32-bit words
      //Inputs
      //      lrowp->stats_alu_rd_data(&stats_meter_rd_data); //4-32b
      lrowp->get_meter_alu_input_data(&stats_meter_rd_data); //4-32b

//JN:Sep 12,2017 Legacy code - alu_io is capturing 28 elements in
//it's indices. The datapath scoreboard in DV
//is positionaly sensitive to the index. So if we
//capture more here and the indices shift, then the mau_datapath_sb.svh
//define ALU_IO_SIZE will need to be adjusted
//and checks for the shifted indices as well
      alu_io[idx++] = stats_meter_rd_data.get_word(0,  32);
      alu_io[idx++] = stats_meter_rd_data.get_word(32, 32);
      alu_io[idx++] = stats_meter_rd_data.get_word(64, 32);
      alu_io[idx++] = stats_meter_rd_data.get_word(96, 32);

      if ( lrow % 4 == 3 ) { // get ALU data with hash_mask applied
        MauMeterAlu* malu = lrowp->mau_meter_alu();
        assert( malu );

        BitVector<MauDefs::kStatefulMeterAluDataBits> mss_alu_data_wide = malu->get_alu_data_wide(next_iphv);

        alu_io[idx++] = mss_alu_data_wide.get_word(0,  32);
        alu_io[idx++] = mss_alu_data_wide.get_word(32, 32);
        alu_io[idx++] = mss_alu_data_wide.get_word(64, 32);
        alu_io[idx++] = mss_alu_data_wide.get_word(96, 32);
      } else {
        BitVector<MauDefs::kStatefulMeterAluDataBits> mss_alu_data_wide = lrowp->get_meter_stateful_selector_alu_data(next_iphv);
        // JBay mss_alu_data can be 128bits

        mss_alu_data0 = mss_alu_data_wide.get_word(0);
        mss_alu_data1 = mss_alu_data_wide.get_word(64);
        alu_io[idx++] = (uint32_t) mss_alu_data0 & 0xFFFFFFFF;
        alu_io[idx++] = (uint32_t) (mss_alu_data0 >> 32) & 0xFFFFFFFF;
        alu_io[idx++] = (uint32_t) mss_alu_data1 & 0xFFFFFFFF;
        alu_io[idx++] = (uint32_t) (mss_alu_data1 >> 32) & 0xFFFFFFFF;
      }

      lrowp->meter_rd_addr(&meter_rd_addr); //1-32b
      alu_io[idx++] = meter_rd_addr;

      lrowp->stats_rd_addr(&stats_rd_addr); //1-32b
      alu_io[idx++] = stats_rd_addr;

      //Outputs
      //lrowp->stats_wr_data(&stats_wr_data); //4-32b

      if(lrow%4 == 3) { //Meter alu rows: 3, 7, 11, 15
        int alu = lrow >> 2;
        int logrow = MauMeterAlu::get_meter_alu_logrow_index(alu);
        uint32_t addr;

        MauMeterAlu* malu = om->mau_get(pipe, stage)->logical_row_lookup(logrow)->mau_meter_alu();
        assert( malu );

        malu->get_output(&data0, &addr, &salu_action_data); //4-32b
        alu_io[idx++] = data0.get_word(0,  32);
        alu_io[idx++] = data0.get_word(32, 32);
        alu_io[idx++] = data0.get_word(64, 32);
        alu_io[idx++] = data0.get_word(96, 32);
      } else {
        alu_io[idx++] = 0;
        alu_io[idx++] = 0;
        alu_io[idx++] = 0;
        alu_io[idx++] = 0;
      }

      lrowp->action_rd_data(&action_rd_data);//4-32b
      alu_io[idx++] = action_rd_data.get_word(0,  32);
      alu_io[idx++] = action_rd_data.get_word(32, 32);
      alu_io[idx++] = action_rd_data.get_word(64, 32);
      alu_io[idx++] = action_rd_data.get_word(96, 32);

      lrowp->selector_rd_addr(&sel_rd_addr); //1-32b
      alu_io[idx++] = sel_rd_addr;

      lrowp->color_write_data(&color_wr_data);//1-32b (8b)
      alu_io[idx++] = color_wr_data;

      //Get the meter alu output
      if(lrow%4 == 3) { //Meter alu rows: 3, 7, 11, 15
        alu_io[idx++] = salu_action_data.get_word(0,  32);
      } else {
        //Below is just for uniformity. Each log row has the same amout of data
        alu_io[idx++] = 0;
      }

      //Get the selector alu input
      lrowp->get_selector_alu_input_data(&sel_inp_data);
      alu_io[idx++] = sel_inp_data.get_word(0,  32);
      alu_io[idx++] = sel_inp_data.get_word(32, 32);
      alu_io[idx++] = sel_inp_data.get_word(64, 32);
      alu_io[idx++] = sel_inp_data.get_word(96, 32);

      //Get the meter alu output
      //This ugly but necessary for Tofino compatibility (we don't want to shift the order)
      if(lrow%4 == 3) { //Meter alu rows: 3, 7, 11, 15
        alu_io[idx++] = salu_action_data.get_word(32, 32);
        alu_io[idx++] = salu_action_data.get_word(64, 32);
        alu_io[idx++] = salu_action_data.get_word(96, 32);
      } else {
        //Below is just for uniformity. Each log row has the same amout of data
        alu_io[idx++] = 0;
        alu_io[idx++] = 0;
        alu_io[idx++] = 0;
      }


      //Grabbing the meter_adr_pre_range (i.e adddress before potential out-of-range- squash by vpn range check)
      uint32_t meter_adr_pre_range;
      if(lrow%4 == 3) { //Meter alu rows: 3, 7, 11, 15
         int alu = lrow >> 2;
         meter_adr_pre_range = mau_p->get_meter_addr_pre_vpn_squash(alu);
         alu_io[idx++] = meter_adr_pre_range;
         if (m_log_level >= 200) {
          printf("ref_model_wrapper::mau_model_process_match_n_action:meter_adr_pre_range=0x%0x,lrow=%0d,alu=%2d,idx=%0d\n",
                 meter_adr_pre_range,lrow,alu,idx);
         }
      } else {
        alu_io[idx++] = 0;
      }


      //DO NOT CHANGE THE ORDER (END)

      if (m_log_level >= 300) {
        cout << "ref_model_wrapper::mau_model_process_match_n_action:Logical row: " << std::dec << lrow
        << " idx= " << idx
        << " INputs: "
        << " meter_rd_addr_" << std::dec << lrow <<  " = 0x" << std::hex << meter_rd_addr
        << " stats_rd_addr_" << std::dec << lrow <<  " = 0x" << std::hex << stats_rd_addr
        << " stats_meters_rd_data_" << std::dec << lrow <<  " = 0x" << std::hex << stats_meter_rd_data.to_string()
        << " mss_alu_data_" << std::dec << lrow <<  " = 0x" << std::hex << mss_alu_data0 << std::hex << mss_alu_data1
        << " OUTputs: "
        << " stats_wr_data_" << std::dec << lrow <<  " = 0x" << std::hex << data0.to_string()
        << " sel_rd_addr_" << std::dec << lrow <<  " = 0x" << std::hex << sel_rd_addr
        << " action_rd_data_" << std::dec << lrow <<  " = 0x" << std::hex << action_rd_data.to_string()
        << " color_wr_data_" << std::dec << lrow <<  " = 0x" << std::hex << static_cast<int>(color_wr_data)
        << " salu_action_data_" << std::dec << lrow <<  " = 0x" << std::hex << salu_action_data.to_string()
        << " sel_inp_data " << std::dec << lrow <<  " = 0x" << std::hex << sel_inp_data.to_string()
        << " meter_adr_pre_range " << std::dec << lrow <<  " = 0x" << std::hex << meter_adr_pre_range << "\n" << std::endl << std::flush;
      }
    } //for (int lrow = 0; lrow <16; lrow++) {

    /*
      idx = 0;
      for (int lrow=0 ; lrow<16 ; lrow++) {
        for (int j=0 ; j<18 ; j++) {
           cout << "ref_model_wrapper::mau_model_process_match_n_action: alu_io[ " << lrow << "][" << j << "] = " << std::hex << alu_io[idx++] << std::endl;
        }
      }
    for (int et = 0 ; et < 2 ; et++) {
      for (int row = 0 ; row < 8 ; row++) {
        for (int bus = 0 ; bus < 2 ; bus++) {
          uint16_t sel_hash;
          mau_p->mau_hash_dist()->get_selector_ram_word_hash(next_iphv, row, bus, et, &sel_hash);
          cout << "ref_model_wrapper::mau_model_process_match_n_action:sel hash[" << et << "],row=[" << row << "], bus=[" << bus << "] =" << std::hex << sel_hash << std::endl;
        }
      }
    }
    */

    /* debug code
    for (int t = 0; t < 16; t++) {
      uint8_t nxt, instr;
      uint32_t imm, actA, statsA, meterA, idleA;
      BitVector<83> tbus(UINT64_C(0));
      MauLogicalTable *tab = mau_p->logical_table_lookup(t);
      MauLookupResult *res = mau_p->mau_lookup_result(t);
      if ((tab != NULL) && (res != NULL))
        { tab->get_table_result_bus(&tbus); tab->extract_addrs(res, tbus, &nxt, &imm, &actA, &instr, &statsA, &meterA, &idleA);
        if (m_log_level >= 200) { printf("ref_model_wrapper::mau_model_process_match_n_action:TABLE_INFO=%d....Nxt=%d Imm=0x%08x Instr=0x%02X \n" "ActA=0x%08x StatsA=0x%08x MeterA=0x%08x IdleA=0x%08x", t, nxt, imm, instr, actA, statsA, meterA, idleA); }
        }
    }
    //End debug code
    */


    // Extract Logical Table info after process_match()
    //    Store data vals in array: Pre_pred ltable in 0-15 & post in 16-31
    for (int i0 = 0 ; i0 < MauDefs::kLogicalTablesPerMau ; i0++) {
       bool ltbl_use = false;
       MauLogicalTable * ltbl = mau_p->logical_table_lookup(i0);
       MauLookupResult *mlres = new MauLookupResult();
       BitVector<MauDefs::kTableResultBusWidth> outbus;
       if (egress) { ltbl_use = ltbl->is_egress();}
       else { ltbl_use = ltbl->is_ingress(); }
       if (ltbl_use) {
          uint16_t next_table;
          uint32_t imm_data;
          uint32_t act_data_addr = 0u;
          uint8_t act_instr_addr = 0u;
          uint32_t stats_addr = 0u;
          uint32_t meter_addr = 0u;
          uint32_t idletime_addr = 0u;
          uint32_t tmp, refi;
          mlres->reset();
          mlres = mau_p->mau_lookup_result(i0);
          // Extract and pack match result into uint32_t array mresult
          extract_mau_lookup_result( mlres, &mresult[MRSIZE*2*i0]); // 6+3+8=16 -> 2x17DWx16 storage needed
          // Result Bus (OutBus)
          ltbl->get_table_result_bus( &outbus); // Result bus is 83 bits or 64 bits (TIND) - treat as 96bits
          for (int k2 = 0 ; k2 < 3 ; k2++) {
             uint32_t val = 0;
             for (int j1=3; j1>=0; --j1) {
                if ( (k2*4+j1) < 11) { // last byte not present in vec
                   val = (val << 8) | outbus.get_byte(k2*4+j1);
                }
             }
             mresult[ k2 + (8+i0*2*MRSIZE)]   = val;
          }
          ltbl->extract_addrs(mlres, outbus,
                              &next_table, &imm_data,
                              &act_data_addr, &act_instr_addr,
                              &stats_addr, &meter_addr, &idletime_addr);
          mresult[ 11+(i0*2*MRSIZE)]   = (next_table << 16) | (act_instr_addr);
          mresult[ 12+(i0*2*MRSIZE)]   = imm_data;
          mresult[ 13+(i0*2*MRSIZE)]   = act_data_addr;
          mresult[ 14+(i0*2*MRSIZE)]   = stats_addr;
          mresult[ 15+(i0*2*MRSIZE)]   = meter_addr; // TBD Idletime adr
          mresult[ 16+(i0*2*MRSIZE)]   = idletime_addr;

          //Overwrite the m_instr using the below API, as it contains the PFE bit
          tmp = mresult[ 6+(i0*2*MRSIZE)];
          refi = mlres->extract_action_instr_addr(mau_p, i0);
          mresult[ 6+(i0*2*MRSIZE)]   = (tmp & 0x00FFFFFF) | (refi << 24);
       }
       // ?? results in Seg fault:  delete mlres;
    }
    // Extract EHit raw from each unit-ram for exact match
    // mau->sram_lookup() and then call sram->hit_mask()
    for (int prn = 0 ; prn < MauDefs::kSramRowsPerMau ; prn++) {
       for (int pcn = 2 ; pcn < MauDefs::kSramColumnsPerMau ; pcn++) {
          MauSram * msram_p = mau_p->sram_lookup( (prn*(MauDefs::kSramColumnsPerMau))+pcn );
          uint32_t hm = msram_p->hit_mask();
          ehit_raw[((prn*3)+(pcn/4))] |= (hm  << ((pcn%4)*8));
       }
    }

    // Extract sram-row results for all rows
    for (int prn = 0 ; prn < MauDefs::kSramRowsPerMau; prn++) { // 8 per row
        MauSramRow * srow_p = mau_p->sram_row_get(prn);
        for (int rb = 0 ; rb < MauDefs::kMatchOutputBusesPerRow ; rb++) { // 2 buses per row
           uint64_t mtch_outdata = srow_p->get_match_output_data(rb);
           em_resbus[ (prn*4)+(rb*2) ] = (uint32_t) mtch_outdata & 0xFFFFFFFF;
           em_resbus[ (prn*4)+(rb*2) + 1 ] = (uint32_t) (mtch_outdata >> 32 ) & 0xFFFFFFFF;
        }
    }

    // Call mau to process iphv/ophv using instructions/actions
    // discovered during calls to process_match()
    next_ophv = mau_p->process_action(iphv, ophv);
    if(next_ophv->teop() != NULL) {
    Teop *teop = next_ophv->teop();
    for(int teop_bus=0; teop_bus<4; teop_bus++) {
      uint32_t teop_addr;
      uint8_t  teop_color;
      uint8_t  teop_stats_en;
      uint8_t  teop_meter_en;

      teop_addr = teop->addr(teop_bus);
      teop_color = teop->color(teop_bus);
      teop_stats_en = teop->stats_en(teop_bus);
      teop_meter_en = teop->meter_en(teop_bus);
      teop_result[teop_bus] = (teop_meter_en & 0x1 ) << 22 | (teop_stats_en & 0x1 ) << 21
                              | (teop_color & 0x3) << 19 | (teop_addr & 0x3FFFF);
      if (m_log_level >= 200) {
       cout <<"ref_model_wrapper::mau_model_process_match_n_action: ->get_teop="<< teop_bus << "\n" << std::endl << std::flush;
       cout <<"teop_addr=0x"<< std::hex << teop_addr << "\n" << std::endl;
       cout <<"teop_color=0x"<< std::hex << teop_color << "\n" << std::endl;
       cout <<"teop_stats_en=0x"<< std::hex << teop_stats_en << "\n" << std::endl;
       cout <<"teop_meter_en=0x"<< std::hex << teop_meter_en << "\n" << std::endl << std::flush;
      }
    }
    }


    MauIO *moiop      = mau_p->mau_io_output();

    *retval = *retval | (next_ophv == NULL);
    *action_cnt = mau_p->mau_instr_store()->instr_get()->get_execute_count();
    if (m_log_level >= 200) { cout <<"ref_model_wrapper::mau_model_process_match_n_action: 1. ->get_execute_count="<< *action_cnt << "\n" << std::endl << std::flush; }

    if (m_log_level >= 300) {  // DEBUG code
       cout << "ref_model_wrapper::mau_model_process_match_n_action: next_oPHV egr="<<
               static_cast<int>(egress) << std::endl;
       cout << "     ntbl{0x" << std::hex << moiop->ghost_nxt_tab() << ", 0x"
            << *egress_next_table << ", 0x"<< *ingress_next_table << "} glob=0x"
            << moiop->global_exec() << "  lbr=0x" << static_cast<int>(moiop->long_branch()) << std::endl;
       cout << " MPR ntbl{0x" << moiop->ghost_mpr_nxt_tab() << ", 0x"
            << moiop->egress_mpr_nxt_tab() << ", 0x" <<  moiop->ingress_mpr_nxt_tab() << "} glob=0x"
            << moiop->mpr_global_exec() << "  lbr=0x" << static_cast<int>(moiop->mpr_long_branch())
            << "\n" << std::dec << std::endl << std::flush;
    }
    extract_data_from_mau_phv(next_ophv, egress, next_ophv_data, next_ophv_vld); // oPHV

    //Extract MPR information
    uint16_t lts_MPR = mau_p->pred_lt_info(Pred::kMpr);
    uint16_t lts_active_mask = mau_p->pred_lt_info(Pred::kActiveMask);

    *mpr_result = ((lts_MPR << 16) & 0xFFFF0000) | lts_active_mask ;
    /*  Extract more data from Match-Action  */
    get_mauio_into_phvside(moiop, phvside);
    tbl_act_vec = mau_p->table_active();
    // Get Powered table vector in upper half of table_active
    *table_active  = ( ( ((uint32_t) mau_p->pred_lt_info(Pred::kPowered)) << 16 ) & 0xFFFF0000) |
            (( (uint32_t) tbl_act_vec.get_byte(1)) << 8) |
            ((uint32_t) tbl_act_vec.get_byte(0) );
    // Extract Logical Table after pred result
    //    Store data vals in array: Pre_pred ltable in 0-15 & post in 16-31
    for (int i0 = 0 ; i0 < MauDefs::kLogicalTablesPerMau ; i0++) {
       bool ltbl_use = false;
       MauLogicalTable * ltbl = mau_p->logical_table_lookup(i0);
       BitVector<MauDefs::kTableResultBusWidth> outbus;
       if (egress) { ltbl_use = ltbl->is_egress(); }
       else { ltbl_use = ltbl->is_ingress(); }
       if (ltbl_use) {
          uint16_t next_table;
          uint32_t imm_data;
          uint32_t act_data_addr = 0u;
          uint8_t act_instr_addr = 0u;
          uint32_t stats_addr = 0u;
          uint32_t meter_addr = 0u;
          uint32_t idletime_addr = 0u;
          MauLookupResult *mlres = new MauLookupResult();
          mlres->reset();
          mlres = mau_p->mau_lookup_result(i0);
          // Extract and pack match result into uint32_t array mresult
          extract_mau_lookup_result( mlres, &mresult[2*MRSIZE*i0+MRSIZE]); // 6+3+8 =17 -> 2x16DWx16 storage needed
          // Result Bus (OutBus)
          ltbl->get_table_result_bus( &outbus); // Result bus is 83 bits or 64 bits (TIND) - treat as 96bits
          for (int k2 = 0 ; k2 < 3 ; k2++) {
             uint32_t val = 0;
             for (int j1=3; j1>=0; --j1) {
                if ( (k2*4+j1) < 11) { // last byte not present in vec
                   val = (val << 8) | outbus.get_byte(k2*4+j1);
                }
             }
             mresult[ k2 + (8+(i0*2*MRSIZE)+MRSIZE)]   = val;
          }
          ltbl->extract_addrs(mlres, outbus,
                              &next_table, &imm_data,
                              &act_data_addr, &act_instr_addr,
                              &stats_addr, &meter_addr, &idletime_addr);
          mresult[ 11+(i0*2*MRSIZE)+MRSIZE]   = (next_table << 16) | (act_instr_addr);
          mresult[ 12+(i0*2*MRSIZE)+MRSIZE]   = imm_data;
          mresult[ 13+(i0*2*MRSIZE)+MRSIZE]   = act_data_addr;
          mresult[ 14+(i0*2*MRSIZE)+MRSIZE]   = stats_addr;
          mresult[ 15+(i0*2*MRSIZE)+MRSIZE]   = meter_addr; // TBD Idletime_adr
          mresult[ 16+(i0*2*MRSIZE)+MRSIZE]   = idletime_addr; // TBD Idletime_adr
          // Results in deg fault ??:  delete mlres;
       }
    }
    // Extract Action Data bus
    myadb = mau_p->action_hv_output_bus();
    // DBG if (m_log_level >= 900) {  // DEBUG code
    // DBG         cout << "ref_model_wrapper::mau_process_match_n_action( action_data_bus egr="<<
    // DBG                  static_cast<int>(egress) << " { " << std::endl;
    // DBG         cout << myadb->to_string();
    // DBG         cout << "  }\n" << std::endl << std::flush;
    // DBG }
    for (int i0 = 0 ; i0 < 32 ; i0++) {
      uint32_t val = 0;
      for (int j1=3; j1>=0; --j1) {
        val = (val << 8) | myadb->get_byte(i0*4+j1);
      }
      actdatabus[i0]   = val;
    }
    // Extract Gateway Tables
    for (int i1 = 0 ; i1 < 2 ; i1++) {
      gateway[i1] = 0;
    }
    for (int i0 = 0 ; i0 < MauDefs::kSramRowsPerMau ; i0++) {
       MauSramRow* row = mau_p->sram_row_lookup( i0 );
       assert (row != nullptr);
       for (int i1 = 0 ; i1 < 2 ; i1++) {
          bool hit;
          int hit_index;
          uint32_t val;
          row->get_gateway_table_result(next_iphv, i1, &hit, &hit_index);
          val = (hit_index & 0x7);
          val = (hit) ? (val | 0x8) : val;
          gateway[i1] |= (val << (i0 * 4));
       }
    }

    *action_cnt = mau_p->mau_instr_store()->instr_get()->get_execute_count();
    if (m_log_level >= 200) { cout <<"ref_model_wrapper::mau_model_process_match_n_action: 2. ->get_execute_count="<< *action_cnt << "\n" << std::endl << std::flush;}
/*
    if ( m_log_level >= 200 ) {
      printf( "ref_model_wrapper::mau_model_process_match_n_action:snapshot triggered returned %0x for ingress\n", ( moiop->ingress_snapshot_triggered() ) );
      printf( "ref_model_wrapper::mau_model_process_match_n_action:snapshot triggered returned %0x for egress\n",  ( moiop->egress_snapshot_triggered() ) );
    }
    if ( moiop->ingress_snapshot_triggered() )
      snapshot_data[0] |= 0x00000004;

    if ( moiop->egress_snapshot_triggered() )
      snapshot_data[0] |= 0x00000008;
*/
    if ( m_log_level >= 200 ) {
      if ( ingress )
        printf( "ref_model_wrapper::mau_model_process_match_n_action:snapshot_o_trigger=%0x\n", ( moiop->ingress_snapshot_triggered() ) );
      else
        printf( "ref_model_wrapper::mau_model_process_match_n_action:snapshot_o_trigger=%0x\n", ( moiop->egress_snapshot_triggered() ) );
    }
    if ( ( ingress && moiop->ingress_snapshot_triggered() ) || ( egress && moiop->egress_snapshot_triggered() ) )
      snapshot_data[0] |= 0x00000002;

    /* Move on to next stage with changed iphv/ophv - free old ones */
    if ((next_iphv != iphv) && (next_ophv != iphv))
            m_rmt_obj_mgr_p[0]->phv_delete(iphv);;
    if ((next_iphv != ophv) && (next_ophv != ophv))
            m_rmt_obj_mgr_p[0]->phv_delete(ophv);
    if (next_iphv != next_ophv)
            m_rmt_obj_mgr_p[0]->phv_delete(next_iphv);
    m_rmt_obj_mgr_p[0]->phv_delete(next_ophv);
    m_rmt_obj_mgr_p[0]->phv_delete(match_inpt_phv); // Delete create Phv
}

void ref_model_wrapper::multipk_model_process_match_n_action(uint32_t pipe, uint32_t stage_start, uint32_t stage_end,
                                                            uint32_t *next_iphv_data, uint32_t *next_iphv_vld,
                                                            uint32_t *next_ophv_data, uint32_t *next_ophv_vld,
                                                            uint32_t *iphv_data, uint32_t *iphv_vld,
                                                            uint32_t *ophv_data, uint32_t *ophv_vld,
                                                            uint32_t *phvside,
                                                            int *ingress_next_table, int *egress_next_table,
                                                            uint32_t *action_cnt, uint32_t lpfid,
                                                            int *retval) {
    uint8_t  egress  = phvside[0] & 0x1;
    Phv *next_iphv = NULL;
    Phv *next_ophv = NULL;
    Phv *iphv;
    Phv *ophv;
    RmtObjectManager *om = m_rmt_obj_mgr_p[0];

   char     prefix[256] = "REFM_MultiPkMatAct";
   char     setLogPrefix[256] = "SetLogPrefix";

   sprintf(prefix, "REFM_MultiPkMatAct#%de%d#", lpfid, egress?1:0);
   set_option(0,setLogPrefix, prefix);

    *retval = 0;

    iphv = create_mau_phv_from_data(phvside, iphv_data, iphv_vld);
    ophv = create_mau_phv_from_data(phvside, ophv_data, ophv_vld);

    if (m_log_level >= 200) {  // DEBUG code
            cout << "ref_model_wrapper::multipk_model_process_match_n_action: pipe=" << pipe <<
                    " stage_start=" << stage_start << " stage_end=" << stage_end <<
                    " prev iPHV  egr="<< static_cast<int>(egress) << "\n" << std::endl << std::flush;
    }

    // Note:
    //     Pipe.cpp   Phv *Pipe::run_maus_internal(Phv *phv);

    // Now loop through all MAUs
    for (uint32_t stage = stage_start; stage <= stage_end; stage++) {
       Mau *mau = om->mau_lookup(pipe, stage);
       if (mau != NULL) {
          MauIO *miop;
          MauIO *moiop;
          mau->reset_resources(); // Reset buses for ingress PHV lookup
          if (stage == stage_start) {
             miop = mau->mau_io_input();
             set_mauio_via_phvside(miop, phvside);
             cout << "ref_model_wrapper::multipk_model_process_match_n_action: Input stage="<< stage
                  << std::endl;
             cout << " start_tbl{0x" << std::hex << miop->ghost_nxt_tab() << ", 0x" << *egress_next_table <<
                     ", 0x" << *ingress_next_table << "} glob=0x" << miop->global_exec() <<
                     "  lbr=0x" << (static_cast<int>(miop->long_branch())) << std::endl;
             cout << " MPR sttbl{0x" << miop->ghost_mpr_nxt_tab() << ", 0x" << miop->egress_mpr_nxt_tab() <<
                    ", 0x" <<  miop->ingress_mpr_nxt_tab() << "} glob=0x" << miop->mpr_global_exec() <<
                     "  lbr=0x" << (static_cast<int>(miop->mpr_long_branch())) <<
                    "\n" << std::dec << std::endl << std::flush;
          }
          // Call mau to match against iphv/ophv
          next_iphv = mau->process_match(iphv, ophv, ingress_next_table, egress_next_table);
          *retval = *retval | (next_iphv == NULL);
          if (m_log_level >= 300) {  // DEBUG code
             cout << "ref_model_wrapper::multipk_model_process_match_n_action: pipe=" << pipe <<
                     " stage=" << stage << " next_oPHV egr="<< static_cast<int>(egress) << "\n" << std::endl << std::flush;
          }
          // Call mau to process iphv/ophv using instructions/actions
          // discovered during calls to process_match()
          next_ophv = mau->process_action(iphv, ophv);
          *retval = *retval | (next_iphv == NULL);
          moiop = mau->mau_io_output();
          if (m_log_level >= 300) {  // DEBUG code
             cout << "ref_model_wrapper::multipk_model_process_match_n_action: pipe=" << std::dec << pipe <<
                     " stage=" << stage << " next_oPHV egr=" << static_cast<int>(egress) << std::endl;
             cout << "     ntbl{0x" << std::hex << moiop->ghost_nxt_tab() << ", 0x"
                  << *egress_next_table << ", 0x"<< *ingress_next_table << "} glob=0x"
                  << moiop->global_exec() << "  lbr=0x" << static_cast<int>(moiop->long_branch()) << std::endl;
             cout << " MPR ntbl{0x" << moiop->ghost_mpr_nxt_tab() << ", 0x"
                  << moiop->egress_mpr_nxt_tab() << ", 0x" << moiop->ingress_mpr_nxt_tab() << "} glob=0x"
                  << moiop->mpr_global_exec() << "  lbr=0x" << static_cast<int>(moiop->mpr_long_branch())
                  << "\n" << std::dec << std::endl << std::flush;
          }
          if (stage == stage_end) { // Extract last stage into
             get_mauio_into_phvside(moiop, phvside);
          }
          // Move on to next stage with new iphv/ophv - free old ones
          if ((next_iphv != iphv) && (next_ophv != iphv))
                  om->phv_delete(iphv);
          if ((next_iphv != ophv) && (next_ophv != ophv))
                  om->phv_delete(ophv);
          iphv = next_iphv;
          ophv = next_ophv;

          //*action_cnt = mau->mau_instr_store()->instr_get()->get_execute_count();
          //cout <<"\ntwlvpk ref_model_wrapper::process_action->get_execute_count="<< *action_cnt << std::endl;
       }
    }

    // Extract iPhv & oPhv at End
    extract_data_from_mau_phv(next_iphv, egress, next_iphv_data, next_iphv_vld);

    extract_data_from_mau_phv(next_ophv, egress, next_ophv_data, next_ophv_vld);

    /* Move on to next stage with new iphv/ophv - free old ones */
    if ((next_iphv != iphv) && (next_ophv != iphv))
            m_rmt_obj_mgr_p[0]->phv_delete(iphv);;
    if ((next_iphv != ophv) && (next_ophv != ophv))
            m_rmt_obj_mgr_p[0]->phv_delete(ophv);
    if (next_iphv != next_ophv)
            m_rmt_obj_mgr_p[0]->phv_delete(next_iphv);
    m_rmt_obj_mgr_p[0]->phv_delete(next_ophv);
}

void ref_model_wrapper::mau_model_get_match_hash(uint32_t pipe, uint32_t stage,
                                                 uint32_t *hashgen,
                                                 uint32_t *iphv_data, uint32_t *iphv_vld,
                                                 uint32_t *ophv_data, uint32_t *ophv_vld,
                                                 uint32_t phvside,
                                                 int *ingress_next_table, int *egress_next_table, int *retval) {

    Phv *next_iphv;
    Phv *iphv;
    Phv *ophv;
    uint32_t  *newphvside = new uint32_t[4]();
    uint8_t  egress  = phvside & 0x1;
    char     prefix[256] = "REFM_Match-Hash";
    char     setLogPrefix[256] = "SetLogPrefix";

    set_option(0,setLogPrefix, prefix);
    newphvside[0] = phvside; newphvside[1] = 0; newphvside[2] = 0; newphvside[3] = 0;

    Mau *   mau_p = get_mau_stage(pipe, stage);

    iphv = create_mau_phv_from_data(newphvside, iphv_data, iphv_vld);
    ophv = create_mau_phv_from_data(newphvside, ophv_data, ophv_vld);
    iphv->set_match_only(true);
    ophv->set_match_only(true);
    if (m_log_level >= 200) {  // DEBUG code
            cout << "ref_model_wrapper::mau_model_get_match_hash: prev iPHV  egr="<<
                    static_cast<int>(egress) << "\n" << std::endl << std::flush;
            if (m_log_level >= 900) {  // DEBUG code
                    iphv->print(NULL, true);
                    cout << "ref_model_wrapper::mau_model_get_match_hash:" << std::endl << "   prev oPHV " << std::endl;
                    ophv->print(NULL, true);
            }
            cout << "\n" << std::endl << std::flush;
    }

    // Phv *process_match(Phv *iphv, Phv *ophv, int *ingress_next_table, int *egress_next_table);
    // internal function will use & modify next_table id(s)
    next_iphv = mau_p->process_match(iphv, ophv, ingress_next_table, egress_next_table);
    //assert(next_iphv != NULL);
    *retval = *retval | (next_iphv == NULL);

    for (int grp0 = 0 ; grp0 < 8 ; grp0++) {
       BitVector<MauDefs::kHashOutputWidth> fooV;
       uint32_t val = 0;
       fooV = mau_p->get_hash_output(iphv, grp0);
       val = (fooV.get_byte(3) << 24) | (fooV.get_byte(2) << 16) |
               (fooV.get_byte(1) << 8) | fooV.get_byte(0);
       hashgen[grp0*2]   = val;
       val = (fooV.get_bit(51) << 19) |
               (fooV.get_bit(50) << 18) |
               (fooV.get_bit(49) << 17) |
               (fooV.get_bit(48) << 16) |
               (fooV.get_byte(5) << 8) | fooV.get_byte(4);
       hashgen[grp0*2 + 1]   = val;
    }


    /* Move on to next stage with new iphv/ophv - free old ones */
    if (next_iphv != iphv)
            m_rmt_obj_mgr_p[0]->phv_delete(iphv);
    if (next_iphv != ophv)
            m_rmt_obj_mgr_p[0]->phv_delete(ophv);
    m_rmt_obj_mgr_p[0]->phv_delete(next_iphv);

    if (m_log_level >= 200) {  // DEBUG code
            cout << "ref_model_wrapper::mau_model_get_match_hash: done e="<<
                    static_cast<int>(egress) << "\n" << std::endl << std::flush;
    }
}


void ref_model_wrapper::run_stateful_alu(
    int       pipe, int stage,
    int       which_alu        /* 0  to 3 */,
    uint32_t  addr,
    uint32_t* phv_data_in,     /* [4] */
    uint32_t* data_in,         /* [4] */
    uint32_t* data_out,        /* [4] */
    uint64_t  present_time,
    uint32_t* action_out_32b,  /* [4] */
    uint32_t* match_in,        /* [4] */
    uint32_t* lmatch_in,       /* [4] */
    char*     match_out,
    char*     learn_out,
    uint32_t  stateful_rng,
    uint32_t* minmax_out
    )
{
  RmtObjectManager *om = m_rmt_obj_mgr_p[0];

  char     prefix[256] = "REFM_Run-Salu";
  char     setLogPrefix[256] = "SetLogPrefix";

  set_option(0,setLogPrefix, prefix);

  /* which_alu 0-3 */
  int logrow = MauMeterAlu::get_meter_alu_logrow_index(which_alu);
  MauStatefulAlu* salu = om->mau_get(pipe, stage)->logical_row_lookup(logrow)->mau_meter_alu()->get_salu();
  assert( salu );

  // don't check that the total is correct in Adjust Total instructions to avoid assertions when
  //  data is put in which hasn't got a correct total (eg some forwarding cases)
  salu->set_check_total_correct(false);
  BitVector<MauDefs::kStatefulMeterAluDataBits> phv_data_word{};
  // JBay phv_data_word is 128 bits wide, needs 4 phv_data_in words
  phv_data_word.set_word( ((static_cast<uint64_t>( phv_data_in[1] ) << 32) | phv_data_in[0]), 0);
  phv_data_word.set_word( ((static_cast<uint64_t>( phv_data_in[3] ) << 32) | phv_data_in[2]), 64);

  BitVector<MauDefs::kDataBusWidth> data_in_bv;
  BitVector<MauDefs::kDataBusWidth> data_out_bv;
  BitVector<MauDefs::kActionOutputBusWidth> action_out;

  MauStatefulAlu::StatefulBus match_bus{};
  MauStatefulAlu::StatefulBus learn_or_match_bus{};

  data_in_bv.set32(0,data_in[0]);
  data_in_bv.set32(1,data_in[1]);
  data_in_bv.set32(2,data_in[2]);
  data_in_bv.set32(3,data_in[3]);

  match_bus = { ( match_in[0] != 0 ), ( match_in[1] != 0 ), ( match_in[2] != 0 ), ( match_in[3] != 0 ) };
  learn_or_match_bus = { ( lmatch_in[0] != 0 ), ( lmatch_in[1] != 0 ), ( lmatch_in[2] != 0 ), ( lmatch_in[3] != 0 ) };

  salu->reset_resources();

  salu->set_random_number_value( static_cast<uint64_t>( stateful_rng ) );

  salu->calculate_output(addr,phv_data_word,
                         &data_in_bv,
                         &data_out_bv,
                         &action_out,
                         present_time,
                         true, // ingress flag, only used for P4 level logging
                         match_bus,
                         learn_or_match_bus
                         );

  // combine both minmax_error and minmax_index into minmax_out
  minmax_out[0] = ( ( salu->get_minmax_index() << 1 ) | ( salu->get_minmax_mask_was_zero() ? 1 : 0 ) );

  match_out[0] = ( salu->get_match_output() ? 1 : 0 );
  learn_out[0] = ( salu->get_learn_output() ? 1 : 0 );

  // JBay can return 128 bits of action_out
  action_out_32b[0] = action_out.get_word(0,  32);
  action_out_32b[1] = action_out.get_word(32, 32);
  action_out_32b[2] = action_out.get_word(64, 32);
  action_out_32b[3] = action_out.get_word(96, 32);

  data_out[0] = data_out_bv.get_word(0,  32);
  data_out[1] = data_out_bv.get_word(32, 32);
  data_out[2] = data_out_bv.get_word(64, 32);
  data_out[3] = data_out_bv.get_word(96, 32);

}

void ref_model_wrapper::mau_movereg_op(uint32_t pipe, uint32_t stage,
                                       int ltid, /* 0  to 15 */
                                       int addr,
                                       uint32_t ops_mask) {
  RmtObjectManager *om = m_rmt_obj_mgr_p[0];

  char     prefix[256] = "REFM_MoveReg";
  char     setLogPrefix[256] = "SetLogPrefix";

  set_option(0,setLogPrefix, prefix);

  MauMoveregs* pltmr = om->mau_get(pipe, stage)->mau_moveregs(ltid);
  assert(pltmr);
  /* ops_mask
     Push
   * [0] void shift_and_load(const int addr);
   * [1] void inhibit();
   * [2] void commit();
   * [3] void prepare();
   * [4] void update_deferred_ram();
   Pop
   * [8] void initialize(const bool idle_init, const bool stats_init_ones);
   * [9]   bool idle_init
   * [10]  bool stats_init_ones
   */
  if (ops_mask & 0x1) {
      pltmr->shift_and_load(addr);
  }
  if (ops_mask & 0x2) {
      pltmr->inhibit();
  }
  if (ops_mask & 0x4) {
      pltmr->commit();
  }
  if (ops_mask & 0x8) {
      pltmr->prepare();
  }
  if (ops_mask & 0x10) {
      pltmr->update_deferred_ram();
  }
  if (ops_mask & 0x100) {
     bool idle_init       = (ops_mask & 0x200) ? true : false;
     bool stats_init_ones = (ops_mask & 0x400) ? true : false;
     pltmr->initialize(idle_init, stats_init_ones);
  }
}

void ref_model_wrapper::mau_read_full_res_stats(uint32_t pipe, uint32_t stage,
                                                int ltid, /* 0  to 15 */
                                                int addr,
                                                uint32_t* data_out  /* [4] 128b data byte[3:2]/pkt[1:0] */ ) {
  std::array<uint64_t,2> entry;
  RmtObjectManager *om = m_rmt_obj_mgr_p[0];
  uint32_t  ladr = (addr & 0x1fffff) | ( (ltid & 0xf) << 21);

  char     prefix[256] = "REFM_RdRes-Stats";
  char     setLogPrefix[256] = "SetLogPrefix";

  set_option(0,setLogPrefix, prefix);

  std::unordered_map<uint32_t, std::array<uint64_t,2>>* pfstats = om->mau_get(pipe, stage)->mau_memory()->full_res_stats();
  try {
    entry = pfstats->at(ladr);
    data_out[0] = (uint32_t) ( entry.at(0) & 0xFFFFFFFF);
    data_out[1] = (uint32_t) ( (entry.at(0) >> 32) & 0xFFFFFFFF);
    data_out[2] = (uint32_t) ( entry.at(1) & 0xFFFFFFFF);
    data_out[3] = (uint32_t) ( (entry.at(1) >> 32) & 0xFFFFFFFF);
    if (m_log_level >= 300) {
       printf("ref_model_wrapper::mau_read_full_res_stats: ltid=%0d of=0x%0x ladr=0x%0x  d3=0x%0x d2=0x%0x  d1=0x%0x d0=0x%0x\n ",
              ltid, addr, ladr, data_out[3], data_out[2],  data_out[1],data_out[0]);
    }
  } catch (const std::exception& e) {
    data_out[0] = 0;
    data_out[1] = 0;
    data_out[2] = 0;
    data_out[3] = 0;
    if (m_log_level >= 300) {
       printf("ref_model_wrapper::mau_read_full_res_stats: ltid=%0d of=0x%0x ladr=0x%0x no entry  d3=0x0 d2=0x0  d1=0x0 d0=0x0 \n",
              ltid, addr, ladr);
    }
  }
}

void ref_model_wrapper::mau_dump_full_res_stats(uint32_t pipe, uint32_t stage) {
  std::array<uint64_t,2> entry;
  RmtObjectManager *om = m_rmt_obj_mgr_p[0];

  char     prefix[256] = "REFM_DumpRes-Stats";
  char     setLogPrefix[256] = "SetLogPrefix";

  set_option(0,setLogPrefix, prefix);

  std::unordered_map<uint32_t, std::array<uint64_t,2>>* pfstats = om->mau_get(pipe, stage)->mau_memory()->full_res_stats();
  std::cout <<  std::endl << "ref_model_wrapper::mau_dump_full_res_stats: START DUMP Mau full res stats " << "\n" << std::endl << std::flush;
  for ( auto it = pfstats->begin(); it != pfstats->end(); ++it ) {
     int  ltid = (it->first >> 21);
     int  offst = (it->first & 0x1fffff);
     entry = it->second;
     std::cout << "ref_model_wrapper::mau_dump_full_res_stats: ltid=" << std::dec << ltid <<  " e=0x" << std::hex << offst << " a=0x" << it->first
               << "  bytes=0x" <<  entry.at(1) <<  " pkt=0x" << entry.at(0) << "\n" << std::endl;
  }
  std::cout <<  std::endl << "ref_model_wrapper::mau_dump_full_res_stats: END DUMP Mau full res stats " << "\n" <<  std::endl << std::flush;

}

void ref_model_wrapper::get_mau_counter(uint32_t pipe, uint32_t stage, char* name, uint32_t* value_out) {
  RmtObjectManager *om = m_rmt_obj_mgr_p[0];
  char     prefix[256] = "REFM_Mau-Cntr";
  char     setLogPrefix[256] = "SetLogPrefix";

  set_option(0,setLogPrefix, prefix);

  *value_out = om->mau_get(pipe, stage)->mau_info_read(name, false /* don't reset after read */ );
}


void ref_model_wrapper::mau_flush_queues(uint32_t pipe, uint32_t stage) {
  char     prefix[256] = "REFM_Flush-Q";
  char     setLogPrefix[256] = "SetLogPrefix";

  set_option(0,setLogPrefix, prefix);

  Mau *   mau_p = get_mau_stage(pipe, stage);
  mau_p->flush_queues();
}



}

#endif