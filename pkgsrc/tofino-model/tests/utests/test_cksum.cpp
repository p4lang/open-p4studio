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

#include "test_cksum.h"

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;


namespace MODEL_CHIP_TEST_NAMESPACE {

  bool cksum_print = true;
  bool cksum_print_lots = false;

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;


  uint16_t ones_cmpl_finalize(uint32_t val32) {
    while ((val32 >> 16) != 0u) val32 = (val32 & 0xFFFFu) + (val32 >> 16);
    return static_cast<uint16_t>(val32);
  }
  uint16_t ones_cmpl_negate(uint16_t val) {
    return ~val;
  }
  uint16_t ones_cmpl_add(uint16_t A, uint16_t B) { // A+B
    return ones_cmpl_finalize(static_cast<uint32_t>(A) + static_cast<uint32_t>(B));
  }
  uint16_t ones_cmpl_subtract(uint16_t A, uint16_t B) { // A-B
    return ones_cmpl_add(A, ones_cmpl_negate(B));
  }
  Packet *make_tcp_packet(RmtObjectManager *om, uint64_t seed, int corrupted_byte,
                          uint16_t payload_len, uint32_t payload_first32byte_mask,
                          uint16_t *packet_checksum, uint16_t *payload_checksum,
                          bool set_ip_id_from_tcp_seg_len,
                          int payload_zero_len,
                          unsigned char *payload_first32byte) {
    // for any bit in payload_first32byte_mask that is zero, set the
    // corresponding payload byte to zero or to the corresponding byte in
    // payload_first32byte
    EXPECT_NE(UINT64_C(0), seed);
    Packet *pkt = NULL;
    std::default_random_engine gen;
    std::uniform_int_distribution<uint64_t> byte_rand(0,255);
    const char *mac_fmt = "08:00:%02X:%02X:%02X:%02X";
    const char *ip_fmt = "10.%d.%d.%d";
    char macsrc_buf[32], macdst_buf[32], ipsrc_buf[32], ipdst_buf[32];
    int b1, b2, b3, b4;

    // Seed random
    gen.seed(seed);

    // Create an Ethernet header
    Crafter::Ethernet eth_header;
    b1 = byte_rand(gen); b2 = byte_rand(gen); b3 = byte_rand(gen); b4 = byte_rand(gen);
    sprintf(macdst_buf, mac_fmt, b1, b2, b3, b4);
    eth_header.SetDestinationMAC(macdst_buf);

    b1 = byte_rand(gen); b2 = byte_rand(gen); b3 = byte_rand(gen); b4 = byte_rand(gen);
    sprintf(macsrc_buf, mac_fmt, b1, b2, b3, b4);
    eth_header.SetSourceMAC(macsrc_buf);

    // Create an IP header
    Crafter::IP ip_header;
    b1 = byte_rand(gen); b2 = byte_rand(gen); b3 = byte_rand(gen);
    sprintf(ipsrc_buf, ip_fmt, b1, b2, b3);
    ip_header.SetSourceIP(std::string(ipsrc_buf));
    b1 = byte_rand(gen); b2 = byte_rand(gen); b3 = byte_rand(gen);
    sprintf(ipdst_buf, ip_fmt, b1, b2, b3);
    ip_header.SetDestinationIP(std::string(ipdst_buf));

    // Maybe put TCP segment length into IP ID field so
    // we can calculate residuals for TCP correctly
    if (set_ip_id_from_tcp_seg_len) ip_header.SetIdentification(20 + payload_len);

    // NB. ALTERNATIVELY we put the ones-complement negation of the IP_HL*4 in
    // the IP ID field - then as long as we checksum the IP ID (~IP HL * 4)
    // and the IP TotalLength we can calculate residuals correctly, because the
    // TCP length in the TCP/IP pseudo-header is IP TotalLength + (~IP HL * 4)
    //ip_header.SetIdentification(ones_cmpl_negate(20));

    // Create a TCP header
    Crafter::TCP tcp_header;
    b1 = byte_rand(gen); b2 = byte_rand(gen); b3 = byte_rand(gen); b4 = byte_rand(gen);
    uint16_t src_port = static_cast<uint16_t>(b1*256 + b2);
    if (src_port == 0) src_port = 1;
    tcp_header.SetSrcPort(src_port);
    uint16_t dst_port = static_cast<uint16_t>(b3*256 + b4);
    if (dst_port == 0) dst_port = 1;
    tcp_header.SetDstPort(dst_port);

    if (payload_len > 0) {
      unsigned char payload_buf[payload_len];
      for (int i = 0; i < payload_len; i++) {
        payload_buf[i] = static_cast<unsigned char>(byte_rand(gen));
        if ((i < 32) && ((payload_first32byte_mask & (1u<<i)) == 0u)) {
          // Allow any/all of first 32 bytes to be fixed at 0
          if (nullptr == payload_first32byte) payload_buf[i] = 0;
          else payload_buf[i] = payload_first32byte[i];
        } else if (i < payload_zero_len) {
          payload_buf[i] = 0; // force byte to zero
        }
      }
      // A raw layer, this could be any array of bytes or chars
      Crafter::RawLayer payload(static_cast<const Crafter::byte *>(payload_buf), payload_len);
      // Create a packet
      Crafter::Packet the_packet = eth_header / ip_header / tcp_header / payload;
      unsigned char packet_buf[payload_len + 99];
      size_t packet_len = the_packet.GetData(packet_buf);
      if ((corrupted_byte >= 0) && (corrupted_byte < static_cast<int>(packet_len)))
        packet_buf[corrupted_byte] = ~packet_buf[corrupted_byte];
      Crafter::IP *ip = the_packet.GetLayer<Crafter::IP>();
      Crafter::TCP *tcp = the_packet.GetLayer<Crafter::TCP>();
      EXPECT_EQ(payload_len + 20 + 20, ip->GetTotalLength());
      pkt = om->pkt_create(packet_buf, packet_len);
      if (packet_checksum != NULL) *packet_checksum = tcp->GetCheckSum();
      if (payload_checksum != NULL) {
        uint32_t cksum = 0u;
        for (int i = 0; i < payload_len; i += 2) {
          uint8_t b0 = payload_buf[i];
          uint8_t b1 = (i+1 < payload_len) ?payload_buf[i+1] :0;
          uint32_t prev_cksum = cksum;
          cksum += (static_cast<uint32_t>(b0) << 8) + (static_cast<uint32_t>(b1));
          if (cksum_print_lots)
            printf("cksum[%3d]: 0x%08x + 0x%02x%02x -> 0x%08x\n", i, prev_cksum, b0, b1, cksum);
        }
        if (cksum_print_lots)
          printf("cksum[FIN]: 0x%08x ==> 0x%04x ==> 0x%04x\n", cksum,
                 ones_cmpl_finalize(cksum), ones_cmpl_negate(ones_cmpl_finalize(cksum)));
        *payload_checksum = ones_cmpl_negate(ones_cmpl_finalize(cksum));
      }
    } else {
      // Create a packet
      unsigned char packet_buf[99];
      Crafter::Packet the_packet = eth_header / ip_header / tcp_header;
      size_t packet_len = the_packet.GetData(packet_buf);
      if ((corrupted_byte >= 0) && (corrupted_byte < static_cast<int>(packet_len)))
        packet_buf[corrupted_byte] = ~packet_buf[corrupted_byte];
      Crafter::IP *ip = the_packet.GetLayer<Crafter::IP>();
      Crafter::TCP *tcp = the_packet.GetLayer<Crafter::TCP>();
      EXPECT_EQ(20 + 20, ip->GetTotalLength());
      pkt = om->pkt_create(packet_buf, packet_len);
      if (packet_checksum != NULL) *packet_checksum = tcp->GetCheckSum();
      if (payload_checksum != NULL) *payload_checksum = 0;
    }
    return pkt;
  }

  void BFN_TEST_NAME(CksumTestFixture)::SetUp() {
    BaseTest::SetUp();
    prsr_ = om_->parser_get(0,1)->ingress();
    ASSERT_TRUE(prsr_ != NULL);
    prsr_->reset();
    // Setup a priority map that does nothing
    prsr_->set_identity_priority_map();
  }

  void BFN_TEST_NAME(CksumTestFixture)::TearDown() {
    tu_->quieten_log_flags();
  }

  uint16_t BFN_TEST_NAME(CksumTestFixture)::get_residual_checksum(
      uint16_t payload_len,
      int payload_zero_len) {
    // make a packet whose first payload_zero_len bytes of payload are zero and
    // return its payload checksum
    uint16_t payload_checksum;
    Packet *pkt1 = make_tcp_packet(om_, seed_, -1, payload_len, 0xFFFFFFFF,
                                   NULL, &payload_checksum, true,
                                   payload_zero_len);
    EXPECT_TRUE(pkt1 != nullptr) << "Failed to make packet, using bogus cksum";
    if (pkt1 == nullptr) return 0;
    EXPECT_EQ(payload_len + hdr_len_, pkt1->len());
    om_->pkt_delete(pkt1);
    return payload_checksum;
  };

  void BFN_TEST_NAME(CksumTestFixture)::config_checksums(Parser *p) {
    // parse eth + ip + [tcp|udp] + 32bytes payload
    using CONF = ParserStaticConfig;
    // configure parser to perform a verify and residual checksum
    bool program_extractor0_checksum = RmtObject::is_jbay_or_later();
    // ENTRY 0:
    ParserStaticConfig::config_eth_header(prsr_, 255, 0, 1);

    // tweak index 255 to extract imm val 0xF to IP4_ERR - will be overridden
    // if any hdr cksum err
    uint8_t immediate_val = 0x0F;
    uint8_t  a0_u8_src[]  = {0xA0,0xA1,0xA2,immediate_val};
    uint16_t a0_u8_dst[]  = {CONF::PHV8_0, CONF::PHV8_1, CONF::PHV8_2, CONF::IP4_ERR};
    uint8_t  a0_u16_src[] = {0,6,12,0};
    uint16_t a0_u16_dst[] = {CONF::DA_HI_16,CONF::SA_HI_16,CONF::ETH_TYPE,CONF::NoX};
    uint8_t  a0_u32_src[] = {2,8,0,0};
    uint16_t a0_u32_dst[] = {CONF::DA_LO_32,CONF::SA_LO_32,CONF::NoX,CONF::NoX};

    p->set_action(255,
                  CONF::F, 0, CONF::b_FF, CONF::u8_00,                 // adj  RESET, adj_inc, checksum ENABLE, checksum_addr
                  CONF::b_TTTT, CONF::b_FFFF, a0_u8_src,  a0_u8_dst,   // extract8:  SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
                  CONF::b_FFFF, CONF::b_FFFF, a0_u16_src, a0_u16_dst,  // extract16: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
                  CONF::b_FFFF, CONF::b_FFFF, a0_u32_src, a0_u32_dst); // extract32: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
    // ENTRY 1:
    // Stripped off Ether header - see if we have Eth.Ethertype=IPv4 and IP.version=4 - stash IP fields

    // Also calculate/verify IP checksum - look at lower 20 bytes - set err-bit on IP4_ERR PHV
    uint8_t cksum_dst_bit = 7;
    //                    ROTL_F    SWAP        MASK  ROTR_P  ADD  DST_BIT
    p->set_checksum(0, 1, 0x0000, 0x0000, 0x000FFFFF,  false,   0, cksum_dst_bit,
                //  FINAL       DST_PHV   UPD? RESID? START?
                    true, CONF::IP4_ERR, true, false,  true);

    // Setup checksum entry 2 in checksum ram 1 - start accumulating TCP residual checksum
    // Here we accumulate checksum corresponding to TCP pseudo-header - so we checksum IP_SRC, IP_DST, IP_PROTO
    // and TCP_LEN - unfortunately there is NO field in IP/TCP packet hdrs corresponding to TCP segment length
    // but for the purposes of this unit-test the value is stashed in the IP_ID field so we checksum that
    // (set mask = 0xFF230)
    //
    // NB. If we put the ones_cmpl_negation of the IP_HL*4 in IP_ID (so 0xFFEB when IP_HL=5) then we can can
    //     get the same effect by checksumming the IP_ID *and* the IP_TOTAL_LENGTH
    //     (in that case set mask = 0xFF23C)
    //                    ROTL_F    SWAP        MASK  ROTR_P  ADD  DST_BIT
    p->set_checksum(1, 2, 0x0000, 0x0000, 0x000FF230,  false,   0,       0,
                //  FINAL DST_PHV   UPD? RESID? START?
                    false,      0, true,  true,  true);

    ParserStaticConfig::config_ip_header(p, 254, 1, 2);

    // Enable checksum engine 0 - use checksum addr=1. Enabled checksum engine 1 - use checksum addr=2
    p->set_checksum_enable(254, 0, true);
    p->set_checksum_ram_addr(254, 0, 1);
    p->set_checksum_enable(254, 1, true);
    p->set_checksum_ram_addr(254, 1, 2);
    if (program_extractor0_checksum) p->set_extract16_dst_phv_by_phv(254, 0, CONF::IP4_ERR);  // Force extractor0 (JBay)

    // ENTRY 2:
    // Stripped off IP header - see if we have IP_PROTO==TCP - stash TCP_SPORT/TCP_DPORT fields
    //
    // Setup checksum entry 3 in checksum ram 1 - continue to accumulate residual - checksum all TCP header bytes (20)
    //                    ROTL_F    SWAP        MASK  ROTR_P  ADD  DST_BIT
    p->set_checksum(1, 3, 0x0000, 0x0000, 0x000FFFFF,  false,   0,       0,
                //  FINAL DST_PHV   UPD? RESID? START?
                    false,      0, true,  true,  false);

    ParserStaticConfig::config_tcp_header(p, 253, 2, 3, false);

    // tweak default index 253 setup
    uint8_t  a2_u8_src[]  = {0,0,0,0};
    uint16_t a2_u8_dst[]  = {CONF::NoX,CONF::NoX,CONF::NoX,CONF::NoX};
    uint8_t  a2_u16_src[] = {0,2,16,0};
    uint16_t a2_u16_dst[] = {CONF::P_SPORT,CONF::P_DPORT,CONF::TCP_CKSM,CONF::NoX};
    uint8_t  a2_u32_src[] = {0,0,0,0};
    uint16_t a2_u32_dst[] = {CONF::NoX,CONF::NoX,CONF::NoX,CONF::NoX};

    // Enable checksum engine 1 - use checksum addr=3
    uint8_t   u8_03[] = { 0,3 };
    p->set_action(253,
                  CONF::F, 0, CONF::b_FT, u8_03,                       // adj  RESET, adj_inc, checksum **ENABLE**, checksum_addr=3
                  CONF::b_FFFF, CONF::b_FFFF, a2_u8_src,  a2_u8_dst,   // extract8:  SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
                  CONF::b_FFFF, CONF::b_FFFF, a2_u16_src, a2_u16_dst,  // extract16: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
                  CONF::b_FFFF, CONF::b_FFFF, a2_u32_src, a2_u32_dst); // extract32: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv

    // ENTRY 3:
    ParserStaticConfig::config_udp_header(p, 252, 2, 255);

    // ENTRY 4:
    // Stripped off TCP header - accumulate a residual from the next 32 bytes
    //
    // Setup checksum entry 4 in checksum ram 1 - continue to accumulate residual - checksum 32 bytes from payload
    // Update TCP_RESID PHV. Note we'll dynamically futz with this value packet by packet
    //                    ROTL_F    SWAP        MASK  ROTR_P  ADD  DST_BIT
    p->set_checksum(1, 4, 0x0000, 0x0000, 0x00000000,  false,   0,      31,
                //  FINAL         DST_PHV   UPD? RESID? START?
                    true, CONF::TCP_RESID, true,  true,  false);

    ParserStaticConfig::config_payload_n(p, 251, 3, 255, true);

    p->set_checksum_enable(251, 0, false);
    p->set_checksum_ram_addr(251, 0, 1);
    p->set_checksum_enable(251, 1, true);
    p->set_checksum_ram_addr(251, 1, 4);

    // ENTRY 5:
    ParserStaticConfig::config_catch_all(p, 250, 0, 255);

    p->set_parser_mode(RmtDefs::kParserChannels, RmtDefs::kParserMaxStates);
    p->set_channel(0, true, 0);

    // WIP requires hdr_len_inc to be set; ignored for jbay...
    std::list<int> states {255, 254, 253, 252, 251};
    for (int state : states) {
      p->set_hdr_len_inc(state, true);
    }
    // Dump out parser state
    p->print();
  }

