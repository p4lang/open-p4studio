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

#include <utests/test_util.h>
#include <egress-buf.h>

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;

namespace MODEL_CHIP_TEST_NAMESPACE {

using namespace MODEL_CHIP_NAMESPACE;

class BFN_TEST_NAME(EgressBuf) : public BaseTest {
  struct RawReg { volatile void * addr; int width;};
  struct RawRegs {
    RawReg dprsr_rcv_pkt;
    RawReg dprsr_err_pkt;
    RawReg dprsr_runt_pkt;
    RawReg dprsr_ct_timeout_pkt;
    RawReg dprsr_drp_pkt;
    RawReg warp_rcv_pkt;
    RawReg warp_err_pkt;
    RawReg warp_runt_pkt;
    RawReg warp_drp_pkt;
    RawReg mac_xmt_pkt;
    RawReg mac_err_pkt;
  };

 public:
  template<typename EBUF_REG_T>
  void test_chnl_pktnum_counters(EgressBuf *egress_buf,
                                 EBUF_REG_T *ebuf_addr,
                                 int port_index,
                                 int chan_index) {
    RawRegs reg = {
#ifdef MODEL_CHIP_JBAYXX
      {&ebuf_addr->chan_group[chan_index].chnl_pktnum.chnl_pktnum_0_14, 64},
      {&ebuf_addr->chan_group[chan_index].chnl_pktnum.chnl_pktnum_2_14, 32},
      {&ebuf_addr->chan_group[chan_index].chnl_pktnum.chnl_pktnum_3_14, 32},
      {&ebuf_addr->chan_group[chan_index].chnl_pktnum.chnl_pktnum_4_14, 32},
      {&ebuf_addr->chan_group[chan_index].chnl_pktnum.chnl_pktnum_5_14, 32},
      {&ebuf_addr->chan_group[chan_index].chnl_pktnum.chnl_pktnum_6_14, 64},
      {&ebuf_addr->chan_group[chan_index].chnl_pktnum.chnl_pktnum_8_14, 32},
      {&ebuf_addr->chan_group[chan_index].chnl_pktnum.chnl_pktnum_9_14, 32},
      {&ebuf_addr->chan_group[chan_index].chnl_pktnum.chnl_pktnum_10_14, 32},
      {&ebuf_addr->chan_group[chan_index].chnl_pktnum.chnl_pktnum_11_14, 64},
      {&ebuf_addr->chan_group[chan_index].chnl_pktnum.chnl_pktnum_13_14, 32},
#endif
#ifdef MODEL_CHIP_CB_OR_LATER
      {&ebuf_addr->chan_group[chan_index].chnl_pktnum_dprsr_rcv.chnl_pktnum_dprsr_rcv_0_2, 36},
      {&ebuf_addr->chan_group[chan_index].chnl_pktnum_dprsr_err.chnl_pktnum_dprsr_err_0_2, 36},
      {&ebuf_addr->chan_group[chan_index].chnl_pktnum_dprsr_runt.chnl_pktnum_dprsr_runt_0_2, 36},
      {nullptr, -1},
      {nullptr, -1},
      {&ebuf_addr->chan_group[chan_index].chnl_pktnum_warp_rcv.chnl_pktnum_warp_rcv_0_2, 36},
      {&ebuf_addr->chan_group[chan_index].chnl_pktnum_warp_err.chnl_pktnum_warp_err_0_2, 36},
      {&ebuf_addr->chan_group[chan_index].chnl_pktnum_warp_runt.chnl_pktnum_warp_runt_0_2, 36},
      {nullptr, -1},
      {&ebuf_addr->chan_group[chan_index].chnl_pktnum_mac_xmt.chnl_pktnum_mac_xmt_0_2, 36},
      {&ebuf_addr->chan_group[chan_index].chnl_pktnum_mac_err.chnl_pktnum_mac_err_0_2, 36},
#endif
    };

    SCOPED_TRACE(port_index);

    // TODO what are the widths of the MODEL_CHIP_CB_OR_LATER registers?
    //      should the `width` argument be part of the `address` table?
    tu_->check_counter(
        reg.dprsr_rcv_pkt.addr, reg.dprsr_rcv_pkt.width,
        [egress_buf, port_index]() {
          egress_buf->increment_dprsr_rcv_pkt(port_index);
        },
        "dprsr_rcv_pkt");

    tu_->check_counter(
        reg.dprsr_err_pkt.addr, reg.dprsr_err_pkt.width,
        NULL, "dprsr_err_pkt");

    tu_->check_counter(
        reg.dprsr_runt_pkt.addr, reg.dprsr_runt_pkt.width,
        NULL, "dprsr_runt_pkt");

    if (reg.dprsr_ct_timeout_pkt.addr)
      tu_->check_counter(
        reg.dprsr_ct_timeout_pkt.addr, reg.dprsr_ct_timeout_pkt.width,
        NULL, "dprsr_ct_timeout_pkt");

    if (reg.dprsr_drp_pkt.addr)
      tu_->check_counter(
        reg.dprsr_drp_pkt.addr, reg.dprsr_drp_pkt.width,
        NULL, "dprsr_drp_pkt");

    tu_->check_counter(
        reg.warp_rcv_pkt.addr, reg.warp_rcv_pkt.width,
        [egress_buf, port_index]() {
          egress_buf->increment_warp_rcv_pkt(port_index);
        },
        "warp_rcv_pkt");

    tu_->check_counter(
        reg.warp_err_pkt.addr, reg.warp_err_pkt.width,
        NULL, "warp_err_pkt");

    tu_->check_counter(
        reg.warp_runt_pkt.addr, reg.warp_runt_pkt.width,
        NULL, "warp_runt_pkt");

    if (reg.warp_drp_pkt.addr)
      tu_->check_counter(
        reg.warp_drp_pkt.addr, reg.warp_drp_pkt.width,
        NULL, "warp_drp_pkt");

    tu_->check_counter(
        reg.mac_xmt_pkt.addr, reg.mac_xmt_pkt.width,
        [egress_buf, port_index]() {
          egress_buf->increment_mac_xmt_pkt(port_index);
        },
        "mac_xmt_pkt");

    tu_->check_counter(
        reg.mac_err_pkt.addr, reg.mac_err_pkt.width,
        NULL, "mac_err_pkt");
  }
};


#ifdef MODEL_CHIP_JBAYXX
TEST_F(BFN_TEST_NAME(EgressBuf), GetChanGroup) {
  ASSERT_EQ(1, GLOBAL_THROW_ON_ASSERT);  // sanity check
  EgressBuf *egress_buf = om_->egress_buf_get(0);
  // Test deliberately provokes RMT_ASSERT failures, so capture them to avoid
  // noise in test logs
  EXPECT_TRUE(nullptr != rmt_stdout_capture()->start());
  EXPECT_TRUE(nullptr != rmt_stderr_capture()->start());
  EXPECT_THROW(egress_buf->get_chan_group(-1), std::runtime_error);
  EXPECT_THROW(egress_buf->get_chan_group(72), std::runtime_error);

  for (int pipe_index = 0; pipe_index < tu_->kPipesMax; pipe_index++) {
    int pipe_base = 0x80 * pipe_index;
    EgressBuf *egress_buf = om_->egress_buf_get(pipe_index);
    for (int i = pipe_base; i < pipe_base + 72; i++) {
      EgressBufChanGroup *chan_group = nullptr;
      EXPECT_NO_THROW(chan_group = egress_buf->get_chan_group(i));
      EXPECT_FALSE(nullptr == chan_group) << i;
    }
    int bad_pipe_base = 0x80 * ((pipe_index + 1) % tu_->kPipesMax);
    for (int i = bad_pipe_base; i < bad_pipe_base + 72; i++) {
      EXPECT_THROW(egress_buf->get_chan_group(i), std::runtime_error);
    }
  }
  rmt_stdout_capture()->stop();
  rmt_stderr_capture()->stop();
  // check all lines start with "ERROR ASSERT:"
  // 2 + (4 * 72) = 290 expected rmt assert errors
  EXPECT_EQ(290, rmt_stdout_capture()->for_each_line_starts_with(
      "ERROR ASSERT:")) << rmt_stdout_capture()->dump_lines(200);
  EXPECT_EQ(290, rmt_stderr_capture()->for_each_line_starts_with(
      "ERROR ASSERT:")) << rmt_stderr_capture()->dump_lines(200);
}
#endif  // MODEL_CHIP_JBAYXX

TEST_F(BFN_TEST_NAME(EgressBuf), CheckChnlPktnumCounters) {
  for (int pipe_index = 0; pipe_index < tu_->kPipesMax; pipe_index++) {
    SCOPED_TRACE(pipe_index);
    for (int slice_index = 0; slice_index < 4; slice_index++) {
      SCOPED_TRACE(slice_index);
      EgressBuf *egress_buf = om_->egress_buf_get(pipe_index);
      ASSERT_FALSE(nullptr == egress_buf);

      // each slice has 1 ebuf100 reg with 2 channels
      for (int chan_index = 0; chan_index < 2; chan_index++) {
        // ebuf 100 reg's 2 channels map to ports thus:
        //   ebuf 100 channels[0:1] for slice[0] map to ports [0:1]
        //   ebuf 100 channels[0:1] for slice[1] map to ports [2:3]
        //   ebuf 100 channels[0:1] for slice[2] map to ports [3:4]
        //   ebuf 100 channels[0:1] for slice[3] map to ports [5:6]
        SCOPED_TRACE("ebuf100reg");
        int port_index = (128 * pipe_index) + (2* slice_index) + chan_index;
        test_chnl_pktnum_counters(
            egress_buf,
            &RegisterUtils::addr_pmarb(pipe_index)->ebuf900reg[slice_index].ebuf100reg,
            port_index,
            chan_index);
      }

      // each slice has 2 ebuf400 regs with 8 channels each
      for (int chan_index = 0; chan_index < 8; chan_index++) {
        // first ebuf 400 reg's 8 channels map to ports thus:
        //   ebuf 400 channels[0:8] for slice[0] map to ports [8:15]
        //   ebuf 400 channels[0:8] for slice[1] map to ports [24:31]
        //   ebuf 400 channels[0:8] for slice[2] map to ports [40:47]
        //   ebuf 400 channels[0:8] for slice[3] map to ports [56:63]
        SCOPED_TRACE("ebuf400reg[0]");
        int port_index = (128 * pipe_index) + 8 + (16 * slice_index) + chan_index;
        test_chnl_pktnum_counters(
            egress_buf,
            &RegisterUtils::addr_pmarb(pipe_index)->ebuf900reg[slice_index].ebuf400reg[0],
            port_index,
            chan_index);
      }

      for (int chan_index = 0; chan_index < 8; chan_index++) {
        // second ebuf 400 reg's 8 channels map to ports thus:
        //   ebuf 400 channels[0:8] for slice[0] map to ports [16:23]
        //   ebuf 400 channels[0:8] for slice[1] map to ports [32:39]
        //   ebuf 400 channels[0:8] for slice[2] map to ports [48:55]
        //   ebuf 400 channels[0:8] for slice[3] map to ports [64:71]
        SCOPED_TRACE("ebuf400reg[1]");
        int port_index = (128 * pipe_index) + 16 + (16 * slice_index) + chan_index;
        test_chnl_pktnum_counters(
            egress_buf,
            &RegisterUtils::addr_pmarb(pipe_index)->ebuf900reg[slice_index].ebuf400reg[1],
            port_index,
            chan_index);
      }
    }
  }
}

TEST_F(BFN_TEST_NAME(EgressBuf), CheckBadPipeIndex) {
  ASSERT_EQ(1, GLOBAL_THROW_ON_ASSERT);  // sanity check
  EgressBuf *egress_buf = om_->egress_buf_get(0);
  // Test deliberately provokes RMT_ASSERT failures, so capture them to avoid
  // noise in test logs
  EXPECT_TRUE(nullptr != rmt_stdout_capture()->start());
  EXPECT_TRUE(nullptr != rmt_stderr_capture()->start());
  EXPECT_THROW(egress_buf->increment_dprsr_rcv_pkt(-1), std::runtime_error);
  EXPECT_THROW(egress_buf->increment_warp_rcv_pkt(-1), std::runtime_error);
  EXPECT_THROW(egress_buf->increment_mac_xmt_pkt(-1), std::runtime_error);
  EXPECT_THROW(egress_buf->increment_dprsr_rcv_pkt(72), std::runtime_error);
  EXPECT_THROW(egress_buf->increment_warp_rcv_pkt(72), std::runtime_error);
  EXPECT_THROW(egress_buf->increment_mac_xmt_pkt(72), std::runtime_error);
  rmt_stdout_capture()->stop();
  rmt_stderr_capture()->stop();
  // check all lines start with "ERROR ASSERT:"
  EXPECT_EQ(6, rmt_stdout_capture()->for_each_line_starts_with(
      "ERROR ASSERT:")) << rmt_stdout_capture()->dump_lines(200);
  EXPECT_EQ(6, rmt_stderr_capture()->for_each_line_starts_with(
      "ERROR ASSERT:")) << rmt_stderr_capture()->dump_lines(200);
}

TEST_F(BFN_TEST_NAME(EgressBuf), CheckLookup) {
  // get should always return an object...
  EgressBuf *egress_buf = om_->egress_buf_get(0);
  ASSERT_FALSE(nullptr == egress_buf);
  // lookup after get should always return the *same* object...
  EgressBuf *egress_buf2 = om_->egress_buf_lookup(0);
  ASSERT_FALSE(nullptr == egress_buf2);
  ASSERT_TRUE(egress_buf == egress_buf2);
  // lookup for different pipe should return different object
  EgressBuf *egress_buf3 = om_->egress_buf_lookup(1);
  ASSERT_FALSE(nullptr == egress_buf3);
  ASSERT_FALSE(egress_buf == egress_buf3);
  // after reset, lookups should return nullptr
  GLOBAL_MODEL->Reset();
  EgressBuf *egress_buf4 = om_->egress_buf_lookup(0);
  ASSERT_TRUE(nullptr == egress_buf4);
  EgressBuf *egress_buf5 = om_->egress_buf_lookup(1);
  ASSERT_TRUE(nullptr == egress_buf5);
}

}
