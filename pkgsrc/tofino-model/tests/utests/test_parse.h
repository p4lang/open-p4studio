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

#ifndef MODEL_TEST_PARSE_H
#define MODEL_TEST_PARSE_H

#include <ipb.h>
#include <epb.h>
#include <utests/test_namespace.h>
#include <utests/test_util.h>
#include "gtest.h"


namespace MODEL_CHIP_NAMESPACE {
class Parser;
class Packet;
}

namespace MODEL_CHIP_TEST_NAMESPACE {

using namespace MODEL_CHIP_NAMESPACE;

// Base class that provides helper methods for parser tests.
// Chip-specific implementations in test_parse_<chip>.cpp
class BFN_TEST_NAME(ParserTestBase) : public BaseTest {
 public:
  void SetUp() {
    set_pipe_index(0);
    BaseTest::SetUp();
  }
  virtual void verify_chnl_parser_send_pkt_err(IpbCounters *ipb_counters,
                                               uint expected);
  virtual void set_max_chnl_parser_send_pkt(IpbCounters *ipb_counters);
  virtual void set_max_chnl_parser_send_pkt_err(IpbCounters *ipb_counters);
  virtual void verify_epb_counters(Epb *epb,
                                   int epb_chan,
                                   uint64_t expected_sent);
  virtual uint64_t set_max_epb_chnl_parser_send_pkt(Epb *epb, int epb_chan);
};

class MCN_CONCAT(BFN_TEST_NAME(ParseTest), UsingConfigBase) : public BaseTest {
 protected:
  Packet *pkt1_, *pkt2_;
  Parser *parser_;

 public:
  //                         <   DA           SA      ET I4 C LN  ID  OF  T P CK   SRC     DST    SP  DP>
  //                         <   DA     ><    SA    ><ET><><><LN><ID><OF><><><CK>< SRC  >< DST  ><SP><DP>
  const char *udp_pktstr_ = "080022AABBCC080011DDEEFF080045000100123400001011FFFF0A1122330A4455661188119900000000000000000000000000000000";
  const char *tcp_pktstr_ = "080022AABBCC080011DDEEFF080045000100123400001006FFFF0A1122330A4455660688069900000000000000000000000000000000";

  void SetUp() override;
  void do_test();
  void TearDown() override;;
};

}
#endif //MODEL_TEST_PARSE_H
