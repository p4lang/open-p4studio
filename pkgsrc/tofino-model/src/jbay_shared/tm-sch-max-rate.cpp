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
#include <tm-sch-max-rate.h>

namespace MODEL_CHIP_NAMESPACE {

TmSchMaxRate::TmSchMaxRate(RmtObjectManager *om, uint8_t pipe_index) : TmObject(om, pipe_index) {
  memset(port_acc, 0, sizeof(port_acc));
  memset(l1_acc, 0, sizeof(l1_acc));
  memset(q_acc, 0, sizeof(q_acc));
  memset(port_max_bucket, 0, sizeof(port_max_bucket));
  memset(l1_max_bucket, 0, sizeof(l1_max_bucket));
  memset(q_max_bucket, 0, sizeof(q_max_bucket));
  one_ns_tick_cnt = 0;
  eighty_ns_tick_cnt = 0;
  m_config = NULL;
  m_state = NULL;
}

void TmSchMaxRate::init(TmSchConfig *sch_config, TmSchState *sch_state) {
  m_config = sch_config;
  m_state = sch_state;
}

void TmSchMaxRate::pps_tick() {
  RMT_ASSERT(m_config != NULL);
  RMT_ASSERT(m_state != NULL);
  process_pex_bupd();
  // Without this fix change in register
  // configuration will take effect only
  // at 80ns boundary
  update_sch_state();
}

void TmSchMaxRate::process_pex_bupd() {
  pex_bupd_info info;
  int glb_bcnt_adj;

  glb_bcnt_adj = m_config->get_glb_bcnt_adj().value();

  while(pex_sch_bupd_fifo.size()) {

    // Pop from front
    info = pex_sch_bupd_fifo.front();
    pex_sch_bupd_fifo.erase(pex_sch_bupd_fifo.begin());

    // Subtract bytes to the accumulator
    port_acc[info.port] -= info.bytes;
    l1_acc[info.l1] -= info.bytes;
    q_acc[info.qid] -= info.bytes;

    // Subtract Global Bcnt Adj per packet
    if (info.eop) {
      port_acc[info.port] -= glb_bcnt_adj;
      l1_acc[info.l1] -= glb_bcnt_adj;
      q_acc[info.qid] -= glb_bcnt_adj;
    }

    // printf("SCH_%0d_MAX_RATE::Processing Bupd port:%0d l1:%0d q:%0d bytes:%0d adj:%0d acc:%0d\n",
    //	   m_config->get_pipe_index(), info.port, info.l1, info.qid, info.bytes, glb_bcnt_adj, port_acc[info.port]);
  }
}

void TmSchMaxRate::one_ns_tick() {

  // Make sure state & config
  // are not NULL
  RMT_ASSERT(m_config != NULL);
  RMT_ASSERT(m_state != NULL);

  // Increment one ns cnt
  one_ns_tick_cnt++;

  // Every 80ns, check if buckets need to be
  // refreshed and commit bytes to the bucket
  if (!(one_ns_tick_cnt % TmDefs::kSchRefreshIntNs)) {
    // printf("SCH_%0d_MAX_RATE::Processing 80ns tick\n", m_config->get_pipe_index());
    eighty_ns_tick_cnt++;
    sch_bucket_refresh();
    sch_bucket_commit();
    update_sch_state();
  }
}

void TmSchMaxRate::update_sch_state() {
  bool max_rate_enb, rate_pos;

  // Port
  for (int port_pipe_idx=0; port_pipe_idx<TmDefs::kNumPortPerPipe; port_pipe_idx++) {
    max_rate_enb = m_config->get_port_config().max_rate_enb(port_pipe_idx);
    rate_pos = (!max_rate_enb) ? 1 : (port_max_bucket[port_pipe_idx] >= 0);
    m_state->set_port_max_rate_pos(port_pipe_idx, rate_pos);
  }

  // L1
  for (int l1_pipe_idx=0; l1_pipe_idx<TmDefs::kNumL1PerPipe; l1_pipe_idx++) {
    max_rate_enb = m_config->get_l1_config().max_rate_enb(l1_pipe_idx);
    rate_pos = (!max_rate_enb) ? 1 : (l1_max_bucket[l1_pipe_idx] >= 0);
    m_state->set_l1_max_rate_pos(l1_pipe_idx, rate_pos);
  }

  // Q
  for (int q_pipe_idx=0; q_pipe_idx<TmDefs::kNumVqPerPipe; q_pipe_idx++) {
    max_rate_enb = m_config->get_q_config().max_rate_enb(q_pipe_idx);
    rate_pos = (!max_rate_enb) ? 1 : (q_max_bucket[q_pipe_idx] >= 0);
    m_state->set_q_max_rate_pos(q_pipe_idx, rate_pos);
  }
}

uint8_t TmSchMaxRate::get_eighty_ns_tick_mask(uint8_t rate_exp) {
  return ((1 << rate_exp) - 1);
}

uint8_t TmSchMaxRate::get_byte_multiplier(uint8_t rate_exp) {
  return (1 << rate_exp);
}

void TmSchMaxRate::sch_bucket_refresh() {
  sch_port_bucket_refresh();
  sch_l1_bucket_refresh();
  sch_q_bucket_refresh();
}

void TmSchMaxRate::sch_bucket_commit() {
  sch_port_bucket_commit();
  sch_l1_bucket_commit();
  sch_q_bucket_commit();
}

void TmSchMaxRate::sch_port_bucket_refresh() {
  sch_max_lb_static rate_cfg;
  uint8_t rate_exp;
  uint32_t bytes_to_refresh;

  for (int port_pipe_idx=0; port_pipe_idx<TmDefs::kNumPortPerPipe; port_pipe_idx++) {

    // Find out max rate cfg for port
    rate_cfg = m_config->get_port_max_rate_config(port_pipe_idx);

    // Calculate Rate Exp
    rate_exp = (rate_cfg.pps) ? (rate_cfg.rate_exp * 2) : rate_cfg.rate_exp;

    // Calculate Bytes to refresh
    bytes_to_refresh = rate_cfg.rate_mant * get_byte_multiplier(rate_exp);

    // If rate exp is zero, refresh every 80ns
    // else in multiple of 80ns
    // Add bytes to accumulator
    if (rate_exp != 0) {
      if (!(eighty_ns_tick_cnt & get_eighty_ns_tick_mask(rate_exp))) {
	port_acc[port_pipe_idx] += bytes_to_refresh;
      }
    } else {
      port_acc[port_pipe_idx] += bytes_to_refresh;
    }
  }
}

void TmSchMaxRate::sch_l1_bucket_refresh() {
  sch_max_lb_static rate_cfg;
  uint8_t rate_exp;
  uint32_t bytes_to_refresh;

  for (int l1_pipe_idx=0; l1_pipe_idx<TmDefs::kNumL1PerPipe; l1_pipe_idx++) {

    // Find out max rate cfg for port
    rate_cfg = m_config->get_l1_max_rate_config(l1_pipe_idx);

    // Calculate Rate Exp
    rate_exp = (rate_cfg.pps) ? (rate_cfg.rate_exp * 2) : rate_cfg.rate_exp;

    // Calculate Bytes to refresh
    bytes_to_refresh = rate_cfg.rate_mant * get_byte_multiplier(rate_exp);

    // If rate exp is zero, refresh every 80ns
    // else in multiple of 80ns
    // Add bytes to accumulator
    if (rate_exp != 0) {
      if (!(eighty_ns_tick_cnt & get_eighty_ns_tick_mask(rate_exp))) {
	l1_acc[l1_pipe_idx] += bytes_to_refresh;
      }
    } else {
      l1_acc[l1_pipe_idx] += bytes_to_refresh;
    }
  }
}

void TmSchMaxRate::sch_q_bucket_refresh() {
  sch_max_lb_static rate_cfg;
  uint8_t rate_exp;
  uint32_t bytes_to_refresh;

  for (int q_pipe_idx=0; q_pipe_idx<TmDefs::kNumVqPerPipe; q_pipe_idx++) {

    // Find out max rate cfg for port
    rate_cfg = m_config->get_q_max_rate_config(q_pipe_idx);

    // Calculate Rate Exp
    rate_exp = (rate_cfg.pps) ? (rate_cfg.rate_exp * 2) : rate_cfg.rate_exp;

    // Calculate Bytes to refresh
    bytes_to_refresh = rate_cfg.rate_mant * get_byte_multiplier(rate_exp);

    // If rate exp is zero, refresh every 80ns
    // else in multiple of 80ns
    // Add bytes to accumulator
    if (rate_exp != 0) {
      if (!(eighty_ns_tick_cnt & get_eighty_ns_tick_mask(rate_exp))) {
	q_acc[q_pipe_idx] += bytes_to_refresh;
      }
    } else {
      q_acc[q_pipe_idx] += bytes_to_refresh;
    }
  }
}

void TmSchMaxRate::sch_port_bucket_commit() {
  sch_max_lb_static rate_cfg;
  int bucket_size;

  for (int port_pipe_idx=0; port_pipe_idx<TmDefs::kNumPortPerPipe; port_pipe_idx++) {

    // Find out max rate cfg for port
    rate_cfg = m_config->get_port_max_rate_config(port_pipe_idx);

    // Find out max bucket size
    bucket_size = rate_cfg.bs_mant << rate_cfg.bs_exp;

    // Commit the accumulator bytes to the bucket
    port_max_bucket[port_pipe_idx] += port_acc[port_pipe_idx];

    // Clear the accumulator
    port_acc[port_pipe_idx] = 0;

    // When we add bytes to the bucket, we cannot let it
    // increment beyond the bucket size. But when we
    // decrement bytes, we have to let the bucket go as
    // negative as needed since SCH can send more bytes
    // than that in the bucket.
    if (port_max_bucket[port_pipe_idx] > bucket_size)
      port_max_bucket[port_pipe_idx] = bucket_size;
  }
}

void TmSchMaxRate::sch_l1_bucket_commit() {
  sch_max_lb_static rate_cfg;
  int bucket_size;

  for (int l1_pipe_idx=0; l1_pipe_idx<TmDefs::kNumL1PerPipe; l1_pipe_idx++) {

    // Find out max rate cfg for port
    rate_cfg = m_config->get_l1_max_rate_config(l1_pipe_idx);

    // Find out max bucket size
    bucket_size = rate_cfg.bs_mant << rate_cfg.bs_exp;

    // Commit the accumulator bytes to the bucket
    l1_max_bucket[l1_pipe_idx] += l1_acc[l1_pipe_idx];

    // Clear the accumulator
    l1_acc[l1_pipe_idx] = 0;

    // When we add bytes to the bucket, we cannot let it
    // increment beyond the bucket size. But when we
    // decrement bytes, we have to let the bucket go as
    // negative as needed since SCH can send more bytes
    // than that in the bucket.
    if (l1_max_bucket[l1_pipe_idx] > bucket_size)
      l1_max_bucket[l1_pipe_idx] = bucket_size;
  }
}

void TmSchMaxRate::sch_q_bucket_commit() {
  sch_max_lb_static rate_cfg;
  int bucket_size;

  for (int q_pipe_idx=0; q_pipe_idx<TmDefs::kNumVqPerPipe; q_pipe_idx++) {

    // Find out max rate cfg for port
    rate_cfg = m_config->get_q_max_rate_config(q_pipe_idx);

    // Find out max bucket size
    bucket_size = rate_cfg.bs_mant << rate_cfg.bs_exp;

    // Commit the accumulator bytes to the bucket
    q_max_bucket[q_pipe_idx] += q_acc[q_pipe_idx];

    // Clear the accumulator
    q_acc[q_pipe_idx] = 0;

    // When we add bytes to the bucket, we cannot let it
    // increment beyond the bucket size. But when we
    // decrement bytes, we have to let the bucket go as
    // negative as needed since SCH can send more bytes
    // than that in the bucket.
    if (q_max_bucket[q_pipe_idx] > bucket_size)
      q_max_bucket[q_pipe_idx] = bucket_size;
  }
}

}
