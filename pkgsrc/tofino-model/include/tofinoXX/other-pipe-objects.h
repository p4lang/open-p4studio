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

#ifndef _TOFINOXX_OTHER_PIPE_OBJECTS_
#define _TOFINOXX_OTHER_PIPE_OBJECTS_

#include <rmt-object-manager.h>
namespace MODEL_CHIP_NAMESPACE {

/**
 * Provide dummy classes with no-op method implementations for components that
 * do not exist in Tofino. These enable calling code to be shared between chip
 * types.
 */

class EgressBuf {
 public:
  EgressBuf(RmtObjectManager *om, int pipeIndex) {}
  void increment_dprsr_rcv_pkt(int portIndex) {}
  void increment_warp_rcv_pkt(int portIndex) {}
  void increment_mac_xmt_pkt(int portIndex) {}
};

class Converter {
 public:
  Converter(RmtObjectManager *om, int pipeIndex) {}
  void increment_pkt_ctr(int portIndex, uint64_t amount=1) {}
  void increment_byte_ctr(int portIndex, uint64_t amount=1) {}
};

class P2s : public Converter {
  using Converter::Converter;
};

class S2p : public Converter {
  using Converter::Converter;
 public:
  void map_logical_to_physical(Packet *pkt, bool mirrored) {}

};

}

#endif // _TOFINOXX_OTHER_PIPE_OBJECTS_
