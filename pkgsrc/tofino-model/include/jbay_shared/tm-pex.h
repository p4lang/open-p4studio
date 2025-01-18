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

#ifndef __JBAY_SHARED_TM_PEX_BLK__
#define __JBAY_SHARED_TM_PEX_BLK__
#include <tm-object.h>
#include <tm-defines.h>
#include <tm-structs.h>
#include <vector>
#include <rate-lim.h>

namespace MODEL_CHIP_NAMESPACE {

  class TmPex : public TmObject {

 public:
  TmPex(RmtObjectManager *om, uint8_t pipe_index);
  void pps_tick();

  // PSC->PEX Fifo
  std::vector<psc_pex_enq_info> psc_pex_enq_fifo;

  // PEX->SCH Bupd Fifos
  std::vector<pex_bupd_info> pex_sch_bupd_fifo[TmDefs::kNumPexSchCrdUpdIfc];

  // PEX->SCH Credit Fifos
  std::vector<pex_crd_info> pex_sch_crd_fifo[TmDefs::kNumPexSchCrdUpdIfc];

 private:
  // Port Rate Limiters
  RateLim m_rl[TmDefs::kNumPortPerPipe];

  // Cellified Vectors
  std::vector<pex_bupd_info> pex_port_cell_fifo[TmDefs::kNumPortPerPipe];

  // Functions
  void convert_pkt_to_cells();
  void commit_cells_to_sch();
  void drain_port_rl();
  uint8_t port_ptr;
  uint8_t num_commits_to_sch;
  bool prev_upd_eop[TmDefs::kNumPortPerPipe];

};

}
#endif
