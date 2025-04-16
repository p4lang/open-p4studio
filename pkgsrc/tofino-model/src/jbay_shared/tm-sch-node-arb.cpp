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
#include <tm-sch-node-arb.h>
#include <cstring>

namespace MODEL_CHIP_NAMESPACE {

template <uint8_t NUM_NODE_PER_MAC>
TmSchNodeArb<NUM_NODE_PER_MAC>::TmSchNodeArb(RmtObjectManager *om, uint8_t pipe_index) : TmObject(om, pipe_index) {
  // No Winner
  prev_winner = 0xff;

  // Zero out arrays
  memset(node_max_rdy, 0, NUM_NODE_PER_MAC*sizeof(bool));
  memset(node_max_pri, 0, sizeof(node_max_pri));
  memset(node_min_rdy, 0, NUM_NODE_PER_MAC*sizeof(bool));
  memset(node_min_pri, 0, sizeof(node_min_pri));

  // Init Ptrs
  memset(min_pri_ptr, 0, sizeof(min_pri_ptr));
  memset(max_pri_ptr, 0, sizeof(max_pri_ptr));
}

template <uint8_t NUM_NODE_PER_MAC>
void TmSchNodeArb<NUM_NODE_PER_MAC>::set_node_max_rdy(uint8_t node_id, bool rdy) {
  node_max_rdy[node_id] = rdy;
}

template <uint8_t NUM_NODE_PER_MAC>
void TmSchNodeArb<NUM_NODE_PER_MAC>::set_node_max_pri(uint8_t node_id, uint8_t pri) {
  node_max_pri[node_id] = pri;
}

template <uint8_t NUM_NODE_PER_MAC>
void TmSchNodeArb<NUM_NODE_PER_MAC>::set_node_min_rdy(uint8_t node_id, bool rdy) {
  node_min_rdy[node_id] = rdy;
}

template <uint8_t NUM_NODE_PER_MAC>
void TmSchNodeArb<NUM_NODE_PER_MAC>::set_node_min_pri(uint8_t node_id, uint8_t pri) {
  node_min_pri[node_id] = pri;
}

template <uint8_t NUM_NODE_PER_MAC>
uint8_t TmSchNodeArb<NUM_NODE_PER_MAC>::search_max_pri_winner(uint8_t which_pri) {
  // Strict Priority
  // RR between nodes of same priority
  // We have to maintain a node pointer
  // per priority to ensure fairness of
  // RR between nodes of same priority
  uint8_t temp_winner = 0xff, node_idx;

  for(uint8_t node = 0; node < NUM_NODE_PER_MAC; node++) {

    // Assign NODE index
    node_idx = max_pri_ptr[which_pri];

    // Move Ptr to next node
    max_pri_ptr[which_pri] = ((max_pri_ptr[which_pri] + 1) % NUM_NODE_PER_MAC);

    if (node_max_rdy[node_idx] && (node_max_pri[node_idx] == which_pri)) {

      // Assign a temp winner
      temp_winner = node_idx;

      // Same Node cannot be scheduled
      // in back to back cycles
      if (temp_winner != prev_winner) {
	break;
      } else {
	temp_winner = 0xff;
      }
    }
  }
  return(temp_winner);
}

template <uint8_t NUM_NODE_PER_MAC>
uint8_t TmSchNodeArb<NUM_NODE_PER_MAC>::do_sp_max_node_selection() {

  uint8_t final_winner, which_pri;

  // Search for a NODE winner in all 8 priorities
  for (uint8_t pri = 0; pri < TmDefs::kNumPriPerPort; pri++) {
    which_pri = ((~pri) & 7);
    final_winner = search_max_pri_winner(which_pri);

    // Break out of the loop
    // if any priority has a winner
    if (final_winner != 0xff)
      break;
  }

  // Set prev winner for next clk
  //prev_winner = final_winner;

  // Return winner
  return(final_winner);
}

template <uint8_t NUM_NODE_PER_MAC>
uint8_t TmSchNodeArb<NUM_NODE_PER_MAC>::search_min_pri_winner(uint8_t which_pri) {
  // Strict Priority
  // RR between nodes of same priority
  // We have to maintain a node pointer
  // per priority to ensure fairness of
  // RR between nodes of same priority
  uint8_t temp_winner = 0xff, node_idx;

  for(uint8_t node = 0; node < NUM_NODE_PER_MAC; node++) {

    // Assign Node index
    node_idx = min_pri_ptr[which_pri];

    // Move Ptr to next node
    min_pri_ptr[which_pri] = ((min_pri_ptr[which_pri] + 1) % NUM_NODE_PER_MAC);

    if (node_min_rdy[node_idx] && (node_min_pri[node_idx] == which_pri)) {

      // Assign a temp winner
      temp_winner = node_idx;

      // Same Node cannot be scheduled
      // in back to back cycles
      if (temp_winner != prev_winner) {
	break;
      } else {
	temp_winner = 0xff;
      }
    }
  }
  return(temp_winner);
}

template <uint8_t NUM_NODE_PER_MAC>
uint8_t TmSchNodeArb<NUM_NODE_PER_MAC>::do_sp_min_node_selection() {

  uint8_t final_winner, which_pri;

  // Search for a Node winner in all 8 priorities
  for (int pri = 0; pri < TmDefs::kNumPriPerPort; pri++) {
    which_pri = ((~pri) & 7);
    final_winner = search_min_pri_winner(which_pri);

    // Break out of the loop
    // if any priority has a winner
    if (final_winner != 0xff)
      break;
  }

  // Set prev winner for next clk
  //prev_winner = final_winner;

  // Return winner
  return(final_winner);
}

template class TmSchNodeArb<TmDefs::kNumL1PerMac>;
template class TmSchNodeArb<TmDefs::kNumVqPerMac>;

}
