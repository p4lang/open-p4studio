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

#include <parser-arbiter.h>

namespace MODEL_CHIP_NAMESPACE {

ParserArbiter::ParserArbiter(RmtObjectManager *om, int pipeIndex)
    : ParserArbiterCommon<counter_reg_t>(om, pipeIndex, create_counter) { }

ParserArbiter::counter_reg_t ParserArbiter::create_counter(
    int chipIndex,
    int pipeIndex,
    CtrEnum generic_reg_enum) {
  switch (generic_reg_enum) {
    case CtrEnum::EEopCount:
      return counter_reg_t(
        chipIndex,
        pipeIndex,
        counter_reg_t::ParbRightRegEnum::kEEopCount);
    case CtrEnum::EPhvCount:
      return counter_reg_t(
        chipIndex,
        pipeIndex,
        counter_reg_t::ParbRightRegEnum::kEPhvCount);
    case CtrEnum::IEopCount:
      return counter_reg_t(
        chipIndex,
        pipeIndex,
        counter_reg_t::ParbLeftRegEnum::kIEopCount);
    case CtrEnum::INormEopCount:
      return counter_reg_t(
        chipIndex,
        pipeIndex,
        counter_reg_t::ParbLeftRegEnum::kINormEopCount);
    case CtrEnum::INormPhvCount:
      return counter_reg_t(
        chipIndex,
        pipeIndex,
        counter_reg_t::ParbLeftRegEnum::kINormPhvCount);
    case CtrEnum::IPhvCount:
      return counter_reg_t(
        chipIndex,
        pipeIndex,
        counter_reg_t::ParbLeftRegEnum::kIPhvCount);
    case CtrEnum::IResubEopCount:
      return counter_reg_t(
        chipIndex,
        pipeIndex,
        counter_reg_t::ParbLeftRegEnum::kIResubEopCount);
    case CtrEnum::IResubPhvCount:
      return counter_reg_t(
        chipIndex,
        pipeIndex,
        counter_reg_t::ParbLeftRegEnum::kIResubPhvCount);
    case CtrEnum::CtrEnumUnknown:
    default: RMT_ASSERT(0);
  }
}
}