  void BFN_TEST_NAME(CksumTestFixture)::config_residual_checksum(Parser *p) {
    // parse eth + ip + [tcp|udp] + 32bytes payload + 32bytes payload
    using CONF = ParserStaticConfig;
    // ENTRY 0:
    ParserStaticConfig::config_eth_header(p, 255, 0, 1);

    // ENTRY 1:
    // Stripped off Ether header - see if we have Eth.Ethertype=IPv4 and
    // IP.version=4 - stash IP fields
    ParserStaticConfig::config_ip_header(p, 254, 1, 2);
    // Setup checksum entry 2 in checksum ram 1 - start accumulating TCP
    // residual checksum Here we accumulate checksum corresponding to TCP
    // pseudo-header - so we checksum IP_SRC, IP_DST, IP_PROTO and TCP_LEN -
    // unfortunately there is NO field in IP/TCP packet hdrs corresponding to
    // TCP segment length but for the purposes of this unit-test the value is
    // stashed in the IP_ID field so we checksum that (set mask = 0xFF230)
    //
    // NB. If we put the ones_cmpl_negation of the IP_HL*4 in IP_ID (so 0xFFEB
    // when IP_HL=5) then we can can get the same effect by checksumming the
    // IP_ID *and* the IP_TOTAL_LENGTH (in that case set mask = 0xFF23C)
    //                    ROTL_F    SWAP        MASK  ROTR_P  ADD  DST_BIT
    p->set_checksum(1, 2, 0x0000, 0x0000, 0x000FF230,  false,   0,       0,
                 // FINAL DST_PHV   UPD? RESID? START?
                    false,      0, true,  true,  true);
    // Enable checksum engine 1 - use checksum addr=2
    p->set_checksum_enable(254, 1, true);
    p->set_checksum_ram_addr(254, 1, 2);

    // ENTRY 2:
    // Stripped off IP header - see if we have IP_PROTO==TCP - stash
    // TCP_SPORT/TCP_DPORT fields
    ParserStaticConfig::config_tcp_header(p, 253, 2, 3, false);
    // Setup checksum entry 3 in checksum ram 1 - continue to accumulate
    // residual - checksum all TCP header bytes (20)
    //                    ROTL_F    SWAP        MASK  ROTR_P  ADD  DST_BIT
    p->set_checksum(1, 3, 0x0000, 0x0000, 0x000FFFFF,  false,   0,       0,
                 // FINAL DST_PHV   UPD? RESID? START?
                    false,      0, true,  true, false);
    // Enable checksum engine 1 - use checksum addr=3
    p->set_checksum_enable(253, 1, true);
    p->set_checksum_ram_addr(253, 1, 3);

    // ENTRY 3:
    ParserStaticConfig::config_udp_header(p, 252, 2, 255);

    // ENTRY 4:
    // Stripped off TCP header - accumulate a residual from the next 32 bytes
    std::array<uint16_t, 4> dst_phv = {CONF::PHV8_4, CONF::PHV8_5,
                                       CONF::PHV8_6, CONF::PHV8_7};
    ParserStaticConfig::config_payload_n(p, 251, 3, 4, false, dst_phv);
    // Setup checksum entry 4 in checksum ram 1 - continue to accumulate
    // residual - checksum 32 bytes from payload; set residual dest phv to
    // TCP_RESID PHV.
    //                    ROTL_F    SWAP        MASK  ROTR_P  ADD  DST_BIT
    p->set_checksum(1, 4, 0x0000, 0x0000, 0xFFFFFFFF,  false,   0,      31,
                 // FINAL         DST_PHV   UPD? RESID? START?
                    true, CONF::TCP_RESID, true,  true, false);
    p->set_checksum_enable(251, 0, false);
    p->set_checksum_ram_addr(251, 0, 1);
    p->set_checksum_enable(251, 1, true);
    p->set_checksum_ram_addr(251, 1, 4);

    // ENTRY 5:
    // accumulate residual checksum in **automatic phase** from a further 32
    // bytes of payload
    std::array<uint16_t, 4> dst_phv2 = {CONF::PHV8_8, CONF::PHV8_9,
                                        CONF::PHV8_10, CONF::PHV8_11};
    ParserStaticConfig::config_payload_n(p, 250, 4, 255, true, dst_phv2);

    // ENTRY 6:
    ParserStaticConfig::config_catch_all(p, 100, 0, 255);

    p->set_parser_mode(RmtDefs::kParserChannels, RmtDefs::kParserMaxStates);
    p->set_channel(0, true, 0);
    // WIP requires hdr_len_inc to be set; ignored for jbay...
    std::list<int> states {255, 254, 253, 252, 251, 250};
    for (int state : states) {
      p->set_hdr_len_inc(state, true);
    }
  }

  TEST_F(BFN_TEST_NAME(CksumTestFixture),Residual) {
    using CONF = ParserStaticConfig;
    // Switch on some debug
    int      n_pkts = 10000, div = 100;
    om_->update_log_flags(ALL,ALL,TYP_PARSER_CHECKSUM,ALL,ALL,FEW,FEW);
    if (n_pkts < 100) om_->update_log_flags(ALL,ALL,TYP_PARSER_CHECKSUM,ALL,ALL,ALL,ALL);

    // config parser with checksums
    config_checksums(prsr_);
    // Dump out parser state
    prsr_->print();

    // Now define some packets to lookup
    //                     <   DA           SA      ET I4 C LN  ID  OF  T P CK   SRC     DST     SP  DP>
    //                     <   DA     ><    SA    ><ET><><><LN><ID><OF><><><CK>< SRC  >< DST   ><SP><DP>
    const char *pktstr1 = "080022AABBCC080011DDEEFF080045000100123400001011FFFF0A1122330A44556611881199"
                          "0000000000000000000000000000000000000000000000000000000000000000000000000000"
                          "0000000000000000000000000000000000000000000000000000000000000000000000000000";
    const char *pktstr2 = "080022AABBCC080011DDEEFF080045000100123400001006FFFF0A1122330A44556606880699"
                          "0000000000000000000000000000000000000000000000000000000000000000000000000000"
                          "0000000000000000000000000000000000000000000000000000000000000000000000000000";
    // First off create some Packets on heap
    Packet *pkt1 = om_->pkt_create(pktstr1);
    Packet *pkt2 = om_->pkt_create(pktstr2);
    EXPECT_EQ(114, pkt1->len());
    EXPECT_EQ(114, pkt2->len());

    // Do some basic lookups to check everything is working
    // UDP packet
    Phv *phv1 = prsr_->parse(pkt1, 0);
    ASSERT_TRUE(phv1 != NULL);
    EXPECT_EQ(0x1188u, phv1->get_p(CONF::P_SPORT));
    EXPECT_EQ(0x1199u, phv1->get_p(CONF::P_DPORT));
    EXPECT_EQ(0xA0u, phv1->get_p(CONF::PHV8_0));
    EXPECT_EQ(0xA1u, phv1->get_p(CONF::PHV8_1));
    EXPECT_EQ(0xA2u, phv1->get_p(CONF::PHV8_2));
    int cksum_dst_bit = 7;  // as per parser config
    int immediate_val = 0xF;  // as per parser config
    uint32_t expected_ip4_err = (0x1u << cksum_dst_bit) | immediate_val;
    EXPECT_EQ(expected_ip4_err, phv1->get_p(CONF::IP4_ERR)); // Expect checksum error
     // UDP - do not expect residual to be written: done state not entered
    EXPECT_EQ(0u, phv1->get_p(CONF::TCP_RESID));

    Phv *phv2 = prsr_->parse(pkt2, 0);
    EXPECT_EQ(0x0688u, phv2->get_p(CONF::P_SPORT));
    EXPECT_EQ(0x0699u, phv2->get_p(CONF::P_DPORT));
    EXPECT_EQ(0xA0u, phv2->get_p(CONF::PHV8_0));
    EXPECT_EQ(0xA1u, phv2->get_p(CONF::PHV8_1));
    EXPECT_EQ(0xA2u, phv2->get_p(CONF::PHV8_2));
    EXPECT_EQ(expected_ip4_err, phv2->get_p(CONF::IP4_ERR)); // Expect checksum error
    EXPECT_NE(0u, phv2->get_p(CONF::TCP_RESID)); // TCP - expect residual to be written

    // Cleanup
    om_->phv_delete(phv1);
    om_->phv_delete(phv2);
    om_->pkt_delete(pkt1);
    om_->pkt_delete(pkt2);

    // Seed determines prsr_mask, pkt_len and pkt contents
    std::default_random_engine gen;
    std::uniform_int_distribution<uint64_t> seed_rand;
    uint64_t seed0 = UINT64_C(123456789);
    gen.seed(seed0);
    uint64_t seed = seed_rand(gen);
    //seed = UINT64_C(0x0277ccfd56b036dd); // This one gets resid=0

    Packet *pkt;
    Phv *phv;
    uint16_t pkt_cksum, payload_cksum, prsr_cksum;
    uint16_t hdr_len, pkt_len, payload_len, prsr_resid;
    bool     sum_ok = true;
    prsr_->set_log_flags(UINT64_C(7));  // turn down warnings

    // Now create a number of random TCP packets
    for (int i = 0; i < n_pkts; i++) {

      // Setup a random 32b payload mask - the Parser will accumulate a
      // residual for {TCP PseudoHdr/TCP Hdr/these masked bytes in first 32B}
      // generating a residual for all *other* payload bytes
      uint32_t mask = static_cast<uint32_t>((seed >> 32) ^ (seed & 0xFFFFFFFF));

      // Program parser to use this mask when checksumming first 32B of payload
      prsr_->set_checksum(1, 4, 0x0000, 0x0000, mask, false, 0, 31,
                          true, CONF::TCP_RESID, true, true, false);

      // Make random packet - first time using inverted payload mask as
      // programmed into the Parser - this causes bytes that were NOT examined
      // while calculating residual to be left alone - but all examined bytes
      // to be set to 0. The *payload checksum* of this first packet should
      // match the residual calculated by the Parser over a second, 0xFFFFFFFF
      // masked packet in which NO bytes have been set to 0.
      //
      // Parse the 0xFFFFFFFF masked packet, accumulating a TCP checksum
      // residual under mask. Expect TCP residual to match payload checksum
      // from *first* packet
      //
      // Also should see various fields put into PHV words

      hdr_len = 14 + 20 + 20; // Eth=14 IP=20 TCP=20
      pkt_cksum = 0; payload_cksum = 0;
      payload_len = static_cast<uint16_t>(seed % UINT64_C(8999)) + 32; // Min size 32
      if ((cksum_print) && ((i < 10) || (!sum_ok)))
        printf("Packet[%d] LEN=%4d....\n", i, payload_len + hdr_len);
      EXPECT_LE(32, payload_len);
      EXPECT_GE(9999, payload_len);

      pkt = make_tcp_packet(om_, seed, -1, payload_len, ~mask, NULL, &payload_cksum);
      ASSERT_TRUE(pkt != NULL);
      EXPECT_EQ(payload_len + hdr_len, pkt->len());
      pkt_len = pkt->len();
      om_->pkt_delete(pkt);

      pkt = make_tcp_packet(om_, seed, -1, payload_len, 0xFFFFFFFF, &pkt_cksum, NULL);
      ASSERT_TRUE(pkt != NULL);
      EXPECT_EQ(payload_len + hdr_len, pkt->len());
      EXPECT_EQ(pkt_len, pkt->len());

      phv = prsr_->parse(pkt, 0); // Parse second packet
      ASSERT_TRUE(phv != NULL);
      prsr_cksum = phv->get_p(CONF::TCP_CKSM);
      prsr_resid = phv->get_p(CONF::TCP_RESID);
      EXPECT_EQ(0xFu, phv->get_p(CONF::IP4_ERR)); // Check no IP csum error
      EXPECT_EQ(pkt_cksum, prsr_cksum);  // Check extracted cksum matches one in pkt

      uint16_t sum = ones_cmpl_add(payload_cksum, prsr_resid);
      if ((cksum_print) && ((i < 10) || ((i % div) == 0) || (!sum_ok) || (sum == 0)))
        printf("Packet[%d] LEN=%4d PKT_CKSUM=0x%04x PAYLOAD_CKSUM=0x%04x  "
               "PRSR_CKSUM=0x%04x PRSR_RESID=0x%04x SUM=0x%04x\n",
               i, pkt_len, pkt_cksum, payload_cksum, prsr_cksum, prsr_resid, sum);
      om_->phv_delete(phv);
      om_->pkt_delete(pkt);

      if (!sum_ok) break;
      // allow sum to be
      // 0x0000 but only when payload_sum is 0xFFFF
      if (ones_cmpl_negate(payload_cksum) == 0xFFFF)
        sum_ok = ((sum == 0xFFFF) || (sum == 0x0000));
      else
        sum_ok = (sum == 0xFFFF);
      // Check residue + payload checksum == 0xFFFF (or 0x0000)
      EXPECT_TRUE(sum_ok);
      if (!sum_ok) {
        // Run this packet again but with more debug on
        cksum_print = true; cksum_print_lots = true;
        om_->update_log_flags(ALL,ALL,TYP_PARSER_CHECKSUM,ALL,ALL,ALL,ALL);
      } else {
        seed = seed_rand(gen);// New seed
      }

    }
  }


