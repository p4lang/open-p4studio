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

#ifndef _JBAYXX_EGRESS_BUF_
#define _JBAYXX_EGRESS_BUF_

#include <array>

#include "rmt-defs.h"
#include "common/rmt-util.h"
#include "pipe-object.h"
#include "rmt-defs-jbay-shared.h"
#include "register_includes/ebuf100_chnl_pktnum_mutable.h"
#include "register_includes/ebuf400_chnl_pktnum_mutable.h"

#include "utests/test_namespace.h"
namespace MODEL_CHIP_TEST_NAMESPACE {
class MCN_CONCAT(BFN_TEST_NAME(EgressBuf),_GetChanGroup_Test);
}

namespace MODEL_CHIP_NAMESPACE {

class RmtObjectManager;

using namespace model_common;

/**
 * Defines an interface to Ebuf wrapper objects.
 */
class EgressBufChanGroup {
 public:
  virtual void increment_dprsr_rcv_pkt() = 0;
  virtual void increment_warp_rcv_pkt() = 0;
  virtual void increment_mac_xmt_pkt() = 0;
};

/**
 * Parameterised Ebuf channel group wrapper.
 * @tparam CHNL_PKTNUM_T The type of the Ebuf ChnlPktnum register class.
 */
template <typename CHNL_PKTNUM_T>
class EgressBufChanGroupImpl : public EgressBufChanGroup {
  // Wraps members of an EgressBuf channel group (currently just chnl_pktnum) and
  // implements a common interface of accessor methods. Templated for use with
  // both Ebuf400 and Ebuf100 variants.
 public:
  EgressBufChanGroupImpl(int chipIndex,
                         int pipeIndex,
                         int sliceIndex,
                         int chanIndex) :
      chnl_pktnum_(CHNL_PKTNUM_T(chipIndex,
                                 pipeIndex,
                                 sliceIndex,
                                 chanIndex)) {}

  EgressBufChanGroupImpl(int chipIndex,
                         int pipeIndex,
                         int sliceIndex,
                         int egressBufIndex,
                         int chanIndex) :
      chnl_pktnum_(CHNL_PKTNUM_T(chipIndex,
                                 pipeIndex,
                                 sliceIndex,
                                 egressBufIndex,
                                 chanIndex)) {}

  void reset() {
    chnl_pktnum_.reset();
  }

  void increment_dprsr_rcv_pkt() override {
    chnl_pktnum_.dprsr_rcv_pkt(
        Util::increment_and_wrap(chnl_pktnum_.dprsr_rcv_pkt(), 64));
  }

  void increment_warp_rcv_pkt() override {
    chnl_pktnum_.warp_rcv_pkt(
        Util::increment_and_wrap(chnl_pktnum_.warp_rcv_pkt(), 64));
  }

  void increment_mac_xmt_pkt() override {
    chnl_pktnum_.mac_xmt_pkt(
        Util::increment_and_wrap(chnl_pktnum_.mac_xmt_pkt(), 64));
  }

 private:
  CHNL_PKTNUM_T chnl_pktnum_;
};

/**
 * Models a pipe's Egress Buffer.
 */
class EgressBuf : public PipeObject {
 public:
  EgressBuf(RmtObjectManager *om, int pipeIndex);
  void reset();
  void increment_dprsr_rcv_pkt(int portIndex);
  void increment_warp_rcv_pkt(int portIndex);
  void increment_mac_xmt_pkt(int portIndex);

 private:
  friend MODEL_CHIP_TEST_NAMESPACE::MCN_CONCAT(BFN_TEST_NAME(EgressBuf),_GetChanGroup_Test);
  EgressBufChanGroup* get_chan_group(int portIndex);
  static const int kEbuf400ChannelsPerPipe =
      RmtDefs::kEgressBufChannelsPerEbuf400 *
          RmtDefs::kEgressBufEbuf400PerSlice *
          RmtDefs::kEgressBufSlicesPerPipe;
  static const int kEbuf100ChannelsPerPipe =
      RmtDefs::kEgressBufChannelsPerEbuf100 *
          RmtDefs::kEgressBufEbuf100PerSlice *
          RmtDefs::kEgressBufSlicesPerPipe;

  std::array<
      EgressBufChanGroupImpl<register_classes::Ebuf100ChnlPktnumMutable>,
      kEbuf100ChannelsPerPipe> ebuf100_chan_groups_;
  std::array<
      EgressBufChanGroupImpl<register_classes::Ebuf400ChnlPktnumMutable>,
      kEbuf400ChannelsPerPipe> ebuf400_chan_groups_;
};

}  // namespace MODEL_CHIP_NAMESPACE

#endif //_JBAYXX_EGRESS_BUF_
