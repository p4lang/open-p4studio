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

#ifndef __JBAY_SHARED_TM_SCH_STATE__
#define __JBAY_SHARED_TM_SCH_STATE__
#include <tm-object.h>
#include <tm-defines.h>

namespace MODEL_CHIP_NAMESPACE {

class TmSchState : public TmObject {
 public:
  TmSchState(RmtObjectManager *om, uint8_t pipe_index);
  void set_port_occ(uint8_t port_id, bool value);
  void set_l1_occ(uint16_t l1_id, bool value);
  void set_q_occ(uint16_t q_id, bool value);
  void set_port_max_rate_pos(uint8_t port_id, bool value);
  void set_l1_max_rate_pos(uint16_t l1_id, bool value);
  void set_q_max_rate_pos(uint16_t q_id, bool value);
  void set_l1_min_rate_pos(uint16_t l1_id, bool value);
  void set_q_min_rate_pos(uint16_t q_id, bool value);
  void set_l1_exc_rate_pos(uint16_t l1_id, bool value);
  void set_q_exc_rate_pos(uint16_t q_id, bool value);
  void set_pex_cr_avail(uint8_t port_id, bool value);
  void en_queue_for_pri_prop(uint16_t q_id);
  void dis_queue_for_pri_prop(uint16_t q_id);
  size_t get_num_qid_for_pri_prop();
  uint16_t get_qid_for_pri_prop(uint16_t index);

  bool get_port_occ(uint8_t port_id);
  bool get_l1_occ(uint16_t l1_id);
  bool get_q_occ(uint16_t q_id);
  bool get_port_max_rate_pos(uint8_t port_id);
  bool get_l1_max_rate_pos(uint16_t l1_id);
  bool get_q_max_rate_pos(uint16_t q_id);
  bool get_l1_min_rate_pos(uint16_t l1_id);
  bool get_q_min_rate_pos(uint16_t q_id);
  bool get_l1_exc_rate_pos(uint16_t l1_id);
  bool get_q_exc_rate_pos(uint16_t q_id);
  bool get_pex_cr_avail(uint8_t port_id);
  
 private:
  bool port_occ[TmDefs::kNumPortPerPipe];
  bool l1_occ[TmDefs::kNumL1PerPipe];
  bool q_occ[TmDefs::kNumVqPerPipe];

  bool port_max_rate_pos[TmDefs::kNumPortPerPipe];
  bool l1_max_rate_pos[TmDefs::kNumL1PerPipe];
  bool q_max_rate_pos[TmDefs::kNumVqPerPipe];

  bool l1_min_rate_pos[TmDefs::kNumL1PerPipe];
  bool q_min_rate_pos[TmDefs::kNumVqPerPipe];

  bool l1_exc_rate_pos[TmDefs::kNumL1PerPipe];
  bool q_exc_rate_pos[TmDefs::kNumVqPerPipe];

  bool pex_cr_avail[TmDefs::kNumPortPerPipe];
  std::vector<uint16_t> queues_for_pri_prop;

};

}
#endif
