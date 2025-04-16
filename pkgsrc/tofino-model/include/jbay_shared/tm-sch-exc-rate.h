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

#ifndef __JBAY_SHARED_TM_SCH_EXC_RATE_BLK__
#define __JBAY_SHARED_TM_SCH_EXC_RATE_BLK__
#include <tm-object.h>
#include <tm-defines.h>
#include <tm-structs.h>
#include <tm-sch-config.h>
#include <tm-sch-state.h>
#include <vector>

namespace MODEL_CHIP_NAMESPACE {

class TmSchExcRate : public TmObject {

 public:
  TmSchExcRate(RmtObjectManager *om, uint8_t pipe_index);
  void pps_tick();
  void one_ns_tick();
  void init(TmSchConfig *sch_config, TmSchState *sch_state);

  // PEX Bupd Fifo
  std::vector<pex_bupd_info> pex_sch_bupd_fifo;

 private:
  // Tick Counters
  uint32_t one_ns_tick_cnt, eighty_ns_tick_cnt;

  // Config & State
  TmSchConfig *m_config;
  TmSchState *m_state;

  // L1 per Port
  // Used for refresh calculations
  std::vector<uint16_t> l1_id_per_port[TmDefs::kNumPortPerPipe];
  std::vector<uint16_t> q_id_per_l1[TmDefs::kNumL1PerPipe];

  // Accumulators
  int l1_acc[TmDefs::kNumL1PerPipe];
  int q_acc[TmDefs::kNumVqPerPipe];

  // Buckets
  // Int has a range of +- 2 Billion,
  // exc bucket size is 1M so plenty
  // of space
  int l1_exc_bucket[TmDefs::kNumL1PerPipe];
  int q_exc_bucket[TmDefs::kNumVqPerPipe];

  // Functions
  void process_pex_bupd();
  void sch_bucket_commit();
  void sch_l1_acc_commit();
  void sch_q_acc_commit();
  void sch_l1_bucket_adjust();
  void sch_q_bucket_adjust();
  void update_sch_state();
  void populate_l1_id_per_port();
  bool check_port_l1_pos(uint8_t port_id);
  void populate_q_id_per_l1();
  bool check_l1_q_pos(uint16_t l1_id);

};

}
#endif
