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

#ifndef __JBAY_SHARED_TM_SCH_OCC_BLK__
#define __JBAY_SHARED_TM_SCH_OCC_BLK__
#include <tm-object.h>
#include <tm-defines.h>
#include <tm-structs.h>
#include <tm-sch-config.h>
#include <tm-sch-state.h>
#include <vector>

namespace MODEL_CHIP_NAMESPACE {

class TmSchOcc : public TmObject {

 public:
  // Enqueue Updates from QLC
  std::vector<sch_deq_info> enq_upd_fifo;

  // Dequeue Updates from QLC
  std::vector<sch_deq_info> deq_upd_fifo;

  // SCH Dequeue to QLC
  std::vector<sch_deq_info> deq_fifo;

  // Constructor
  TmSchOcc(RmtObjectManager *om, uint8_t pipe_index);
  void pps_tick();
  void init(TmSchConfig *sch_cfg, TmSchState *sch_state);

 private:
  // SCH Config
  TmSchConfig *m_config;

  // SCH State
  TmSchState *m_state;

  // OCC Cnt Per Q
  uint8_t q_occ_cnt[TmDefs::kNumVqPerPipe];

  // OCC Cnt Per L1
  uint8_t l1_occ_cnt[TmDefs::kNumL1PerPipe];

  // OCC Cnt Per Port
  uint8_t port_occ_cnt[TmDefs::kNumPortPerPipe];

  // Functions
  void qlc_sch_enq_update();
  void qlc_sch_deq_update();
  void sch_qlc_deq();
  void update_sch_state(int port_pipe_idx, int l1_pipe_idx, int q_pipe_idx);
};

}
#endif
