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

#ifndef __JBAY_SHARED_TM_SCH_SEL_BLK__
#define __JBAY_SHARED_TM_SCH_SEL_BLK__
#include <tm-object.h>
#include <vector>
#include <tm-sch-port-arb.h>
#include <tm-sch-node-arb.h>
#include <tm-sch-state.h>
#include <tm-sch-config.h>
#include <tm-structs.h>
#include <tm-defines.h>

namespace MODEL_CHIP_NAMESPACE {

class TmSchSel : public TmObject {
 public:
  TmSchSel(RmtObjectManager *om, uint8_t pipe_index);
  void init(TmSchConfig *sch_cfg, TmSchState *sch_state);
  void pps_tick();

  std::vector<sch_deq_info> sch_winner_fifo;

 private:
  // Variables
  TmSchState *m_state;
  TmSchConfig *m_config;
  TmSchPortArb m_prt_arb;
  TmSchNodeArb<TmDefs::kNumL1PerMac> m_l1_arb;
  TmSchNodeArb<TmDefs::kNumVqPerMac> m_q_arb;
  uint8_t min_pkt_sel;
  uint8_t max_pri_prop_q_l1[TmDefs::kNumL1PerPipe];
  uint8_t min_pri_prop_q_l1[TmDefs::kNumL1PerPipe];

  // Functions
  void port_sel();
  void l1_sel(sch_deq_info info);
  void q_sel(sch_deq_info info);
  void propagate_sch_q_pri();
  uint8_t pick_l1_max_rate_pri(uint16_t l1_pipe_idx);
  uint8_t pick_l1_min_rate_pri(uint16_t l1_pipe_idx);

};

}
#endif
