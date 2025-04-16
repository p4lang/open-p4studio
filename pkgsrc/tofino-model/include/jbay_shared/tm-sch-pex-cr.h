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

#ifndef __JBAY_SHARED_TM_SCH_PEX_CR_BLK__
#define __JBAY_SHARED_TM_SCH_PEX_CR_BLK__
#include <tm-defines.h>
#include <tm-sch-config.h>
#include <tm-sch-state.h>
#include <vector>

namespace MODEL_CHIP_NAMESPACE {

class TmSchPexCr : public TmObject {
public:
  TmSchPexCr(RmtObjectManager *om, uint8_t pipe_index);
  std::vector<sch_deq_info> sch_qlc_fifo;
  std::vector<pex_crd_info> pex_sch_fifo;
  void init(TmSchConfig *sch_config, TmSchState *sch_state);
  void pps_tick();

private:
  // SCH Configuration
  TmSchConfig *m_config;

  // SCH State
  TmSchState *m_state;

  // PEX Credit Count
  uint8_t pex_cr_cnt[TmDefs::kNumPriPerPipe];

  // Current PFC State
  uint8_t pfc{0};

  // Functions
  void decr_sch_credit();
  void incr_sch_credit();
  void update_sch_state();
  uint8_t get_pex_cr_cnt(uint8_t port_id);
  bool check_pex_cr_avail(uint8_t port_id);
  void process_pfc();
};

}
#endif
