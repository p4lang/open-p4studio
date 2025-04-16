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

#include "test_parse.h"

namespace MODEL_CHIP_TEST_NAMESPACE {

using namespace MODEL_CHIP_NAMESPACE;

void BFN_TEST_NAME(ParserTestBase)::verify_chnl_parser_send_pkt_err(IpbCounters *ipb_counters,
                                                                    uint expected) {
  // not implemented for tofino
}

void BFN_TEST_NAME(ParserTestBase)::set_max_chnl_parser_send_pkt(IpbCounters *ipb_counters) {
   // 64 bits for tofino
  ipb_counters->chnl_parser_send_pkt_.chnl_parser_send_pkt(UINT64_C(0xffffffffffffffff));
}

void BFN_TEST_NAME(ParserTestBase)::set_max_chnl_parser_send_pkt_err(IpbCounters *ipb_counters) {
  // not implemented for tofino
}

void BFN_TEST_NAME(ParserTestBase)::verify_epb_counters(
    Epb *epb,
    int epb_chan,
    uint64_t expected_sent) {
  EXPECT_EQ(expected_sent, epb->get_epb_counters(epb_chan)->get_egr_pipe_count());
}

uint64_t BFN_TEST_NAME(ParserTestBase)::set_max_epb_chnl_parser_send_pkt(
    Epb *epb,
    int epb_chan) {
  uint64_t max_val = 0xffffffffffffffff;
  epb->get_epb_counters(epb_chan)->set_egr_pipe_count(max_val);
  return max_val;
}

}
