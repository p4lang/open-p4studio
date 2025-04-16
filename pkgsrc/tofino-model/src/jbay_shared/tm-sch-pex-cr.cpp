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
#include <tm-sch-pex-cr.h>
#include <cstring>

namespace MODEL_CHIP_NAMESPACE {

TmSchPexCr::TmSchPexCr(RmtObjectManager *om, uint8_t pipe_index) : TmObject(om, pipe_index) {
  memset(pex_cr_cnt, 0, TmDefs::kNumPriPerPipe * sizeof(pex_cr_cnt[0]));
  m_config = NULL;
  m_state = NULL;
}

uint8_t TmSchPexCr::get_pex_cr_cnt(uint8_t port_id) {
  uint8_t port_mode;

  // Figure out port mode
  port_mode = m_config->get_port_config().port_speed_mode(port_id);

  // Return credit based on speed mode
  switch(port_mode) {
  case 7:
    // 400G
    return(32);
    break;
  case 6:
    // 200G
    return(16);
    break;
  case 5:
    // 100G
    return(8);
    break;
  default:
    // 50G and lower
    return(4);
    break;
  }
}

void TmSchPexCr::init(TmSchConfig *sch_cfg, TmSchState *sch_state) {
  m_config = sch_cfg;
  m_state = sch_state;
}


void TmSchPexCr::pps_tick() {
  RMT_ASSERT(m_config != NULL);
  RMT_ASSERT(m_state != NULL);
  incr_sch_credit();
  decr_sch_credit();
  process_pfc();
  update_sch_state();
}

void TmSchPexCr::incr_sch_credit() {
  sch_deq_info info;

  while(sch_qlc_fifo.size()) {

    // Pop First Element
    info = sch_qlc_fifo.front();
    sch_qlc_fifo.erase(sch_qlc_fifo.begin());

    // Consume PEX Credit
    pex_cr_cnt[(info.port * TmDefs::kNumPriPerPort) + info.ecos]++;

    // printf("SCH_%0d_PEX_CR::incr_sch_credit port:%0d ecos:%0d credit:%0d\n",
    //   m_config->get_pipe_index(), info.port, info.ecos, pex_cr_cnt[(info.port * TmDefs::kNumPriPerPort) + info.ecos]);

    // Checks
    RMT_ASSERT(pex_cr_cnt[(info.port * TmDefs::kNumPriPerPort) + info.ecos] <= get_pex_cr_cnt(info.port));
  }
}

void TmSchPexCr::decr_sch_credit() {
  pex_crd_info info;

  while(pex_sch_fifo.size()) {

    // Pop First Element
    info = pex_sch_fifo.front();
    pex_sch_fifo.erase(pex_sch_fifo.begin());

    // Consume PEX Credit
    pex_cr_cnt[(info.port * TmDefs::kNumPriPerPort) + info.ecos]--;

    // Debug
    // printf("SCH_%0d_PEX_CR::decr_sch_credit port:%0d ecos:%0d credit:%0d\n",
    //	   m_config->get_pipe_index(), info.port, info.ecos, pex_cr_cnt[(info.port * TmDefs::kNumPriPerPort) + info.ecos]);
  }
}

void TmSchPexCr::process_pfc() {
  // TO-DO
  pfc = 0;
}

bool TmSchPexCr::check_pex_cr_avail(uint8_t port_id) {
  uint8_t init_cr_val;
  uint16_t cr_used = 0;

  // Find out initial credit loaded
  init_cr_val = get_pex_cr_cnt(port_id);

  // Find out used credit across all priorities
  for(int pri_idx=0; pri_idx < TmDefs::kNumPriPerPort; pri_idx++) {
    if (!(pfc & (1 << pri_idx)))
      cr_used += pex_cr_cnt[(port_id * TmDefs::kNumPriPerPort) + pri_idx];
  }
  return((cr_used < init_cr_val));
}

void TmSchPexCr::update_sch_state() {
  // Update credit availabilty

  for (int port_idx=0; port_idx < TmDefs::kNumPortPerPipe; port_idx++) {
    m_state->set_pex_cr_avail(port_idx, check_pex_cr_avail(port_idx));
  }
}

}
