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

#include "test_parser_arbiter.h"

namespace MODEL_CHIP_TEST_NAMESPACE {

using namespace MODEL_CHIP_NAMESPACE;

// run generic test using chip-specific register addresses
TEST_F(BFN_TEST_NAME(ParserArbiter), Counters) {
  check_egress_counters(
      &RegisterUtils::addr_pmarb(pipe_index())->parbreg.right);
  check_ingress_counters(
      &RegisterUtils::addr_pmarb(pipe_index())->parbreg.left);
}

// run generic test using chip-specific register addresses
TEST_F(BFN_TEST_NAME(ParserArbiter), CountersIncrement) {
  test_increment(
      &RegisterUtils::addr_pmarb(pipe_index())->parbreg.left,
      &RegisterUtils::addr_pmarb(pipe_index())->parbreg.right);
}

}
