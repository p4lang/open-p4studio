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

#include <utests/test_rmt_packet_coordinator.h>
#include <register_includes/reg.h>

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;

namespace MODEL_CHIP_TEST_NAMESPACE {

using namespace MODEL_CHIP_NAMESPACE;

TEST_F(BFN_TEST_NAME(RmtPacketCoordinator), EpbBypassCounter) {
  // replace pipe process function with one that skips all pipe processing and
  // forwards packets from ingress port to TM and from TM to egress buffer.
  pipe_process_fn_t pipe_process_fn = [this](Packet *ingr_pkt,
                                             Packet *egr_pkt,
                                             Packet **queued_pkt,
                                             Packet **sent_pkt,
                                             Packet **resubmit_pkt,
                                             Packet **ing_mirror_pkt,
                                             Packet **egr_mirror_pkt
  ) {
    *queued_pkt = ingr_pkt;
    *sent_pkt = egr_pkt;
    if (nullptr != egr_pkt) {
      egr_pkt->set_metadata_added(true);
      egr_pkt->set_egress();
      egr_pkt->set_port(
          om_->port_get(egr_pkt->i2qing_metadata()->egress_uc_port()));
    }
  };
  rmt_packet_coordinator_->set_pipe_process_fn(pipe_process_fn);
  //om_->update_log_flags(ALL,ALL,ALL,ALL,ALL,ALL,ALL);

  // set up fake regs to verify epb counters
  auto& ebp18_reg = RegisterUtils::addr_pmarb(pipe_index())->ebp18_reg;
  int egr_port = 1;
  int epb_index = 0;
  int epb_chan = 1;
  // sanity checks...
  EXPECT_EQ(epb_index, Port::get_epb_num(egr_port));
  EXPECT_EQ(epb_chan, Port::get_epb_chan(egr_port));

  FakeRegister egr_bypass_cnt_0_1(tu_, tu_->reg_ptr_to_addr(
    &ebp18_reg.egrNx_reg[epb_index].epb_disp_port_regs.egr_bypass_count[epb_chan]),
    64);
  FakeRegister egr_bypass_cnt_0_2(tu_, tu_->reg_ptr_to_addr(
    &ebp18_reg.egrNx_reg[epb_index].epb_disp_port_regs.egr_bypass_count[2]),
    64);
  FakeRegister egr_bypass_cnt_0_3(tu_, tu_->reg_ptr_to_addr(
    &ebp18_reg.egrNx_reg[epb_index].epb_disp_port_regs.egr_bypass_count[3]),
    64);
  EXPECT_EQ(UINT64_C(0), egr_bypass_cnt_0_1.read());
  EXPECT_EQ(UINT64_C(0), egr_bypass_cnt_0_2.read());
  EXPECT_EQ(UINT64_C(0), egr_bypass_cnt_0_3.read());

  // sanity check choice of egress ports
  uint32_t pipes_en = GLOBAL_MODEL->GetPipesEnabled(om_->chip_index());
  EXPECT_FALSE(RmtDefs::is_pcie_port(1, pipes_en));
  EXPECT_EQ(0, test_rmt_pkt_coord_tx_pkts);
  // send 1 packet: ing port 1 -> egr port 1
  int ing_port = 1;
  send_packets(ing_port, egr_port, 1);
  // send 1 packet: ing port 2 -> egr port 1, pkt bypasses egress pipe
  ing_port = 2;
  send_packets(ing_port, egr_port, 1, 0x1);
  // send 1 packet: ing port 2 -> egr port 2
  egr_port = 2;
  send_packets(ing_port, egr_port, 1);
  // send 1 packet: ing port 3 -> egr port 2, pkt bypasses egress pipe
  ing_port = 3;
  send_packets(ing_port, egr_port, 1, 0x1);

  run_rmt_packet_coordinator(4);

  EXPECT_EQ(4, test_rmt_pkt_coord_tx_pkts);
  // verify epb bypass counters
  EXPECT_EQ(UINT64_C(1), egr_bypass_cnt_0_1.read());
  EXPECT_EQ(UINT64_C(1), egr_bypass_cnt_0_2.read());
  EXPECT_EQ(UINT64_C(0), egr_bypass_cnt_0_3.read());
  // NB other epb counters will not have been incremented because the pipe
  // process function has been swapped out
}

}
