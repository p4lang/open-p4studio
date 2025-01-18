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

#include "cmp_helper.h"

testing::AssertionResult CmpHelperIntHex(const char* expected_expr, const char* actual_expr, int expected, int actual)
{
    if(actual == expected)
        return testing::AssertionSuccess();

    std::stringstream ss, msg;
    std::string expected_str, actual_str;

    ss.str("");
    ss << std::showbase << std::hex << expected;
    expected_str = ss.str();

    ss.str("");
    ss << std::showbase << std::hex << actual;
    actual_str = ss.str();

    msg << "Value of: " << actual_expr;
    if(actual_str != actual_expr) {
        msg << "\n  Actual: " << std::showbase << std::hex << actual;
    }
    msg << "\nExpected: " << expected_expr;
    if(expected_str != expected_expr) {
        msg << "\nWhich is: " << std::showbase << std::hex << expected;
    }

    return testing::AssertionFailure() << msg.str();
}
