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

#ifndef _UTESTS_CMP_HELPER_
#define _UTESTS_CMP_HELPER_

#include "gtest.h"

// This lets you get Hex prints of expected and actual values, use like this:
//   EXPECT_PRED_FORMAT2(CmpHelperIntHex , expected , actual );
testing::AssertionResult CmpHelperIntHex(const char* expected_expr, const char* actual_expr, int expected, int actual);


#endif // _UTESTS_CMP_HELPER_

