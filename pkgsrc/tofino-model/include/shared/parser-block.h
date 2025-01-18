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

#ifndef _SHARED_PARSER_BLOCK_
#define _SHARED_PARSER_BLOCK_

#include <cstdint>
#include <rmt-log.h>
#include <pipe-object.h>
#include <phv.h>
#include <packet.h>
#include <parser.h>
#include <epb.h>


namespace MODEL_CHIP_NAMESPACE {

  class ParserBlock : public PipeObject {

 public:
    ParserBlock(RmtObjectManager *om, int pipeIndex, int parseIndex, const ParserConfig &config);
    virtual ~ParserBlock();

    inline Parser *ingress()  { return &ingress_; }
    //inline Epb   *epb()     { return &epb_; }
    inline Parser *egress()   { return &egress_; }


    inline Phv *parse_ingress(Packet *packet, int chan) {
      return ingress_.parse(packet, chan);
    }
    inline Phv *parse_egress(Packet *packet, int chan) {
      return egress_.parse(packet, chan);
    }

    // As convenience shim these through to ingress parser
    inline  int   parser_index() const { return ingress_.parser_index(); }
    inline  Pipe *pipe()         const { return ingress_.pipe(); }
    virtual void  set_pipe(Pipe *pipe) {
      PipeObject::set_pipe(pipe);
      ingress_.set_pipe(pipe);
      egress_.set_pipe(pipe);
    }

    // For testing
    inline Phv *parse(Packet *packet, int chan=0) {
      return parse_ingress(packet, chan);
    }


 private:
    //register_classes::PrsrRegCommonRspecMutable  prs_common_rspec_; - No longer exists since regs_7217_parser-split
    //Epb                                    epb_;
    Parser                                   ingress_;
    Parser                                   egress_;
  };
}

#endif // _SHARED_PARSER_BLOCK_