  TEST(BFN_TEST_NAME(CksumTest),Residual2) {
    // Configure parser from within test
    // Here will also try and validate TCP/UDP checksum for short packets
    // by using Parser counter capability
    // Checksum errors are deliberately induced
    GLOBAL_MODEL->Reset();
    if (cksum_print) RMT_UT_LOG_INFO("test_cksum_residual2()\n");
    RMT_UT_LOG_INFO("***** CHECKSUM_ERRs are deliberately induced *****\n");
    TestUtil tu(GLOBAL_MODEL.get());
    RmtObjectManager *om = tu.get_objmgr();
    bool jbay_or_later = RmtObject::is_jbay_or_later();
    int  CMG = (RmtDefs::kClotMinGap > 0) ?RmtDefs::kClotMinGap :0;

    bool      T = true;
    bool      F = false;
    uint16_t  NoX = k_phv::kBadPhv;
    uint16_t  DA_HI_16 = Phv::make_word(4,0);
    uint16_t  DA_LO_32 = Phv::make_word(0,0);
    uint16_t  SA_HI_16 = Phv::make_word(4,1);
    uint16_t  SA_LO_32 = Phv::make_word(0,1);
    uint16_t  ETH_TYPE = Phv::make_word(4,2);
    uint16_t  IP4_ERR  = Phv::make_word(2,30);
    uint16_t  IP4_HL   = Phv::make_word(2,0);
    uint16_t  IP4_TTL  = Phv::make_word(2,2);
    uint16_t  IP4_PROTO= Phv::make_word(2,3);
    uint16_t  IMM8_DST = Phv::make_word(2,6);
    uint16_t  IP4_LEN  = Phv::make_word(4,4);
    uint16_t  IP4_ID   = Phv::make_word(4,3);
    uint16_t  IP4_FRAG = Phv::make_word(4,5);
    uint16_t  IP4_CKSM = Phv::make_word(4,6);
    uint16_t  IP4_SRC  = Phv::make_word(0,2);
    uint16_t  IP4_DST  = Phv::make_word(0,3);
    uint16_t  P_SPORT  = Phv::make_word(4,7);
    uint16_t  P_DPORT  = Phv::make_word(4,8);
    uint16_t  TCP_CKSM = Phv::make_word(4,9);
    uint16_t  TCP_RESID= Phv::make_word(4,31);
    uint16_t  PROTO_ERR= Phv::make_word(2,31);
    uint16_t  PROT2_ERR= Phv::make_word(6,29);
    uint16_t  DUMMY    = Phv::make_word(6,26);
    // These next 3 unused - just as a reminder to AVOID!!!
    //uint16_t TM_STAT0= Phv::make_word(6,30);
    //uint16_t TM_STAT1= Phv::make_word(6,31);
    //uint16_t PHV8_0  = Phv::make_word(3,0);
    uint16_t  PHV8_1   = Phv::make_word(3,1);
    uint16_t  PHV8_2   = Phv::make_word(3,2);
    //uint16_t  PHV8_3   = Phv::make_word(3,3);
    //bool      b_TT[] = { T,T };
    bool      b_FF[] = { F,F };
    bool      b_TF[] = { T,F };
    bool      b_FT[] = { F,T };
    bool      b_FFFF[] = { F,F,F,F };
    bool      b_TTTT[] = { T,T,T,T };
    bool      b_FFTT[] = { F,F,T,T };
    uint8_t   u8_00[] = { 0,0 };
    uint8_t   u8_10[] = { 1,0 };
    uint8_t   u8_02[] = { 0,2 };
    uint8_t   u8_03[] = { 0,3 };
    uint8_t   u8_04[] = { 0,4 };
    uint8_t   u8_05[] = { 0,5 };
    uint8_t   u8_06[] = { 0,6 };
    uint8_t   u8_07[] = { 0,7 };
    uint8_t   u8_08[] = { 0,8 };
    uint8_t   u8_09[] = { 0,9 };
    uint8_t   u8_0A[] = { 0,10 };

    // Create a parser and initialize with config
    Parser *p = om->parser_get(0,0)->ingress();
    ASSERT_TRUE(p != NULL);
    p->reset();
    p->set_parser_mode(RmtDefs::kParserChannels, RmtDefs::kParserMaxStates);
    p->set_channel(0, true, 0);
    // Setup a priority map that does nothing
    p->set_identity_priority_map();
    // Also set HLA to 0 as no IPB metadata used here
    p->set_hdr_len_adj(0);

    // Arrange for TM_Status in last 2 16b PHV words (222,223)
    if (jbay_or_later) p->phv_set_tm_status_phv(0, 254); // Pipe0
    // OR arrange for TM_Status in 32b PHV word (0,31)
    //if (jbay_or_later) p->phv_set_tm_status_phv(0, 62); // Pipe0, 32b PHV word 0,31

    // Create another parser and make it snoop writes to first parser
    // This only works on JBay
    Parser *p2 = om->parser_get(0,2)->ingress();
    p2->reset();
    p2->set_parser_mode(RmtDefs::kParserChannels, RmtDefs::kParserMaxStates);
    p2->set_channel(0, true, 0);
    p2->set_identity_priority_map();
    p2->set_hdr_len_adj(0);

    bool fake_callback = jbay_or_later;
    bool link_snooping_parser = true; // May happen automatically if mem_ctrl reset val=0
    bool use_snooping_parser = jbay_or_later;
    bool program_extractor0_checksum = jbay_or_later;
    int  prsr_index = (use_snooping_parser) ?2 :0;

    // Maybe make p2 snoop p (only does anything on JBay)
    // (and this might happen automatically on reset)
    if (link_snooping_parser) p2->set_mem_ctrl(0);

    // All memory writes below will be to parser p
    // But parser p2 will get to see them too so should end up configured identically
    // If on JBay we then do packet lookup using p2 NOT p to check snooping works


    // Set ALL TCAM entries initially to match on state=255 (which we never use)
    // and to immediately exit (DONE=true) - should NEVER see these entries run
    //
    for (int i = 0; i < 256; i++) {
      p->set_tcam_match(i,                    // Use 0x99 in value field if we don't care
                        p->make_tcam_entry( 255, (uint16_t)0x9999, (uint8_t)0x99, (uint8_t)0x99),  // value (state=255)
                        p->make_tcam_entry(0xFF, (uint16_t)     0, (uint8_t)   0, (uint8_t)   0)); // mask
      if (fake_callback) p->memory_change_callback(ParserMemoryType::kSwTcam, 0, i);
      p->set_early_action(i,
                          0, F, F,    // Counter load src, LD_SRC, LOAD
                          T, 0,       // ****DONE***, shift_amount
                          0,0,0,      // field8_1 off, field8_0 off, field16 off
                          F,F,F,      // field8_1 LOAD, field8_0 LOAD, field16 LOAD ***NO LOAD***
                          0xFF, 255); // next_state_mask, next_state (IP)
      if (fake_callback) p->memory_change_callback(ParserMemoryType::kEarlyAction, 0, i);
      // WIP requires hdr_len_inc to be set; ignored for jbay...
      p->set_hdr_len_inc(i, true);
      p2->set_hdr_len_inc(i, true);
      // WIP requires ActionRam banks to be explicitly enabled
      p->set_action_ram_en(i, 3);
      p2->set_action_ram_en(i, 3);
    }




    // ENTRY 254 (reset state ZERO)
    // Raw packet header - match on anything
    //
    p->set_tcam_match(254,                    // Use 0x99 in value field if we don't care
                      p->make_tcam_entry(   0, (uint16_t)0x9999, (uint8_t)0x99, (uint8_t)0x99),  // value (state=0)
                      p->make_tcam_entry(0xFF, (uint16_t)     0, (uint8_t)   0, (uint8_t)   0)); // mask
    if (fake_callback) p->memory_change_callback(ParserMemoryType::kSwTcam, 0, 254);

    // Load ETH_TYPE and IP_VER from ETH header (assume *no* VLAN)
    p->set_early_action(254,
                        0, F, F,    // Counter load src, LD_SRC, LOAD
                        F, 14,      // DONE, shift_amount (strip off ETH - no VLAN)
                        14,0,12,    // field8_1 off (14=IP_VER), field8_0 off, field16 off (12=ETH_TYPE)
                        T,F,T,      // field8_1 LOAD, field8_0 LOAD, field16 LOAD
                        0xFF, 240); // next_state_mask, next_state (IP)
    if (fake_callback) p->memory_change_callback(ParserMemoryType::kEarlyAction, 0, 254);

    uint8_t  a254_u8_src[]  = {0xF,0x11,0x22,0x0F};
    uint16_t a254_u8_dst[]  = {IP4_ERR,PHV8_1,PHV8_2,PROTO_ERR};
    uint8_t  a254_u16_src[] = {0,6,12,0};
    uint16_t a254_u16_dst[] = {DA_HI_16,SA_HI_16,ETH_TYPE,NoX};
    uint8_t  a254_u32_src[] = {2,8,0,0};
    uint16_t a254_u32_dst[] = {DA_LO_32,SA_LO_32,NoX,NoX};

    // Stash DA/SA/ETH_TYPE
    p->set_action(254,
                  F, 0, b_FF, u8_00,                           // adj  RESET, adj_inc, checksum ENABLE, checksum_addr
                  b_TTTT, b_FFFF, a254_u8_src,  a254_u8_dst,   // extract8:  SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
                  b_FFFF, b_FFFF, a254_u16_src, a254_u16_dst,  // extract16: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
                  b_FFFF, b_FFFF, a254_u32_src, a254_u32_dst); // extract32: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
    // On JBay setup a CLOT tuple to capture ETH header
    // NB. Since XXX we store ClotLen-1 in ActionRam hence X-CMG => X-CMG-1 in set_action_clot
    if (jbay_or_later) p->set_action_clot(254,
                                 0, 0, 14-CMG-1, 1, // ClotIndex=0 ClotType=0 (IMM), ClotLen=14, ClotEN,
                                 0x3F, 0,           // ClotLenMask=0x3F, ClotLenAdd=0,
                                 0, 11, F, 0);      // ClotOffset=0 ClotTag=11 ClotTagOffsetAdd=false ClotHasCsum=0
    if (fake_callback) p->memory_change_callback(ParserMemoryType::kAction, 0, 254);



    // ENTRY 232 (state 240 == IP)
    // Stripped off Ether header - see if we have ETH_TYPE=IPv4 and IP_VER=4
    //
    p->set_tcam_match(232,                    // Use 0x99 in value field if we don't care
                      p->make_tcam_entry( 240, (uint16_t)0x0800, (uint8_t)0x40, (uint8_t)0x99),  // value (state=240)
                      p->make_tcam_entry(0xFF, (uint16_t)0xFFFF, (uint8_t)0xF0, (uint8_t)   0)); // mask
    if (fake_callback) p->memory_change_callback(ParserMemoryType::kSwTcam, 0, 232);

    // Now load IP4_ID and IP4_PROTO from IP header
    // We load IP4_ID as it contains TCP/UDP length - use this to allow full-checksum of short packets
    p->set_early_action(232,
                        0, F, F,    // Counter load src, LD_SRC, LOAD
                        F, 0,       // DONE, shift_amount (DONT get rid IP yet - may need to do residual checksum over IP fields)
                        9,0,4,      // field8_1 off(9=IP4_PROTO), field8_0 off, field16 off (4=IP4_ID)
                        T,F,T,      // field8_1 LOAD, field8_0 LOAD, field16 LOAD
                        0xFF, 220); // next_state_mask, next_state (TCP_or_UDP)
    // Set buf_req to 20 since we actually _want_ data in the buffer
    p->set_buf_req(232, 20);
    if (fake_callback) p->memory_change_callback(ParserMemoryType::kEarlyAction, 0, 232);

    // NB. XXX: ActionRam[232] must have phv_8b_dst_3 active or we'll see an error
    uint16_t a232_u8_ext3   = (jbay_or_later) ?NoX :IP4_ERR;
    uint8_t  a232_u8_src[]  = {0,8,9,0};
    uint16_t a232_u8_dst[]  = {IP4_HL,IP4_TTL,IP4_PROTO,a232_u8_ext3}; // XXX: extract3 ignored here (Tofino)
    uint8_t  a232_u16_src[] = {2,4,6,10};
    uint16_t a232_u16_dst[] = {IP4_LEN,IP4_ID,IP4_FRAG,IP4_CKSM};
    uint8_t  a232_u32_src[] = {12,16,0,0};
    uint16_t a232_u32_dst[] = {IP4_SRC,IP4_DST,NoX,NoX};

    // Also calculate/verify IP checksum - look at lower 20 bytes - set err-bit on IP4_ERR PHV
    //                    ROTL_F    SWAP        MASK  ROTR_P  ADD  DST_BIT  FINAL DST_PHV   UPD? RESID? START?
    p->set_checksum(0, 1, 0x0000, 0x0000, 0x000FFFFF,  false,   0,       7, true, IP4_ERR, true, false,  true);
    if (fake_callback) p->memory_change_callback(ParserMemoryType::kChecksum, 0, 1);


    // Stash IP fields. Enable checksum engine 0 - use checksum addr=1
    p->set_action(232,
                  F, 0, b_TF, u8_10,                           // adj  RESET, adj_inc, checksum **ENABLE**, checksum_addr=1
                  b_FFFF, b_FFFF, a232_u8_src,  a232_u8_dst,   // extract8:  SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
                  b_FFFF, b_FFFF, a232_u16_src, a232_u16_dst,  // extract16: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
                  b_FFFF, b_FFFF, a232_u32_src, a232_u32_dst); // extract32: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
    if (program_extractor0_checksum) p->set_extract16_dst_phv_by_phv(232, 0, IP4_ERR);  // Force extractor0 (JBay)
    if (fake_callback) p->memory_change_callback(ParserMemoryType::kAction, 0, 232);



    // ENTRY 218 (state 220 == TCP_or_UDP)
    // Stripped off ETH header only at this point - still got IP
    // See if we have IP_PROTO==TCP and see if we have a **SHORT** packet with an EVEN number of payload bytes
    //  (tcp hdr len + payload len must be even in [0-127])
    // (NB the TCP len is in the IP_ID field so just look at that)
    //
    p->set_tcam_match(218,                    // Use 0x99 in value field if we don't care
                      p->make_tcam_entry( 220, (uint16_t)0x0000, (uint8_t)0x06, (uint8_t)0x99),  // value (state=220 REUSE)
                      p->make_tcam_entry(0xFF, (uint16_t)0xFF81, (uint8_t)0xFF, (uint8_t)   0)); // mask
    if (fake_callback) p->memory_change_callback(ParserMemoryType::kSwTcam, 0, 218);

    // Setup counter to come from lo byte of f16 (0x0) and deduct 20 (TCP header len) from it (mask=7 ==> 0xFF)
    //                                           ADD MSK ROT   MAX  SRC ADD_TO_STACK
    p->set_counter_init(1, static_cast<uint8_t>(-20),  7,  0, 0xFF, 0x0,           1);
    if (fake_callback) p->memory_change_callback(ParserMemoryType::kCounter, 0, 1);


    p->set_early_action(218,
                        1, T, T,    // Counter load src(counter_init_entry=1), LD_SRC(true==> load from counter ram 1), **LOAD**
                        F, 20,      // DONE, shift_amount (now get rid IP)
                        0,0,0,      // field8_1 off, field8_0 off, field16 off
                        F,F,F,      // field8_1 LOAD, field8_0 LOAD, field16 LOAD **NO LOAD**
                        0xFF, 210); // next_state_mask, next_state (TCP)
    if (RmtObject::is_chip1_or_later()) {
      // NB set_early_action calls set_counter_ctr_op with an OP2 - this is mapped to an OP4 on WIP
      // CTR_AMT_IDX[3:0] must contain 1 to indicate we're using CounterInitRam[1]
      p->set_counter_load_imm(218, 1); // 1=use CounterInitRam[1]
    }
    if (fake_callback) p->memory_change_callback(ParserMemoryType::kEarlyAction, 0, 218);

    // TEST for XXX - extract val_const[0] == 256 into PHV8(2,6)=70 - should result in IMM8_DST==1 (MSB)
    uint8_t  a218_u8_src[]  = {62,0,1,0};
    uint16_t a218_u8_dst[]  = {IMM8_DST,NoX,NoX,NoX};
    uint8_t  a218_u16_src[] = {0,2,16,0};
    uint16_t a218_u16_dst[] = {NoX,NoX,NoX,NoX};
    uint8_t  a218_u32_src[] = {0,0,0,0};
    uint16_t a218_u32_dst[] = {NoX,NoX,NoX,NoX};

    // Setup checksum entry 9 in checksum ram 1 - **verify** TCP checksum
    // we checksum IP_SRC, IP_DST, IP_PROTO and TCP_LEN (from IP_ID field)
    //                    ROTL_F    SWAP        MASK  ROTR_P  ADD  DST_BIT  FINAL DST_PHV   UPD? RESID? START?
    p->set_checksum(1, 9, 0x0000, 0x0000, 0x000FF230,  false,   0,       0, false,      0, false, false,  true);
    if (fake_callback) p->memory_change_callback(ParserMemoryType::kChecksum, 1, 9);

    // No extracts. Enable checksum engine 1 - use checksum addr=9
    p->set_action(218,
                  F, 0, b_FT, u8_09,                           // adj  RESET, adj_inc, checksum **ENABLE**, checksum_addr=9
                  b_FFTT, b_FFFF, a218_u8_src,  a218_u8_dst,   // extract8:  SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
                  b_FFFF, b_FFFF, a218_u16_src, a218_u16_dst,  // extract16: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
                  b_FFFF, b_FFFF, a218_u32_src, a218_u32_dst); // extract32: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
    // On JBay setup a CLOT tuple to capture IP header
    // NB. Since XXX we store ClotLen-1 in ActionRam hence X-CMG => X-CMG-1 in set_action_clot
    if (jbay_or_later) p->set_action_clot(218,
                                 0, 0, 20-CMG-1, 1, // ClotIndex=0 ClotType=0 (IMM), ClotLen=20, ClotEN,
                                 0x3F, 0,           // ClotLenMask=0x3F, ClotLenAdd=0,
                                 0, 12, F, 0);      // ClotOffset=0 ClotTag=12 ClotTagOffsetAdd=false ClotHasCsum=0
    if (fake_callback) p->memory_change_callback(ParserMemoryType::kAction, 0, 218);


    // ENTRY 217 (state 210 == TCP)
    // Stripped off ETH header and IP header at this point
    // Recheck we have IP_PROTO==TCP (should do as got here from entry 218 which did NOT overwrite it)
    // On this path we've got a **SHORT** packet so we can just validate TCP checksum
    //
    p->set_tcam_match(217,                    // Use 0x99 in value field if we don't care
                      p->make_tcam_entry( 210, (uint16_t)0x0000, (uint8_t)0x06, (uint8_t)0x99),  // value (state=210)
                      p->make_tcam_entry(0xFF, (uint16_t)0xFF81, (uint8_t)0xFF, (uint8_t)   0)); // mask
    if (fake_callback) p->memory_change_callback(ParserMemoryType::kSwTcam, 0, 217);

    p->set_early_action(217,
                        0, F, F,    // Counter load src, LD_SRC, LOAD
                        F, 20,      // DONE, shift_amount (now get rid TCP)
                        0,0,0,      // field8_1 off, field8_0 off, field16 off
                        F,F,F,      // field8_1 LOAD, field8_0 LOAD, field16 LOAD **NO LOAD**
                        0xFF, 150); // next_state_mask, next_state (CHECKSUM_LOOP)
    if (fake_callback) p->memory_change_callback(ParserMemoryType::kEarlyAction, 0, 217);

    uint8_t  a217_u8_src[]  = {0,0,0,0};
    uint16_t a217_u8_dst[]  = {NoX,NoX,NoX,NoX};
    uint8_t  a217_u16_src[] = {0,2,16,0};
    uint16_t a217_u16_dst[] = {P_SPORT,P_DPORT,TCP_CKSM,NoX};
    uint8_t  a217_u32_src[] = {0,0,0,0};
    uint16_t a217_u32_dst[] = {NoX,NoX,NoX,NoX};

    // Setup checksum entry 10 in checksum ram 1 - continue to **verify** TCP checksum
    // Here we checksum all TCP header bytes (20)
    //                    ROTL_F    SWAP        MASK  ROTR_P  ADD  DST_BIT  FINAL DST_PHV   UPD? RESID? START?
    p->set_checksum(1, 10, 0x0000, 0x0000, 0x000FFFFF,  false,   0,       0, false,      0, true,  false,  false);
    if (fake_callback) p->memory_change_callback(ParserMemoryType::kChecksum, 1, 10);

    // Stash TCP_SPORT/TCP_DPORT fields - continue TCP residual checksum (checksum engine=1, checksum addr=10)
    p->set_action(217,
                  F, 0, b_FT, u8_0A,                           // adj  RESET, adj_inc, checksum **ENABLE**, checksum_addr=10(0xA)
                  b_FFFF, b_FFFF, a217_u8_src,  a217_u8_dst,   // extract8:  SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
                  b_FFFF, b_FFFF, a217_u16_src, a217_u16_dst,  // extract16: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
                  b_FFFF, b_FFFF, a217_u32_src, a217_u32_dst); // extract32: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
    // On JBay setup a CLOT tuple to capture TCP header
    // NB. Since XXX we store ClotLen-1 in ActionRam hence X-CMG => X-CMG-1 in set_action_clot
    if (jbay_or_later) p->set_action_clot(217,
                                 0, 0, 20-CMG-1, 1, // ClotIndex=0 ClotType=0 (IMM), ClotLen=20, ClotEN,
                                 0x3F, 0,           // ClotLenMask=0x3F, ClotLenAdd=0,
                                 0, 13, F, 0);      // ClotOffset=0 ClotTag=13 ClotTagOffsetAdd=false ClotHasCsum=0
    if (fake_callback) p->memory_change_callback(ParserMemoryType::kAction, 0, 217);






    // ENTRY 216 (state 220 == TCP_or_UDP)
    // Stripped off ETH header only at this point - still got IP
    // See if we have IP_PROTO==TCP
    //
    p->set_tcam_match(216,                    // Use 0x99 in value field if we don't care
                      p->make_tcam_entry( 220, (uint16_t)0x9999, (uint8_t)0x06, (uint8_t)0x99),  // value (state=220 REUSE)
                      p->make_tcam_entry(0xFF, (uint16_t)     0, (uint8_t)0xFF, (uint8_t)   0)); // mask
    if (fake_callback) p->memory_change_callback(ParserMemoryType::kSwTcam, 0, 216);

    p->set_early_action(216,
                        0, F, F,    // Counter load src, LD_SRC, LOAD
                        F, 20,      // DONE, shift_amount (now get rid IP)
                        0,0,0,      // field8_1 off, field8_0 off, field16 off
                        F,F,F,      // field8_1 LOAD, field8_0 LOAD, field16 LOAD **NO LOAD**
                        0xFF, 210); // next_state_mask, next_state (TCP)
    if (fake_callback) p->memory_change_callback(ParserMemoryType::kEarlyAction, 0, 216);

    // TEST for XXX - extract val_const[0] == 256 into PHV8(2,6)=70 - should result in IMM8_DST==1 (MSB)
    // Note: a216_u8_src is being used to pass in both an explicit src index
    // (62) by virtue of the first extractor NOT having its
    // _extract8_src_imm_val flag set, and to set up the const value via the
    // third extractor having its _extract8_src_imm_val flag set which causes
    // set_action to treat the src as a value for the const.
    uint8_t  a216_u8_src[]  = {62,0,1,0};
    uint16_t a216_u8_dst[]  = {IMM8_DST,NoX,NoX,NoX};
    uint8_t  a216_u16_src[] = {0,2,16,0};
    uint16_t a216_u16_dst[] = {NoX,NoX,NoX,NoX};
    uint8_t  a216_u32_src[] = {0,0,0,0};
    uint16_t a216_u32_dst[] = {NoX,NoX,NoX,NoX};

    // Setup checksum entry 2 in checksum ram 1 - start accumulating TCP residual checksum
    // Here we accumulate checksum corresponding to TCP pseudo-header - so we checksum IP_SRC, IP_DST, IP_PROTO
    // and TCP_LEN - unfortunately there is NO field in IP/TCP packet hdrs corresponding to TCP segment length
    // but for the purposes of this unit-test the value is stashed in the IP_ID field so we checksum that
    p->set_checksum(1, 2, 0x0000, 0x0000, 0x000FF230,  false,   0,       0, false,      0, true,  true,  true);
    if (fake_callback) p->memory_change_callback(ParserMemoryType::kChecksum, 1, 2);

    // No extracts. Enable checksum engine 1 - use checksum addr=2
    p->set_action(216,
                  F, 0, b_FT, u8_02,                           // adj  RESET, adj_inc, checksum **ENABLE**, checksum_addr=2
                  b_FFTT, b_FFFF, a216_u8_src,  a216_u8_dst,   // extract8:  SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
                  b_FFFF, b_FFFF, a216_u16_src, a216_u16_dst,  // extract16: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
                  b_FFFF, b_FFFF, a216_u32_src, a216_u32_dst); // extract32: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
    // On JBay setup a CLOT tuple to capture IP header
    // NB. Since XXX we store ClotLen-1 in ActionRam hence X-CMG => X-CMG-1 in set_action_clot
    if (jbay_or_later) p->set_action_clot(216,
                                 0, 0, 20-CMG-1, 1, // ClotIndex=0 ClotType=0 (IMM), ClotLen=20, ClotEN,
                                 0x3F, 0,           // ClotLenMask=0x3F, ClotLenAdd=0,
                                 0, 12, F, 0);      // ClotOffset=0 ClotTag=12 ClotTagOffsetAdd=false ClotHasCsum=0
    if (fake_callback) p->memory_change_callback(ParserMemoryType::kAction, 0, 216);



    // ENTRY 215 (state 210 == TCP)
    // Stripped off ETH header and IP header at this point
    // Recheck we have IP_PROTO==TCP (should do as got here from entry 216 which did NOT overwrite it)
    //
    p->set_tcam_match(215,                    // Use 0x99 in value field if we don't care
                      p->make_tcam_entry( 210, (uint16_t)0x9999, (uint8_t)0x06, (uint8_t)0x99),  // value (state=210)
                      p->make_tcam_entry(0xFF, (uint16_t)     0, (uint8_t)0xFF, (uint8_t)   0)); // mask
    if (fake_callback) p->memory_change_callback(ParserMemoryType::kSwTcam, 0, 215);

    p->set_early_action(215,
                        0, F, F,    // Counter load src, LD_SRC, LOAD
                        F, 20,      // DONE, shift_amount (now get rid TCP)
                        0,0,0,      // field8_1 off, field8_0 off, field16 off
                        F,F,F,      // field8_1 LOAD, field8_0 LOAD, field16 LOAD **NO LOAD**
                        0xFF, 170); // next_state_mask, next_state (ACCUMULATE_RESIDUAL)
    if (fake_callback) p->memory_change_callback(ParserMemoryType::kEarlyAction, 0, 215);

    uint8_t  a215_u8_src[]  = {0,0,0,0};
    uint16_t a215_u8_dst[]  = {NoX,NoX,NoX,NoX};
    uint8_t  a215_u16_src[] = {0,2,16,0};
    uint16_t a215_u16_dst[] = {P_SPORT,P_DPORT,TCP_CKSM,NoX};
    uint8_t  a215_u32_src[] = {0,0,0,0};
    uint16_t a215_u32_dst[] = {NoX,NoX,NoX,NoX};

    // Setup checksum entry 3 in checksum ram 1 - continue to accumulate TCP residual checksum
    // Here we checksum all TCP header bytes (20)
    //                    ROTL_F    SWAP        MASK  ROTR_P  ADD  DST_BIT  FINAL DST_PHV   UPD? RESID? START?
    p->set_checksum(1, 3, 0x0000, 0x0000, 0x000FFFFF,  false,   0,       0, false,      0, true,  true,  false);
    if (fake_callback) p->memory_change_callback(ParserMemoryType::kChecksum, 1, 3);

    // Stash TCP_SPORT/TCP_DPORT fields - continue TCP residual checksum (checksum engine=1, checksum addr=3)
    p->set_action(215,
                  F, 0, b_FT, u8_03,                           // adj  RESET, adj_inc, checksum **ENABLE**, checksum_addr=3
                  b_FFFF, b_FFFF, a215_u8_src,  a215_u8_dst,   // extract8:  SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
                  b_FFFF, b_FFFF, a215_u16_src, a215_u16_dst,  // extract16: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
                  b_FFFF, b_FFFF, a215_u32_src, a215_u32_dst); // extract32: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
    // On JBay setup a CLOT tuple to capture TCP header
    // NB. Since XXX we store ClotLen-1 in ActionRam hence X-CMG => X-CMG-1 in set_action_clot
    if (jbay_or_later) p->set_action_clot(215,
                                 0, 0, 20-CMG-1, 1, // ClotIndex=0 ClotType=0 (IMM), ClotLen=20, ClotEN,
                                 0x3F, 0,           // ClotLenMask=0x3F, ClotLenAdd=0,
                                 0, 13, F, 0);      // ClotOffset=0 ClotTag=13 ClotTagOffsetAdd=false ClotHasCsum=0
    if (fake_callback) p->memory_change_callback(ParserMemoryType::kAction, 0, 215);




    // ENTRY 206 (state 220 == TCP_or_UDP)
    // Stripped off ETH header only at this point - still got IP
    // See if we have IP_PROTO==UDP
    //
    p->set_tcam_match(206,                    // Use 0x99 in value field if we don't care
                      p->make_tcam_entry( 220, (uint16_t)0x9999, (uint8_t)0x11, (uint8_t)0x99),  // value (state=220 REUSE)
                      p->make_tcam_entry(0xFF, (uint16_t)     0, (uint8_t)0xFF, (uint8_t)   0)); // mask
    if (fake_callback) p->memory_change_callback(ParserMemoryType::kSwTcam, 0, 206);

    p->set_early_action(206,
                        0, F, F,    // Counter load src, LD_SRC, LOAD
                        F, 20,      // DONE, shift_amount (now get rid IP)
                        0,0,0,      // field8_1 off, field8_0 off, field16 off
                        F,F,F,      // field8_1 LOAD, field8_0 LOAD, field16 LOAD **NO LOAD**
                        0xFF, 200); // next_state_mask, next_state (UDP)
    if (fake_callback) p->memory_change_callback(ParserMemoryType::kEarlyAction, 0, 206);

    // TEST for XXX - extract val_const[0] == 256 into PHV8(2,6)=70 - should result in IMM8_DST==1 (MSB)
    uint8_t  a206_u8_src[]  = {62,0,1,0};
    uint16_t a206_u8_dst[]  = {IMM8_DST,NoX,NoX,NoX};
    uint8_t  a206_u16_src[] = {0,2,0,0};
    uint16_t a206_u16_dst[] = {NoX,NoX,NoX,NoX};
    uint8_t  a206_u32_src[] = {0,0,0,0};
    uint16_t a206_u32_dst[] = {NoX,NoX,NoX,NoX};

    // Setup checksum entry 4 in checksum ram 1 - start accumulating UDP residual checksum
    // Here we accumulate checksum corresponding to UDP pseudo-header - so we checksum IP_SRC, IP_DST, IP_PROTO
    // and UDP_LEN (note we're getting UDP_LEN from UDP header here - we get it AGAIN later
    //                    ROTL_F    SWAP        MASK  ROTR_P  ADD  DST_BIT  FINAL DST_PHV  UPD? RESID? START?
    p->set_checksum(1, 4, 0x0000, 0x0000, 0x000FF200,  false,   0,       0, false,      0, true,  true,  true);
    if (fake_callback) p->memory_change_callback(ParserMemoryType::kChecksum, 1, 4);

    // No extracts. Enable checksum engine 1 - use checksum addr=4
    p->set_action(206,
                  F, 0, b_FT, u8_04,                           // adj  RESET, adj_inc, checksum **ENABLE**, checksum_addr=4
                  b_FFTT, b_FFFF, a206_u8_src,  a206_u8_dst,   // extract8:  SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
                  b_FFFF, b_FFFF, a206_u16_src, a206_u16_dst,  // extract16: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
                  b_FFFF, b_FFFF, a206_u32_src, a206_u32_dst); // extract32: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
    // On JBay setup a CLOT tuple to capture IP header
    // NB. Since XXX we store ClotLen-1 in ActionRam hence X-CMG => X-CMG-1 in set_action_clot
    if (jbay_or_later) p->set_action_clot(206,
                                 0, 0, 20-CMG-1, 1, // ClotIndex=0 ClotType=0 (IMM), ClotLen=20, ClotEN,
                                 0x3F, 0,           // ClotLenMask=0x3F, ClotLenAdd=0,
                                 0, 12, F, 0);      // ClotOffset=0 ClotTag=12 ClotTagOffsetAdd=false ClotHasCsum=0
    if (fake_callback) p->memory_change_callback(ParserMemoryType::kAction, 0, 206);



    // ENTRY 205 (state 200 == UDP)
    // Stripped off ETH header and IP header at this point
    // Recheck we have IP_PROTO==UDP (should do as got here from entry 206 which did NOT overwrite it)
    //
    p->set_tcam_match(205,                    // Use 0x99 in value field if we don't care
                      p->make_tcam_entry( 200, (uint16_t)0x9999, (uint8_t)0x11, (uint8_t)0x99),  // value (state=200)
                      p->make_tcam_entry(0xFF, (uint16_t)     0, (uint8_t)0xFF, (uint8_t)   0)); // mask
    if (fake_callback) p->memory_change_callback(ParserMemoryType::kSwTcam, 0, 205);

    p->set_early_action(205,
                        0, F, F,    // Counter load src, LD_SRC, LOAD
                        F, 8,       // DONE, shift_amount (now get rid UDP)
                        0,0,0,      // field8_1 off, field8_0 off, field16 off
                        F,F,F,      // field8_1 LOAD, field8_0 LOAD, field16 LOAD **NO LOAD**
                        0xFF, 170); // next_state_mask, next_state (ACCUMULATE RESIDUAL)
    if (fake_callback) p->memory_change_callback(ParserMemoryType::kEarlyAction, 0, 205);

    uint8_t  a205_u8_src[]  = {0,0,0,0};
    uint16_t a205_u8_dst[]  = {NoX,NoX,NoX,NoX};
    uint8_t  a205_u16_src[] = {0,2,6,0};
    uint16_t a205_u16_dst[] = {P_SPORT,P_DPORT,TCP_CKSM,NoX};
    uint8_t  a205_u32_src[] = {0,0,0,0};
    uint16_t a205_u32_dst[] = {NoX,NoX,NoX,NoX};

    // Setup checksum entry 5 in checksum ram 1 - continue to accumulate UDP residual
    // Here we checksum all UDP header bytes (8)
    //                    ROTL_F    SWAP        MASK  ROTR_P  ADD  DST_BIT  FINAL DST_PHV   UPD? RESID? START?
    p->set_checksum(1, 5, 0x0000, 0x0000, 0x000000FF,  false,   0,       0, false,      0, true,  true,  false);
    if (fake_callback) p->memory_change_callback(ParserMemoryType::kChecksum, 1, 5);

    // Stash UDP_SPORT/UDP_DPORT fields - continue residual checksum (checksum engine=1, checksum addr=5)
    p->set_action(205,
                  F, 0, b_FT, u8_05,                           // adj  RESET, adj_inc, checksum **ENABLE**, checksum_addr=5
                  b_FFFF, b_FFFF, a205_u8_src,  a205_u8_dst,   // extract8:  SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
                  b_FFFF, b_FFFF, a205_u16_src, a205_u16_dst,  // extract16: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
                  b_FFFF, b_FFFF, a205_u32_src, a205_u32_dst); // extract32: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
    // On JBay setup a CLOT tuple to capture UDP header
    // NB. Since XXX we store ClotLen-1 in ActionRam hence X-CMG => X-CMG-1 in set_action_clot
    if (jbay_or_later) p->set_action_clot(205,
                                 0, 0, 8-CMG-1, 1, // ClotIndex=0 ClotType=0 (IMM), ClotLen=8, ClotEN,
                                 0x3F, 0,          // ClotLenMask=0x3F, ClotLenAdd=0,
                                 0, 13, F, 0);     // ClotOffset=0 ClotTag=13 ClotTagOffsetAdd=false ClotHasCsum=0
    if (fake_callback) p->memory_change_callback(ParserMemoryType::kAction, 0, 205);





    // ENTRY 170 (state 170 == ACCUMULATE_RESIDUAL)
    // Stripped off UDP/TCP header - accumulate a **RESIDUAL** checksum from the next 32 bytes
    p->set_tcam_match(170,                    // Use 0x99 in value field if we don't care
                      p->make_tcam_entry( 170, (uint16_t)0x9999, (uint8_t)0x99, (uint8_t)0x99),  // value (state=170)
                      p->make_tcam_entry(0xFF, (uint16_t)     0, (uint8_t)   0, (uint8_t)   0)); // mask
    if (fake_callback) p->memory_change_callback(ParserMemoryType::kSwTcam, 0, 170);

    // Setup checksum entry 6 in checksum ram 1 - finish accumulating residual - maybe checksum 32 bytes from payload
    // Update TCP_RESID PHV word.
    // Note we'll dynamically futz with this value packet by packet to checksum diff combinations of 32 bytes
    //                    ROTL_F    SWAP        MASK  ROTR_P  ADD  DST_BIT  FINAL   DST_PHV   UPD? RESID? START?
    p->set_checksum(1, 6, 0x0000, 0x0000, 0x00000000,  false,   0,      31, true, TCP_RESID, true,  true,  false);
    if (fake_callback) p->memory_change_callback(ParserMemoryType::kChecksum, 1, 6);

    p->set_early_action(170,
                        0, F, F,    // Counter load src, LD_SRC, LOAD
                        F, 32,      // DONE , shift_amount (get rid 32 bytes payload)
                        0,0,0,      // field8_1 off, field8_0 off, field16 off
                        T,T,T,      // field8_1 LOAD, field8_0 LOAD, field16 LOAD
                        0xFF, 100); // next_state_mask, next_state (CATCH_ALL)
    if (fake_callback) p->memory_change_callback(ParserMemoryType::kEarlyAction, 0, 170);

    uint8_t  a170_u8_src[]  = {0,0,0,0};
    uint16_t a170_u8_dst[]  = {NoX,NoX,NoX,NoX};
    uint8_t  a170_u16_src[] = {0,0,0,0};
    uint16_t a170_u16_dst[] = {NoX,NoX,NoX,NoX};
    uint8_t  a170_u32_src[] = {0,0,0,0};
    uint16_t a170_u32_dst[] = {NoX,NoX,NoX,NoX};

    // Enable checksum engine 1 - use checksum addr=6
    p->set_action(170,
                  F, 0, b_FT, u8_06,                           // adj  RESET, adj_inc, checksum **ENABLE**, checksum_addr=6
                  b_FFFF, b_FFFF, a170_u8_src,  a170_u8_dst,   // extract8:  SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
                  b_FFFF, b_FFFF, a170_u16_src, a170_u16_dst,  // extract16: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
                  b_FFFF, b_FFFF, a170_u32_src, a170_u32_dst); // extract32: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
    // On JBay setup a CLOT tuple to capture PAYLOAD 32B
    // NB. Since XXX we store ClotLen-1 in ActionRam hence X-CMG => X-CMG-1 in set_action_clot
    if (jbay_or_later) p->set_action_clot(170,
                                 0, 0, 32-CMG-1, 1, // ClotIndex=0 ClotType=0 (IMM), ClotLen=32, ClotEN,
                                 0x3F, 0,           // ClotLenMask=0x3F, ClotLenAdd=0,
                                 0, 14, F, 0);      // ClotOffset=0 ClotTag=14 ClotTagOffsetAdd=false ClotHasCsum=0
    if (fake_callback) p->memory_change_callback(ParserMemoryType::kAction, 0, 170);




    // ENTRY 150 (state == CHECKSUM_LOOP)
    // We use counters to loop through upto 256 bytes of packet 2 bytes at a time to VERIFY checksum
    // So this state gets hit repeatedly, the counter decrementing by 2 each time, and each time
    //  checksumming another 2 bytes of payload
    // Note below value/masks (FFTF/FFTF ==> eq0, FFFT/FFFT ==> lt0, FFFF/FFTT ==> gt0)
    // XXX: removed setting of VER1 field in value/masks passed to make_tcam_entry (first param *was* T)

    p->set_tcam_match(150,                    // Use 0x99 in value field if we don't care - ***** match if counter GT 0 *****
                      p->make_tcam_entry(F,F,F,F,  150, (uint16_t)0x9999, (uint8_t)0x99, (uint8_t)0x99),  // value (state=150 REUSE)
                      p->make_tcam_entry(F,F,T,T, 0xFF, (uint16_t)     0, (uint8_t)   0, (uint8_t)   0)); // mask
    if (fake_callback) p->memory_change_callback(ParserMemoryType::kSwTcam, 0, 150);

    // Note below LOAD=F so -2 is *added* to running counter value (otherwise counter would be reset to -2)
    uint8_t dec2 = static_cast<uint8_t>((int8_t)-2);
    p->set_early_action(150,
                        dec2, F, F, // Counter load src, LD_SRC, LOAD, **ADD -2 to COUNTER**
                        F, 2,       // DONE, shift_amount (get rid 2 bytes payload)
                        0,0,0,      // field8_1 off, field8_0 off, field16 off
                        F,F,F,      // field8_1 LOAD, field8_0 LOAD, field16 LOAD
                        0xFF, 150); // next_state_mask, next_state (CHECKSUM_LOOP)
    if (fake_callback) p->memory_change_callback(ParserMemoryType::kEarlyAction, 0, 150);

    uint8_t  a150_u8_src[]  = {0,0,0,0};
    uint16_t a150_u8_dst[]  = {NoX,NoX,NoX,NoX};
    uint8_t  a150_u16_src[] = {0,0,0,0};
    uint16_t a150_u16_dst[] = {NoX,NoX,NoX,NoX};
    uint8_t  a150_u32_src[] = {0,0,0,0};
    uint16_t a150_u32_dst[] = {NoX,NoX,NoX,NoX};

    // Setup checksum entry 7 in checksum ram 1 - continue to calculate checksum of short packet - examine 2 bytes
    //                    ROTL_F    SWAP        MASK  ROTR_P  ADD  DST_BIT  FINAL   DST_PHV   UPD? RESID? START?
    p->set_checksum(1, 7, 0x0000, 0x0000, 0x00000003,  false,   0,       7, false, PROT2_ERR, true, false, false);
    if (fake_callback) p->memory_change_callback(ParserMemoryType::kChecksum, 1, 7);

    // Enable checksum engine 1 - use checksum addr=7
    p->set_action(150,
                  F, 0, b_FT, u8_07,                           // adj  RESET, adj_inc, checksum **ENABLE**, checksum_addr=7
                  b_FFFF, b_FFFF, a150_u8_src,  a150_u8_dst,   // extract8:  SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
                  b_FFFF, b_FFFF, a150_u16_src, a150_u16_dst,  // extract16: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
                  b_FFFF, b_FFFF, a150_u32_src, a150_u32_dst); // extract32: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
    if (fake_callback) p->memory_change_callback(ParserMemoryType::kAction, 0, 150);



    // ENTRY 149 (state == CHECKSUM_LOOP)
    // We use counters to loop through upto 256 bytes of packet 2 bytes at a time to VERIFY checksum
    // This entry should match when the counter gets to 0
    p->set_tcam_match(149,                    // Use 0x99 in value field if we don't care - ***** matches when counter not GT 0 *****
                      p->make_tcam_entry( 150, (uint16_t)0x9999, (uint8_t)0x99, (uint8_t)0x99),  // value (state=150 REUSE)
                      p->make_tcam_entry(0xFF, (uint16_t)     0, (uint8_t)   0, (uint8_t)   0)); // mask
    if (fake_callback) p->memory_change_callback(ParserMemoryType::kSwTcam, 0, 149);

    p->set_early_action(149,
                        0, F, F,    // Counter load src, LD_SRC, LOAD
                        F, 0,       // DONE, shift_amount
                        0,0,0,      // field8_1 off, field8_0 off, field16 off
                        F,F,F,      // field8_1 LOAD, field8_0 LOAD, field16 LOAD
                        0xFF, 100); // next_state_mask, next_state (CATCH_ALL)
    if (fake_callback) p->memory_change_callback(ParserMemoryType::kEarlyAction, 0, 149);

    // NB. XXX: ActionRam[149] must have phv_16b_dst_3 active or we'll see an error
    uint16_t a149_u16_ext3  = (jbay_or_later) ?DUMMY :PROT2_ERR;
    uint8_t  a149_u8_src[]  = {0,0,0,0};
    uint16_t a149_u8_dst[]  = {NoX,NoX,NoX,NoX};
    uint8_t  a149_u16_src[] = {0,0,0,62}; // XXX: don't use src 0 when shifting 0 for dummy csum extract
    uint16_t a149_u16_dst[] = {NoX,NoX,NoX,a149_u16_ext3}; // XXX: extract3 ignored here (Tofino)
    uint8_t  a149_u32_src[] = {0,0,0,0};
    uint16_t a149_u32_dst[] = {NoX,NoX,NoX,NoX};

    // Continue checksum entry 6 in checksum ram 1 - now VERIFY UDP/TCP checksum of short packet
    //                    ROTL_F    SWAP        MASK  ROTR_P  ADD  DST_BIT  FINAL   DST_PHV   UPD? RESID? START?
    p->set_checksum(1, 8, 0x0000, 0x0000, 0x00000000,  false,   0,       7,  true, PROT2_ERR, true, false, false);
    if (fake_callback) p->memory_change_callback(ParserMemoryType::kChecksum, 1, 8);

    // Enable checksum engine 1 - use checksum addr=8
    p->set_action(149,
                  F, 0, b_FT, u8_08,                           // adj  RESET, adj_inc, checksum **ENABLE**, checksum_addr=8
                  b_FFFF, b_FFFF, a149_u8_src,  a149_u8_dst,   // extract8:  SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
                  b_FFFF, b_FFFF, a149_u16_src, a149_u16_dst,  // extract16: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
                  b_FFFF, b_FFFF, a149_u32_src, a149_u32_dst); // extract32: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
    if (program_extractor0_checksum) {
      // Force extractor0 to extract to PROT2_ERR (JBay or later)
      uint8_t extract_type = p->set_extract16_dst_phv_by_phv(149, 0, PROT2_ERR);
      if (RmtObject::is_chip1_or_later()) {
        // required to enable the extractor for WIP
        tu.parser_update_extract16_type_cnt(p, 149, extract_type, 1);
      }
    }
    if (fake_callback) p->memory_change_callback(ParserMemoryType::kAction, 0, 149);




    // ENTRY 100 (state == CATCH_ALL)
    // Catch-all entry - will match anything not matched above
    p->set_tcam_match(100,                    // Use 0x99 in value field if we don't care
                      p->make_tcam_entry(0x99, (uint16_t)0x9999, (uint8_t)0x99, (uint8_t)0x99),  // value (state=*)
                      p->make_tcam_entry(   0, (uint16_t)     0, (uint8_t)   0, (uint8_t)   0)); // mask
    if (fake_callback) p->memory_change_callback(ParserMemoryType::kSwTcam, 0, 100);

    p->set_early_action(100,
                        0, F, F,    // Counter load src, LD_SRC, LOAD
                        T, 0,       // ****DONE****, shift_amount
                        0,0,0,      // field8_1 off, field8_0 off, field16 off
                        F,F,F,      // field8_1 LOAD, field8_0 LOAD, field16 LOAD
                        0xFF, 100); // next_state_mask, next_state
    if (fake_callback) p->memory_change_callback(ParserMemoryType::kEarlyAction, 0, 100);

    uint8_t  a100_u8_src[]  = {0,0,0,0};
    uint16_t a100_u8_dst[]  = {NoX,NoX,NoX,NoX};
    uint8_t  a100_u16_src[] = {0,0,0,0};
    uint16_t a100_u16_dst[] = {NoX,NoX,NoX,NoX};
    uint8_t  a100_u32_src[] = {0,0,0,0};
    uint16_t a100_u32_dst[] = {NoX,NoX,NoX,NoX};

    p->set_action(100,
                  F, 0, b_FF, u8_00,                           // adj  RESET, adj_inc, checksum ENABLE, checksum_addr
                  b_FFFF, b_FFFF, a100_u8_src,  a100_u8_dst,   // extract8:  SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
                  b_FFFF, b_FFFF, a100_u16_src, a100_u16_dst,  // extract16: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
                  b_FFFF, b_FFFF, a100_u32_src, a100_u32_dst); // extract32: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
    if (fake_callback) p->memory_change_callback(ParserMemoryType::kAction, 0, 100);



    // Dump out parser state
    p->print();


    // Pick a Parser to use (try using 'snooping' Parser on JBay)
    Parser *pp = (use_snooping_parser) ?p2 :p;

    // Switch on some debug
    int      n_pkts = 10000, div = 100;
    uint64_t FEWER = UINT64_C(0x3);
    om->update_log_flags(ALL,ALL,TYP_PARSER_CHECKSUM,ALL,ALL,FEWER,FEWER);
    if (n_pkts < 100) om->update_log_flags(ALL,ALL,TYP_PARSER_CHECKSUM,ALL,ALL,ALL,ALL);


    // Now define some packets to lookup
    //                    <   DA           SA      ET I4 C LN  ID  OF  T P CK   SRC     DST     SP  DP>
    //                    <   DA     ><    SA    ><ET><><><LN><ID><OF><><><CK>< SRC  >< DST   ><SP><DP>
    const char *pktstr1 = "080022AABBCC080011DDEEFF080045000100123400001011FFFF0A1122330A44556611881199"
                          "0000000000000000000000000000000000000000000000000000000000000000000000000000"
                          "0000000000000000000000000000000000000000000000000000000000000000000000000000";
    const char *pktstr2 = "080022AABBCC080011DDEEFF080045000100123400001006FFFF0A1122330A44556606880699"
                          "0000000000000000000000000000000000000000000000000000000000000000000000000000"
                          "0000000000000000000000000000000000000000000000000000000000000000000000000000";
    // First off create some Packets on heap
    Packet *pkt1 = om->pkt_create(pktstr1);
    Packet *pkt2 = om->pkt_create(pktstr2);
    EXPECT_EQ(114, pkt1->len());
    EXPECT_EQ(114, pkt2->len());

    // Do some basic lookups to check everything is working
    Phv *phv1 = pp->parse(pkt1, 0); // Use p or p2
    ASSERT_TRUE(phv1 != NULL);
    EXPECT_EQ(0x0A112233u, phv1->get(IP4_SRC));
    EXPECT_EQ(0x0A445566u, phv1->get(IP4_DST));
    EXPECT_EQ(0x1188u, phv1->get_p(P_SPORT));
    EXPECT_EQ(0x1199u, phv1->get_p(P_DPORT));
    EXPECT_EQ(0x11u, phv1->get_p(PHV8_1));
    EXPECT_EQ(0x22u, phv1->get_p(PHV8_2));
    if (jbay_or_later) {
      EXPECT_EQ(0x1u, phv1->get_p(IMM8_DST));
    }
    EXPECT_NE(0xFu, phv1->get_p(IP4_ERR)); // Expect checksum error
    Phv *phv2 = pp->parse(pkt2, 0); // Use p or p2
    EXPECT_EQ(0x0A112233u, phv1->get(IP4_SRC));
    EXPECT_EQ(0x0A445566u, phv1->get(IP4_DST));
    EXPECT_EQ(0x0688u, phv2->get_p(P_SPORT));
    EXPECT_EQ(0x0699u, phv2->get_p(P_DPORT));
    EXPECT_EQ(0x11u, phv2->get_p(PHV8_1));
    EXPECT_EQ(0x22u, phv2->get_p(PHV8_2));
    if (jbay_or_later) {
      EXPECT_EQ(0x1u, phv1->get_p(IMM8_DST));
    }
    EXPECT_NE(0xFu, phv2->get_p(IP4_ERR)); // Expect checksum error

    // Cleanup
    om->phv_delete(phv1);
    om->phv_delete(phv2);
    om->pkt_delete(pkt1);
    om->pkt_delete(pkt2);


    // Seed determines prsr_mask, pkt_len and pkt contents
    std::default_random_engine gen;
    std::uniform_int_distribution<uint64_t> seed_rand;
    uint64_t seed0 = UINT64_C(123456789);
    gen.seed(seed0);
    uint64_t seed = seed_rand(gen);
    //seed = UINT64_C(0x0277ccfd56b036dd); // This one gets resid=0

    Packet  *pkt = NULL;
    Phv     *phv;
    uint32_t tm_status_lsb, qinfo = 0u;
    uint16_t pkt_cksum, payload_cksum, prsr_cksum;
    uint16_t hdr_len, pkt_len, payload_len, prsr_resid;
    bool     pkt_ok = true, sum_ok = true;
    RmtLoggerCapture log_capture(om);

    // Now create a number of random TCP packets
    for (int i = 0; i < n_pkts; i++) {
      SCOPED_TRACE(i);
      // Setup a random 32b payload mask - the Parser will accumulate a
      // residual for {TCP PseudoHdr/TCP Hdr/these masked bytes in first 32B}
      // generating a residual for all *other* payload bytes
      uint32_t mask = static_cast<uint32_t>((seed >> 32) ^ (seed & 0xFFFFFFFF));

      // Program parser to use this mask when checksumming first 32B of payload
      // (use original Parser p - write should be snooped)
      p->set_checksum(1, 6, 0x0000, 0x0000, mask, false, 0, 31,
                      true, TCP_RESID, true, true, false);
      if (fake_callback) p->memory_change_callback(ParserMemoryType::kChecksum, 1, 6);

      // Make random packet - first time using inverted payload mask as
      // programmed into the Parser - this causes bytes that were NOT examined
      // while calculating residual to be left alone - but all examined bytes
      // to be set to 0. The *payload checksum* of this first packet should
      // match the residual calculated by the Parser over a second, 0xFFFFFFFF
      // masked packet in which NO bytes have been set to 0.
      //
      // Parse the 0xFFFFFFFF masked packet, accumulating a TCP checksum
      // residual under mask. Expect TCP residual to match payload checksum
      // from *first* packet
      //
      // Also should see various fields put into PHV words

      hdr_len = 14 + 20 + 20; // Eth=14 IP=20 TCP=20
      pkt_cksum = 0; payload_cksum = 0;
      bool short_pkt = false, corrupted_pkt = false;
      // One time in 10 force a short packet
      if ((seed % UINT64_C(10)) == UINT64_C(0)) {
        // Make payload SHORT (in [32,107]) and EVEN length for moment
        payload_len = static_cast<uint16_t>(seed % UINT64_C(76)) + 32;
        if ((payload_len % 2) == 1) payload_len--;
        short_pkt = true;
      } else {
        // Allow payload in [32,8960] bytes
        payload_len = static_cast<uint16_t>(seed % UINT64_C(8929)) + 32; // Min size 32
        short_pkt = ((payload_len <= 107) && ((payload_len % 2) == 0));
      }
      // Every 10 short packets corrupt 1
      corrupted_pkt = ((short_pkt) && ((seed % UINT64_C(100)) == UINT64_C(0)));

      if ((cksum_print) && ((i < 100) || (!sum_ok)))
        printf("Packet[%d] LEN=%4d....\n", i, payload_len + hdr_len);
      EXPECT_LE(32, payload_len);
      EXPECT_GE(9999, payload_len);

      pkt = make_tcp_packet(om, seed, -1, payload_len, ~mask, NULL, &payload_cksum);
      ASSERT_TRUE(pkt != NULL);
      EXPECT_EQ(payload_len + hdr_len, pkt->len());
      pkt_len = pkt->len();
      om->pkt_delete(pkt);

      // If we're going to corrupt a pkt always corrupt first payload byte
      int corrupted_byte = corrupted_pkt ?hdr_len :-1;
      pkt = make_tcp_packet(om, seed, corrupted_byte, payload_len, 0xFFFFFFFF, &pkt_cksum, NULL);
      ASSERT_TRUE(pkt != NULL);
      EXPECT_EQ(payload_len + hdr_len, pkt->len());
      EXPECT_EQ(pkt_len, pkt->len());

      // Set PacketPipeData within short pkts - try and capture counters per cycle
      if (short_pkt) {
        pkt->set_pipe_data_ctrl(true, prsr_index, PacketData::kPrsrCntr, PacketDataCtrl::kCalcAndStore);
      }
      // Configure some q_info to see if TM_Status injection works
      // Fake inbound QID from packet length and fake QLEN from packet counter
      // (this will only have any effect on JBay)
      if ((i%2) == 0) {
        // Only update qinfo and push as TM_Status on EVEN packets.
        // (ODD packets should reuse previous TM_Status (with MSB set))
        qinfo =  ((pkt->len() & 0x7FFF) << 16) | (i & 0xFFFF);
        tm_status_lsb = i & 0x000F;  // arbitrary value, not used
        pp->set_tm_status_input(qinfo, tm_status_lsb);
      }

      // captured logs have many lines and are therefore slow to search, so
      // only check logging for a subsample of iterations
      bool do_capture = jbay_or_later && (i > 0) && (i < 5);
      if (do_capture) {
        // switch on more P4 logging
        om->update_log_type_levels(0x1, ALL, RMT_LOG_TYPE_P4, ALL, RmtDebug::kRmtDebugVerbose);
        log_capture.start();
      }
      phv = pp->parse(pkt, 0); // Parse second packet (use p or p2)
      if (do_capture) {
        tu.quieten_p4_log_flags(0x1);
        log_capture.stop();
      }
      ASSERT_TRUE(phv != NULL);

      // Verify captured per-cycle counters for short packets (in the case of short pkts
      // parse tree does checksum *validation* using counters - expect counter to tick down)
      if (short_pkt) {
        int first_cycle = 3; // Function of parse graph - in our case counting starts cycle 3
        int n_cycles = payload_len / 2; // Should count down through payload to verify checksum
        int last_cycle = std::min(first_cycle + n_cycles, 255); // Only 256 cycles ever captured
        //printf("Packet[%d] SHORT LEN=%d PAYLOAD_LEN=%d....\n", i, pkt_len, payload_len);
        for (int cycle = first_cycle; cycle <= last_cycle; cycle++) {
          uint64_t data  = pkt->get_pipe_data(true, prsr_index, PacketData::kPrsrCntr, cycle * 64, 64);
          uint8_t  data8 = static_cast<uint8_t>(data & UINT64_C(0xFF)); // LO Byte is counter_
          int   act_cntr = static_cast<int>(data8);
          int   exp_cntr = payload_len - ((cycle - first_cycle) * 2);
          // On cycle 3 should see cntr initialised with payload_len
          // Each subsequent cycle cntr should tick down by 2
          // printf("Cycle[%d] Cntr=%d (%d)\n", cycle, act_cntr, exp_cntr);
          EXPECT_EQ(act_cntr, exp_cntr);
        }
      }

      // If JBay, qinfo we pushed in should now be in last 2 16b PHV words
      if (jbay_or_later) {
        uint32_t msb = ((i%2) == 0) ?0x0 :0x80000000;
        uint32_t qlo = phv->get_p(222);
        uint32_t qhi = phv->get_p(223);
        uint32_t qhilo = (qhi<<16)|(qlo<<0);
        //qhilo = phv->get_p(31); // OR in 32b PHV word (0,31)
        EXPECT_EQ(msb|qinfo, qhilo);
        // Also check IMM8_DST as expected
        EXPECT_EQ(0x1u, phv->get_p(IMM8_DST));
        if (do_capture) {
          // check logging lines include TM status being written to PHV
          char expected_str[128];
          sprintf(expected_str,
                  "Ingress Parser emitted TM status 0x%08x, "
                  "updating PHV word 223 to value 0x%04x ",
                  qhilo, qhi);
          int line_count = log_capture.for_each_line_containing(expected_str, nullptr);
          EXPECT_EQ(line_count, 1) << "Expected string:\n" << expected_str
                                   << "not found in:\n" << log_capture.dump_lines();
          sprintf(expected_str,
                  "Ingress Parser emitted TM status 0x%08x, "
                  "updating PHV word 222 to value 0x%04x ",
                  qhilo, qlo);
          line_count = log_capture.for_each_line_containing(expected_str, nullptr);
          EXPECT_EQ(line_count, 1) << "Expected string:\n" << expected_str
                                   << "not found in:\n" << log_capture.dump_lines();
          EXPECT_EQ(line_count, 1) << log_capture.dump_lines();
        }
      }

      prsr_cksum = phv->get_p(TCP_CKSM);
      prsr_resid = (short_pkt) ?0 :phv->get_p(TCP_RESID);
      EXPECT_EQ(0xFu, phv->get_p(IP4_ERR)); // Check no IP csum error
      EXPECT_EQ(pkt_cksum, prsr_cksum);  // Check extracted cksum matches one in pkt
      uint16_t sum = (short_pkt) ?0xFFFF :ones_cmpl_add(payload_cksum, prsr_resid);

      Clot *clot = pkt->clot();
      if (jbay_or_later) {
        int HLA = p->hdr_len_adj(); // Use original Parser
        // Do some JBay specific checks: 1. CLOT check
        int len, off;
        uint16_t csum;
        ASSERT_TRUE(clot != NULL);
        EXPECT_LE(3, clot->n_tags_set());
        EXPECT_GE(4, clot->n_tags_set());
        for (int i = 0; i < 11; i++) {
          EXPECT_FALSE(clot->get_signed(i, &len, &off, &csum));
        }
        EXPECT_TRUE(clot->get_signed(11, &len, &off, &csum));
        EXPECT_EQ(14-CMG, len); EXPECT_EQ( 0-HLA, off);
        EXPECT_TRUE(clot->get_signed(12, &len, &off, &csum));
        EXPECT_EQ(20-CMG, len); EXPECT_EQ(14-HLA, off);
        EXPECT_TRUE(clot->get_signed(13, &len, &off, &csum));
        EXPECT_EQ(20-CMG, len); EXPECT_EQ(34-HLA, off);
        if (clot->n_tags_set() == 4) {
          EXPECT_TRUE(clot->get_signed(14, &len, &off, &csum));
          EXPECT_EQ(32-CMG, len); EXPECT_EQ(54-HLA, off);
        }
        for (int i = 15; i < 64; i++) {
          EXPECT_FALSE(clot->get_signed(i, &len, &off, &csum));
        }
      } else {
        ASSERT_TRUE(clot == NULL);
      }


      if ((cksum_print) &&
          ((i < 100) || ((i % div) == 0) || (!pkt_ok) || (!sum_ok) || (sum == 0)))
        printf("Packet[%d] LEN=%4d PKT_CKSUM=0x%04x PAYLOAD_CKSUM=0x%04x  "
               "PRSR_CKSUM=0x%04x PRSR_RESID=0x%04x SUM=0x%04x %s\n",
               i, pkt_len, pkt_cksum, payload_cksum, prsr_cksum, prsr_resid, sum,
               short_pkt ?"***SHORT PACKET***" :"");

      if (!sum_ok || !pkt_ok) break;
      if (short_pkt) {
        // We may have deliberately corrupted a payload byte - check we spot it
        //pkt_ok = (corrupted_pkt) ?(phv->get_p(PROTO_ERR) != 0xF) :(phv->get_p(PROTO_ERR) == 0xF);
        pkt_ok = (corrupted_pkt) ?(phv->get_p(PROT2_ERR) != 0) :(phv->get_p(PROT2_ERR) == 0);
        sum_ok = true;
      } else {
        pkt_ok = true;
        // From Hangout with GG 09/05/16 - allow sum to be
        // 0x0000 but only when payload_sum is 0xFFFF
        if (ones_cmpl_negate(payload_cksum) == 0xFFFF)
          sum_ok = ((sum == 0xFFFF) || (sum == 0x0000));
        else
          sum_ok = (sum == 0xFFFF);
      }

      om->phv_delete(phv);
      if (i == n_pkts-1) {
        // Output final Clot
        om->update_log_flags(ALL,ALL,TYP_PARSER_CHECKSUM,ALL,ALL,ALL,ALL);
        p->clot_print(pkt);
      }
      om->pkt_delete(pkt);

      // Check residue + payload checksum == 0xFFFF (or 0x0000)
      EXPECT_TRUE(sum_ok);
      EXPECT_TRUE(pkt_ok);
      if (!sum_ok || !pkt_ok) {
        // Run this packet again but with more debug on
        cksum_print = true; cksum_print_lots = true;
        om->update_log_flags(ALL,ALL,TYP_PARSER_CHECKSUM,ALL,ALL,ALL,ALL);
      } else {
        seed = seed_rand(gen);// New seed
      }

    }

    // Schtumm
    tu.quieten_log_flags();
    RMT_UT_LOG_INFO("***** CHECKSUM_ERRs are deliberately induced *****\n");
  }

TEST_F(BFN_TEST_NAME(CksumTestFixture),ResidualAuto) {
  // verify that automatic residual checksum includes all bytes after final
  // programmed state
  //om_->update_log_flags(ALL,ALL,TYP_PARSER_CHECKSUM,ALL,ALL,ALL,ALL);
  prsr_->set_log_flags(UINT64_C(7));

  // set hdr_len_adj to zero so that remaining tests are consistent with no IPB
  // metatdata being prepended
  prsr_->set_hdr_len_adj(0);

  Packet *pkt;
  uint16_t payload_len = 70;
  Phv *phv;
  const int parser_chan = 0;

  auto sanity_check_phv = [&pkt, &phv](int phv8_10_pkt_byte) {
    // phv8_10_pkt_byte is pkt byte index that we expect to be extracted to
    // PHV8_10; this will vary as the test changes the payload state shift
    // amounts
    ASSERT_TRUE(phv != nullptr);
    // check sample of expected extractions
    EXPECT_EQ(0x33u, phv->get_p(ParserStaticConfig::PHV8_3));
    uint8_t expected[1];
    pkt->get_buf(expected, phv8_10_pkt_byte, 1);
    EXPECT_EQ(expected[0], phv->get_p(ParserStaticConfig::PHV8_10));
  };

  config_residual_checksum(prsr_);

  // vary final pos in the final *programmed* residual checksum state; this
  // checks that when final_pos < shift then the remaining bytes after final
  // pos are accumulated by *automatic* residual checksum
  for (int final_pos = 0; final_pos < 32; final_pos++) {
    SCOPED_TRACE(final_pos);
    prsr_->set_checksum(1, 4, 0x0000, 0x0000, 0xFFFFFFFF, false, 0, final_pos,
                        true, ParserStaticConfig::TCP_RESID, true,  true, false);
    pkt = make_tcp_packet(om_, seed_, -1, payload_len, 0xFFFFFFFF,
                          nullptr, nullptr);
    ASSERT_TRUE(pkt != nullptr);
    phv = prsr_->parse(pkt, parser_chan);
    sanity_check_phv(102);
    EXPECT_EQ(hdr_len_ + 64, pkt->orig_hdr_len());
    uint16_t prsr_resid = phv->get_p(ParserStaticConfig::TCP_RESID);
    uint16_t expected_cksum = get_residual_checksum(payload_len, 64);
    uint16_t sum = ones_cmpl_add(expected_cksum, prsr_resid);
    if (ones_cmpl_negate(expected_cksum) == 0xFFFF)
      EXPECT_TRUE((sum == 0xFFFF) || (sum == 0x0000));
    else
      EXPECT_EQ(0xFFFF, sum);
    om_->phv_delete(phv);
    om_->pkt_delete(pkt);
    prsr_->set_checksum(1, 4, 0x0000, 0x0000, 0xFFFFFFFF, false, 0, 31,
                        true, ParserStaticConfig::TCP_RESID, true,  true, false);
  }

  {
    // tweak first 32bytes payload state to only shift 31 bytes; this will
    // check that residual checksum rotbuf logic correctly deals with odd
    // number of bytes in a state when progressing to next state
    prsr_->set_shift_amount(251, 31);
    prsr_->set_checksum(1, 4, 0x0000, 0x0000, 0x7FFFFFFF, false, 0, 30,
                        true, ParserStaticConfig::TCP_RESID, true,  true, false);
    pkt = make_tcp_packet(om_, seed_, -1, payload_len, 0xFFFFFFFF,
                          nullptr, nullptr);
    ASSERT_TRUE(pkt != nullptr);
    phv = prsr_->parse(pkt, parser_chan);
    sanity_check_phv(101);
    EXPECT_EQ(hdr_len_ + 63, pkt->orig_hdr_len());
    uint16_t prsr_resid = phv->get_p(ParserStaticConfig::TCP_RESID);
    // only 63 bytes of payload are parsed and included in residual checksum...
    uint16_t expected_cksum = get_residual_checksum(payload_len, 63);
    uint16_t sum = ones_cmpl_add(expected_cksum, prsr_resid);
    if (ones_cmpl_negate(expected_cksum) == 0xFFFF)
      EXPECT_TRUE((sum == 0xFFFF) || (sum == 0x0000));
    else
      EXPECT_EQ(0xFFFF, sum);
    om_->phv_delete(phv);
    om_->pkt_delete(pkt);
    prsr_->set_shift_amount(251, 32);
    prsr_->set_checksum(1, 4, 0x0000, 0x0000, 0x7FFFFFFF, false, 0, 31,
                        true, ParserStaticConfig::TCP_RESID, true,  true, false);
  }
  {
    // repeat, but tweak parser config so programmed residual checksum ends in
    // tcp state and both payload states are handled by automatic residual
    // checksum...
    prsr_->set_shift_amount(251, 31);
    prsr_->set_checksum(1, 3, 0x0000, 0x0000, 0x000FFFFF, false, 0, 19, true,
                        ParserStaticConfig::TCP_RESID, true,  true,  false);
    prsr_->set_checksum_enable(251, 1, false);
    pkt = make_tcp_packet(om_, seed_, -1, payload_len, 0xFFFFFFFF,
                          nullptr, nullptr);
    ASSERT_TRUE(pkt != nullptr);
    phv = prsr_->parse(pkt, parser_chan);
    sanity_check_phv(101);
    EXPECT_EQ(hdr_len_ + 63, pkt->orig_hdr_len());
    uint16_t prsr_resid = phv->get_p(ParserStaticConfig::TCP_RESID);
    // only 63 bytes of payload are parsed and included in residual checksum...
    uint16_t expected_cksum = get_residual_checksum(payload_len, 63);
    uint16_t sum = ones_cmpl_add(expected_cksum, prsr_resid);
    if (ones_cmpl_negate(expected_cksum) == 0xFFFF)
      EXPECT_TRUE((sum == 0xFFFF) || (sum == 0x0000));
    else
      EXPECT_EQ(0xFFFF, sum);
    om_->phv_delete(phv);
    om_->pkt_delete(pkt);
    prsr_->set_shift_amount(251, 32);
    prsr_->set_checksum(1, 3, 0x0000, 0x0000, 0x000FFFFF, false, 0, 0, false,
                        0, true,  true,  false);
    prsr_->set_checksum_enable(251, 1, true);
  }

  {
    // repeat, but tweak parser config so programmed residual checksum ends in
    // tcp state and the end_pos is greater than the shift amount; remaining bytes
    // after end_pos and both payload states are handled by automatic residual
    // checksum...
    prsr_->set_shift_amount(251, 31);
    int end_pos = 24;  // NB shift_amt for same state is 20
    prsr_->set_checksum(1, 3, 0x0000, 0x0000, 0x01FFFFFF, false, 0, end_pos, true,
                        ParserStaticConfig::TCP_RESID, true,  true,  false);
    prsr_->set_checksum_enable(251, 1, false);
    pkt = make_tcp_packet(om_, seed_, -1, payload_len, 0xFFFFFFFF,
                          nullptr, nullptr);
    ASSERT_TRUE(pkt != nullptr);
    om_->update_log_flags(ALL,ALL,TYP_PARSER_CHECKSUM,ALL,ALL,ALL,FEW);
    RmtLoggerCapture *log_capture = rmt_logger_capture();
    log_capture->start();
    phv = prsr_->parse(pkt, parser_chan);
    log_capture->stop();
    sanity_check_phv(101);
    EXPECT_EQ(hdr_len_ + 63, pkt->orig_hdr_len());
    uint16_t prsr_resid = phv->get_p(ParserStaticConfig::TCP_RESID);
    // only 63 bytes of payload are parsed and included in residual checksum...
    uint16_t expected_cksum = get_residual_checksum(payload_len, 63);
    uint16_t sum = ones_cmpl_add(expected_cksum, prsr_resid);
    if (ones_cmpl_negate(expected_cksum) == 0xFFFF)
      EXPECT_TRUE((sum == 0xFFFF) || (sum == 0x0000));
    else
      EXPECT_EQ(0xFFFF, sum);
    // check logging lines include error for end_pos >= shift_amount
    int line_count = log_capture->for_each_line_containing(
        "ERROR ChecksumEngineShared::do_checksum: end_pos 24 >= shift_amt 20",
        nullptr);
    EXPECT_EQ(1, line_count) << log_capture->dump_lines();
    om_->phv_delete(phv);
    om_->pkt_delete(pkt);
    prsr_->set_shift_amount(251, 32);
    prsr_->set_checksum(1, 3, 0x0000, 0x0000, 0x000FFFFF, false, 0, 0, false,
                        0, true,  true,  false);
    prsr_->set_checksum_enable(251, 1, true);
  }

  // re-program parser with *three* payload parsing states to use more
  // interesting combinations of shifts that exercise:
  // - final programmed checksum state uses a shift-right
  // - final programmed checksum state has some bytes that must be
  //   automatically checksummed
  // - automatic residual needs shift right
  std::array<uint16_t, 4> dst_nox = {ParserStaticConfig::NoX,
                                     ParserStaticConfig::NoX,
                                     ParserStaticConfig::NoX,
                                     ParserStaticConfig::NoX};
  // third payload state stays unchanged: accumulate residual checksum in
  // **automatic phase** from a further 32 bytes of payload
  ParserStaticConfig::config_payload_n(prsr_, 249, 5, 255, true, dst_nox, 32);
  prsr_->set_hdr_len_inc(249, true);
  for (int shift_1 = 10; shift_1 < 12; shift_1++) {
    // vary parser shift_1 amount in first payload state
    SCOPED_TRACE(shift_1);
    ParserStaticConfig::config_payload_n(prsr_, 251, 3, 4, false, dst_nox, shift_1);
    prsr_->set_checksum_enable(251, 1, true);
    prsr_->set_checksum_ram_addr(251, 1, 4);
    uint32_t mask_1 = (1 << shift_1) - 1;
    prsr_->set_checksum(1, 4, 0x0000, 0x0000, mask_1, false, 0, shift_1 - 1,
                        false, 0, true, true, false);
    for (int shift_2 = 12; shift_2 < 14; shift_2++) {
      // vary parser shift_2 amount in second payload state
      SCOPED_TRACE(shift_2);
      ParserStaticConfig::config_payload_n(prsr_, 250, 4, 5, false, dst_nox, shift_2);
      prsr_->set_checksum_enable(250, 1, true);
      prsr_->set_checksum_ram_addr(250, 1, 5);
      for (int final_pos = 6; final_pos < 8; final_pos++) {
        // vary final_pos for checksum in second payload state
        SCOPED_TRACE(final_pos);
        // second payload state: shift right if odd shift in first payload
        // state, final_pos < shift_2 amount
        bool rotbuf = shift_1 % 2;
        uint32_t mask_2 = (1 << shift_2) - 1;
        prsr_->set_checksum(1, 5, 0x0000, 0x0000, mask_2, rotbuf, 0, final_pos,
                            true, ParserStaticConfig::TCP_RESID, true, true, false);

        pkt = make_tcp_packet(om_, seed_, -1, payload_len, 0xFFFFFFFF,
                              nullptr, nullptr);
        //om_->rmt_log_packet(pkt, 120);
        ASSERT_TRUE(pkt != nullptr);
        phv = prsr_->parse(pkt, parser_chan);
        ASSERT_TRUE(phv != nullptr);
        int parsed_payload_len = shift_1 + shift_2 + 32;
        EXPECT_EQ(hdr_len_ + parsed_payload_len, pkt->orig_hdr_len());
        uint16_t prsr_resid = phv->get_p(ParserStaticConfig::TCP_RESID);
        uint16_t expected_cksum = get_residual_checksum(payload_len, parsed_payload_len);
        uint16_t sum = ones_cmpl_add(expected_cksum, prsr_resid);
        if (ones_cmpl_negate(expected_cksum) == 0xFFFF)
          EXPECT_TRUE((sum == 0xFFFF) || (sum == 0x0000));
        else
          EXPECT_EQ(0xFFFF, sum);
        om_->phv_delete(phv);
        om_->pkt_delete(pkt);
      }
    }
  }
}

TEST_F(BFN_TEST_NAME(CksumTestFixture),VerificationError) {
  // XXX: verify that when a checksum verification error occurs the
  // parser still writes residual checksum and sets pkt priority
  //om_->update_log_flags(ALL,ALL,TYP_PARSER_CHECKSUM,ALL,ALL,ALL,ALL);
  prsr_->set_log_flags(UINT64_C(7));

  // set hdr_len_adj to zero so that remaining tests are consistent with no IPB
  // metatdata being prepended
  prsr_->set_hdr_len_adj(0);
  config_checksums(prsr_);  // parse eth + ip + tcp + 32 bytes payload

  // configure priority
  prsr_->set_pri_upd_type(253, 0);
  prsr_->set_pri_upd_en_shr(253, 1);
  prsr_->set_pri_upd_val_mask(253, 7);

  uint16_t pkt_cksum = 0;
  uint16_t payload_len = 345;  // arbitrary

  Packet *pkt = make_tcp_packet(om_, seed_, -1, payload_len, 0xFFFFFFFF,
                                &pkt_cksum, nullptr, true);
  ASSERT_TRUE(pkt != NULL);
  EXPECT_EQ(payload_len + hdr_len_, pkt->len());

  pkt->set_byte(28, 0x00);  // corrupt the IP4 src addr
  //om_->rmt_log_packet(pkt);

  // parse the corrupted packet
  Phv *phv = prsr_->parse(pkt, 0);
  ASSERT_TRUE(phv != NULL);
  uint16_t prsr_cksum = phv->get_p(ParserStaticConfig::TCP_CKSM);
  uint16_t prsr_resid = phv->get_p(ParserStaticConfig::TCP_RESID);
   // Check IP csum error = (err flag:0x8) << 8 | immediate value
  EXPECT_EQ(0x8Fu, phv->get_p(ParserStaticConfig::IP4_ERR));
  EXPECT_EQ(pkt_cksum, prsr_cksum);  // Check extracted cksum matches original
  EXPECT_EQ(hdr_len_ + 32, pkt->orig_hdr_len());
  EXPECT_NE(0u, prsr_resid);  // residual is written
  EXPECT_EQ(3, pkt->priority());  // priority is set to 7 and masked giving 3
  om_->phv_delete(phv);
  om_->pkt_delete(pkt);
}

}
