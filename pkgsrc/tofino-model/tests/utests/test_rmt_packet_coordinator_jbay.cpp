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

TEST_F(BFN_TEST_NAME(RmtPacketCoordinator), PacketProcess) {
  // replace pipe process function with one that skips all pipe processing and
  // forwards packets from ingress port to TM and from TM to egress buffer.
  // Note that this means any metadata prepended to the egress packet by epb is
  // not removed before transmitting.
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
    if (nullptr != ingr_pkt) {
      // Mirror all ingress pkts to CPU - should just 'disappear'.
      // However mirrored packets **should NOT be counted** in S2P if JBay
      Packet *ing_mir = ingr_pkt->clone();
      ing_mir->i2qing_metadata()->reset();
      ing_mir->i2qing_metadata()->set_copy_to_cpu(1);
      // Here set ingress port to 0 (think H/W uses 72)
      ing_mir->set_port(om_->port_get(0));
      *ing_mirror_pkt = ing_mir;
    }
    if (nullptr != egr_pkt) {
      // Also mirror all egress pkts to CPU - again should disappear.
      // And *should NOT be counted** in S2P if JBay
      Packet *egr_mir = egr_pkt->clone();
      egr_mir->i2qing_metadata()->reset();
      egr_mir->i2qing_metadata()->set_copy_to_cpu(1);
      // Here set ingress port to 0 (think H/W uses 72)
      egr_mir->set_port(om_->port_get(0));
      *egr_mirror_pkt = egr_mir;

      egr_pkt->set_metadata_added(true);
      egr_pkt->set_egress();
      egr_pkt->set_port(
          om_->port_get(egr_pkt->i2qing_metadata()->egress_uc_port()));
    }
  };
  rmt_packet_coordinator_->set_pipe_process_fn(pipe_process_fn);
  //om_->update_log_flags(ALL,ALL,ALL,ALL,ALL,ALL,ALL);

  // set up fake regs to verify egress buf counters
  auto& ebuf900reg = RegisterUtils::addr_pmarb(pipe_index())->ebuf900reg[0];
  // channel 0 counters - this channel maps to chan_group 0 in the ebuf100
  // slice 0 N.B. this is the pcie port for which RmtPacketCoordinator does not
  // call its tx_function_
  FakeRegister dprsr_rcv_0(tu_, tu_->reg_ptr_to_addr(
    &ebuf900reg.ebuf100reg.chan_group[0].chnl_pktnum.chnl_pktnum_0_14), 64);
  FakeRegister warp_rcv_0(tu_, tu_->reg_ptr_to_addr(
    &ebuf900reg.ebuf100reg.chan_group[0].chnl_pktnum.chnl_pktnum_6_14), 64);
  FakeRegister mac_xmt_0(tu_, tu_->reg_ptr_to_addr(
    &ebuf900reg.ebuf100reg.chan_group[0].chnl_pktnum.chnl_pktnum_11_14), 64);
  EXPECT_EQ(UINT64_C(0), dprsr_rcv_0.read());
  EXPECT_EQ(UINT64_C(0), warp_rcv_0.read());
  EXPECT_EQ(UINT64_C(0), mac_xmt_0.read());
  // channel 1 counters - this channel maps to chan_group 1 in the ebuf100
  // slice 0
  FakeRegister dprsr_rcv_1(tu_, tu_->reg_ptr_to_addr(
    &ebuf900reg.ebuf100reg.chan_group[1].chnl_pktnum.chnl_pktnum_0_14), 64);
  FakeRegister warp_rcv_1(tu_, tu_->reg_ptr_to_addr(
    &ebuf900reg.ebuf100reg.chan_group[1].chnl_pktnum.chnl_pktnum_6_14), 64);
  FakeRegister mac_xmt_1(tu_, tu_->reg_ptr_to_addr(
    &ebuf900reg.ebuf100reg.chan_group[1].chnl_pktnum.chnl_pktnum_11_14), 64);
  EXPECT_EQ(UINT64_C(0), dprsr_rcv_1.read());
  EXPECT_EQ(UINT64_C(0), warp_rcv_1.read());
  EXPECT_EQ(UINT64_C(0), mac_xmt_1.read());
  // channel 8 counters - this channel maps to chan_group 0 in the first
  // ebuf400 in slice 0 (channels 2:7 map to ebuf100's in other slices).
  FakeRegister dprsr_rcv_8(tu_, tu_->reg_ptr_to_addr(
    &ebuf900reg.ebuf400reg[0].chan_group[0].chnl_pktnum.chnl_pktnum_0_14), 64);
  FakeRegister warp_rcv_8(tu_, tu_->reg_ptr_to_addr(
    &ebuf900reg.ebuf400reg[0].chan_group[0].chnl_pktnum.chnl_pktnum_6_14), 64);
  FakeRegister mac_xmt_8(tu_, tu_->reg_ptr_to_addr(
    &ebuf900reg.ebuf400reg[0].chan_group[0].chnl_pktnum.chnl_pktnum_11_14), 64);
  EXPECT_EQ(UINT64_C(0), dprsr_rcv_8.read());
  EXPECT_EQ(UINT64_C(0), warp_rcv_8.read());
  EXPECT_EQ(UINT64_C(0), mac_xmt_8.read());
  // channel 19 counters - this channel maps to chan_group 3 in the second
  // ebuf400 in slice 0
  FakeRegister dprsr_rcv_19(tu_, tu_->reg_ptr_to_addr(
    &ebuf900reg.ebuf400reg[1].chan_group[3].chnl_pktnum.chnl_pktnum_0_14), 64);
  FakeRegister warp_rcv_19(tu_, tu_->reg_ptr_to_addr(
    &ebuf900reg.ebuf400reg[1].chan_group[3].chnl_pktnum.chnl_pktnum_6_14), 64);
  FakeRegister mac_xmt_19(tu_, tu_->reg_ptr_to_addr(
    &ebuf900reg.ebuf400reg[1].chan_group[3].chnl_pktnum.chnl_pktnum_11_14), 64);
  EXPECT_EQ(UINT64_C(0), dprsr_rcv_19.read());
  EXPECT_EQ(UINT64_C(0), warp_rcv_19.read());
  EXPECT_EQ(UINT64_C(0), mac_xmt_19.read());

  // set up fake regs to verify s2p counters
  // N.B. these are associated with ingress ports
  FakeRegister s2p_pkt_ctr_0(tu_, tu_->reg_ptr_to_addr(
    &RegisterUtils::addr_pmarb(pipe_index())->s2preg.pkt_ctr[0]), 36);
  FakeRegister s2p_byte_ctr_0(tu_, tu_->reg_ptr_to_addr(
    &RegisterUtils::addr_pmarb(pipe_index())->s2preg.byte_ctr[0]), 48);
  EXPECT_EQ(UINT64_C(0), s2p_pkt_ctr_0.read());
  EXPECT_EQ(UINT64_C(0), s2p_byte_ctr_0.read());
  FakeRegister s2p_pkt_ctr_1(tu_, tu_->reg_ptr_to_addr(
    &RegisterUtils::addr_pmarb(pipe_index())->s2preg.pkt_ctr[1]), 36);
  FakeRegister s2p_byte_ctr_1(tu_, tu_->reg_ptr_to_addr(
    &RegisterUtils::addr_pmarb(pipe_index())->s2preg.byte_ctr[1]), 48);
  EXPECT_EQ(UINT64_C(0), s2p_pkt_ctr_1.read());
  EXPECT_EQ(UINT64_C(0), s2p_byte_ctr_1.read());
  FakeRegister s2p_pkt_ctr_2(tu_, tu_->reg_ptr_to_addr(
    &RegisterUtils::addr_pmarb(pipe_index())->s2preg.pkt_ctr[2]), 36);
  FakeRegister s2p_byte_ctr_2(tu_, tu_->reg_ptr_to_addr(
    &RegisterUtils::addr_pmarb(pipe_index())->s2preg.byte_ctr[2]), 48);
  EXPECT_EQ(UINT64_C(0), s2p_pkt_ctr_2.read());
  EXPECT_EQ(UINT64_C(0), s2p_byte_ctr_2.read());
  FakeRegister s2p_pkt_ctr_3(tu_, tu_->reg_ptr_to_addr(
    &RegisterUtils::addr_pmarb(pipe_index())->s2preg.pkt_ctr[3]), 36);
  FakeRegister s2p_byte_ctr_3(tu_, tu_->reg_ptr_to_addr(
    &RegisterUtils::addr_pmarb(pipe_index())->s2preg.byte_ctr[3]), 48);
  EXPECT_EQ(UINT64_C(0), s2p_pkt_ctr_3.read());
  EXPECT_EQ(UINT64_C(0), s2p_byte_ctr_3.read());

  // set up fake regs to verify p2s counters
  // N.B. these are associated with egress ports
  FakeRegister p2s_pkt_ctr_0(tu_, tu_->reg_ptr_to_addr(
    &RegisterUtils::addr_pmarb(pipe_index())->p2sreg.pkt_ctr[0]), 36);
  FakeRegister p2s_byte_ctr_0(tu_, tu_->reg_ptr_to_addr(
    &RegisterUtils::addr_pmarb(pipe_index())->p2sreg.byte_ctr[0]), 48);
  EXPECT_EQ(UINT64_C(0), p2s_pkt_ctr_0.read());
  EXPECT_EQ(UINT64_C(0), p2s_byte_ctr_0.read());
  FakeRegister p2s_pkt_ctr_1(tu_, tu_->reg_ptr_to_addr(
    &RegisterUtils::addr_pmarb(pipe_index())->p2sreg.pkt_ctr[1]), 36);
  FakeRegister p2s_byte_ctr_1(tu_, tu_->reg_ptr_to_addr(
    &RegisterUtils::addr_pmarb(pipe_index())->p2sreg.byte_ctr[1]), 48);
  EXPECT_EQ(UINT64_C(0), p2s_pkt_ctr_1.read());
  EXPECT_EQ(UINT64_C(0), p2s_byte_ctr_1.read());
  FakeRegister p2s_pkt_ctr_19(tu_, tu_->reg_ptr_to_addr(
    &RegisterUtils::addr_pmarb(pipe_index())->p2sreg.pkt_ctr[19]), 36);
  FakeRegister p2s_byte_ctr_19(tu_, tu_->reg_ptr_to_addr(
    &RegisterUtils::addr_pmarb(pipe_index())->p2sreg.byte_ctr[19]), 48);
  EXPECT_EQ(UINT64_C(0), p2s_pkt_ctr_19.read());
  EXPECT_EQ(UINT64_C(0), p2s_byte_ctr_19.read());
  FakeRegister p2s_pkt_ctr_8(tu_, tu_->reg_ptr_to_addr(
    &RegisterUtils::addr_pmarb(pipe_index())->p2sreg.pkt_ctr[8]), 36);
  FakeRegister p2s_byte_ctr_8(tu_, tu_->reg_ptr_to_addr(
    &RegisterUtils::addr_pmarb(pipe_index())->p2sreg.byte_ctr[8]), 48);
  EXPECT_EQ(UINT64_C(0), p2s_pkt_ctr_8.read());
  EXPECT_EQ(UINT64_C(0), p2s_byte_ctr_8.read());

  // port 1 is ebuf0,ch1 - set it up to recirculate
  auto& pgr_common_reg = RegisterUtils::addr_pmarb(pipe_index())->pgrreg.pgr_common;
  uint32_t eb_csr = 0;
  setp_pgr_ebuf_port_ctrl_port_en(&eb_csr, 1);  // recirc enabled
  setp_pgr_ebuf_port_ctrl_channel_en(&eb_csr, 0x2);  // recirc channel 1 only
  setp_pgr_ebuf_port_ctrl_channel_mode(&eb_csr, 1);  // 2 channel mode
  tu_->OutWord(&pgr_common_reg.ebuf_port_ctrl[0], eb_csr);

  // sanity check choice of egress ports
  uint32_t pipes_en = GLOBAL_MODEL->GetPipesEnabled(om_->chip_index());
  EXPECT_TRUE(RmtDefs::is_pcie_port(0, pipes_en));
  EXPECT_FALSE(RmtDefs::is_pcie_port(1, pipes_en));
  EXPECT_FALSE(RmtDefs::is_pcie_port(8, pipes_en));
  EXPECT_FALSE(RmtDefs::is_pcie_port(19, pipes_en));

  EXPECT_EQ(0, test_rmt_pkt_coord_tx_pkts);
  // send 1 packet to egr port 0 (pcie port so not transmitted)
  send_packets(1 /*ing port*/, 0 /*egr port*/, 1);
  // send 1 packet to egr port 1, which recirculates
  send_packets(1 /*ing port*/, 1 /*egr port*/, 1);
  // send 1 packet to egr port 1, which recirculates, pkt bypasses egress pipe
  send_packets(2 /*ing port*/, 1 /*egr port*/, 1, 0x1);
  // send 2 packets to egr port 8
  send_packets(2 /*ing port*/, 8 /*egr port*/, 2);
  // send 2 packets to egr port 19
  send_packets(3 /*ing port*/, 19 /*egr port*/, 2);
  // send 1 packet to egr port 19, pkt bypasses egress pipe
  send_packets(3 /*ing port*/, 19 /*egr port*/, 1, 0x1);

  run_rmt_packet_coordinator(7);

  EXPECT_EQ(7, test_rmt_pkt_coord_tx_pkts);
  // verify channel 0 egress buf counters
  EXPECT_EQ(UINT64_C(1), dprsr_rcv_0.read());
  EXPECT_EQ(UINT64_C(0), warp_rcv_0.read());
  EXPECT_EQ(UINT64_C(1), mac_xmt_0.read());  // XXX: pcie pkts *are* counted
  // verify channel 1 egress buf counters
  EXPECT_EQ(UINT64_C(2), dprsr_rcv_1.read());  // pkt recirculates
  EXPECT_EQ(UINT64_C(2), warp_rcv_1.read());  // pkt recirculates
  EXPECT_EQ(UINT64_C(4), mac_xmt_1.read());  // XXX: recirc pkts *are* counted
  // verify channel 8 egress buf counters
  EXPECT_EQ(UINT64_C(2), dprsr_rcv_8.read());
  EXPECT_EQ(UINT64_C(0), warp_rcv_8.read());
  EXPECT_EQ(UINT64_C(2), mac_xmt_8.read());
  // verify channel 9 egress buf counters
  EXPECT_EQ(UINT64_C(2), dprsr_rcv_19.read());
  EXPECT_EQ(UINT64_C(1), warp_rcv_19.read());
  EXPECT_EQ(UINT64_C(3), mac_xmt_19.read());

  // verify s2p counters - should be zero until sampled
  EXPECT_EQ(UINT64_C(0), s2p_pkt_ctr_0.read());
  EXPECT_EQ(UINT64_C(0), s2p_byte_ctr_0.read());
  EXPECT_EQ(UINT64_C(0), s2p_pkt_ctr_1.read());
  EXPECT_EQ(UINT64_C(0), s2p_byte_ctr_1.read());
  EXPECT_EQ(UINT64_C(0), s2p_pkt_ctr_2.read());
  EXPECT_EQ(UINT64_C(0), s2p_byte_ctr_2.read());
  EXPECT_EQ(UINT64_C(0), s2p_pkt_ctr_3.read());
  EXPECT_EQ(UINT64_C(0), s2p_byte_ctr_3.read());
  //sample s2p counters
  uint32_t sample_ctr_csr = 0;
  setp_s2p_reg_ctr_sample_sample(&sample_ctr_csr, 0x1);
  tu_->OutWord(&RegisterUtils::addr_pmarb(pipe_index())->s2preg.ctr_sample,
               sample_ctr_csr);
  // 2 pkts ingress port 1 and 2 recirculate to port 1
  EXPECT_EQ(UINT64_C(4), s2p_pkt_ctr_1.read());
  // ingress packets are length 8; packet that does not bypass egress path has
  // 4 bytes of metadata appended by epb; both packets recirculate so get
  // counted twice; so the complete packet paths are (block<pkt_length>):
  // ingr<8> -> s2p<8> -> p2s<8> -> egr<12> -> recirc<12>
  //     -> ingr<12> -> s2p<12> -> p2s<12>-> egr<12> -> tx
  // ingr<8> -> s2p<8> -> p2s<8> -> BYPASS -> recirc<8>
  //     -> ingr<8> -> s2p<8> -> p2s<8>-> egr<8> -> tx
  // i.e. for first packet s2p and p2s each count 8 + 12 = 20 bytes, for second
  // packet s2p and p2s each count 8 + 8 = 16 bytes, so expect total of 36
  //
  // XXX: EPB logic moved insided Pipe::process() so
  //            no longer 4B from EPB; thus now expect 2x32B below

  EXPECT_EQ(UINT64_C(32), s2p_byte_ctr_1.read());
  // 3 pkts ingress port 2
  EXPECT_EQ(UINT64_C(3), s2p_pkt_ctr_2.read());
  EXPECT_EQ(UINT64_C(24), s2p_byte_ctr_2.read());
  // 3 pkts ingress port 3
  EXPECT_EQ(UINT64_C(3), s2p_pkt_ctr_3.read());
  EXPECT_EQ(UINT64_C(24), s2p_byte_ctr_3.read());
  // Mirrored packets should appear to come from port 0
  // but in case of JBay S2P does *NOT* count mirror
  // packets so port 0 should still have count 0
  EXPECT_EQ(UINT64_C(0), s2p_pkt_ctr_0.read());
  EXPECT_EQ(UINT64_C(0), s2p_byte_ctr_0.read());


  // verify p2s counters - should be zero until sampled
  EXPECT_EQ(UINT64_C(0), p2s_pkt_ctr_0.read());
  EXPECT_EQ(UINT64_C(0), p2s_byte_ctr_0.read());
  EXPECT_EQ(UINT64_C(0), p2s_pkt_ctr_1.read());
  EXPECT_EQ(UINT64_C(0), p2s_byte_ctr_1.read());
  EXPECT_EQ(UINT64_C(0), p2s_pkt_ctr_8.read());
  EXPECT_EQ(UINT64_C(0), p2s_byte_ctr_8.read());
  EXPECT_EQ(UINT64_C(0), p2s_pkt_ctr_19.read());
  EXPECT_EQ(UINT64_C(0), p2s_byte_ctr_19.read());
  //sample p2s counters
  uint32_t p2s_sample_ctr_csr = 0;
  setp_p2s_reg_ctr_sample_sample(&p2s_sample_ctr_csr, 0x1);
  tu_->OutWord(&RegisterUtils::addr_pmarb(pipe_index())->p2sreg.ctr_sample,
               p2s_sample_ctr_csr);
  // 1 pkts egress port 1
  EXPECT_EQ(UINT64_C(1), p2s_pkt_ctr_0.read());
  EXPECT_EQ(UINT64_C(8), p2s_byte_ctr_0.read());
  // 2 pkts egress port 1, both recirculate -> 4 counted
  EXPECT_EQ(UINT64_C(4), p2s_pkt_ctr_1.read());
  EXPECT_EQ(UINT64_C(32), p2s_byte_ctr_1.read());
  // 2 pkts egress port 8
  EXPECT_EQ(UINT64_C(2), p2s_pkt_ctr_8.read());
  EXPECT_EQ(UINT64_C(16), p2s_byte_ctr_8.read());
  // 3 pkts egress port 19
  EXPECT_EQ(UINT64_C(3), p2s_pkt_ctr_19.read());
  EXPECT_EQ(UINT64_C(24), p2s_byte_ctr_19.read());
}

