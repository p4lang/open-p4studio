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

#include <utests/test_util.h>
#include <iostream>
#include <string>
#include <array>
#include <cassert>

#include "gtest.h"

#include <rmt-object-manager.h>

#include <model_core/model.h>
extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;


namespace MODEL_CHIP_TEST_NAMESPACE {

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;


  TEST(BFN_TEST_NAME(OverlapsTest),Test) {
    GLOBAL_MODEL->Reset();
    int chip = 212;
    TestUtil tu(GLOBAL_MODEL.get(), chip);
    RmtObjectManager *om = tu.get_objmgr();    
    ASSERT_TRUE(om != NULL);

    // do the overlap checking
    ASSERT_TRUE( GLOBAL_MODEL->CheckNoOverlaps() );
  }

}
