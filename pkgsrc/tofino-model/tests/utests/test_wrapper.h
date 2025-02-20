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

#ifndef _TEST_WRAPPER_H_
#define _TEST_WRAPPER_H_

#include <utests/test_util.h>

#include <iostream>
#include <string>
#include <array>
#include <cassert>

#include "gtest.h"

#define INCLUDED_FROM_TEST_WRAPPER

#include <wrapper/ref_model_wrapper.hpp>
#include <model_core/model.h>
#include <rmt-object-manager.h>

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;

namespace MODEL_CHIP_TEST_NAMESPACE {

using namespace MODEL_CHIP_NAMESPACE;

class BFN_TEST_NAME(WrapperTestFixture) : public BaseTest {
 public:
  ref_model_wrapper *wrapper_;

  void SetUp() {
    // ref model wrapper uses chip 0
    set_chip_index(0);
    set_pipe_index(0);
    BaseTest::SetUp();
    tu_->set_dv_test(999);
    // Next line resets all chips/inits chip 0 again!
    // So now there's a new RmtObjectManager for chip 0
    wrapper_ = ref_model_wrapper::getInstance(true);
    ASSERT_TRUE(wrapper_ != nullptr);
    // Tell TestUtil obj to get hold of new RmtObjectManager
    om_ = tu_->get_objmgr();
    ASSERT_TRUE(om_ != nullptr);
    // Make things quieten down (means Wrapper log flags ignored)
    tu_->update_log_flags(ALL, ALL, ALL, ALL, ALL, FEW, FEW);
  }
  void TearDown() {
    ref_model_wrapper::deleteInstance();
  }
};

}

#endif // _TEST_WRAPPER_H_
