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

#include <iostream>
#include <tm-sch-sel.h>

namespace MODEL_CHIP_NAMESPACE {

TmSchSel::TmSchSel(RmtObjectManager *om, uint8_t pipe_index) :
  TmObject(om, pipe_index),
  m_prt_arb(om, pipe_index),
  m_l1_arb(om, pipe_index),
  m_q_arb(om, pipe_index)
{
  m_state = NULL;
  m_config = NULL;
  min_pkt_sel = 0;
}

void TmSchSel::init(TmSchConfig *sch_cfg, TmSchState *sch_state) {
  m_config = sch_cfg;
  m_state = sch_state;
}

void TmSchSel::pps_tick() {
  RMT_ASSERT(m_state != NULL);
  RMT_ASSERT(m_config != NULL);
  if (m_state->get_num_qid_for_pri_prop())
    propagate_sch_q_pri();
  port_sel();
}

void TmSchSel::propagate_sch_q_pri() {
  int mac_id, l1_pipe_idx, q_pipe_idx; 
  bool q_eligible, q_max_rate_pos, q_min_rate_pos, enb;
  uint8_t min_rate_pri, max_rate_pri, l1_id;
  
  // Clear L1 State & Priority
  memset(min_pri_prop_q_l1, 0, sizeof(min_pri_prop_q_l1));
  memset(max_pri_prop_q_l1, 0, sizeof(max_pri_prop_q_l1));

  // Propagate Q State to L1
  for (size_t num_q=0; num_q < m_state->get_num_qid_for_pri_prop(); num_q++) {

    // Get Qid
    q_pipe_idx = m_state->get_qid_for_pri_prop(num_q);

    // Figure out variables
    mac_id = (q_pipe_idx /  TmDefs::kNumVqPerMac); 

    // Get Q Config
    enb = m_config->get_q_config().enb(q_pipe_idx);
    l1_id = m_config->get_q_config().l1_id(q_pipe_idx);
    max_rate_pri = m_config->get_q_config().max_rate_pri(q_pipe_idx);
    min_rate_pri = m_config->get_q_config().min_rate_pri(q_pipe_idx);
    
    // Check if Q is eligible to 
    // propagate priority
    q_eligible = 1;
    q_eligible &= enb;
    q_eligible &= m_state->get_q_occ(q_pipe_idx);
    // Add PFC to eligibilty
    q_max_rate_pos = m_state->get_q_max_rate_pos(q_pipe_idx);
    l1_pipe_idx = (mac_id * TmDefs::kNumL1PerMac) + l1_id;
    
    // Propagate Q Max Priority
    if (q_eligible && q_max_rate_pos) {
      if (max_rate_pri > max_pri_prop_q_l1[l1_pipe_idx]) {
	max_pri_prop_q_l1[l1_pipe_idx] = max_rate_pri;
      }
    }
    
    // Propagate Q Min Priority
    q_min_rate_pos = m_state->get_q_min_rate_pos(q_pipe_idx);
    if (q_eligible && q_min_rate_pos) {
      if (min_rate_pri > min_pri_prop_q_l1[l1_pipe_idx]) {
	min_pri_prop_q_l1[l1_pipe_idx] = min_rate_pri;
      }
    }
  }
}

uint8_t TmSchSel::pick_l1_max_rate_pri(uint16_t l1_pipe_idx) {
  uint8_t max_rate_pri; 
  bool pri_prop;
  
  pri_prop = m_config->get_l1_config().pri_prop(l1_pipe_idx);
  max_rate_pri = m_config->get_l1_config().max_rate_pri(l1_pipe_idx);
  
  if (pri_prop) 
    return(max_pri_prop_q_l1[l1_pipe_idx]);
  else 
    return(max_rate_pri);
}

uint8_t TmSchSel::pick_l1_min_rate_pri(uint16_t l1_pipe_idx) {
  uint8_t min_rate_pri; 
  bool pri_prop;
  
  pri_prop = m_config->get_l1_config().pri_prop(l1_pipe_idx);
  min_rate_pri = m_config->get_l1_config().min_rate_pri(l1_pipe_idx);
  
  if (pri_prop) 
    return(min_pri_prop_q_l1[l1_pipe_idx]);
  else 
    return(min_rate_pri);
}

void TmSchSel::port_sel() {
  uint8_t port_win;
  bool port_eligible;
  sch_deq_info info;

  // Traverse all ports of a pipe
  for (int port_idx = 0; port_idx < TmDefs::kNumPortPerPipe; port_idx++) {

    // Form eligible port vector
    port_eligible = 1;
    port_eligible &= m_config->get_port_config().enb(port_idx);
    port_eligible &= m_state->get_port_max_rate_pos(port_idx);
    port_eligible &= m_state->get_port_occ(port_idx);
    // Check if PEX credit is available for the port
    port_eligible &= m_state->get_pex_cr_avail(port_idx);
    m_prt_arb.set_port_rdy(port_idx, port_eligible);
    m_prt_arb.set_port_mode(port_idx, m_config->get_port_config().port_speed_mode(port_idx));
    //if (port_idx == 8)
    //printf("TMSCHSEL::port:%0d port_eligible:%0d\n", port_idx, port_eligible);
  }

  // Get port winner from arbiter
  port_win = m_prt_arb.do_port_selection();
  if (port_win != 0xff) {
    info.port = port_win;
    //printf("SCH_%0d_SEL::Sent packet to l1_sel on port:%0d l1:%0d qid:%0d min_pkt:%0d\n",
    //   m_config->get_pipe_index(), info.port, info.l1, info.qid, info.min_pkt);
    l1_sel(info);
  }
}

void TmSchSel::l1_sel(sch_deq_info info) {
  bool l1_eligible;
  sch_exc_lb_static rate_cfg;
  uint32_t l1_pipe_idx;
  uint8_t mac_id, mac_chnl, l1_win, l1_port; 
    //min_rate_pri, max_rate_pri;
  bool enb;

  // Clean Min Pkt
  min_pkt_sel = 0;

  // Find Mac ID
  mac_id = (info.port / TmDefs::kNumPortPerMac);
  mac_chnl = (info.port & 7);

  // Traverse all L1 for the port
  for (int l1_idx = 0; l1_idx < TmDefs::kNumL1PerMac; l1_idx++) {
    // Generate Pipe Index
    l1_pipe_idx = (mac_id * TmDefs::kNumL1PerMac) + l1_idx;

    // Get L1 Config
    enb = m_config->get_l1_config().enb(l1_pipe_idx);
    l1_port = m_config->get_l1_config().l1_port(l1_pipe_idx);
    //min_rate_pri = m_config->get_l1_config().min_rate_pri(l1_pipe_idx);
    //max_rate_pri = m_config->get_l1_config().max_rate_pri(l1_pipe_idx);

    // Get L1 Exc Rate Cfg
    rate_cfg = m_config->get_l1_exc_rate_config(l1_pipe_idx);

    // Generate min eligibility vector
    if (l1_port == mac_chnl) {
      l1_eligible = 1;
      l1_eligible &= enb;
      l1_eligible &= m_state->get_l1_occ(l1_pipe_idx);
      //printf("TMSCHSEL::l1_idx:%0d l1_eligible:%0d\n", l1_idx, l1_eligible);
      l1_eligible &= m_state->get_l1_min_rate_pos(l1_pipe_idx);
      // Min cannot excess max
      l1_eligible &= m_state->get_l1_max_rate_pos(l1_pipe_idx);
    } else {
      l1_eligible = 0;
    }
    m_l1_arb.set_node_min_rdy(l1_idx, l1_eligible);
    m_l1_arb.set_node_min_pri(l1_idx, pick_l1_min_rate_pri(l1_pipe_idx));

    // Generate max/excess eligibility vector
    if (l1_port == mac_chnl) {
      l1_eligible = 1;
      l1_eligible &= enb;
      l1_eligible &= m_state->get_l1_occ(l1_pipe_idx);
      // For all L1's with weight of zero,
      // DWRR is disabled, only look at max
      // rate, also max and DWRR cannot be
      // enabled together
      if (!rate_cfg.wt)
	l1_eligible &= m_state->get_l1_max_rate_pos(l1_pipe_idx);
      else
	l1_eligible &= m_state->get_l1_exc_rate_pos(l1_pipe_idx);
    } else {
      l1_eligible = 0;
    }
    m_l1_arb.set_node_max_rdy(l1_idx, l1_eligible);
    m_l1_arb.set_node_max_pri(l1_idx, pick_l1_max_rate_pri(l1_pipe_idx));
  }

  // Get L1 winner from arbiter
  // If L1 belongs to the port, first try to find a
  // winner in min, if there is no winner in min,
  // try to find a winner in max/excess
  l1_win = m_l1_arb.do_sp_min_node_selection();
  if (l1_win == 0xff) {
    l1_win = m_l1_arb.do_sp_max_node_selection();
  } else {
    min_pkt_sel |= 4;
  }

  if (l1_win != 0xff) {
    info.l1 = (mac_id * TmDefs::kNumL1PerMac) + l1_win;
    info.min_pkt = min_pkt_sel;

    // printf("SCH_%0d_SEL::Sent packet to q_sel on port:%0d l1:%0d qid:%0d min_pkt:%0d\n",
    //   m_config->get_pipe_index(), info.port, info.l1, info.qid, info.min_pkt);

    // Send winner to qsel
    q_sel(info);
  }

}

void TmSchSel::q_sel(sch_deq_info info) {
  bool q_eligible;
  sch_exc_lb_static rate_cfg;
  uint32_t q_pipe_idx;
  uint8_t mac_id, l1_id_mac, q_win,
    max_rate_pri, min_rate_pri, l1_id;
  bool enb;

  // Clean Min Pkt
  min_pkt_sel = 0;

  // Find Mac ID
  mac_id = (info.port / TmDefs::kNumPortPerMac);
  l1_id_mac = (info.l1 & 31);

  // Traverse all Q for the L1
  for (int q_idx = 0; q_idx < TmDefs::kNumVqPerMac; q_idx++) {
    // Generate Pipe Index
    q_pipe_idx = (mac_id * TmDefs::kNumVqPerMac) + q_idx;

    // Get Q Config
    enb = m_config->get_q_config().enb(q_pipe_idx);
    l1_id = m_config->get_q_config().l1_id(q_pipe_idx);
    max_rate_pri = m_config->get_q_config().max_rate_pri(q_pipe_idx);
    min_rate_pri = m_config->get_q_config().min_rate_pri(q_pipe_idx);

    // Get Q Exc Rate Cfg
    rate_cfg = m_config->get_q_exc_rate_config(q_pipe_idx);

    // Generate min eligibility vector
    if (l1_id == l1_id_mac) {
      q_eligible = 1;
      q_eligible &= enb;
      q_eligible &= m_state->get_q_occ(q_pipe_idx);
      //printf("TMSCHSEL::q_idx:%0d q_eligible:%0d\n", q_idx, q_eligible);
      q_eligible &= m_state->get_q_min_rate_pos(q_pipe_idx);
      // Min cannot excess max
      q_eligible &= m_state->get_q_max_rate_pos(q_pipe_idx);
    } else {
      q_eligible = 0;
    }
    m_q_arb.set_node_min_rdy(q_idx, q_eligible);
    m_q_arb.set_node_min_pri(q_idx, min_rate_pri);

    // Generate max/excess eligibility vector
    if (l1_id == l1_id_mac) {
      q_eligible = 1;
      q_eligible &= enb;
      q_eligible &= m_state->get_q_occ(q_pipe_idx);
      //printf("TMSCHSEL::q_idx:%0d q_eligible:%0d\n", q_idx, q_eligible);
      // For all Q's with weight of zero,
      // DWRR is disabled, only look at max
      // rate, also max and DWRR cannot be
      // enabled together
      if (!rate_cfg.wt)
	q_eligible &= m_state->get_q_max_rate_pos(q_pipe_idx);
      else
	q_eligible &= m_state->get_q_exc_rate_pos(q_pipe_idx);
    } else {
      q_eligible = 0;
    }
    m_q_arb.set_node_max_rdy(q_idx, q_eligible);
    m_q_arb.set_node_max_pri(q_idx, max_rate_pri);
  }

  // Get Q winner from arbiter
  // First try to find a winner in min,
  // if there is no winner in min,
  // try to find a winner in max/excess
  q_win = m_q_arb.do_sp_min_node_selection();
  if (q_win == 0xff) {
    q_win = m_q_arb.do_sp_max_node_selection();
  } else {
    min_pkt_sel |= 1;
  }

  if (q_win != 0xff) {
    info.qid = (mac_id * TmDefs::kNumVqPerMac) + q_win;
    info.min_pkt |= min_pkt_sel;
    info.ecos = m_config->get_q_config().pfc_pri(info.qid);

    // Schedule the packet
    sch_winner_fifo.push_back(info);

    RMT_LOG(RmtDebug::info(), "SCH_%0d_SEL::Scheduled a packet on port:%0d l1:%0d qid:%0d min_pkt:%0d\n",
	    m_config->get_tm_index(), info.port, info.l1, info.qid, info.min_pkt);
  }
}

}
