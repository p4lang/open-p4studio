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
#include <tm-defines.h>
#include <tm-sch-model-wrapper-defines.h>
#include <tm-sch-model-wrapper.h>
#include <cstring>

namespace MODEL_CHIP_NAMESPACE{

TmSchModelWrapper::TmSchModelWrapper(RmtObjectManager *om, uint8_t pipe_index)
  : DefaultLogger(om, RmtTypes::kRmtTypeTm)
{
  // New the Objects
  m_qlc = new TmQlc(om, pipe_index);
  m_pex = new TmPex(om, pipe_index);
  m_sch = new TmSch(om, pipe_index);

  RMT_LOG(RmtDebug::info(), "Newing SCH Model Wrapper with pipe:%0d\n", pipe_index);
}

TmSchModelWrapper::~TmSchModelWrapper() {
  delete m_qlc;
  delete m_pex;
  delete m_sch;
}

void TmSchModelWrapper::move_data() {
  // Make sure each interface generated
  // only one set of data per clock
  // and then move data

  // QLC -> SCH Packet Enq
  for (int i=0; i<TmDefs::kNumQlcSchUpdIfc; i++) {
    RMT_ASSERT(m_qlc->qlc_sch_fifo[i].size() <= 1);
    if (m_qlc->qlc_sch_fifo[i].size()) {
      m_sch->qlc_sch_fifo[i].push_back(m_qlc->qlc_sch_fifo[i].front());
      m_qlc->qlc_sch_fifo[i].erase(m_qlc->qlc_sch_fifo[i].begin());
    }
  }

  // SCH Dequeue
  RMT_ASSERT(m_sch->sch_qlc_fifo.size() <= 1);
  if (m_sch->sch_qlc_fifo.size()) {

    // SCH -> PEX Packet Deq
    psc_pex_enq_info info;
    info.port = m_sch->sch_qlc_fifo.front().port;
    info.l1 = m_sch->sch_qlc_fifo.front().l1;
    info.qid = m_sch->sch_qlc_fifo.front().qid;
    info.min_pkt = m_sch->sch_qlc_fifo.front().min_pkt;
    info.ecos = m_sch->sch_qlc_fifo.front().ecos;
    info.pkt_size = m_qlc->packet_size[info.qid].front();
    m_qlc->packet_size[info.qid].erase(m_qlc->packet_size[info.qid].begin());
    m_pex->psc_pex_enq_fifo.push_back(info);

    // Calculate Packets/Bytes Processed
    packets_processed_port[info.port]++;
    bytes_acc_port[info.port] += info.pkt_size;
    packets_processed_l1[info.l1]++;
    bytes_acc_l1[info.l1] += info.pkt_size;
    packets_processed_q[info.qid]++;
    bytes_acc_q[info.qid] += info.pkt_size;

    // SCH -> QLC Packet Deq
    m_qlc->sch_qlc_fifo.push_back(m_sch->sch_qlc_fifo.front());
    m_sch->sch_qlc_fifo.erase(m_sch->sch_qlc_fifo.begin());
  }

  // PEX -> SCH BYTE UPDATE
  for (int i=0; i<TmDefs::kNumPexSchByteUpdIfc; i++) {
    RMT_ASSERT(m_pex->pex_sch_bupd_fifo[i].size() <= 1);
    if (m_pex->pex_sch_bupd_fifo[i].size()) {
      m_sch->pex_sch_bupd_fifo[i].push_back(m_pex->pex_sch_bupd_fifo[i].front());
      m_pex->pex_sch_bupd_fifo[i].erase(m_pex->pex_sch_bupd_fifo[i].begin());
    }
  }

  // PEX -> SCH CREDIT UPDATE
  for (int i=0; i<TmDefs::kNumPexSchCrdUpdIfc; i++) {
    RMT_ASSERT(m_pex->pex_sch_crd_fifo[i].size() <= 1);
    if (m_pex->pex_sch_crd_fifo[i].size()) {
      m_sch->pex_sch_crd_fifo[i].push_back(m_pex->pex_sch_crd_fifo[i].front());
      m_pex->pex_sch_crd_fifo[i].erase(m_pex->pex_sch_crd_fifo[i].begin());
    }
  }
}

/*
void test_9x400G_perf_mix_pkt(qlc* qlc_ptr, sch_config* sch_cfg) {
  bool vq_bmp[TmDefs::kNumVqPerMac];
  uint32_t value;

  // 9 x 400G, 1 L1 & 1 Q per port
  // QLC Config
  memset(vq_bmp, 0, sizeof(vq_bmp));
  vq_bmp[0] = 1;
  for (int mac = 0; mac < TmDefs::kNumMacPerPipe; mac++)
    m_qlc.set_active_vq_in_mac(mac, vq_bmp);

  // Set Packet size in QLC
  for (int vq_idx = 0; vq_idx < TmDefs::kNumVqPerPipe; vq_idx++) {
    if (vq_idx < 128)
      m_qlc.set_packet_size_gen(vq_idx, 64);
    else
      m_qlc.set_packet_size_gen(vq_idx, 262);
  }

  // Configure SCH Port
  for (int port = 0; port < TmDefs::kNumPortPerPipe; port++) {
    // Port Config
    value = ((port % 8) == 0) ? 0x77 : 0;
    sch_cfg->set_port_config(port, value);

    // Rate Config
    value = (0x154 | (4000 << 17));
    sch_cfg->set_port_max_rate_config(port, value);
  }

  // Configure SCH L1
  for (int l1 = 0; l1 < TmDefs::kNumL1PerPipe; l1++) {
    value = ((l1 & 31) == 15) ? 0x5 : 0;
    sch_cfg->set_l1_config(l1, value);


    value = (0x154 | (4000 << 17));
    sch_cfg->set_l1_max_rate_config(l1, value);
  }

  // Configure SCH Queues
  for (int q = 0; q < TmDefs::kNumVqPerPipe; q++) {
    value = ((q & 127) == 0) ? (0xb + (15 << 4)) : 0;
    sch_cfg->set_q_config(q, value);

    value = (0x154 | (4000 << 17));
    sch_cfg->set_q_max_rate_config(q, value);
  }
}
*/

void TmSchModelWrapper::set_active_vq_in_mac(uint8_t mac, bool vq_bmp[TmDefs::kNumVqPerMac]) {
  m_qlc->set_active_vq_in_mac(mac, vq_bmp);
}

void TmSchModelWrapper::compute_q_rates(uint16_t l1_pipe_id, float sim_time) {
  // Q Pipe Index
  uint16_t q_pipe_idx;
  uint8_t l1_id;

  // Process L1s for the port
  for (int q = 0; q < TmDefs::kNumVqPerMac; q++) {
    // Find Q Index
    q_pipe_idx = ((l1_pipe_id / TmDefs::kNumL1PerMac) * TmDefs::kNumVqPerMac) + q;
    l1_id = m_sch->get_sch_config()->get_q_config().l1_id(q_pipe_idx);
    if (l1_id == (l1_pipe_id & 31)) {
      if (bytes_acc_q[q_pipe_idx]) {
	// Add IPG Bytes
	bytes_acc_q[q_pipe_idx] += (packets_processed_q[q_pipe_idx] *  m_sch->get_sch_config()->get_glb_bcnt_adj().value());
	q_rate[q_pipe_idx] = ((bytes_acc_q[q_pipe_idx] / sim_time) * 8);
	RMT_LOG(RmtDebug::info(), "Sim time elapsed:%0f ns, q_id:%0d packets:%0u, rate:%0f Gbps\n",
		sim_time, q_pipe_idx, packets_processed_q[q_pipe_idx], q_rate[q_pipe_idx]);
      }
    }
  }
}

void TmSchModelWrapper::compute_l1_rates(uint8_t port, float sim_time) {
  // L1 Pipe Index
  uint16_t l1_pipe_idx;

  // Config Variables
  uint8_t l1_port;

  // Process L1s for the port
  for (int l1 = 0; l1 < TmDefs::kNumL1PerMac; l1++) {
    // Find L1 Index
    l1_pipe_idx = ((port / TmDefs::kNumPortPerMac) * TmDefs::kNumL1PerMac) + l1;
    l1_port = m_sch->get_sch_config()->get_l1_config().l1_port(l1_pipe_idx);
    if (l1_port == (port & 7)) {
      if (bytes_acc_l1[l1_pipe_idx]) {
	// Add IPG Bytes
	bytes_acc_l1[l1_pipe_idx] += (packets_processed_l1[l1_pipe_idx] *  m_sch->get_sch_config()->get_glb_bcnt_adj().value());
	l1_rate[l1_pipe_idx] = ((bytes_acc_l1[l1_pipe_idx] / sim_time) * 8);
	RMT_LOG(RmtDebug::info(), "Sim time elapsed:%0f ns, l1_id:%0d packets:%0u, rate:%0f Gbps\n",
		sim_time, l1_pipe_idx, packets_processed_l1[l1_pipe_idx], l1_rate[l1_pipe_idx]);
	compute_q_rates(l1_pipe_idx, sim_time);
      }
    }
  }
}

uint32_t  TmSchModelWrapper::get_port_rate_mbps(uint8_t port) {
  return(port_rate[port] * 1000);
}

uint32_t  TmSchModelWrapper::get_l1_rate_mbps(uint16_t l1_pipe_id) {
  return(l1_rate[l1_pipe_id] * 1000);
}

uint32_t  TmSchModelWrapper::get_q_rate_mbps(uint16_t q_pipe_id) {
  return(q_rate[q_pipe_id] * 1000);
}

void TmSchModelWrapper::run_sch_model() {
    std::cout << "Launching SCH Sim!\n";

    // Sim Time Variables
    float sim_time = 0, next_pps_time = 0, next_one_ns_time = 0;

    // Sim Clk Variables
    uint32_t pps_clk_count = 0, one_ns_clk_count = 0;;

    bool pps_tick = 0;

    // Zero out rate counters
    memset(packets_processed_port, 0, sizeof(packets_processed_port));
    memset(bytes_acc_port, 0, sizeof(bytes_acc_port));
    memset(packets_processed_l1, 0, sizeof(packets_processed_l1));
    memset(bytes_acc_l1, 0, sizeof(bytes_acc_l1));
    memset(packets_processed_q, 0, sizeof(packets_processed_q));
    memset(bytes_acc_q, 0, sizeof(bytes_acc_q));

    // Start Sim, need to simulate two clocks.
    while (sim_time < SIM_TIMEOUT) {

      // Find out next time ticks
      next_pps_time = (pps_clk_count + 1) * SYS_CLK_PERIOD_NS;
      next_one_ns_time = (one_ns_clk_count + 1) * 1;

      // Determine next tick
      if (next_pps_time <= next_one_ns_time) pps_tick = 1;
      else pps_tick = 0;

      // Do work
      if (pps_tick) {
        sim_time = next_pps_time;
        pps_clk_count++;
        // RMT_LOG(RmtDebug::info(), "%0f ns::pps_tick\n", sim_time);
        m_qlc->pps_tick();
        m_sch->pps_tick();
        m_pex->pps_tick();
        move_data();
      } else {
        sim_time = next_one_ns_time;
        one_ns_clk_count++;
        // RMT_LOG(RmtDebug::info(), "%0f ns::1ns_tick\n", sim_time);
        m_sch->one_ns_tick();
      }
    }

    // Measure Rate
    RMT_LOG(RmtDebug::info(), "\n#### SCH Rates ####\n");
    for (int port = 0; port < TmDefs::kNumPortPerPipe; port++) {
      if (packets_processed_port[port]) {
        // Add IPG Bytes
        bytes_acc_port[port] += (packets_processed_port[port] *  m_sch->get_sch_config()->get_glb_bcnt_adj().value());
        port_rate[port] = ((bytes_acc_port[port] / sim_time) * 8);
        RMT_LOG(RmtDebug::info(), "Sim time elapsed:%0f ns, port:%0d packets:%0u, rate:%0f Gbps\n",
            sim_time, port, packets_processed_port[port], port_rate[port]);
        compute_l1_rates(port, sim_time);
        RMT_LOG(RmtDebug::info(), "\n");
      }
    }
}

}
