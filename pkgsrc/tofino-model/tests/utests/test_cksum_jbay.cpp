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

namespace MODEL_CHIP_TEST_NAMESPACE {

TEST_F(BFN_TEST_NAME(CksumTestFixture),PartialHeader) {
  // verify behaviour when a partial header error occurs
  //om_->update_log_flags(ALL,ALL,TYP_PARSER_CHECKSUM,ALL,ALL,ALL,ALL);
  prsr_->set_log_flags(UINT64_C(7));

  config_residual_checksum(prsr_);

  // phv dest word for any parse errors
  const uint16_t perr_word = Phv::make_word_p(0, 10);
  const int parser_chan = 0;

  // setup priority in TCP and payload extraction states
  // Output priority will be determined by priority of last non-error state
  //  and by final priority mapping which simply maps internal priority,
  //  to identical output priority and masks with 0x3 to output 2b.
  prsr_->set_pri_upd_type(253, 0);
  prsr_->set_pri_upd_en_shr(253, 1);
  prsr_->set_pri_upd_val_mask(253, 7); // Output pri(2b) will be 3
  prsr_->set_pri_upd_type(251, 0);
  prsr_->set_pri_upd_en_shr(251, 1);
  prsr_->set_pri_upd_val_mask(251, 6); // Output pri(2b) will be 2
  prsr_->set_pri_upd_type(250, 0);
  prsr_->set_pri_upd_en_shr(250, 1);
  prsr_->set_pri_upd_val_mask(250, 5); // Output pri(2b) will be 1

  // set hdr_len_adj to zero so that remaining tests are consistent with no IPB
  // metatdata being prepended
  prsr_->set_hdr_len_adj(0);

  Packet *pkt;
  uint16_t payload_len = 70;
  Phv *phv;

  auto make_packet = [this, &pkt](
      uint16_t payload_len,
      uint16_t trim_len) {
    pkt = make_tcp_packet(om_, seed_, -1, payload_len, 0xFFFFFFFF,
                          nullptr, nullptr);
    ASSERT_TRUE(pkt != nullptr);
    // om_->rmt_log_packet(pkt, 96);
    EXPECT_EQ(payload_len + hdr_len_, pkt->len());
    RMT_ASSERT(trim_len < pkt->len());
    pkt->trim_back(trim_len);
    EXPECT_EQ(payload_len + hdr_len_ - trim_len, pkt->len());
  };

  auto check_extraction = [&pkt, &phv](int pkt_pos, int phv_word) {
    uint8_t expected[1];
    pkt->get_buf(expected, pkt_pos, 1);
    EXPECT_EQ(expected[0], phv->get_p(phv_word)) << "PHV word " << phv_word;
  };

  auto check_imm_extraction = [&phv](uint8_t imm_val, int phv_word) {
    EXPECT_EQ(imm_val, phv->get_p(phv_word)) << "PHV word " << phv_word;
  };

  auto check_no_extraction = [&phv](int phv_word) {
    EXPECT_EQ(0u, phv->get_p(phv_word)) << "PHV word " << phv_word;
  };

  auto check_no_payload_extractions = [&phv]() {
    EXPECT_EQ(0u, phv->get_p(ParserStaticConfig::PHV8_4));
    EXPECT_EQ(0u, phv->get_p(ParserStaticConfig::PHV8_5));
    EXPECT_EQ(0u, phv->get_p(ParserStaticConfig::PHV8_6));
    EXPECT_EQ(0u, phv->get_p(ParserStaticConfig::PHV8_7));
    EXPECT_EQ(0u, phv->get_p(ParserStaticConfig::PHV8_8));
    EXPECT_EQ(0u, phv->get_p(ParserStaticConfig::PHV8_9));
    EXPECT_EQ(0u, phv->get_p(ParserStaticConfig::PHV8_10));
    EXPECT_EQ(0u, phv->get_p(ParserStaticConfig::PHV8_11));
  };

  {
    SCOPED_TRACE("Good packet with sufficient payload bytes for parser states");
    make_packet(payload_len, 0);
    // init pkt with non-zero orig_hdr_len: should be reset by parse
    pkt->set_orig_hdr_len(123);
    phv = prsr_->parse(pkt, parser_chan);
    ASSERT_TRUE(phv != nullptr);
    EXPECT_EQ(hdr_len_ + 64, pkt->orig_hdr_len());
    EXPECT_EQ(0x33u, phv->get_p(ParserStaticConfig::PHV8_3)); // sanity check
    check_extraction(54, ParserStaticConfig::PHV8_4);
    check_extraction(62, ParserStaticConfig::PHV8_5);
    check_extraction(70, ParserStaticConfig::PHV8_6);
    check_imm_extraction(0xba, ParserStaticConfig::PHV8_7);
    check_extraction(86, ParserStaticConfig::PHV8_8);
    check_extraction(94, ParserStaticConfig::PHV8_9);
    check_extraction(102, ParserStaticConfig::PHV8_10);
    check_imm_extraction(0xba, ParserStaticConfig::PHV8_11);
    EXPECT_EQ(0u, phv->get_p(perr_word));
    uint16_t prsr_resid = phv->get_p(ParserStaticConfig::TCP_RESID);
    uint16_t expected_cksum = get_residual_checksum(payload_len, 64);
    uint16_t sum = ones_cmpl_add(expected_cksum, prsr_resid);
    if (ones_cmpl_negate(expected_cksum) == 0xFFFF)
      EXPECT_TRUE((sum == 0xFFFF) || (sum == 0x0000));
    else
      EXPECT_EQ(0xFFFF, sum);
    EXPECT_EQ(1, pkt->priority());  // priority is set to 5 and masked giving 1
    om_->phv_delete(phv);
    om_->pkt_delete(pkt);
  }

  // Parse short packet: insufficient payload bytes so that a partial
  // header error occurs - pkt is dropped
  uint16_t short_payload_len = 10;
  make_packet(payload_len, payload_len - short_payload_len);
  phv = prsr_->parse(pkt, parser_chan);
  ASSERT_TRUE(phv == nullptr);
  om_->pkt_delete(pkt);

  // config parser to write partial header error flag to phv, so short pkts
  // should not be dropped by parser
  prsr_->set_perr_phv_output(parser_chan, Parser::kErrPartialHdr, perr_word);

  {
    // check partial hdr error reported in phv
    SCOPED_TRACE("Short packet: insufficient payload bytes, partial hdr err");
    make_packet(payload_len, payload_len - short_payload_len);
    phv = prsr_->parse(pkt, parser_chan);
    ASSERT_TRUE(phv != nullptr);
    check_no_payload_extractions();
    // verify hdr len is set to pkt len (jbay) or successfully parsed len (WIP)
    uint16_t expected_len = RmtObject::is_jbayXX() ? pkt->len() : hdr_len_;
    EXPECT_EQ(expected_len, pkt->orig_hdr_len());
    // verify partial hdr error is reported
    uint32_t expected_err = Parser::kErrPartialHdr;
    EXPECT_EQ(expected_err, phv->get_p(perr_word));
    // verify residual checksum not written to phv
    uint16_t prsr_resid = phv->get_p(ParserStaticConfig::TCP_RESID);
    EXPECT_EQ(0u, prsr_resid);
    EXPECT_EQ(3, pkt->priority());  // priority is set to 7 and masked giving 3
    om_->phv_delete(phv);
    om_->pkt_delete(pkt);
  }

  {
    // check partial hdr error NOT reported in phv
    SCOPED_TRACE("Short packet: disable_partial_hdr_err set");
    prsr_->set_disable_partial_hdr_err(251, true);
    make_packet(payload_len, payload_len - short_payload_len);
    phv = prsr_->parse(pkt, parser_chan);
    ASSERT_TRUE(phv != nullptr);
    // parsing ends on partial hdr err - no payload bytes extracted
    check_no_payload_extractions();
    // verify hdr len is set to pkt len (jbay) or successfully parsed len (WIP)
    uint16_t expected_len = RmtObject::is_jbayXX() ? pkt->len() : hdr_len_;
    EXPECT_EQ(expected_len, pkt->orig_hdr_len());
    // verify partial hdr error is NOT reported
    EXPECT_EQ(0u, phv->get_p(perr_word));
    // XXX: verify residual checksum IS written to phv
    uint16_t prsr_resid = phv->get_p(ParserStaticConfig::TCP_RESID);
    uint16_t expected_cksum = get_residual_checksum(payload_len, short_payload_len);
    uint16_t sum = ones_cmpl_add(expected_cksum, prsr_resid);
    if (ones_cmpl_negate(expected_cksum) == 0xFFFF)
      EXPECT_TRUE((sum == 0xFFFF) || (sum == 0x0000));
    else
      EXPECT_EQ(0xFFFF, sum);
    EXPECT_EQ(3, pkt->priority());  // priority is set to 7 and masked giving 3
    om_->phv_delete(phv);
    om_->pkt_delete(pkt);
    prsr_->set_disable_partial_hdr_err(251, false);
  }

  {
    // check combination of hdr len inc stop with later partial header error
    SCOPED_TRACE("Short packet: hdr_len_inc_stop *before* partial hdr error");
    prsr_->set_hdr_len_inc_stop(254, true);
    prsr_->set_hdr_len_inc_final_amt(254, 15);  // jbay only
    make_packet(payload_len, payload_len - short_payload_len);
    phv = prsr_->parse(pkt, parser_chan);
    ASSERT_TRUE(phv != nullptr);
    // parsing ends on partial hdr err - no payload bytes extracted
    check_no_payload_extractions();
    // verify hdr len is stopped at ip hdr state
    uint16_t expected_len = RmtObject::is_jbayXX() ? 14 + 15 : 14 + 20;
    EXPECT_EQ(expected_len, pkt->orig_hdr_len());
    // verify partial hdr error is reported
    uint32_t expected_err = Parser::kErrPartialHdr;
    EXPECT_EQ(expected_err, phv->get_p(perr_word));
    // verify residual checksum not written to phv
    uint16_t prsr_resid = phv->get_p(ParserStaticConfig::TCP_RESID);
    EXPECT_EQ(0u, prsr_resid);
    EXPECT_EQ(3, pkt->priority());  // priority is set to 7 and masked giving 3
    om_->phv_delete(phv);
    om_->pkt_delete(pkt);
    prsr_->set_hdr_len_inc_stop(254, false);
    prsr_->set_hdr_len_inc_final_amt(254, 0);
  }

  {
    // check combination of hdr len inc stop with partial header error in same state
    SCOPED_TRACE("Short packet: hdr_len_inc_stop + partial hdr error");
    prsr_->set_hdr_len_inc_stop(251, true);
    prsr_->set_hdr_len_inc_final_amt(251, 15);  // jbay only
    make_packet(payload_len, payload_len - short_payload_len);
    phv = prsr_->parse(pkt, parser_chan);
    ASSERT_TRUE(phv != nullptr);
    // parsing ends on partial hdr err - no payload bytes extracted
    check_no_payload_extractions();
    // verify hdr len is set to pkt len (jbay) or successfully parsed len (WIP)
    // i.e. hdr_len_inc_final_amt is ignored
    uint16_t expected_len = RmtObject::is_jbayXX() ? pkt->len() : hdr_len_;
    EXPECT_EQ(expected_len, pkt->orig_hdr_len());
    // verify partial hdr error is reported
    uint32_t expected_err = Parser::kErrPartialHdr;
    EXPECT_EQ(expected_err, phv->get_p(perr_word));
    // verify residual checksum not written to phv
    uint16_t prsr_resid = phv->get_p(ParserStaticConfig::TCP_RESID);
    EXPECT_EQ(0u, prsr_resid);
    EXPECT_EQ(3, pkt->priority());  // priority is set to 7 and masked giving 3
    om_->phv_delete(phv);
    om_->pkt_delete(pkt);
    prsr_->set_hdr_len_inc_stop(251, false);
    prsr_->set_hdr_len_inc_final_amt(251, 0);
  }

  //
  // WIP tests with partial_hdr_err_proc set...
  //

  if (RmtObject::is_chip1_or_later()) {
    // truncate packet part way into TCP header: parsing should terminate at this
    // state; residual checksum is not output since not yet in automatic mode
    SCOPED_TRACE("Short packet: partial_hdr_err_proc, tcp state");
    prsr_->set_disable_partial_hdr_err(253, true);
    prsr_->set_partial_hdr_err_proc(253, true);
    // trim off entire payload plus 4 bytes of tcp header
    make_packet(payload_len, payload_len + 4);
    phv = prsr_->parse(pkt, parser_chan);
    ASSERT_TRUE(phv != nullptr);
    check_no_payload_extractions();
    // verify hdr len is set to pkt len
    EXPECT_EQ(hdr_len_ - 4, pkt->orig_hdr_len());
    // verify partial hdr error is NOT reported
    EXPECT_EQ(0u, phv->get_p(perr_word));
    // verify residual checksum not written to phv - not yet in automatic phase
    // of residual checksum
    uint16_t prsr_resid = phv->get_p(ParserStaticConfig::TCP_RESID);
    EXPECT_EQ(0u, prsr_resid);
    EXPECT_EQ(3, pkt->priority());  // priority is set to 7 and masked giving 3
    om_->phv_delete(phv);
    om_->pkt_delete(pkt);
    prsr_->set_disable_partial_hdr_err(253, false);
    prsr_->set_partial_hdr_err_proc(253, false);
  }

  if (RmtObject::is_chip1_or_later()) {
    // truncate packet part way into first 32 bytes of payload: residual
    // checksum should complete despite partial header error; parsing should
    // terminate at this state
    SCOPED_TRACE("Short packet: partial_hdr_err_proc, first payload state");
    prsr_->set_disable_partial_hdr_err(251, true);
    prsr_->set_partial_hdr_err_proc(251, true);
    make_packet(payload_len, payload_len - short_payload_len);
    // init pkt with non-zero orig_hdr_len: should be reset by parse
    pkt->set_orig_hdr_len(123);
    phv = prsr_->parse(pkt, parser_chan);
    ASSERT_TRUE(phv != nullptr);
    // penultimate state - partial header error
    check_extraction(54, ParserStaticConfig::PHV8_4);  // in payload
    check_extraction(62, ParserStaticConfig::PHV8_5);  // in payload
    check_no_extraction(ParserStaticConfig::PHV8_6);   // missing
    check_imm_extraction(0xba, ParserStaticConfig::PHV8_7);
    // ultimate state - parser should not proceed to this state
    check_no_extraction(ParserStaticConfig::PHV8_8);
    check_no_extraction(ParserStaticConfig::PHV8_9);
    check_no_extraction(ParserStaticConfig::PHV8_10);
    // NB no immediate extraction here...
    check_no_extraction(ParserStaticConfig::PHV8_11);
    // verify hdr len is set to pkt len
    EXPECT_EQ(hdr_len_ + short_payload_len, pkt->orig_hdr_len());
    // verify partial hdr error is NOT reported
    EXPECT_EQ(0u, phv->get_p(perr_word));
    // verify residual checksum is updated and written
    uint16_t prsr_resid = phv->get_p(ParserStaticConfig::TCP_RESID);
    uint16_t expected_cksum = get_residual_checksum(payload_len, short_payload_len);
    uint16_t sum = ones_cmpl_add(expected_cksum, prsr_resid);
    if (ones_cmpl_negate(expected_cksum) == 0xFFFF)
      EXPECT_TRUE((sum == 0xFFFF) || (sum == 0x0000));
    else
      EXPECT_EQ(0xFFFF, sum);
    EXPECT_EQ(2, pkt->priority());  // priority is set to 6 and masked giving 2
    om_->phv_delete(phv);
    om_->pkt_delete(pkt);
    prsr_->set_disable_partial_hdr_err(251, false);
    prsr_->set_partial_hdr_err_proc(251, false);
  }

  if (RmtObject::is_chip1_or_later()) {
    // truncate packet part way into first 32 bytes of payload: hdr_len_inc is
    // NOT set in this state so hdr len should not be updated from previous
    // state, but the residual checksum DOES include all bytes in this state;
    // parsing should terminate at this state
    SCOPED_TRACE("Short packet: partial_hdr_err_proc, no hdr_len_inc, first payload state");
    prsr_->set_disable_partial_hdr_err(251, true);
    prsr_->set_partial_hdr_err_proc(251, true);
    prsr_->set_hdr_len_inc(251, false);
    make_packet(payload_len, payload_len - short_payload_len);
    // init pkt with non-zero orig_hdr_len: should be reset by parse
    pkt->set_orig_hdr_len(123);
    phv = prsr_->parse(pkt, parser_chan);
    ASSERT_TRUE(phv != nullptr);
    // penultimate state - partial header error
    check_extraction(54, ParserStaticConfig::PHV8_4);  // in payload
    check_extraction(62, ParserStaticConfig::PHV8_5);  // in payload
    check_no_extraction(ParserStaticConfig::PHV8_6);   // missing
    check_imm_extraction(0xba, ParserStaticConfig::PHV8_7);
    // ultimate state - parser should not proceed to this state
    check_no_extraction(ParserStaticConfig::PHV8_8);
    check_no_extraction(ParserStaticConfig::PHV8_9);
    check_no_extraction(ParserStaticConfig::PHV8_10);
    // NB no immediate extraction here...
    check_no_extraction(ParserStaticConfig::PHV8_11);
    // verify hdr len does not get updated in this state
    EXPECT_EQ(hdr_len_, pkt->orig_hdr_len());
    // verify partial hdr error is NOT reported
    EXPECT_EQ(0u, phv->get_p(perr_word));
    // verify residual checksum is updated and written; NB: when partial hdr
    // err occurs in the final pre-automatic mode state the residual includes
    // all bytes in packet despite hdr_len_inc not being set
    uint16_t prsr_resid = phv->get_p(ParserStaticConfig::TCP_RESID);
    uint16_t expected_cksum = get_residual_checksum(payload_len, short_payload_len);
    uint16_t sum = ones_cmpl_add(expected_cksum, prsr_resid);
    if (ones_cmpl_negate(expected_cksum) == 0xFFFF)
      EXPECT_TRUE((sum == 0xFFFF) || (sum == 0x0000));
    else
      EXPECT_EQ(0xFFFF, sum);
    EXPECT_EQ(2, pkt->priority());  // priority is set to 6 and masked giving 2
    om_->phv_delete(phv);
    om_->pkt_delete(pkt);
    prsr_->set_disable_partial_hdr_err(251, false);
    prsr_->set_partial_hdr_err_proc(251, false);
    prsr_->set_hdr_len_inc(251, true);
  }

  if (RmtObject::is_chip1_or_later()) {
    // truncate packet part way into first 32 bytes of payload: hdr_len_inc is
    // NOT set in this state so hdr len and residual checksum should not be
    // updated from previous state; parsing should terminate at this state;
    // residual calc is auto in this state
    SCOPED_TRACE("Short packet: partial_hdr_err_proc, no hdr_len_inc, "
                 "first payload state auto residual");
    prsr_->set_disable_partial_hdr_err(251, true);
    prsr_->set_partial_hdr_err_proc(251, true);
    prsr_->set_hdr_len_inc(251, false);
    // tweak parser config so programmed residual checksum ends in tcp state and
    // both payload states are handled by automatic residual checksum...
    prsr_->set_checksum(1, 3, 0x0000, 0x0000, 0x000FFFFF, false, 0, 19, true,
                        ParserStaticConfig::TCP_RESID, true,  true,  false);
    prsr_->set_checksum_enable(251, 1, false);
    make_packet(payload_len, payload_len - short_payload_len);
    // init pkt with non-zero orig_hdr_len: should be reset by parse
    pkt->set_orig_hdr_len(123);
    phv = prsr_->parse(pkt, parser_chan);
    ASSERT_TRUE(phv != nullptr);
    // penultimate state - partial header error
    check_extraction(54, ParserStaticConfig::PHV8_4);  // in payload
    check_extraction(62, ParserStaticConfig::PHV8_5);  // in payload
    check_no_extraction(ParserStaticConfig::PHV8_6);   // missing
    check_imm_extraction(0xba, ParserStaticConfig::PHV8_7);
    // ultimate state - parser should not proceed to this state
    check_no_extraction(ParserStaticConfig::PHV8_8);
    check_no_extraction(ParserStaticConfig::PHV8_9);
    check_no_extraction(ParserStaticConfig::PHV8_10);
    // NB no immediate extraction here...
    check_no_extraction(ParserStaticConfig::PHV8_11);
    // verify hdr len does not get updated in this state
    EXPECT_EQ(hdr_len_, pkt->orig_hdr_len());
    // verify partial hdr error is NOT reported
    EXPECT_EQ(0u, phv->get_p(perr_word));
    // verify residual checksum is not updated i.e. matches entire payload
    uint16_t prsr_resid = phv->get_p(ParserStaticConfig::TCP_RESID);
    // check residual checksum should stopped after tcp hdr when hdr len inc last set
    uint16_t expected_cksum = get_residual_checksum(payload_len, 0);
    uint16_t sum = ones_cmpl_add(expected_cksum, prsr_resid);
    if (ones_cmpl_negate(expected_cksum) == 0xFFFF)
      EXPECT_TRUE((sum == 0xFFFF) || (sum == 0x0000));
    else
      EXPECT_EQ(0xFFFF, sum);
    EXPECT_EQ(2, pkt->priority());  // priority is set to 6 and masked giving 2
    om_->phv_delete(phv);
    om_->pkt_delete(pkt);
    prsr_->set_disable_partial_hdr_err(251, false);
    prsr_->set_partial_hdr_err_proc(251, false);
    prsr_->set_hdr_len_inc(251, true);
    // reset parser config...
    prsr_->set_checksum(1, 3, 0x0000, 0x0000, 0x000FFFFF, false, 0, 0, false,
                        0, true,  true,  false);
    prsr_->set_checksum_enable(251, 1, true);
  }

  if (RmtObject::is_chip1_or_later()) {
    // extend packet part way into second 32 bytes of payload: **automatic**
    // residual checksum should complete despite partial header error
    short_payload_len = 35;
    SCOPED_TRACE("Short packet: partial_hdr_err_proc, second payload state");
    prsr_->set_disable_partial_hdr_err(250, true);
    prsr_->set_partial_hdr_err_proc(250, true);
    make_packet(payload_len, payload_len - short_payload_len);
    phv = prsr_->parse(pkt, parser_chan);
    ASSERT_TRUE(phv != nullptr);
    // penultimate state - all ok
    check_extraction(54, ParserStaticConfig::PHV8_4);  // in payload
    check_extraction(62, ParserStaticConfig::PHV8_5);  // in payload
    check_extraction(70, ParserStaticConfig::PHV8_6);  // in payload
    check_imm_extraction(0xba, ParserStaticConfig::PHV8_7);
    // ultimate state - partial header error
    check_extraction(86, ParserStaticConfig::PHV8_8);  // in payload
    check_no_extraction(ParserStaticConfig::PHV8_9);
    check_no_extraction(ParserStaticConfig::PHV8_10);
    check_imm_extraction(0xba, ParserStaticConfig::PHV8_11);
    // verify hdr len is set to pkt len
    EXPECT_EQ(hdr_len_ + short_payload_len, pkt->orig_hdr_len());
    // verify partial hdr error is NOT reported
    EXPECT_EQ(0u, phv->get_p(perr_word));
    // verify residual checksum is updated and written
    uint16_t prsr_resid = phv->get_p(ParserStaticConfig::TCP_RESID);
    uint16_t expected_cksum = get_residual_checksum(payload_len, short_payload_len);
    uint16_t sum = ones_cmpl_add(expected_cksum, prsr_resid);
    if (ones_cmpl_negate(expected_cksum) == 0xFFFF)
      EXPECT_TRUE((sum == 0xFFFF) || (sum == 0x0000));
    else
      EXPECT_EQ(0xFFFF, sum);
    EXPECT_EQ(1, pkt->priority());  // priority is set to 5 and masked giving 1
    prsr_->set_disable_partial_hdr_err(250, false);
    prsr_->set_partial_hdr_err_proc(250, false);
    om_->phv_delete(phv);
    om_->pkt_delete(pkt);
  }

  if (RmtObject::is_chip1_or_later()) {
    // extend packet part way into second 32 bytes of payload: **automatic**
    // residual checksum should complete despite partial header error and
    // **despite disable_partial_header_err being false**
    short_payload_len = 35;
    SCOPED_TRACE("Short packet: NO disable_partial_hdr_err, second payload state");
    prsr_->set_disable_partial_hdr_err(250, false);
    prsr_->set_partial_hdr_err_proc(250, true);
    make_packet(payload_len, payload_len - short_payload_len);
    phv = prsr_->parse(pkt, parser_chan);
    ASSERT_TRUE(phv != nullptr);
    // penultimate state - all ok
    check_extraction(54, ParserStaticConfig::PHV8_4);  // in payload
    check_extraction(62, ParserStaticConfig::PHV8_5);  // in payload
    check_extraction(70, ParserStaticConfig::PHV8_6);  // in payload
    check_imm_extraction(0xba, ParserStaticConfig::PHV8_7);
    // ultimate state - partial header error
    check_extraction(86, ParserStaticConfig::PHV8_8);  // in payload
    check_no_extraction(ParserStaticConfig::PHV8_9);
    check_no_extraction(ParserStaticConfig::PHV8_10);
    check_imm_extraction(0xba, ParserStaticConfig::PHV8_11);
    // verify hdr len is set to pkt len
    EXPECT_EQ(hdr_len_ + short_payload_len, pkt->orig_hdr_len());
    // verify partial hdr error is reported
    uint32_t expected_err = Parser::kErrPartialHdr;
    EXPECT_EQ(expected_err, phv->get_p(perr_word));
    // verify residual checksum is updated and written
    uint16_t prsr_resid = phv->get_p(ParserStaticConfig::TCP_RESID);
    uint16_t expected_cksum = get_residual_checksum(payload_len, short_payload_len);
    uint16_t sum = ones_cmpl_add(expected_cksum, prsr_resid);
    if (ones_cmpl_negate(expected_cksum) == 0xFFFF)
      EXPECT_TRUE((sum == 0xFFFF) || (sum == 0x0000));
    else
      EXPECT_EQ(0xFFFF, sum);
    EXPECT_EQ(1, pkt->priority());  // priority is set to 5 and masked giving 1
    prsr_->set_partial_hdr_err_proc(250, false);
    om_->phv_delete(phv);
    om_->pkt_delete(pkt);
  }

  if (RmtObject::is_chip1_or_later()) {
    // extend packet part way into second 32 bytes of payload: **automatic**
    // residual calc; hdr_len_inc not set in final state, so residual checksum
    // should not be updated but value from penultimate state should be written
    short_payload_len = 35;
    SCOPED_TRACE("Short packet: partial_hdr_err_proc, no hdr_len_inc, second payload state");
    prsr_->set_disable_partial_hdr_err(250, true);
    prsr_->set_partial_hdr_err_proc(250, true);
    prsr_->set_hdr_len_inc(250, false);
    make_packet(payload_len, payload_len - short_payload_len);
    phv = prsr_->parse(pkt, parser_chan);
    ASSERT_TRUE(phv != nullptr);
    // penultimate state - all ok
    check_extraction(54, ParserStaticConfig::PHV8_4);  // in payload
    check_extraction(62, ParserStaticConfig::PHV8_5);  // in payload
    check_extraction(70, ParserStaticConfig::PHV8_6);  // in payload
    check_imm_extraction(0xba, ParserStaticConfig::PHV8_7);
    // ultimate state - partial header error
    check_extraction(86, ParserStaticConfig::PHV8_8);  // in payload
    check_no_extraction(ParserStaticConfig::PHV8_9);
    check_no_extraction(ParserStaticConfig::PHV8_10);
    check_imm_extraction(0xba, ParserStaticConfig::PHV8_11);
    // verify hdr len is stopped at first payload state
    EXPECT_EQ(hdr_len_ + 32, pkt->orig_hdr_len());
    // verify partial hdr error is NOT reported
    EXPECT_EQ(0u, phv->get_p(perr_word));
    // verify residual checksum is NOT updated but is written
    uint16_t prsr_resid = phv->get_p(ParserStaticConfig::TCP_RESID);
    uint16_t expected_cksum = get_residual_checksum(payload_len, 32);
    uint16_t sum = ones_cmpl_add(expected_cksum, prsr_resid);
    if (ones_cmpl_negate(expected_cksum) == 0xFFFF)
      EXPECT_TRUE((sum == 0xFFFF) || (sum == 0x0000));
    else
      EXPECT_EQ(0xFFFF, sum);
    EXPECT_EQ(1, pkt->priority());  // priority is set to 5 and masked giving 1
    prsr_->set_disable_partial_hdr_err(250, false);
    prsr_->set_partial_hdr_err_proc(250, false);
    prsr_->set_hdr_len_inc(250, true);
    om_->phv_delete(phv);
    om_->pkt_delete(pkt);
  }
}


TEST_F(BFN_TEST_NAME(CksumTestFixture),ResidualStop) {
  // verify that residual checksum stops when hdr len inc stops
  //om_->update_log_flags(ALL,ALL,TYP_PARSER_CHECKSUM,ALL,ALL,ALL,ALL);
  prsr_->set_log_flags(UINT64_C(7));

  // set hdr_len_adj to zero so that remaining tests are consistent with no IPB
  // metatdata being prepended
  prsr_->set_hdr_len_adj(0);

  Packet *pkt;
  uint16_t payload_len = 70;
  Phv *phv;
  const int parser_chan = 0;

  auto sanity_check_phv = [&pkt, &phv]() {
    ASSERT_TRUE(phv != nullptr);
    // check sample of expected extractions
    EXPECT_EQ(0x33u, phv->get_p(ParserStaticConfig::PHV8_3));
    uint8_t expected[1];
    pkt->get_buf(expected, 102, 1);
    EXPECT_EQ(expected[0], phv->get_p(ParserStaticConfig::PHV8_10));
  };

  config_residual_checksum(prsr_);
  {
    SCOPED_TRACE("hdr_len_inc_stop in tcp state");
    prsr_->set_hdr_len_inc_stop(253, true);
    prsr_->set_hdr_len_inc_final_amt(253, 20);  // jbay only
    pkt = make_tcp_packet(om_, seed_, -1, payload_len, 0xFFFFFFFF,
                          nullptr, nullptr);
    ASSERT_TRUE(pkt != nullptr);
    phv = prsr_->parse(pkt, parser_chan);
    sanity_check_phv();
    EXPECT_EQ(hdr_len_, pkt->orig_hdr_len());
    uint16_t prsr_resid = phv->get_p(ParserStaticConfig::TCP_RESID);
    // jbay residual includes all parser states, WIP *automatic residual* stops
    // with hdr_len_inc_stop, but automatic residual mode does not start until
    // end of first 32bytes of payload,
    uint16_t checksummed_payload = RmtObject::is_jbayXX() ? 64 : 32;
    uint16_t expected_cksum = get_residual_checksum(payload_len, checksummed_payload);
    uint16_t sum = ones_cmpl_add(expected_cksum, prsr_resid);
    if (ones_cmpl_negate(expected_cksum) == 0xFFFF)
      EXPECT_TRUE((sum == 0xFFFF) || (sum == 0x0000));
    else
      EXPECT_EQ(0xFFFF, sum);
    om_->phv_delete(phv);
    om_->pkt_delete(pkt);
    prsr_->set_hdr_len_inc_stop(253, false);
    prsr_->set_hdr_len_inc_final_amt(253, 0);  // jbay only
  }
  {
    SCOPED_TRACE("hdr_len_inc_stop in first 32bytes payload");
    prsr_->set_hdr_len_inc_stop(251, true);
    prsr_->set_hdr_len_inc_final_amt(251, 32);  // jbay only
    pkt = make_tcp_packet(om_, seed_, -1, payload_len, 0xFFFFFFFF,
                          nullptr, nullptr);
    ASSERT_TRUE(pkt != nullptr);
    phv = prsr_->parse(pkt, parser_chan);
    sanity_check_phv();
    EXPECT_EQ(hdr_len_ + 32, pkt->orig_hdr_len());
    uint16_t prsr_resid = phv->get_p(ParserStaticConfig::TCP_RESID);
    // jbay residual includes all parser states, WIP stops with hdr_len_inc_stop
    uint16_t checksummed_payload = RmtObject::is_jbayXX() ? 64 : 32;
    uint16_t expected_cksum = get_residual_checksum(payload_len, checksummed_payload);
    uint16_t sum = ones_cmpl_add(expected_cksum, prsr_resid);
    if (ones_cmpl_negate(expected_cksum) == 0xFFFF)
      EXPECT_TRUE((sum == 0xFFFF) || (sum == 0x0000));
    else
      EXPECT_EQ(0xFFFF, sum);
    om_->phv_delete(phv);
    om_->pkt_delete(pkt);
    prsr_->set_hdr_len_inc_stop(251, false);
    prsr_->set_hdr_len_inc_final_amt(251, 0);  // jbay only
  }
  {
    SCOPED_TRACE("hdr_len_inc_stop in first 32bytes payload, auto residual");
    prsr_->set_hdr_len_inc_stop(251, true);
    prsr_->set_hdr_len_inc_final_amt(251, 32);  // jbay only
    // tweak parser config so programmed residual checksum ends in tcp state and
    // both payload states are handled by automatic residual checksum...
    prsr_->set_checksum(1, 3, 0x0000, 0x0000, 0x000FFFFF, false, 0, 19, true,
                        ParserStaticConfig::TCP_RESID, true,  true,  false);
    prsr_->set_checksum_enable(251, 1, false);
    pkt = make_tcp_packet(om_, seed_, -1, payload_len, 0xFFFFFFFF,
                          nullptr, nullptr);
    ASSERT_TRUE(pkt != nullptr);
    phv = prsr_->parse(pkt, parser_chan);
    sanity_check_phv();
    EXPECT_EQ(hdr_len_ + 32, pkt->orig_hdr_len());
    uint16_t prsr_resid = phv->get_p(ParserStaticConfig::TCP_RESID);
    // jbay residual includes all parser states, WIP stops with hdr_len_inc_stop
    uint16_t checksummed_payload = RmtObject::is_jbayXX() ? 64 : 32;
    uint16_t expected_cksum = get_residual_checksum(payload_len, checksummed_payload);
    uint16_t sum = ones_cmpl_add(expected_cksum, prsr_resid);
    if (ones_cmpl_negate(expected_cksum) == 0xFFFF)
      EXPECT_TRUE((sum == 0xFFFF) || (sum == 0x0000));
    else
      EXPECT_EQ(0xFFFF, sum);
    om_->phv_delete(phv);
    om_->pkt_delete(pkt);
    prsr_->set_checksum(1, 3, 0x0000, 0x0000, 0x000FFFFF, false, 0, 0, false,
                        0, true,  true,  false);
    prsr_->set_checksum_enable(251, 1, true);
    prsr_->set_hdr_len_inc_stop(251, false);
    prsr_->set_hdr_len_inc_final_amt(251, 0);  // jbay only
  }
  {
    SCOPED_TRACE("hdr_len_inc not set in second 32bytes payload");
    prsr_->set_hdr_len_inc(250, false);  // no effect for jbay
    pkt = make_tcp_packet(om_, seed_, -1, payload_len, 0xFFFFFFFF,
                          nullptr, nullptr);
    ASSERT_TRUE(pkt != nullptr);
    phv = prsr_->parse(pkt, parser_chan);
    sanity_check_phv();
    uint16_t expected_len = RmtObject::is_jbayXX() ? 64 : 32;
    EXPECT_EQ(hdr_len_ + expected_len, pkt->orig_hdr_len());
    uint16_t prsr_resid = phv->get_p(ParserStaticConfig::TCP_RESID);
    uint16_t expected_cksum = get_residual_checksum(payload_len, expected_len);
    uint16_t sum = ones_cmpl_add(expected_cksum, prsr_resid);
    if (ones_cmpl_negate(expected_cksum) == 0xFFFF)
      EXPECT_TRUE((sum == 0xFFFF) || (sum == 0x0000));
    else
      EXPECT_EQ(0xFFFF, sum);
    om_->phv_delete(phv);
    om_->pkt_delete(pkt);
    prsr_->set_hdr_len_inc(250, true);
  }
  {
    SCOPED_TRACE("hdr_len_inc not set in first 32bytes payload but set in second");
    prsr_->set_hdr_len_inc(251, false);
    pkt = make_tcp_packet(om_, seed_, -1, payload_len, 0xFFFFFFFF,
                          nullptr, nullptr);
    ASSERT_TRUE(pkt != nullptr);
    phv = prsr_->parse(pkt, parser_chan);
    sanity_check_phv();
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
    prsr_->set_hdr_len_inc(251, true);
  }
}

TEST_F(BFN_TEST_NAME(CksumTestFixture),ResidualShiftFromPkt) {
  // XXX: verify that when the parser counter is configured to load a
  // shift value from the packet, then this shift value will determine where
  // the residual checksum calculation stops. Note: this test is targeted at WIP
  // but also covers jbay for fixed shift amount of 32.
  //om_->update_log_flags(ALL,ALL,TYP_PARSER_CHECKSUM,ALL,ALL,ALL,ALL);
  prsr_->set_log_flags(UINT64_C(7));

  // set hdr_len_adj to zero so that remaining tests are consistent with no IPB
  // metatdata being prepended
  prsr_->set_hdr_len_adj(0);
  // parse eth + ip + tcp + 32 bytes payload + 32 bytes payload ...
  config_residual_checksum(prsr_);

  auto do_check = [this](uint16_t pkt_shift_amt, uint16_t pkt_shift_src) {
    // pkt_shift_amt: amount of shift value that parser should load from packet;
    // pkt_shift_src: which of first 32 payload bytes the parser should load
    // the shift value from
    ASSERT_LE(pkt_shift_amt, 32);
    // Note: pkt_shift_src uses bits 7:3 of parser ctr_amt_idx register
    // therefore must be < 32
    ASSERT_LT(pkt_shift_src, 32);
    uint16_t expected_cksum = 0, pkt_cksum = 0;
    uint16_t payload_len = 345;  // arbitrary
    seed_ = 99 - pkt_shift_amt;
    uint32_t first32byte_mask;
    unsigned char first32byte[32] = {0};
    // use this packet just to get an expected_cksum value...
    Packet *pkt = make_tcp_packet(om_, seed_, -1, payload_len, 0xFFFFFFFF,
                                  nullptr, &expected_cksum, true,
                                  pkt_shift_amt + 32);
    ASSERT_TRUE(pkt != NULL);
    // om_->rmt_log_packet(pkt, 128);
    EXPECT_EQ(payload_len + hdr_len_, pkt->len());
    uint16_t pkt_len = pkt->len();
    om_->pkt_delete(pkt);

    // Make second packet with full mask so all payload bytes are intact
    first32byte_mask = 0xFFFFFFFF;
    // ... except for the pkt_shift_src byte which we force to have the
    // pkt_shift_amt value; note that pkt_shift_src is always less than 32 so
    // will always be a byte that is included in the residual checksum
    // calculation; its value does not therefore need to be set in the
    // previously constructed reference packet since it always corresponds to
    // one of the bytes that is zero'd in that packet
    first32byte_mask &= ~(0x1u << pkt_shift_src);
    first32byte[pkt_shift_src] = pkt_shift_amt;
    pkt = make_tcp_packet(om_, seed_, -1, payload_len, first32byte_mask,
                          &pkt_cksum, nullptr, true, 0, first32byte);
    ASSERT_TRUE(pkt != NULL);
    // om_->rmt_log_packet(pkt, 128);
    EXPECT_EQ(payload_len + hdr_len_, pkt->len());
    EXPECT_EQ(pkt_len, pkt->len());

    if (RmtObject::is_chip1_or_later()) {
      // only available on WIP: program the parser counter to load the
      // pkt_shift_amt from the pkt_shift_src byte in the state that parses the
      // first 32 bytes of payload
      prsr_->set_counter_ctr_op4(251, Parser::kCounter4LoadPacketShift);
      prsr_->set_counter_load_addr(251, pkt_shift_src << 3);
      // Note in the case of ops 12,13,14 (get shift from packet) the
      // shift_amount (SA) field in EarlyActionRAM needs to be configured
      // appropriately as it acts as a mask on the value extracted from
      // the packet. The mask is derived like so (see uArch):
      //    mask = ((0xFF >> SA[5:3]) & (0xFF << SA[2::0]))
      // so value 0 actually produces mask 0xFF
      prsr_->set_shift_amount(251, 0);
    }
    // parse the second packet
    RmtLoggerCapture *log_capture = rmt_logger_capture();
    om_->update_log_flags(ALL,ALL,TYP_PARSER_CHECKSUM,ALL,ALL,ALL,ALL);
    log_capture->clear_and_start();
    Phv *phv = prsr_->parse(pkt, 0);
    log_capture->stop();
    // std::cout << log_capture->dump_lines();
    om_->update_log_flags(ALL,ALL,TYP_PARSER_CHECKSUM,ALL,ALL,ALL,NON);
    ASSERT_TRUE(phv != NULL);
    uint16_t prsr_resid = phv->get_p(ParserStaticConfig::TCP_RESID);
    if (RmtObject::is_chip1_or_later()) {
      EXPECT_EQ(hdr_len_ + pkt_shift_amt + 32, pkt->orig_hdr_len());
    } else {
      EXPECT_EQ(hdr_len_ + 32 + 32, pkt->orig_hdr_len());
    }

    uint16_t sum = ones_cmpl_add(expected_cksum, prsr_resid);
    // printf("Packet[] LEN=%4d PKT_CKSUM=0x%04x PAYLOAD_CKSUM=0x%04x  "
    //        "PRSR_CKSUM=0x%04x PRSR_RESID=0x%04x SUM=0x%04x\n",
    //        pkt->len(), pkt_cksum, expected_cksum, prsr_cksum, prsr_resid, sum);
    if (ones_cmpl_negate(expected_cksum) == 0xFFFF)
      EXPECT_TRUE((sum == 0xFFFF) || (sum == 0x0000));
    else
      EXPECT_EQ(0xFFFF, sum);
    // XXX: for WIP the parser reg shift amount is set to zero above. This
    // previously caused an ERROR to always be logged because this shift amount
    // is less than the programmed checksum hdr_end_pos. Check this ERROR is no
    // longer logged for WIP *when the shift amount is actually being loaded
    // from packet* and not the programmed amount.
    int line_count = log_capture->for_each_line_containing("ERROR ", nullptr);
    EXPECT_EQ(0, line_count) << log_capture->dump_lines();

    om_->phv_delete(phv);
    om_->pkt_delete(pkt);
  };

  // set checksum mask for first payload state to include all 32 bytes -
  // loading shift from packet should cause fewer bytes to be included in the
  // residual calculation
  prsr_->set_checksum(1, 4, 0x0000, 0x0000, 0xFFFFFFFF, false, 0, 31, true,
                      ParserStaticConfig::TCP_RESID, true, true, false);

  do_check(32, 1);  // works for WIP and also jbay since 32 == programmed shift
  if (RmtObject::is_chip1_or_later()) {
    // Don't use pkt_shift_amt 0 as that translates to a shift of 32
    for (uint16_t pkt_shift_amt = 1; pkt_shift_amt < 31; pkt_shift_amt += 1) {
      SCOPED_TRACE(pkt_shift_amt);
      uint16_t pkt_shift_src = 31 - pkt_shift_amt;
      do_check(pkt_shift_amt, pkt_shift_src);
    }
  }

  // set fin_pos for first payload state to check that the shift from pkt is
  // used when doing automatic residual over bytes between fin_pos and shift
  // amount
  prsr_->set_checksum(1, 4, 0x0000, 0x0000, 0x0003FFFF, false, 0, 17, true,
                      ParserStaticConfig::TCP_RESID, true, true, false);

  do_check(32, 1);  // works for WIP and also jbay since 32 == programmed shift
  if (RmtObject::is_chip1_or_later()) {
    for (uint16_t pkt_shift_amt = 2; pkt_shift_amt < 3; pkt_shift_amt += 1) {
      SCOPED_TRACE(pkt_shift_amt);
      uint16_t pkt_shift_src = 31 - pkt_shift_amt;
      do_check(pkt_shift_amt, pkt_shift_src);
    }
  }

  // finish checksum in match index 253 (tcp header); disable checksum for
  // match index 251 (first payload state); this will cause the stop_checksum
  // residual accumulation code to finish the residual calculation
  prsr_->set_checksum(1, 3, 0x0000, 0x0000, 0x000FFFFF, false, 0, 19, true,
                      ParserStaticConfig::TCP_RESID, true,  true,  false);
  prsr_->set_checksum_enable(251, 1, false);
  do_check(32, 1);  // works for WIP and also jbay since 32 == programmed shift
  if (RmtObject::is_chip1_or_later()) {
    for (uint16_t pkt_shift_amt = 1; pkt_shift_amt < 31; pkt_shift_amt += 1) {
      SCOPED_TRACE(pkt_shift_amt);
      uint16_t pkt_shift_src = 31 - pkt_shift_amt;
      do_check(pkt_shift_amt, pkt_shift_src);
    }
  }
}

TEST_F(BFN_TEST_NAME(CksumTestFixture),ResidualWithHdrLenAdj) {
  // verify that residual checksum stop byte is independent of hdr_len_adj
  //om_->update_log_flags(ALL,ALL,TYP_PARSER_CHECKSUM,ALL,ALL,ALL,ALL);
  prsr_->set_log_flags(UINT64_C(7));

  // set hdr_len_adj to usual default of 32; the test does not actually prepend
  // IPB metadata, so hdr_len_adj of 32 is not required, but we set it in this
  // test to verify that the checksum calculation is not impacted by non-zero
  // hdr_len_adj
  uint8_t hdr_len_adj = 32;
  prsr_->set_hdr_len_adj(hdr_len_adj);

  Packet *pkt;
  uint16_t payload_len = 70;
  Phv *phv;
  const int parser_chan = 0;

  auto sanity_check_phv = [&pkt, &phv]() {
    ASSERT_TRUE(phv != nullptr);
    // check sample of expected extractions
    EXPECT_EQ(0x33u, phv->get_p(ParserStaticConfig::PHV8_3));
    uint8_t expected[1];
    pkt->get_buf(expected, 102, 1);
    EXPECT_EQ(expected[0], phv->get_p(ParserStaticConfig::PHV8_10));
  };

  // parse eth + ip + tcp + 32 bytes payload + 32 bytes payload ...
  config_residual_checksum(prsr_);
  pkt = make_tcp_packet(om_, seed_, -1, payload_len, 0xFFFFFFFF,
                        nullptr, nullptr);
  ASSERT_TRUE(pkt != nullptr);
  phv = prsr_->parse(pkt, parser_chan);
  sanity_check_phv();
  uint16_t checksummed_payload = 32 + 32;
  EXPECT_EQ(hdr_len_ + checksummed_payload - hdr_len_adj, pkt->orig_hdr_len());
  uint16_t prsr_resid = phv->get_p(ParserStaticConfig::TCP_RESID);
  uint16_t expected_cksum = get_residual_checksum(payload_len, checksummed_payload);
  uint16_t sum = ones_cmpl_add(expected_cksum, prsr_resid);
  if (ones_cmpl_negate(expected_cksum) == 0xFFFF)
    EXPECT_TRUE((sum == 0xFFFF) || (sum == 0x0000));
  else
    EXPECT_EQ(0xFFFF, sum);
  om_->phv_delete(phv);
  om_->pkt_delete(pkt);
}

TEST_F(BFN_TEST_NAME(CksumTestFixture),ByteSwapAndMultiplyErrors) {
  // verify errors are logged when:
  //  - byte swap and/or multiply by two selects bytes beyond end_pos
  //  - mask bits extend beyond hdr_end_pos
  //om_->update_log_flags(ALL,ALL,TYP_PARSER_CHECKSUM,ALL,ALL,ALL,ALL);
  config_residual_checksum(prsr_); // eth + ip + [tcp|udp] + 32bytes payload + 32bytes payload
  prsr_->set_log_flags(UINT64_C(0xFF));

  // set hdr_len_adj to zero so that remaining tests are consistent with no IPB
  // metatdata being prepended
  prsr_->set_hdr_len_adj(0);

  RmtLoggerCapture *log_capture = rmt_logger_capture();


  auto do_parse = [this, log_capture](bool verify=true) {
    uint16_t payload_len = 70;
    const int parser_chan = 0;
    Packet *pkt;
    Phv *phv;
    pkt = make_tcp_packet(om_, seed_, -1, payload_len, 0xFFFFFFFF,
                          nullptr, nullptr);
    ASSERT_TRUE(pkt != nullptr);
    om_->update_log_flags(ALL,ALL,TYP_PARSER_CHECKSUM,ALL,ALL,ALL,ALL);
    log_capture->clear_and_start();
    phv = prsr_->parse(pkt, parser_chan);
    log_capture->stop();
    ASSERT_TRUE(phv != nullptr);
    if (verify) {
      // check sample of expected extractions
      EXPECT_EQ(0x33u, phv->get_p(ParserStaticConfig::PHV8_3));
      uint8_t expected[1];
      pkt->get_buf(expected, 102, 1);
      EXPECT_EQ(expected[0], phv->get_p(ParserStaticConfig::PHV8_10));
      EXPECT_EQ(hdr_len_ + 64, pkt->orig_hdr_len());
    }
    om_->phv_delete(phv);
    om_->pkt_delete(pkt);
    prsr_->set_checksum(1, 4, 0x0000, 0x0000, 0xFFFFFFFF, false, 0, 31,
                        true, ParserStaticConfig::TCP_RESID, true, true, false);
  };

  {
    // set end pos 17, no byte-swap, no mul2
    SCOPED_TRACE("hdr end_pos 17, no ERROR log expected");
    prsr_->set_checksum(1, 4, 0x0000, 0x0000, 0x0003FFFF, false, 0, 17,
                        true, ParserStaticConfig::TCP_RESID, true, true, false);
    do_parse();
    int line_count = log_capture->for_each_line_containing("ERROR ", nullptr);
    EXPECT_EQ(0, line_count) << log_capture->dump_lines();
  }

  // byte swap checks...
  {
    // set byte-swap for bytes [16:17] , end pos 17
    SCOPED_TRACE("hdr end_pos 17, byte_swap 0x00100, no ERROR log expected");
    prsr_->set_checksum(1, 4, 0x0000, 0x0100, 0x0003FFFF, false, 0, 17,
                        true, ParserStaticConfig::TCP_RESID, true, true, false);
    do_parse();
    int line_count = log_capture->for_each_line_containing("ERROR ", nullptr);
    EXPECT_EQ(0, line_count) << log_capture->dump_lines();
  }
  {
    // set byte-swap for bytes [18:19] , end pos 17
    SCOPED_TRACE("hdr end_pos 17, byte_swap 0x00300, ERROR log expected");
    prsr_->set_checksum(1, 4, 0x0000, 0x0300, 0x0003FFFF, false, 0, 17,
                        true, ParserStaticConfig::TCP_RESID, true, true, false);
    do_parse();
    auto line_checker = [](int line_num, size_t pos, std::string line)->void {
      EXPECT_EQ(
          "<202,0,1> ERROR ChecksumEngineShared::do_checksum: byte_swap 0x00300 selects "
          "bytes beyond end_pos 17 for engine (1,4) match index 251 (rotbuf false)\n", line);
    };
    int line_count = log_capture->for_each_line_containing("ERROR ", line_checker);
    EXPECT_EQ(1, line_count) << log_capture->dump_lines();
  }
  {
    // set byte-swap for bytes [18:19] , end pos 18
    SCOPED_TRACE("hdr end_pos 18, byte_swap 0x00300, ERROR log expected");
    prsr_->set_checksum(1, 4, 0x0000, 0x0300, 0x0007FFFF, false, 0, 18,
                        true, ParserStaticConfig::TCP_RESID, true, true, false);
    do_parse();
    auto line_checker = [](int line_num, size_t pos, std::string line)->void {
      EXPECT_EQ(
          "<202,0,1> ERROR ChecksumEngineShared::do_checksum: byte_swap 0x00300 selects "
          "bytes beyond end_pos 18 for engine (1,4) match index 251 (rotbuf false)\n", line);
    };
    int line_count = log_capture->for_each_line_containing("ERROR ", line_checker);
    EXPECT_EQ(1, line_count) << log_capture->dump_lines();
  }
  {
    // set byte-swap for bytes [18:19] , end pos 18, rotbuf/shift right true
    SCOPED_TRACE("hdr end_pos 17, byte_swap 0x00100, no ERROR log expected");
    prsr_->set_checksum(1, 4, 0x0000, 0x0300, 0x0007FFFF, true, 0, 18,
                        true, ParserStaticConfig::TCP_RESID, true, true, false);
    do_parse();
    int line_count = log_capture->for_each_line_containing("ERROR ", nullptr);
    EXPECT_EQ(0, line_count) << log_capture->dump_lines();
  }
  {
    // set byte-swap for bytes [18:19] , end pos 19, rotbuf/shift right true
    SCOPED_TRACE("hdr end_pos 17, byte_swap 0x00100, no ERROR log expected");
    prsr_->set_checksum(1, 4, 0x0000, 0x0300, 0x000FFFFF, true, 0, 19,
                        true, ParserStaticConfig::TCP_RESID, true, true, false);
    do_parse();
    int line_count = log_capture->for_each_line_containing("ERROR ", nullptr);
    EXPECT_EQ(0, line_count) << log_capture->dump_lines();
  }

  // mul2 swap checks...
  {
    // set mul2 for bytes [16:17] , end pos 17
    SCOPED_TRACE("hdr end_pos 17, mul2 0x00100, no ERROR log expected");
    prsr_->set_checksum(1, 4, 0x0000, 0x0000, 0x0003FFFF, false, 0, 17,
                        true, ParserStaticConfig::TCP_RESID, true, true, false, 0x0100);
    do_parse();
    int line_count = log_capture->for_each_line_containing("ERROR ", nullptr);
    EXPECT_EQ(0, line_count) << log_capture->dump_lines();
  }
  {
    // set mul2 for bytes [18:19] , end pos 17
    SCOPED_TRACE("hdr end_pos 17, mul2 0x00300, ERROR log expected");
    prsr_->set_checksum(1, 4, 0x0000, 0x0000, 0x0003FFFF, false, 0, 17,
                        true, ParserStaticConfig::TCP_RESID, true, true, false, 0x0300);
    do_parse();
    auto line_checker = [](int line_num, size_t pos, std::string line)->void {
      EXPECT_EQ(
          "<202,0,1> ERROR ChecksumEngineShared::do_checksum: mul2 0x00300 selects "
          "bytes beyond end_pos 17 for engine (1,4) match index 251 (rotbuf false)\n", line);
    };
    int line_count = log_capture->for_each_line_containing("ERROR ", line_checker);
    EXPECT_EQ(1, line_count) << log_capture->dump_lines();
  }
  {
    // set mul2 for bytes [18:19] , end pos 18
    SCOPED_TRACE("hdr end_pos 18, mul2 0x00300, ERROR log expected");
    prsr_->set_checksum(1, 4, 0x0000, 0x0000, 0x0007FFFF, false, 0, 18,
                        true, ParserStaticConfig::TCP_RESID, true, true, false, 0x0300);
    do_parse();
    auto line_checker = [](int line_num, size_t pos, std::string line)->void {
      EXPECT_EQ(
          "<202,0,1> ERROR ChecksumEngineShared::do_checksum: mul2 0x00300 selects "
          "bytes beyond end_pos 18 for engine (1,4) match index 251 (rotbuf false)\n", line);
    };
    int line_count = log_capture->for_each_line_containing("ERROR ", line_checker);
    EXPECT_EQ(1, line_count) << log_capture->dump_lines();
  }
  {
    // set mask bits beyond end pos 18
    SCOPED_TRACE("hdr end_pos 18, mask 0x000FFFFF, ERROR log expected");
    prsr_->set_checksum(1, 4, 0x0000, 0x0000, 0x000FFFFF, false, 0, 18,
                        true, ParserStaticConfig::TCP_RESID, true, true, false, 0x0000);
    do_parse();
    auto line_checker = [](int line_num, size_t pos, std::string line)->void {
      EXPECT_EQ(
          "<202,0,1> ERROR ChecksumEngineShared::do_checksum: mask 0x000fffff "
          "extends beyond end_pos 18 for engine (1,4) match index 251\n", line);
    };
    int line_count = log_capture->for_each_line_containing("ERROR ", line_checker);
    EXPECT_EQ(1, line_count) << log_capture->dump_lines();
  }
  if (RmtObject::is_chip1_or_later()) {
    // only available on WIP: program the parser counter to load the
    // pkt_shift_amt from byte 1 in the state that parses the
    // first 32 bytes of payload
    prsr_->set_counter_ctr_op4(251, Parser::kCounter4LoadPacketShift);
    prsr_->set_counter_load_addr(251, 1 << 3);
    prsr_->set_shift_amount(251, 0);
    // set mask bits beyond end pos 18
    SCOPED_TRACE("hdr end_pos 18, mask 0x000FFFFF, shift from pkt, ERROR log not expected");
    prsr_->set_checksum(1, 4, 0x0000, 0x0000, 0x000FFFFF, false, 0, 18,
                        true, ParserStaticConfig::TCP_RESID, true, true, false, 0x0000);
    do_parse(false);
    int line_count = log_capture->for_each_line_containing("ERROR ", nullptr);
    EXPECT_EQ(0, line_count) << log_capture->dump_lines();
  }
}

}
