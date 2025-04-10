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
#include <tm-sch-model-wrapper-defines.h>
#include <tm-pex.h>

namespace MODEL_CHIP_NAMESPACE {

  TmPex::TmPex(RmtObjectManager *om, uint8_t pipe_index) : TmObject(om, pipe_index) {

  for (int port_idx = 0; port_idx < TmDefs::kNumPortPerPipe; port_idx++) {
    // Set up Port Rate Limiters
    m_rl[port_idx].set_port_rate(400);
    m_rl[port_idx].set_rl_id((char *)"TmPex", port_idx);
    m_rl[port_idx].set_drain_time(SYS_CLK_PERIOD_NS);
    m_rl[port_idx].set_drain_params();
    prev_upd_eop[port_idx] = 1;
  }
  port_ptr = 0;
  num_commits_to_sch = 0;
}

void TmPex::pps_tick() {
  convert_pkt_to_cells();
  commit_cells_to_sch();
  drain_port_rl();
}

void TmPex::convert_pkt_to_cells() {
  uint16_t ccnt, bcnt;
  psc_pex_enq_info enq_info;
  pex_bupd_info bupd_info;


  while (psc_pex_enq_fifo.size()) {

    // Pop first element
    enq_info = psc_pex_enq_fifo.front();
    psc_pex_enq_fifo.erase(psc_pex_enq_fifo.begin());

    // Calculate Ccnt & Bcnt
    ccnt = ((enq_info.pkt_size + TmDefs::kTmCellSize - 1) / TmDefs::kTmCellSize);
    bcnt = (enq_info.pkt_size - ((ccnt - 1) * TmDefs::kTmCellSize));

    RMT_LOG(RmtDebug::verbose(), "PEX_%0d::port:%0d pkt_size:%0d ccnt:%0d bcnt:%0d\n",
	    get_tm_index(), enq_info.port, enq_info.pkt_size, ccnt, bcnt);

    // Populate Bupd structure
    bupd_info.port = enq_info.port;
    bupd_info.l1 = enq_info.l1;
    bupd_info.qid = enq_info.qid;
    bupd_info.min_pkt = enq_info.min_pkt;
    bupd_info.ecos = enq_info.ecos;

    // Split the packet into
    // 3 cell chunks
    while (ccnt) {
      if (ccnt > 3) {
	bupd_info.bytes = 3 * TmDefs::kTmCellSize;
	bupd_info.eop = 0;
	ccnt -= 3;
      } else {
	bupd_info.eop = 1;
	bupd_info.bytes = ((ccnt - 1) * TmDefs::kTmCellSize) + bcnt;
	ccnt = 0;
      }
      // Push into port cell queue
      RMT_LOG(RmtDebug::verbose(), "PEX_%0d::Cellifying port:%0d bytes:%0d\n",
	      get_tm_index(), bupd_info.port, bupd_info.bytes);
      pex_port_cell_fifo[enq_info.port].push_back(bupd_info);
    }
  }
}

void TmPex::commit_cells_to_sch() {
  uint8_t this_upd_sop;
  pex_bupd_info bupd_info;
  pex_crd_info crd_info;

  // Clear State every clock
  num_commits_to_sch = 0;

  for(int port_idx = 0; port_idx < TmDefs::kNumPortPerPipe; port_idx++) {

    // Read out cells from a port until port is empty
    // or you are over the rate
    if ((pex_port_cell_fifo[port_ptr].size()) && (m_rl[port_ptr].check_rl_xoff() == 0)) {

      // Pop first element
      bupd_info = pex_port_cell_fifo[port_ptr].front();
      pex_port_cell_fifo[port_ptr].erase(pex_port_cell_fifo[port_ptr].begin());

      // Figure out if this is an SOP peice of the packet
      if (prev_upd_eop[port_ptr])
        this_upd_sop = 1;
      else
        this_upd_sop = 0;

      // Save Eop State
      prev_upd_eop[port_ptr] = bupd_info.eop;

      // Commit bytes to the RL
      m_rl[port_ptr].process_line_sent(this_upd_sop, bupd_info.eop, bupd_info.bytes);

      // Send bupd to SCH
      pex_sch_bupd_fifo[num_commits_to_sch].push_back(bupd_info);

      // Send PEX credit to SCH on packet sop
      if (this_upd_sop) {
        crd_info.port = bupd_info.port;
        crd_info.ecos = bupd_info.ecos;
	    pex_sch_crd_fifo[num_commits_to_sch].push_back(crd_info);
      }

      // Max two updates per cycle
      num_commits_to_sch++;
      if (num_commits_to_sch >= TmDefs::kNumPexSchCrdUpdIfc)
	    break;

    } else {
      // Increment port ptr
      port_ptr = ((port_ptr + 1) % (TmDefs::kNumPortPerPipe));
    }
  }
}

void TmPex::drain_port_rl() {
  for(int port_idx = 0; port_idx < TmDefs::kNumPortPerPipe; port_idx++) {
    // Drain the Queue RL every clock
    m_rl[port_idx].drain_from_fifo();
  }
}

}
