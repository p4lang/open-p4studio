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
#include <cstring>
#include <tm-sch-occ.h>

namespace MODEL_CHIP_NAMESPACE {

TmSchOcc::TmSchOcc(RmtObjectManager *om, uint8_t pipe_index) : TmObject(om, pipe_index) {
  memset(port_occ_cnt, 0, sizeof(port_occ_cnt));
  memset(l1_occ_cnt, 0, sizeof(l1_occ_cnt));
  memset(q_occ_cnt, 0, sizeof(q_occ_cnt));
  m_config = NULL;
  m_state = NULL;
}

void TmSchOcc::pps_tick() {
  RMT_ASSERT(m_config != NULL);
  RMT_ASSERT(m_state != NULL);
  qlc_sch_enq_update();
  qlc_sch_deq_update();
  sch_qlc_deq();
}

void TmSchOcc::init(TmSchConfig *sch_cfg, TmSchState *sch_state) {
  m_config = sch_cfg;
  m_state = sch_state;
}

void TmSchOcc::update_sch_state(int port_pipe_idx, int l1_pipe_idx, int q_pipe_idx) {
  m_state->set_port_occ(port_pipe_idx, (port_occ_cnt[port_pipe_idx] != 0));
  m_state->set_l1_occ(l1_pipe_idx, (l1_occ_cnt[l1_pipe_idx] != 0));
  m_state->set_q_occ(q_pipe_idx, (q_occ_cnt[q_pipe_idx] != 0));
  if (m_config->prop_pri_for_queue(q_pipe_idx)) {
    if (q_occ_cnt[q_pipe_idx] != 0)
      m_state->en_queue_for_pri_prop(q_pipe_idx);
    else
      m_state->dis_queue_for_pri_prop(q_pipe_idx);
  }
}

void TmSchOcc::qlc_sch_enq_update() {
  sch_deq_info info;
  int mac_id, port_id, l1_id, port_pipe_idx,
    l1_pipe_idx;

  // QLC sends Queue empty to non-empty
  // update to SCH, increment occupancy
  while (enq_upd_fifo.size()) {
    info = enq_upd_fifo.front();
    enq_upd_fifo.erase(enq_upd_fifo.begin());

    // Find the MAC, L1 & Port for queue
    l1_id = m_config->get_q_config().l1_id(info.qid);
    port_id = m_config->get_l1_config().l1_port(l1_id);
    mac_id = info.qid / TmDefs::kNumVqPerMac;

    // Calculate Pipe Index
    // QLC will send pipe idx for qid
    // which doesnt need processing
    l1_pipe_idx = (mac_id * TmDefs::kNumL1PerMac) + l1_id;
    port_pipe_idx = (mac_id * TmDefs::kNumPortPerMac) + port_id;

    // printf("SCH_%0d_OCC::Enqueue p:%0d l1:%0d q:%0d\n",
    //   m_config->get_pipe_index(), port_pipe_idx, l1_pipe_idx, info.qid);

    // Increment Port & L1 Occupancy
    // if VQ is going from empty to
    // non-empty
    if (!q_occ_cnt[info.qid]) {
      port_occ_cnt[port_pipe_idx]++;
      l1_occ_cnt[l1_pipe_idx]++;
    }

    // Increment Q Occupancy
    q_occ_cnt[info.qid]++;

    // Update SCH Occupancy State
    update_sch_state(port_pipe_idx, l1_pipe_idx, info.qid);

    // Checks
    RMT_ASSERT(port_occ_cnt[port_pipe_idx] < TmDefs::kNumVqPerMac);
    RMT_ASSERT(l1_occ_cnt[l1_pipe_idx] < TmDefs::kNumVqPerMac);
    RMT_ASSERT(q_occ_cnt[info.qid] < 9);
  }
}

void TmSchOcc::qlc_sch_deq_update() {
  sch_deq_info info;

  // QLC sends q non empty on SCH deq,
  // update to SCH, increment occupancy

  if (deq_upd_fifo.size()) {
    info = deq_upd_fifo.front();
    deq_upd_fifo.erase(deq_upd_fifo.begin());

    // Since deq info is loop back to SCH
    // port, l1 and qid can be used as is

    // Increment Port & L1 Occupancy
    // if VQ is going from empty to
    // non-empty
    if (!q_occ_cnt[info.qid]) {
      port_occ_cnt[info.port]++;
      l1_occ_cnt[info.l1]++;
    }

    // Increment Q Occupancy
    q_occ_cnt[info.qid]++;

    // Update SCH Occupancy State
    update_sch_state(info.port, info.l1, info.qid);

    // Checks
    RMT_ASSERT(port_occ_cnt[info.port] < TmDefs::kNumVqPerMac);
    RMT_ASSERT(l1_occ_cnt[info.l1] < TmDefs::kNumVqPerMac);
    RMT_ASSERT(q_occ_cnt[info.qid] < 9);
  }
}

void TmSchOcc::sch_qlc_deq() {
  sch_deq_info info;

  // SCH has dequeued a packet to QLC
  // decrement occupancy

  if (deq_fifo.size()) {
    info = deq_fifo.front();
    deq_fifo.erase(deq_fifo.begin());

    // Decrement Q Occupancy
    q_occ_cnt[info.qid]--;

    // Decrement Port & L1 Occupancy
    // if VQ went empty
    if (!q_occ_cnt[info.qid]) {
      port_occ_cnt[info.port]--;
      l1_occ_cnt[info.l1]--;
    }

    // Update SCH Occupancy State
    update_sch_state(info.port, info.l1, info.qid);
  }

}

}
