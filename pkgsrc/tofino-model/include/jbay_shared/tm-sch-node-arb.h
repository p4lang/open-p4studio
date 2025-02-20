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

#ifndef __JBAY_SHARED_TM_SCH_NODE_ARB__
#define __JBAY_SHARED_TM_SCH_NODE_ARB__
#include <tm-object.h>
#include <tm-defines.h>

namespace MODEL_CHIP_NAMESPACE {

template <uint8_t NUM_NODE_PER_MAC>
class TmSchNodeArb : TmObject {
 public:
  TmSchNodeArb(RmtObjectManager *om, uint8_t pipe_index);
  void set_node_min_rdy(uint8_t node_id, bool rdy);
  void set_node_min_pri(uint8_t node_id, uint8_t pri);
  void set_node_max_rdy(uint8_t node_id, bool rdy);
  void set_node_max_pri(uint8_t node_id, uint8_t pri);
  uint8_t do_sp_max_node_selection();
  uint8_t do_sp_min_node_selection();
  uint8_t search_max_pri_winner(uint8_t which_pri);
  uint8_t search_min_pri_winner(uint8_t which_pri);

 private:
  // Winner of last cycle
  uint8_t prev_winner;

  // Data Structures
  bool node_min_rdy[NUM_NODE_PER_MAC];
  uint8_t node_min_pri[NUM_NODE_PER_MAC];
  bool node_max_rdy[NUM_NODE_PER_MAC];
  uint8_t node_max_pri[NUM_NODE_PER_MAC];

  // Node Pointers
  uint8_t min_pri_ptr[TmDefs::kNumPriPerPort];
  uint8_t max_pri_ptr[TmDefs::kNumPriPerPort];

};

}
#endif
