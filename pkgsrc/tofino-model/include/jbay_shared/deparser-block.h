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

#ifndef _JBAY_DEPARSER_BLOCK_
#define _JBAY_DEPARSER_BLOCK_

#include <cstdint>
#include <pipe-object.h>
#include <deparser.h>
#include <deparser-egress.h>
#include <deparser-ingress.h>
#include <deparser-learning.h>
#include <deparser-reg.h>
#include <deparser-chip-reg.h>


namespace MODEL_CHIP_NAMESPACE {

  class Packet;
  class Phv;
  class Pipe;
  class RmtObjectManager;

  class DeparserBlock : public PipeObject {

 public:
    DeparserBlock(RmtObjectManager *om, int pipeIndex);
    virtual ~DeparserBlock();
    // Reset all modules in deparser block.
    void Reset();

    inline Deparser *ingress()  { return &ingress_; }
    inline Deparser *egress()   { return &egress_; }

    inline Packet *DeparseIngress(const Phv &phv,
                                  LearnQuantumType* learn_quantum,
                                  Packet **mirror_pkt,
                                  Packet **resubmit_pkt,
                                  PacketGenMetadata **packet_gen_metadata) {
      return ingress_.Deparse(phv, learn_quantum, mirror_pkt, resubmit_pkt,
                              packet_gen_metadata);
    }
    inline Packet *DeparseEgress(Phv &phv, Packet **mirror_pkt) {
      return egress_.Deparse(phv, mirror_pkt);
    }

    // As convenience shim these through to ingress deparser
    inline Pipe *pipe()         const { return ingress_.pipe(); }
    inline void  set_pipe(Pipe *pipe) {
      ingress_.set_pipe(pipe);
      egress_.set_pipe(pipe);
    }
    DeparserReg *deparser_reg() { return &deparser_reg_; }

 private:
    // DeparserBlock owns all modules in the deparser but
    //  DeparserLearning is now in DeparserIngress as it needs pov
    DeparserReg                  deparser_reg_;
    DeparserIngress              ingress_;
    DeparserEgress               egress_;
  };
}

#endif // _JBAY_DEPARSER_BLOCK_
