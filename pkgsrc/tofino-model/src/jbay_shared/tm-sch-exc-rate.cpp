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
#include <math.h>
#include <tm-sch-exc-rate.h>

namespace MODEL_CHIP_NAMESPACE {

TmSchExcRate::TmSchExcRate(RmtObjectManager *om, uint8_t pipe_index) : TmObject(om, pipe_index) {
  memset(l1_acc, 0, sizeof(l1_acc));
  memset(q_acc, 0, sizeof(q_acc));
  memset(l1_exc_bucket, 0, sizeof(l1_exc_bucket));
  memset(q_exc_bucket, 0, sizeof(q_exc_bucket));
  one_ns_tick_cnt = 0;
  eighty_ns_tick_cnt = 0;
  m_config = NULL;
  m_state = NULL;
}

void TmSchExcRate::init(TmSchConfig *sch_config, TmSchState *sch_state) {
  m_config = sch_config;
  m_state = sch_state;
}

void TmSchExcRate::pps_tick() {
  RMT_ASSERT(m_config != NULL);
  RMT_ASSERT(m_state != NULL);
  process_pex_bupd();
  // Without this fix change in register
  // configuration will take effect only
  // at 80ns boundary
  update_sch_state();
}

void TmSchExcRate::process_pex_bupd() {
  pex_bupd_info info;
  int glb_bcnt_adj;

  glb_bcnt_adj = m_config->get_glb_bcnt_adj().value();

  while(pex_sch_bupd_fifo.size()) {

    // Pop from front
    info = pex_sch_bupd_fifo.front();
    pex_sch_bupd_fifo.erase(pex_sch_bupd_fifo.begin());

    // Subtract bytes from the accumulator
    // for all non-min packets
    if (!(info.min_pkt & 4)) {
      l1_acc[info.l1] -= info.bytes;
      // Subtract Global Bcnt Adj per packet
      if (info.eop)
	l1_acc[info.l1] -= glb_bcnt_adj;
    }

    if (!(info.min_pkt & 1)) {
      q_acc[info.qid] -= info.bytes;
      // Subtract Global Bcnt Adj per packet
      if (info.eop)
	q_acc[info.qid] -= glb_bcnt_adj;
    }

    // printf("SCH_%0d_EXC_RATE::Processing Bupd port:%0d l1:%0d q:%0d adj:%0d bytes:%0d acc:%0d\n",
    //	   m_config->get_pipe_index(), info.port, info.l1, info.qid, glb_bcnt_adj, info.bytes, l1_acc[info.l1]);
  }
}

void TmSchExcRate::one_ns_tick() {

  // Make sure state & config
  // are not NULL
  RMT_ASSERT(m_config != NULL);
  RMT_ASSERT(m_state != NULL);

  // Increment one ns cnt
  one_ns_tick_cnt++;

  // Every 80ns, check if buckets need to be
  // refreshed and commit bytes to the bucket
  if (!(one_ns_tick_cnt % TmDefs::kSchRefreshIntNs)) {
    // printf("SCH_%0d_EXC_RATE::Processing 80ns tick\n", m_config->get_pipe_index());
    eighty_ns_tick_cnt++;
    sch_bucket_commit();
    update_sch_state();
  }
}

void TmSchExcRate::update_sch_state() {
  sch_exc_lb_static rate_cfg;
  bool rate_pos;

  // L1
  for (int l1_pipe_idx=0; l1_pipe_idx<TmDefs::kNumL1PerPipe; l1_pipe_idx++) {
    rate_cfg = m_config->get_l1_exc_rate_config(l1_pipe_idx);
    rate_pos = (!rate_cfg.wt) ? 0 : (l1_exc_bucket[l1_pipe_idx] >= 0);
    m_state->set_l1_exc_rate_pos(l1_pipe_idx, rate_pos);
  }

  // Q
  for (int q_pipe_idx=0; q_pipe_idx<TmDefs::kNumVqPerPipe; q_pipe_idx++) {
    rate_cfg = m_config->get_q_exc_rate_config(q_pipe_idx);
    rate_pos = (!rate_cfg.wt) ? 0 : (q_exc_bucket[q_pipe_idx] >= 0);
    m_state->set_q_exc_rate_pos(q_pipe_idx, rate_pos);
  }
}

void TmSchExcRate::populate_l1_id_per_port() {
  int mac_id, port_id;
  bool enb;
  uint8_t l1_port;
  sch_exc_lb_static rate_cfg;

  // Empty the vector
  for (int port = 0; port < TmDefs::kNumPortPerPipe; port++) {
    l1_id_per_port[port].clear();
  }

  // Populate L1 Id per Port
  for (int l1_pipe_idx = 0; l1_pipe_idx < TmDefs::kNumL1PerPipe; l1_pipe_idx++) {
    // Find out L1 cfg
    enb = m_config->get_l1_config().enb(l1_pipe_idx);
    l1_port = m_config->get_l1_config().l1_port(l1_pipe_idx);

    // Find out L1 rate cfg
    rate_cfg = m_config->get_l1_exc_rate_config(l1_pipe_idx);

    // If L1 is enabled add to port
    // We need to look at the node weight
    // otherwise a node with weight zero
    // will mess up the calculations, since
    // it keeps going negative
    if (enb && rate_cfg.wt) {
      mac_id = (l1_pipe_idx / TmDefs::kNumL1PerMac);
      port_id = (mac_id * TmDefs::kNumPortPerMac) + l1_port;
      l1_id_per_port[port_id].push_back(l1_pipe_idx);
    }
  }
}

bool TmSchExcRate::check_port_l1_pos(uint8_t port_id) {
  int l1_idx;
  // Check if any L1 of the port is positive
  for (size_t element = 0; element < l1_id_per_port[port_id].size(); element++) {
    l1_idx = l1_id_per_port[port_id][element];
    if (l1_exc_bucket[l1_idx] >= 0)
      return(1);
  }
  return(0);
}

void TmSchExcRate::populate_q_id_per_l1() {
  int mac_id, l1_pipe_id, l1_id;
  bool enb;
  sch_exc_lb_static rate_cfg;

  // Empty the vector
  for (int l1_pipe_idx = 0; l1_pipe_idx < TmDefs::kNumL1PerPipe; l1_pipe_idx++) {
    q_id_per_l1[l1_pipe_idx].clear();
  }

  // Populate Q Id per L1
  for (int q_pipe_idx = 0; q_pipe_idx < TmDefs::kNumVqPerPipe; q_pipe_idx++) {
    // Find out L1 cfg
    enb = m_config->get_q_config().enb(q_pipe_idx);
    l1_id = m_config->get_q_config().l1_id(q_pipe_idx);

    // Find out L1 rate cfg
    rate_cfg = m_config->get_q_exc_rate_config(q_pipe_idx);

    // If Q is enabled add to L1
    // We need to look at the node weight
    // otherwise a node with weight zero
    // will mess up the calculations, since
    // it keeps going negative
    if (enb && rate_cfg.wt) {
      mac_id = (q_pipe_idx / TmDefs::kNumVqPerMac);
      l1_pipe_id = (mac_id * TmDefs::kNumL1PerMac) + l1_id;
      q_id_per_l1[l1_pipe_id].push_back(q_pipe_idx);
    }
  }
}

bool TmSchExcRate::check_l1_q_pos(uint16_t l1_id) {
  int q_idx;
  // Check if any L1 of the port is positive
  for (size_t element = 0; element < q_id_per_l1[l1_id].size(); element++) {
    q_idx = q_id_per_l1[l1_id][element];
    if (q_exc_bucket[q_idx] >= 0)
      return(1);
  }
  return(0);
}

void TmSchExcRate::sch_bucket_commit() {
  sch_l1_acc_commit();
  sch_l1_bucket_adjust();
  sch_q_acc_commit();
  sch_q_bucket_adjust();
}

void TmSchExcRate::sch_l1_acc_commit() {

  // Commit the Excess accumulator
  for (int l1_pipe_idx = 0; l1_pipe_idx < TmDefs::kNumL1PerPipe; l1_pipe_idx++) {

    // Commit the accumulator bytes to the bucket
    l1_exc_bucket[l1_pipe_idx] += l1_acc[l1_pipe_idx];

    // Clear the accumulator
    l1_acc[l1_pipe_idx] = 0;
  }
}

void TmSchExcRate::sch_q_acc_commit() {

  // Commit the Excess accumulator
  for (int q_pipe_idx = 0; q_pipe_idx < TmDefs::kNumVqPerPipe; q_pipe_idx++) {

    // Commit the accumulator bytes to the bucket
    q_exc_bucket[q_pipe_idx] += q_acc[q_pipe_idx];

    // Clear the accumulator
    q_acc[q_pipe_idx] = 0;
  }
}

void TmSchExcRate::sch_l1_bucket_adjust() {
  sch_exc_lb_static rate_cfg;
  uint8_t msb_bucket, msb_wt, shift_quant;
  uint16_t l1_idx;
  uint32_t log_or_bucket = 0, log_or_wt = 0;
  int bucket_size;

  // Figure out L1 Ids of each port
  populate_l1_id_per_port();

  // Max exc bucket size
  bucket_size = (1 << 20);

  // Check if buckets need to adjusted
  for (int port = 0; port < TmDefs::kNumPortPerPipe; port++) {

    // If any L1 of a port is positive no
    // more calculations are needed
    if (check_port_l1_pos(port))
      continue;

    // All L1's are negative, so
    // buckets need to be adjusted
    for (size_t element = 0; element < l1_id_per_port[port].size(); element++) {
      // Find L1 Index
      l1_idx = l1_id_per_port[port][element];

      // Find out exc rate cfg for port
      rate_cfg = m_config->get_l1_exc_rate_config(l1_idx);

      // printf("SCH_%0d_EXC::Bucket %0d for l1:%0d\n",
      //     m_config->get_pipe_index(), l1_exc_bucket[l1_idx], l1_idx);

      // Logical OR of bucket
      log_or_bucket |= abs(l1_exc_bucket[l1_idx]);

      // Logical OR of weights
      log_or_wt |= rate_cfg.wt;
    }

    // Find msb for buckets
    msb_bucket = (log2(log_or_bucket) + 1);

    // Find msb for weights
    msb_wt = log2(log_or_wt);

    // Calculate shift quantum
    shift_quant = (msb_bucket > msb_wt) ? (msb_bucket - msb_wt) : 1;

    // Adjust the buckets
    for (size_t element = 0; element < l1_id_per_port[port].size(); element++) {
      // Find L1 Index
      l1_idx = l1_id_per_port[port][element];

      // Find out exc rate cfg for port
      rate_cfg = m_config->get_l1_exc_rate_config(l1_idx);

      // Adjust the bucket
      l1_exc_bucket[l1_idx] += (rate_cfg.wt << shift_quant);
      // printf("SCH_%0d_EXC::Added %0d for l1:%0d bucket:%0d\n",
      //     m_config->get_pipe_index(), (rate_cfg.wt << shift_quant), l1_idx, l1_exc_bucket[l1_idx]);

      // When we add bytes to the bucket, we cannot let it
      // increment beyond the bucket size. But when we
      // decrement bytes, we have to let the bucket go as
      // negative as needed since SCH can send more bytes
      // than that in the bucket.
      if (l1_exc_bucket[l1_idx] > bucket_size)
	l1_exc_bucket[l1_idx] = bucket_size;
    }
  }
}

void TmSchExcRate::sch_q_bucket_adjust() {
  sch_exc_lb_static rate_cfg;
  uint8_t msb_bucket, msb_wt, shift_quant;
  uint16_t q_idx;
  uint32_t log_or_bucket = 0, log_or_wt = 0;
  int bucket_size;

  // Figure out Q Ids of each L1
  populate_q_id_per_l1();

  // Max exc bucket size
  bucket_size = (1 << 20);

  // Check if buckets need to adjusted
  for (int l1 = 0; l1 < TmDefs::kNumL1PerPipe; l1++) {

    // If any Q of a L1 is positive no
    // more calculations are needed
    if (check_l1_q_pos(l1))
      continue;

    // All Q's are negative, so
    // buckets need to be adjusted
    for (size_t element = 0; element < q_id_per_l1[l1].size(); element++) {
      // Find Q Index
      q_idx = q_id_per_l1[l1][element];

      // Find out exc rate cfg for port
      rate_cfg = m_config->get_q_exc_rate_config(q_idx);

      // printf("SCH_%0d_EXC::Bucket %0d for q:%0d\n",
      //    m_config->get_pipe_index(), q_exc_bucket[q_idx], q_idx);

      // Logical OR of bucket
      log_or_bucket |= abs(q_exc_bucket[q_idx]);

      // Logical OR of weights
      log_or_wt |= rate_cfg.wt;
    }

    // Find msb for buckets
    msb_bucket = (log2(log_or_bucket) + 1);

    // Find msb for weights
    msb_wt = log2(log_or_wt);

    // Calculate shift quantum
    shift_quant = (msb_bucket > msb_wt) ? (msb_bucket - msb_wt) : 1;

    // Adjust the buckets
    for (size_t element = 0; element < q_id_per_l1[l1].size(); element++) {
      // Find L1 Index
      q_idx = q_id_per_l1[l1][element];

      // Find out exc rate cfg for port
      rate_cfg = m_config->get_q_exc_rate_config(q_idx);

      // Adjust the bucket
      q_exc_bucket[q_idx] += (rate_cfg.wt << shift_quant);
      // printf("SCH_%0d_EXC::Added %0d for q:%0d bucket:%0d\n",
      //     m_config->get_pipe_index(), (rate_cfg.wt << shift_quant), q_idx, q_exc_bucket[q_idx]);

      // When we add bytes to the bucket, we cannot let it
      // increment beyond the bucket size. But when we
      // decrement bytes, we have to let the bucket go as
      // negative as needed since SCH can send more bytes
      // than that in the bucket.
      if (q_exc_bucket[q_idx] > bucket_size)
	q_exc_bucket[q_idx] = bucket_size;
    }
  }
}

}
