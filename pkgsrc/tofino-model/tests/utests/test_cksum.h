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

#ifndef _TEST_CKSUM_H
#define _TEST_CKSUM_H

#include <utests/test_util.h>
#include <iostream>
#include <string>
#include <array>
#include <cassert>

#include <crafter/Crafter.h>
#include <crafter/Utils/CrafterUtils.h>
#include "gtest.h"

#include <bitvector.h>
#include <tcam.h>
#include <rmt-object-manager.h>
#include <packet-pipe-data.h>
#include <packet.h>
#include <phv.h>
#include <parser.h>
#include <parser-block.h>
#include <parser-static-config.h>
#include <model_core/model.h>

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;


namespace MODEL_CHIP_TEST_NAMESPACE {

using namespace MODEL_CHIP_NAMESPACE;


// defined in test_cksum.cpp ...
uint16_t ones_cmpl_finalize(uint32_t val32);
uint16_t ones_cmpl_negate(uint16_t val);
uint16_t ones_cmpl_add(uint16_t A, uint16_t B);
uint16_t ones_cmpl_subtract(uint16_t A, uint16_t B);
Packet *make_tcp_packet(RmtObjectManager *om, uint64_t seed, int corrupted_byte,
                        uint16_t payload_len, uint32_t payload_first32byte_mask,
                        uint16_t *packet_checksum, uint16_t *payload_checksum,
                        bool set_ip_id_from_tcp_seg_len=true,
                        int payload_zero_len=0,
                        unsigned char *payload_first32byte=nullptr);

class BFN_TEST_NAME(CksumTestFixture) : public BaseTest {
 public:
  Parser *prsr_;
  uint16_t hdr_len_ = 14 + 20 + 20; // Eth=14 IP=20 TCP=20
  uint64_t seed_ = 99;

  void SetUp();
  void TearDown();
  uint16_t get_residual_checksum(uint16_t payload_len,
                                 int payload_zero_len);
  void config_checksums(Parser *p);
  void config_residual_checksum(Parser *p);
};

}
#endif // _TEST_CKSUM_H