TEST_F(BFN_TEST_NAME(RmtPacketCoordinator), DroppedEgressPacket) {
  // replace pipe process function with one that skips all pipe processing and
  // forwards packets from ingress port to TM but drops packets in egress
  // pipeline unless they are bypass packets
  pipe_process_fn_t pipe_process_fn = [this](Packet *ingr_pkt,
                                             Packet *egr_pkt,
                                             Packet **queued_pkt,
                                             Packet **sent_pkt,
                                             Packet **resubmit_pkt,
                                             Packet **ing_mirror_pkt,
                                             Packet **egr_mirror_pkt
  ) {
    *queued_pkt = ingr_pkt;
    if (nullptr != egr_pkt && egr_pkt->i2qing_metadata()->bypass_egr_mode()) {
      *sent_pkt = egr_pkt;
      egr_pkt->set_metadata_added(true);
      egr_pkt->set_egress();
      egr_pkt->set_port(
          om_->port_get(egr_pkt->i2qing_metadata()->egress_uc_port()));
    } else {
      *sent_pkt = nullptr;
    }
  };
  rmt_packet_coordinator_->set_pipe_process_fn(pipe_process_fn);
  //om_->update_log_flags(ALL,ALL,ALL,ALL,ALL,ALL,ALL);

  auto& ebuf900reg = RegisterUtils::addr_pmarb(pipe_index())->ebuf900reg[0];
  // channel 19 counters - this channel maps to chan_group 3 in the second
  // ebuf400 in slice 0
  FakeRegister dprsr_rcv_19(tu_, tu_->reg_ptr_to_addr(
    &ebuf900reg.ebuf400reg[1].chan_group[3].chnl_pktnum.chnl_pktnum_0_14), 64);
  FakeRegister warp_rcv_19(tu_, tu_->reg_ptr_to_addr(
    &ebuf900reg.ebuf400reg[1].chan_group[3].chnl_pktnum.chnl_pktnum_6_14), 64);
  FakeRegister mac_xmt_19(tu_, tu_->reg_ptr_to_addr(
    &ebuf900reg.ebuf400reg[1].chan_group[3].chnl_pktnum.chnl_pktnum_11_14), 64);
  EXPECT_EQ(UINT64_C(0), dprsr_rcv_19.read());
  EXPECT_EQ(UINT64_C(0), warp_rcv_19.read());
  EXPECT_EQ(UINT64_C(0), mac_xmt_19.read());

  EXPECT_EQ(0, test_rmt_pkt_coord_tx_pkts);
  // send 1 packets to egr port 19
  send_packets(0 /*ing port*/, 19 /*egr port*/, 1);
  // send 1 packet to egr port 19, pkt bypasses egress pipe
  send_packets(0 /*ing port*/, 19 /*egr port*/, 1, 0x1);

  run_rmt_packet_coordinator(1);

  EXPECT_EQ(1, test_rmt_pkt_coord_tx_pkts);
  // verify channel 19 counters
  EXPECT_EQ(UINT64_C(0), dprsr_rcv_19.read());
  EXPECT_EQ(UINT64_C(1), warp_rcv_19.read());
  EXPECT_EQ(UINT64_C(1), mac_xmt_19.read());
}

}
