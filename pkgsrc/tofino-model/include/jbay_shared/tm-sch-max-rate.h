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

#ifndef __JBAY_SHARED_TM_SCH_MAX_RATE_BLK__
#define __JBAY_SHARED_TM_SCH_MAX_RATE_BLK__
#include <tm-object.h>
#include <tm-defines.h>
#include <tm-structs.h>
#include <tm-sch-config.h>
#include <tm-sch-state.h>
#include <vector>

namespace MODEL_CHIP_NAMESPACE {

class TmSchMaxRate : public TmObject {

 public:
  TmSchMaxRate(RmtObjectManager *om, uint8_t pipe_index);
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

  // Accumulators
  int port_acc[TmDefs::kNumPortPerPipe];
  int l1_acc[TmDefs::kNumL1PerPipe];
  int q_acc[TmDefs::kNumVqPerPipe];

  // Buckets
  // Int has a range of +- 2 Billion,
  // max bucket size is 8M so plenty
  // of space
  int port_max_bucket[TmDefs::kNumPortPerPipe];
  int l1_max_bucket[TmDefs::kNumL1PerPipe];
  int q_max_bucket[TmDefs::kNumVqPerPipe];

  // Functions
  void process_pex_bupd();
  void sch_bucket_refresh();
  void sch_port_bucket_refresh();
  void sch_l1_bucket_refresh();
  void sch_q_bucket_refresh();
  void sch_bucket_commit();
  void sch_port_bucket_commit();
  void sch_l1_bucket_commit();
  void sch_q_bucket_commit();
  void update_sch_state();
  uint8_t get_eighty_ns_tick_mask(uint8_t rate_exp);
  uint8_t get_byte_multiplier(uint8_t rate_exp);

};

}
#endif
