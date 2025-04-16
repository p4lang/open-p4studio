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
#include <tm-sch-model-wrapper-defines.h>
#include <tm-qlc.h>

namespace MODEL_CHIP_NAMESPACE {

TmQlc::TmQlc(RmtObjectManager *om, uint8_t pipe_index) : TmObject(om, pipe_index) {

  for (int vq_idx = 0; vq_idx < TmDefs::kNumVqPerPipe; vq_idx++) {

    // Set up all Rate Limiters
    m_rl[vq_idx].set_port_rate(400);
    m_rl[vq_idx].set_rl_id((char *)"TmQlc", vq_idx);
    m_rl[vq_idx].set_drain_time(SYS_CLK_PERIOD_NS);
    m_rl[vq_idx].set_drain_params();

    // Clear QLC State
    pq_cnt_table[vq_idx] = 0;
    pq_enq_ptr[vq_idx] = 0;
    pq_deq_ptr[vq_idx] = 0;
    pq_enq_update_pending[vq_idx] = 0;
    pq_deq_update_pending[vq_idx] = 0;

    // Set Default Packet Size
    packet_size_gen[vq_idx] = 280;
  }
}

void TmQlc::set_packet_size_gen(int vq_idx, int packet_size) {
  packet_size_gen[vq_idx] = packet_size;
}

void TmQlc::pps_tick() {
  qlc_enq();
  qlc_deq();
}

void TmQlc::set_active_vq_in_mac(int mac, bool vq_bmp[TmDefs::kNumVqPerMac]) {
  uint8_t vq_cnt_mac = 0;

  // Find which Qs in the mac are active
  for (int vq = 0; vq < TmDefs::kNumVqPerMac; vq++) {
    if (vq_bmp[vq]) {
      vq_cnt_mac += 1;
      active_vq_in_mac[mac].push_back(vq);
    }
  }

  // Figure out # of PQ/VQ
  if (vq_cnt_mac < 16)
    pq_per_vq_in_mac[mac] = 8;
  else
    pq_per_vq_in_mac[mac] = (TmDefs::kNumPqPerMac / vq_cnt_mac);

}

void TmQlc::qlc_enq() {

  int vq_idx, pq_idx, stat_upd_proc = 0;
  sch_deq_info info{};

  // Enqueue packets to queues
  for (int mac = 0; mac < TmDefs::kNumMacPerPipe; mac++) {
    for (uint32_t vq = 0; vq < active_vq_in_mac[mac].size(); vq++) {
      vq_idx = (mac * TmDefs::kNumVqPerMac) + active_vq_in_mac[mac].at(vq);

      // Linear mapping to keep things simple
      // Any PQ can map to any VQ in real HW
      pq_idx = (mac * TmDefs::kNumPqPerMac) + (vq * pq_per_vq_in_mac[mac]) + pq_enq_ptr[vq_idx];

      // Enqueue if rate limiter says xon
      if (m_rl[vq_idx].check_rl_xoff() == 0) {
	// How do I pick packet size??
	m_rl[vq_idx].process_line_sent(1, 1, packet_size_gen[vq_idx]);
	packet_size[vq_idx].push_back(packet_size_gen[vq_idx]);

	// If PQ is empty, send enq update to SCH
	if (pq_cnt_table[pq_idx] == 0) {
	  pq_enq_update_pending[vq_idx] = 1;
	}

	// Increment PQ cnt and enq ptr.
	pq_cnt_table[pq_idx]++;
	pq_enq_ptr[vq_idx] = ((pq_enq_ptr[vq_idx] + 1) % pq_per_vq_in_mac[mac]);
      }

      // Drain the Queue RL every clock
      m_rl[vq_idx].drain_from_fifo();
    }
  }

  // Generate status update to SCH
  vq_idx = 0;
  while (vq_idx < TmDefs::kNumVqPerPipe) {

    // Send Enq status
    if (pq_enq_update_pending[vq_idx]) {
      info.qid = vq_idx;
      qlc_sch_fifo[stat_upd_proc].push_back(info);
      pq_enq_update_pending[vq_idx] = 0;
      stat_upd_proc++;
      RMT_LOG(RmtDebug::verbose(), "QLC_%0d_ENQ::qid processed:%0d\n", get_tm_index(), info.qid);
    }

    // Only 5 Enq status per clock
    if (stat_upd_proc == TmDefs::kNumQlcSchEnqUpdIfc)
      break;

    // Increment vq_idx
    vq_idx++;
  }

}

uint8_t TmQlc::map_vq_to_pq_idx(uint8_t mac, uint8_t vq) {
  std::vector<uint8_t>::iterator it;

  it = std::find(active_vq_in_mac[mac].begin(), active_vq_in_mac[mac].end(), vq);
  return(std::distance(active_vq_in_mac[mac].begin(), it));
}

void TmQlc::qlc_deq() {

  sch_deq_info info;
  int vq, mac, pq_idx, vq_idx;

  if (sch_qlc_fifo.size()) {
    info = sch_qlc_fifo.front();
    sch_qlc_fifo.erase(sch_qlc_fifo.begin());

    // Map Qid to PQ
    mac = (info.port / TmDefs::kNumPortPerMac);
    vq = info.qid & 0x7f;
    vq_idx = info.qid;
    pq_idx = (mac * TmDefs::kNumPqPerMac) + (map_vq_to_pq_idx(mac, vq)  * pq_per_vq_in_mac[mac]) + pq_deq_ptr[vq_idx];

    // Decrement PQ cnt and enq ptr.
    pq_cnt_table[pq_idx]--;
    pq_deq_ptr[vq_idx] = (pq_deq_ptr[vq_idx] + 1) % pq_per_vq_in_mac[mac];

    // Send Deq Update to SCH
    if (pq_cnt_table[pq_idx] > 0) {
      RMT_LOG(RmtDebug::verbose(), "QLC_%0d_DEQ::qid processed:%0d pq_idx:%0d\n", get_tm_index(), info.qid, pq_idx);
      qlc_sch_fifo[5].push_back(info);
    }
  }
}

}
