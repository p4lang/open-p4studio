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

#include "test_ipb_common.h"

namespace MODEL_CHIP_TEST_NAMESPACE {

using namespace MODEL_CHIP_NAMESPACE;

class BFN_TEST_NAME(IpbCountersTest) : public IpbCountersCommonTest {
 protected:
  void *get_counter_addr(int pipeIndex,
                         int ipbIndex,
                         int chanIndex,
                         std::string counter_name,
                         std::string counter_word) override {
    // Given an integer channel index, it's hard to get to a particular channel
    // group in reg.h because the channel groups are not held in an array.
    // Instead, use register_map.h and construct the path element for the
    // channel group from the given index.
    std::string chan = "chan" + std::to_string(chanIndex) + "_group";
    std::vector<PathElement> path{PathElement{"pipes", std::vector<int>{pipeIndex}},
                                  PathElement{"pmarb", boost::none},
                                  PathElement{"ibp18_reg", boost::none},
                                  PathElement{"ibp_reg", std::vector<int>{ipbIndex}},
                                  PathElement{"ing_buf_regs", boost::none},
                                  PathElement{chan, boost::none},
                                  PathElement{counter_name, boost::none},
                                  PathElement{counter_word, boost::none}
    };
    return tu_->lookup_register_map(path);
  }

