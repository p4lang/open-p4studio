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

#ifndef _TEST_DEPARSE_H_
#define _TEST_DEPARSE_H_

#include <utests/test_namespace.h>
#include <utests/test_util.h>
#include "gtest.h"


namespace MODEL_CHIP_NAMESPACE {
class DeparserBlock;
class Packet;
class PacketGenMetadata;
class Phv;
struct LearnQuantumType;
}

namespace MODEL_CHIP_TEST_NAMESPACE {

using namespace MODEL_CHIP_NAMESPACE;

class BFN_TEST_NAME(DeparseTestExtraction) : public BaseTest {
 public:
  void SetUp() {
    BaseTest::SetUp();
    deparser_block_ = tu_->get_objmgr()->deparser_get(pipe_index());
    ASSERT_TRUE(deparser_block_ != nullptr);
  }
  DeparserBlock *deparser_block_;

  void set_default_egress_unicast_port_info(Phv& phv,
                                            bool disable_egress_uc=false) {
    // set PHV bit pointed to for POV index 16
    phv.set_d(Phv::make_word_d(6,2), 0x1u);
    // Set a default egress unicast port so packet is not normally dropped.
    tu_->deparser_set_egress_unicast_port_info(pipe_index(),
                                               0,  // egress unicast port phv
                                               16,  // pov bit
                                               disable_egress_uc);
  }

  Packet* deparse_ingress(Phv& phv, Packet **mirror_pkt) {
    Packet *resubmit_pkt = nullptr;
    PacketGenMetadata* packet_gen_metadata = nullptr;
    LearnQuantumType lq;
    return deparser_block_->DeparseIngress(
      phv, &lq, mirror_pkt, &resubmit_pkt, &packet_gen_metadata);
  }

  Packet* deparse_ingress(Phv& phv) {
    Packet *mirror_pkt = nullptr;
    return deparse_ingress(phv, &mirror_pkt);
  }

  Packet* deparse_egress(Phv& phv) {
    Packet *mirror_pkt = nullptr;
    // set PHV bit pointed to for POV index 16
    phv.set_d(Phv::make_word_d(6,2), 0x1u);
    // Set a default egress unicast port so packet is not normally dropped.
    tu_->deparser_set_egress_unicast_port_info(pipe_index(),
                                               0,  // phv
                                               16,  // pov
                                               false);  // disable
    return deparser_block_->DeparseEgress(phv, &mirror_pkt);
  }
};

}

#endif //_TEST_DEPARSE_H_
