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

#ifndef __JBAY_SHARED_TM_DEFINES__
#define __JBAY_SHARED_TM_DEFINES__
#include <rmt-defs.h>

namespace MODEL_CHIP_NAMESPACE {

class TmDefs {
 public:
  static constexpr int kNumTmPipes = RmtDefs::kPipesMax;
  static constexpr int kNumMacPerPipe = RmtDefs::kPortGroupsPerPipe;
  static constexpr int kNumPortPerMac = RmtDefs::kPortsPerPortGroup;
  static constexpr int kNumL1PerMac = 32;
  static constexpr int kNumVqPerMac = 128;
  static constexpr int kNumPqPerMac = 128;
  static constexpr int kNumPortPerPipe = RmtDefs::kPortsPerPipe;
  static constexpr int kNumL1PerPipe = kNumMacPerPipe*kNumL1PerMac;
  static constexpr int kNumVqPerPipe = kNumMacPerPipe*kNumVqPerMac;
  static constexpr int kNumPqPerPipe = kNumMacPerPipe*kNumPqPerMac;
  static constexpr int kNumPriPerPort = 8;
  static constexpr int kNumPriPerPipe = kNumPortPerPipe * kNumPriPerPort;
  static constexpr int kNumQlcSchUpdIfc = 6;
  static constexpr int kNumQlcSchEnqUpdIfc = 5;
  static constexpr int kNumQlcSchDeqUpdIfc = 1;
  static constexpr int kNumPexSchByteUpdIfc = 2;
  static constexpr int kNumPexSchCrdUpdIfc = 2;
  static constexpr int kSchRefreshIntNs = 80;
  static constexpr int kTmCellSize = 176;
};

}

#endif