  void test_all_counters(int chanIndex) override {
    IpbCountersCommonTest::test_all_counters(chanIndex);

    test_counter(chanIndex,
                 "chnl_recirc_discard_pkt",
                 "chnl_recirc_discard_pkt_0_2",
                 test_ipb_->get_ipb_counters(chanIndex)->chnl_recirc_discard_pkt_);
    test_counter(chanIndex,
                 "chnl_recirc_received_pkt",
                 "chnl_recirc_received_pkt_0_2",
                 test_ipb_->get_ipb_counters(chanIndex)->chnl_recirc_received_pkt_);
  }
};


TEST_F(BFN_TEST_NAME(IpbCountersTest), IpbCounters) {
  for (int chanIndex = 0; chanIndex < Ipb::kChannelsPerIpb; chanIndex++) {
    test_all_counters(chanIndex);
  }
}

TEST_F(BFN_TEST_NAME(IpbCountersTest), IpbCounterIncrements) {
  auto *ctrs = test_ipb_->get_ipb_counters(0);
  test_wrapping_counter<uint64_t>(
      [ctrs]() -> uint64_t { return ctrs->chnl_parser_send_pkt_.chnl_parser_send_pkt(); },
      [ctrs](uint64_t val) { ctrs->chnl_parser_send_pkt_.chnl_parser_send_pkt(val); },
      [ctrs]() { ctrs->increment_chnl_parser_send_pkt(); },
      UINT64_C(0xffffffffffffffff)
  );
  test_wrapping_counter<uint64_t>(
      [ctrs]() -> uint64_t { return ctrs->chnl_deparser_send_pkt_.chnl_deparser_send_pkt(); },
      [ctrs](uint64_t val) { ctrs->chnl_deparser_send_pkt_.chnl_deparser_send_pkt(val); },
      [ctrs]() { ctrs->increment_chnl_deparser_send_pkt(); },
      UINT64_C(0xffffffffffffffff)
  );
  test_wrapping_counter<uint32_t>(
      [ctrs]() -> uint64_t { return ctrs->chnl_deparser_drop_pkt_.chnl_dprs_drop_pkt(); },
      [ctrs](uint64_t val) { ctrs->chnl_deparser_drop_pkt_.chnl_dprs_drop_pkt(val); },
      [ctrs]() { ctrs->increment_chnl_deparser_drop_pkt(); },
      UINT64_C(0xffffffff)
  );
  test_wrapping_counter<uint64_t>(
      [ctrs]() -> uint64_t { return ctrs->chnl_recirc_received_pkt_.chnl_recirc_received_pkt(); },
      [ctrs](uint64_t val) { ctrs->chnl_recirc_received_pkt_.chnl_recirc_received_pkt(val); },
      [ctrs]() { ctrs->increment_chnl_resubmit_received_pkt(); },
      UINT64_C(0xffffffffffffffff)
  );
  test_wrapping_counter<uint64_t>(
      [ctrs]() -> uint64_t { return ctrs->chnl_macs_received_pkt_.chnl_macs_received_pkt(); },
      [ctrs](uint64_t val) { ctrs->chnl_macs_received_pkt_.chnl_macs_received_pkt(val); },
      [ctrs]() { ctrs->increment_chnl_macs_received_pkt(); },
      UINT64_C(0xffffffffffffffff)
  );
}

// test chnl_recirc_received_pkt - specific to tofino
TEST_F(BFN_TEST_NAME(IpbCountersTest), TestRecircCounter) {
  auto call_insert_metadata =
      [this](int channel, bool resubmit, bool meta_enabled = true) {
    test_ipb_->set_meta_enabled(meta_enabled);
    // create a packet
    int base_len = 24;
    uint8_t base_pkt[base_len];
    for (int i = 0; i < base_len; i++) {
      base_pkt[i] = 'A' + i;
    }
    Packet *pkt = tu_->get_objmgr()->pkt_create(base_pkt, sizeof(base_pkt));
    if (resubmit) pkt->mark_for_resubmit();
    test_ipb_->insert_metadata(pkt, channel, resubmit);
    if (meta_enabled) base_len += 16;  // kHeaderSizeBytes prepended
    EXPECT_EQ(pkt->len(), base_len);
    tu_->get_objmgr()->pkt_delete(pkt);
  };

  auto *ipb_counters_0 = test_ipb_->get_ipb_counters(0);
  auto *ipb_counters_3 = test_ipb_->get_ipb_counters(3);
  // sanity checks...
  EXPECT_EQ(0u, ipb_counters_0->chnl_recirc_received_pkt_.chnl_recirc_received_pkt());
  EXPECT_EQ(0u, ipb_counters_3->chnl_recirc_received_pkt_.chnl_recirc_received_pkt());

  // make calls on varying channel, check counter increments on correct channel
  call_insert_metadata(0, false);
  EXPECT_EQ(0u, ipb_counters_0->chnl_recirc_received_pkt_.chnl_recirc_received_pkt());
  EXPECT_EQ(0u, ipb_counters_3->chnl_recirc_received_pkt_.chnl_recirc_received_pkt());
  call_insert_metadata(0, true);  // recirc chan 0
  EXPECT_EQ(1u, ipb_counters_0->chnl_recirc_received_pkt_.chnl_recirc_received_pkt());
  EXPECT_EQ(0u, ipb_counters_3->chnl_recirc_received_pkt_.chnl_recirc_received_pkt());
  call_insert_metadata(0, true);  // recirc chan 0
  EXPECT_EQ(2u, ipb_counters_0->chnl_recirc_received_pkt_.chnl_recirc_received_pkt());
  EXPECT_EQ(0u, ipb_counters_3->chnl_recirc_received_pkt_.chnl_recirc_received_pkt());
  call_insert_metadata(3, true);  // recirc chan 3
  EXPECT_EQ(2u, ipb_counters_0->chnl_recirc_received_pkt_.chnl_recirc_received_pkt());
  EXPECT_EQ(1u, ipb_counters_3->chnl_recirc_received_pkt_.chnl_recirc_received_pkt());
  call_insert_metadata(0, true);  // recirc chan 0
  EXPECT_EQ(3u, ipb_counters_0->chnl_recirc_received_pkt_.chnl_recirc_received_pkt());
  EXPECT_EQ(1u, ipb_counters_3->chnl_recirc_received_pkt_.chnl_recirc_received_pkt());
  call_insert_metadata(0, false);
  EXPECT_EQ(3u, ipb_counters_0->chnl_recirc_received_pkt_.chnl_recirc_received_pkt());
  EXPECT_EQ(1u, ipb_counters_3->chnl_recirc_received_pkt_.chnl_recirc_received_pkt());
  call_insert_metadata(3, true, false);  // recirc chan 3, meta_enabled false
  EXPECT_EQ(3u, ipb_counters_0->chnl_recirc_received_pkt_.chnl_recirc_received_pkt());
  EXPECT_EQ(1u, ipb_counters_3->chnl_recirc_received_pkt_.chnl_recirc_received_pkt());
  call_insert_metadata(3, true);  // recirc chan 3
  EXPECT_EQ(3u, ipb_counters_0->chnl_recirc_received_pkt_.chnl_recirc_received_pkt());
  EXPECT_EQ(2u, ipb_counters_3->chnl_recirc_received_pkt_.chnl_recirc_received_pkt());

  // set counters to their max value (64 bits for tofino) and check they wrap
  ipb_counters_0->chnl_recirc_received_pkt_.chnl_recirc_received_pkt(UINT64_C(0xffffffffffffffff));
  ipb_counters_3->chnl_recirc_received_pkt_.chnl_recirc_received_pkt(UINT64_C(0xffffffffffffffff));
  EXPECT_EQ(UINT64_C(0xffffffffffffffff), ipb_counters_0->chnl_recirc_received_pkt_.chnl_recirc_received_pkt());
  EXPECT_EQ(UINT64_C(0xffffffffffffffff), ipb_counters_3->chnl_recirc_received_pkt_.chnl_recirc_received_pkt());
  call_insert_metadata(0, true);  // recirc chan 3
  EXPECT_EQ(0u, ipb_counters_0->chnl_recirc_received_pkt_.chnl_recirc_received_pkt());
  EXPECT_EQ(UINT64_C(0xffffffffffffffff), ipb_counters_3->chnl_recirc_received_pkt_.chnl_recirc_received_pkt());
  call_insert_metadata(3, true);  // recirc chan 3
  EXPECT_EQ(0u, ipb_counters_0->chnl_recirc_received_pkt_.chnl_recirc_received_pkt());
  EXPECT_EQ(0u, ipb_counters_3->chnl_recirc_received_pkt_.chnl_recirc_received_pkt());
}

// TODO: Uses OLD Tofino regs - so won't work on JBay
TEST(BFN_TEST_NAME(Ipb), IpbTest_1) {
  int chip = 0;
  int pipe = 0;
  int stage = 0;
  TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);

