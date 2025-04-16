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
#include <tm-sch-occ.h>
#include <tm-sch-config.h>
#include <tm-sch-state.h>
#include <tm-sch.h>

namespace MODEL_CHIP_NAMESPACE {

TmSch::TmSch(RmtObjectManager *om, uint8_t pipe_index) :
  TmObject(om, pipe_index),
  m_config(om, pipe_index),
  m_state(om, pipe_index),
  m_occ(om, pipe_index),
  m_max_rate(om, pipe_index),
  m_min_rate(om, pipe_index),
  m_exc_rate(om, pipe_index),
  m_pex_cr(om, pipe_index),
  m_sel(om, pipe_index)
{
  init(&m_config, &m_state);
}

void TmSch::init(TmSchConfig *sch_config, TmSchState *sch_state) {
  m_occ.init(sch_config, sch_state);
  m_min_rate.init(sch_config, sch_state);
  m_max_rate.init(sch_config, sch_state);
  m_exc_rate.init(sch_config, sch_state);
  m_pex_cr.init(sch_config, sch_state);
  m_sel.init(sch_config, sch_state);
}

TmSchConfig* TmSch::get_sch_config() {
  return(&m_config);
}

void TmSch::pps_tick() {

  // Tick sub blocks
  m_occ.pps_tick();
  m_pex_cr.pps_tick();
  m_min_rate.pps_tick();
  m_max_rate.pps_tick();
  m_exc_rate.pps_tick();
  m_sel.pps_tick();

  // Schedule a packet
  if (m_sel.sch_winner_fifo.size()) {

    sch_qlc_fifo.push_back(m_sel.sch_winner_fifo.front());

    // Update Occupacny
    m_occ.deq_fifo.push_back(m_sel.sch_winner_fifo.front());

    // Update PEX Credit
    m_pex_cr.sch_qlc_fifo.push_back(m_sel.sch_winner_fifo.front());

    // Delete Packet
    m_sel.sch_winner_fifo.erase(m_sel.sch_winner_fifo.begin());
  }

  // Move Data
  move_data();
}

void TmSch::one_ns_tick() {

  // Tick sub blocks
  m_min_rate.one_ns_tick();
  m_max_rate.one_ns_tick();
  m_exc_rate.one_ns_tick();

}

void TmSch::move_data() {

  // QLC to SCH OCC
  for(int i=0; i<TmDefs::kNumQlcSchUpdIfc; i++) {
    if (qlc_sch_fifo[i].size()) {
      if (i == (TmDefs::kNumQlcSchUpdIfc-1)) {
	m_occ.deq_upd_fifo.push_back(qlc_sch_fifo[i].front());
	qlc_sch_fifo[i].erase(qlc_sch_fifo[i].begin());
      } else {
	m_occ.enq_upd_fifo.push_back(qlc_sch_fifo[i].front());
	qlc_sch_fifo[i].erase(qlc_sch_fifo[i].begin());
      }
    }
  }

  // PEX to SCH BUPD
  for(int i=0; i<TmDefs::kNumPexSchByteUpdIfc; i++) {
    if (pex_sch_bupd_fifo[i].size()) {
      m_max_rate.pex_sch_bupd_fifo.push_back(pex_sch_bupd_fifo[i].front());
      m_min_rate.pex_sch_bupd_fifo.push_back(pex_sch_bupd_fifo[i].front());
      m_exc_rate.pex_sch_bupd_fifo.push_back(pex_sch_bupd_fifo[i].front());
      pex_sch_bupd_fifo[i].erase(pex_sch_bupd_fifo[i].begin());
    }
  }

  // PEX to SCH CRD
  for(int i=0; i<TmDefs::kNumPexSchCrdUpdIfc; i++) {
    if (pex_sch_crd_fifo[i].size()) {
      m_pex_cr.pex_sch_fifo.push_back(pex_sch_crd_fifo[i].front());
      pex_sch_crd_fifo[i].erase(pex_sch_crd_fifo[i].begin());
    }
  }
}

}
