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

#ifndef _JBAYXX_RMT_DEFS_
#define _JBAYXX_RMT_DEFS_

#include <rmt-defs-jbay-shared.h>

namespace MODEL_CHIP_NAMESPACE {

class RmtDefsJbayXX : public RmtDefsJbayShared {
 public:

  // jbay total pipes == pipes per chip
  static constexpr int kPackagePipeBits = kPipeBits;

  // SKU defs
  static constexpr int kSkuDefault = 200;
  static constexpr int kSkuMin = 200;
  static constexpr int kSkuMax = 299; // Not sure of exact max yet

  // IPB Meta0 has 9b logical ports on JBay
  static constexpr int kIpbMeta0LogicalPortWidth = 9;
  // IPB Meta1 16B on JBay (also on resubmit)
  static constexpr int kIpbMeta1SizeBytes      = 16;
  static constexpr int kIpbMeta1ResubSizeBytes = 16;

  // only 10 of 20 extractors are able to extract a constant
  static constexpr int kParserNumImmConstExtractors = 10;

  static constexpr int kPortDieWidth = 0;  // n/a for jbay
  static constexpr int kPortWidth = 9;     // port[8:0]
  static constexpr int kIngressLocalPortShift = 0; // these 5 n/a for JBay
  static constexpr int kPipeTmLocalPortShift = 0;
  static constexpr int kTmPipeLocalPortShift = 0;
  static constexpr int kEgressLocalPortShift = 0;
  static constexpr int kMacChanShift = 0;

  static constexpr int kDeparserEgressUnicastPortWidth = kPortWidth;
  static constexpr int kDeparserXidL2Width = 9;
  static constexpr int kDeparserMirrEpipePortWidth = kPortWidth;
  static constexpr int kPresTotal = 4;
  static constexpr int kTmPipesMax = 4;

  static constexpr uint16_t kI2qMulticastPipeVectorMask = 0x1Fu;

  // JBay only 8 constants
  static constexpr int kDeparserNumConstants = 8;
  static constexpr int     kTmPortGroupsPerPipe = 9;
  static constexpr int     kTmPortsPerPortGroup = 8;
  static constexpr int     kTmPortsPerPipe = kTmPortsPerPortGroup * kTmPortGroupsPerPipe;

  RmtDefsJbayXX()          {}
  virtual ~RmtDefsJbayXX() {}

};

}

#endif // _JBAYXX_RMT_DEFS_
