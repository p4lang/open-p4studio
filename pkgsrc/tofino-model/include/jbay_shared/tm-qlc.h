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

#ifndef __JBAY_SHARED_TM_QLC_BLK__
#define __JBAY_SHARED_TM_QLC_BLK__
#include <tm-object.h>
#include <tm-defines.h>
#include <tm-structs.h>
#include <rate-lim.h>
#include <vector>
#include <algorithm>

namespace MODEL_CHIP_NAMESPACE {

class TmQlc : public TmObject {

 public:
  // QLC->SCH Fifo
  std::vector<sch_deq_info> qlc_sch_fifo[TmDefs::kNumQlcSchUpdIfc];

  // SCH->QLC Fifo
  std::vector<sch_deq_info> sch_qlc_fifo;

  // Packet Size to be generated
  uint16_t packet_size_gen[TmDefs::kNumVqPerPipe];

  // Packet size sent for VQ
  // Top level will supply this
  // info to PEX for draining
  std::vector<uint16_t> packet_size[TmDefs::kNumVqPerPipe];

  // Constructor
  TmQlc(RmtObjectManager *om, uint8_t pipe_index);
  void pps_tick();
  void set_active_vq_in_mac(int mac, bool vq_bmp[TmDefs::kNumVqPerMac]);
  void set_packet_size_gen(int vq_idx, int packet_size);

 private:
  // Rate limiter per queue
  RateLim m_rl[TmDefs::kNumVqPerPipe];

  // # of active VQ in each MAC
  std::vector<uint8_t> active_vq_in_mac[TmDefs::kNumMacPerPipe];

  // # of PQ per VQ in each MAC
  uint8_t pq_per_vq_in_mac[TmDefs::kNumMacPerPipe];

  // # of packets admitted per PQ
  // Note: For all the below arrays
  // # of indexes in array tie to the
  // type of index to be used in the logic
  uint32_t pq_cnt_table[TmDefs::kNumPqPerPipe];

  // PQ pointer for enqueue
  uint8_t pq_enq_ptr[TmDefs::kNumVqPerPipe];

  // PQ pointer for dequeue
  uint8_t pq_deq_ptr[TmDefs::kNumVqPerPipe];

  // empty->non empty
  bool pq_enq_update_pending[TmDefs::kNumVqPerPipe];

  // non-empty at grant
  bool pq_deq_update_pending[TmDefs::kNumVqPerPipe];

  // Functions
  void qlc_enq();
  void qlc_deq();
  uint8_t map_vq_to_pq_idx(uint8_t mac, uint8_t vq);

};

}

#endif