  RmtObjectManager *om = tu.get_objmgr();
  ASSERT_TRUE(om != NULL);
  Ipb *ib0 = om->ipb_lookup(pipe, 0);  // port 0
  Ipb *ib4 = om->ipb_lookup(pipe, 4);  // port 4

  ib0->set_meta_enabled(true);
  ib4->set_meta_enabled(true);
  // set intrinsic meta - logical port and version using host interface (fix2)
  // and phase0 meta (fix0,1)
  Ing_buf_regs *regs0 = tu.ing_buf_reg_get(pipe, 0);
  Ing_buf_regs *regs4 = tu.ing_buf_reg_get(pipe, 4);

  uint32_t fix0 = 0, fix1 = 0, fix2 = 0;
  uint64_t fix = 0;
  uint32_t chl = 0x100;
  for (int i=0; i<4; i++) {
    setp_chnl_metadata_fix2_chnl_meta_fix2(&fix2, (i << 12) | (chl+i));  // version i and logical port 0x100+i
    fix1 = 0x12345600+i;
    fix0 = 0xABCDEF00+i;
    setp_chnl_metadata_fix_chnl_meta_fix(&fix, (uint64_t)fix1 << 32 | fix0);
    if (model_common::Util::is_little_endian()) {
      fix0 = __builtin_bswap32(fix0);
      fix1 = __builtin_bswap32(fix1);
      fix = __builtin_bswap64(fix);
    }
    switch (i) {
      case 0:
        tu.OutWord(&regs0->chan0_group.chnl_metadata_fix2, fix2);
        tu.OutWord(&regs0->chan0_group.chnl_metadata_fix.chnl_metadata_fix_0_2, fix1);
        tu.OutWord(&regs0->chan0_group.chnl_metadata_fix.chnl_metadata_fix_1_2, fix0);
        break;
      case 1:
        tu.OutWord(&regs0->chan1_group.chnl_metadata_fix2, fix2);
        tu.OutWord(&regs0->chan1_group.chnl_metadata_fix.chnl_metadata_fix_0_2, fix1);
        tu.OutWord(&regs0->chan1_group.chnl_metadata_fix.chnl_metadata_fix_1_2, fix0);
        break;
      case 2:
        tu.OutWord(&regs0->chan2_group.chnl_metadata_fix2, fix2);
        tu.OutWord(&regs0->chan2_group.chnl_metadata_fix.chnl_metadata_fix_0_2, fix1);
        tu.OutWord(&regs0->chan2_group.chnl_metadata_fix.chnl_metadata_fix_1_2, fix0);
        break;
      case 3:
        tu.OutWord(&regs0->chan3_group.chnl_metadata_fix2, fix2);
        tu.OutWord(&regs0->chan3_group.chnl_metadata_fix.chnl_metadata_fix_0_2, fix1);
        tu.OutWord(&regs0->chan3_group.chnl_metadata_fix.chnl_metadata_fix_1_2, fix0);
        break;
    }
  }
  // second port
  chl = 0x10;
  setp_chnl_metadata_fix2_chnl_meta_fix2(&fix2, (1 << 12) | (chl));  // version 1 and logical port 0x10+i
  fix1 = 0x0012345600;
  fix0 = 0x00ABCDEF00;
  setp_chnl_metadata_fix_chnl_meta_fix(&fix, (uint64_t)fix1 << 32 | fix0);
  if (model_common::Util::is_little_endian()) {
    fix = __builtin_bswap64(fix);
    fix0 = __builtin_bswap32(fix0);
    fix1 = __builtin_bswap32(fix1);
  }
  tu.OutWord(&regs4->chan0_group.chnl_metadata_fix2, fix2);
  tu.OutWord(&regs0->chan0_group.chnl_metadata_fix.chnl_metadata_fix_0_2, fix1);
  tu.OutWord(&regs0->chan0_group.chnl_metadata_fix.chnl_metadata_fix_1_2, fix0);
  // create a packet and resubmit buffer
  uint8_t base_pkt[24];
  for (int i=0; i<(int)sizeof(base_pkt); i++) {
    base_pkt[i] = 'A'+i;
  }
  uint32_t   base_len, pkt_len;
  const uint8_t resub_data[8] = {0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18};  // 8 bytes max

