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
#include <tm-sch-state.h>

namespace MODEL_CHIP_NAMESPACE {

TmSchState::TmSchState(RmtObjectManager *om, uint8_t pipe_index) : TmObject(om, pipe_index) {
  memset(port_occ, 0, TmDefs::kNumPortPerPipe*sizeof(bool));
  memset(l1_occ, 0, TmDefs::kNumL1PerPipe*sizeof(bool));
  memset(q_occ, 0, TmDefs::kNumVqPerPipe*sizeof(bool));

  memset(port_max_rate_pos, 1, TmDefs::kNumPortPerPipe*sizeof(bool));
  memset(l1_max_rate_pos, 1, TmDefs::kNumL1PerPipe*sizeof(bool));
  memset(q_max_rate_pos, 1, TmDefs::kNumVqPerPipe*sizeof(bool));

  memset(l1_min_rate_pos, 1, TmDefs::kNumL1PerPipe*sizeof(bool));
  memset(q_min_rate_pos, 1, TmDefs::kNumVqPerPipe*sizeof(bool));

  memset(l1_exc_rate_pos, 1, TmDefs::kNumL1PerPipe*sizeof(bool));
  memset(q_exc_rate_pos, 1, TmDefs::kNumVqPerPipe*sizeof(bool));

  memset(pex_cr_avail, 1, TmDefs::kNumPortPerPipe*sizeof(bool));
}

void TmSchState::set_port_occ(uint8_t port_id, bool value) {
  port_occ[port_id] = value;
}

void TmSchState::set_l1_occ(uint16_t l1_id, bool value) {
  l1_occ[l1_id] = value;
}

void TmSchState::set_q_occ(uint16_t q_id, bool value) {
   q_occ[q_id] = value;
}

bool TmSchState::get_port_occ(uint8_t port_id) {
  return(port_occ[port_id]);
}

bool TmSchState::get_l1_occ(uint16_t l1_id) {
  return(l1_occ[l1_id]);
}

bool TmSchState::get_q_occ(uint16_t q_id) {
  return(q_occ[q_id]);
}

void TmSchState::set_port_max_rate_pos(uint8_t port_id, bool value) {
  port_max_rate_pos[port_id] = value;
}

void TmSchState::set_l1_max_rate_pos(uint16_t l1_id, bool value) {
  l1_max_rate_pos[l1_id] = value;
}

void TmSchState::set_q_max_rate_pos(uint16_t q_id, bool value) {
   q_max_rate_pos[q_id] = value;
}

void TmSchState::set_l1_min_rate_pos(uint16_t l1_id, bool value) {
  l1_min_rate_pos[l1_id] = value;
}

void TmSchState::set_q_min_rate_pos(uint16_t q_id, bool value) {
  q_min_rate_pos[q_id] = value;
}

void TmSchState::set_l1_exc_rate_pos(uint16_t l1_id, bool value) {
  l1_exc_rate_pos[l1_id] = value;
}

void TmSchState::set_q_exc_rate_pos(uint16_t q_id, bool value) {
  q_exc_rate_pos[q_id] = value;
}

bool TmSchState::get_port_max_rate_pos(uint8_t port_id) {
  return(port_max_rate_pos[port_id]);
}

bool TmSchState::get_l1_max_rate_pos(uint16_t l1_id) {
  return(l1_max_rate_pos[l1_id]);
}

bool TmSchState::get_q_max_rate_pos(uint16_t q_id) {
  return(q_max_rate_pos[q_id]);
}

bool TmSchState::get_l1_min_rate_pos(uint16_t l1_id) {
  return(l1_min_rate_pos[l1_id]);
}

bool TmSchState::get_q_min_rate_pos(uint16_t q_id) {
  return(q_min_rate_pos[q_id]);
}

bool TmSchState::get_l1_exc_rate_pos(uint16_t l1_id) {
  return(l1_exc_rate_pos[l1_id]);
}

bool TmSchState::get_q_exc_rate_pos(uint16_t q_id) {
  return(q_exc_rate_pos[q_id]);
}

void TmSchState::set_pex_cr_avail(uint8_t port_id, bool value) {
  pex_cr_avail[port_id] = value;
}

bool TmSchState::get_pex_cr_avail(uint8_t port_id) {
  return (pex_cr_avail[port_id]);
}

void TmSchState::en_queue_for_pri_prop(uint16_t q_id) {
  // Add if QId does not exist in vector
  if (std::find(queues_for_pri_prop.begin(), queues_for_pri_prop.end(), q_id) == queues_for_pri_prop.end()) {
    queues_for_pri_prop.push_back(q_id);
  }
}

void TmSchState::dis_queue_for_pri_prop(uint16_t q_id) {
  // Search QId and delete
  queues_for_pri_prop.erase(std::remove(queues_for_pri_prop.begin(), queues_for_pri_prop.end(), q_id), queues_for_pri_prop.end());
}

size_t TmSchState::get_num_qid_for_pri_prop() {
  return(queues_for_pri_prop.size());
}

uint16_t TmSchState::get_qid_for_pri_prop(uint16_t index) {
  return(queues_for_pri_prop[index]);
}

}