  // sanity checks
  int actual = Ipb::kMeta0VersionShift;
  EXPECT_EQ(60, actual);
  actual = Ipb::kMeta0LogicalPortShift;
  EXPECT_EQ(48, actual);
  actual = Ipb::kMeta0ResubmitFlagShift;
  EXPECT_EQ(63, actual);

  chl = 0x100;
  for (int i=0; i<4; i++) {
    setp_chnl_metadata_fix2_chnl_meta_fix2(&fix2, (i << 12) | (chl+i));  // version i and logical port 0x100+i
    fix1 = 0x12345600+i;
    fix0 = 0xABCDEF00+i;
    setp_chnl_metadata_fix_chnl_meta_fix(&fix, (uint64_t)fix1 << 32 | fix0);

    //PacketBuffer *rsub_buf = new PacketBuffer(resub_data, size_t(8));
    Packet *pkt = om->pkt_create(base_pkt, sizeof(base_pkt));
    base_len = pkt->len();

    ib0->insert_metadata(pkt, i, false/*resubmit*/);
    pkt_len = pkt->len();
    EXPECT_EQ(pkt_len, base_len+16);  // intrinsic and p0 meta added to packet

    std::unique_ptr<uint8_t[]> new_pkt_buf(new uint8_t[pkt_len]);
    pkt->get_buf(new_pkt_buf.get(), 0, pkt_len);
    // extract version bits and port # (bits 49-57)
    uint64_t i_meta = *((uint64_t *)new_pkt_buf.get());
    if (model_common::Util::is_little_endian()) {
      i_meta = __builtin_bswap64(i_meta);
    }
    uint32_t new_f2 = (i_meta >> (Ipb::kMeta0VersionShift)) << 12;
    new_f2 |= ((i_meta >> Ipb::kMeta0LogicalPortShift) & 0x1FF);
    EXPECT_EQ(new_f2, fix2);
    ASSERT_TRUE(!memcmp(&(new_pkt_buf.get()[8]), (uint8_t *)&fix, 8));
    // base pkt
    ASSERT_TRUE(!memcmp(&(new_pkt_buf.get()[16]), base_pkt, base_len));
  }

  {
    chl = 0x10;
    setp_chnl_metadata_fix2_chnl_meta_fix2(&fix2, (1 << 12) | (chl));  // version 1 and logical port 0x10+i

    PacketBuffer *rsub_buf = new PacketBuffer(resub_data, size_t(8));
    Packet *pkt = om->pkt_create(base_pkt, sizeof(base_pkt));
    base_len = pkt->len();
    pkt->set_resubmit_header(rsub_buf);
    pkt->mark_for_resubmit();

    ib4->insert_metadata(pkt, 0, true/*resubmit*/);
    pkt_len = pkt->len();
    EXPECT_EQ(pkt_len, base_len+16);  // intrinsic and resub meta added to packet

    std::unique_ptr<uint8_t[]> new_pkt_buf(new uint8_t[pkt_len]);
    pkt->get_buf(new_pkt_buf.get(), 0, pkt_len);

    // extract version bits and port # (bits 49-57)
    uint64_t i_meta = *((uint64_t *)new_pkt_buf.get());
    if (model_common::Util::is_little_endian()) {
      i_meta = __builtin_bswap64(i_meta);
    }
    uint32_t new_f2 = ((i_meta >> (Ipb::kMeta0VersionShift)) & 0x3) << 12;
    new_f2 |= ((i_meta >> Ipb::kMeta0LogicalPortShift) & 0x1FF);
    bool resub_flag = i_meta & ((uint64_t)1 << Ipb::kMeta0ResubmitFlagShift);
    EXPECT_EQ(new_f2, fix2);
    ASSERT_TRUE(resub_flag);
    // no phase0 meta - resubmit header instead
    ASSERT_TRUE(!memcmp(&(new_pkt_buf.get()[8]), resub_data, 8));
    // base pkt
    ASSERT_TRUE(!memcmp(&(new_pkt_buf.get()[16]), base_pkt, base_len));
  }

}

}
