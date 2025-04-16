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

#ifndef _UTESTS_TEST_NAMESPACE_
#define _UTESTS_TEST_NAMESPACE_

#include <mcn_test.h>

#define MCN_CONCAT0(x,y) x##y
#define MCN_CONCAT(x,y) MCN_CONCAT0(x,y)


// Default to tofino if no MODEL_CHIP_NAMESPACE specified
#ifndef MODEL_CHIP_NAMESPACE
#define MODEL_CHIP_NAMESPACE tofino
#endif

// And define token MODEL_CHIP_TEST_NAMESPACE
#undef MODEL_CHIP_PROPER_NAME
#undef MODEL_CHIP_TEST_NAMESPACE

// Use BFN_DISABLE_* to temporarily disable a test in a namespace when the
// intention is for the test to be fixed to pass; do not use these macros when
// a test is never expected to pass for a particular namespace - move those
// tests to a chip specific compilation unit; by default these macros do NOT
// disable test
#define BFN_DISABLE_TOFINOXX(name)  name
#define BFN_DISABLE_JBAY(name)  name

// tofino
#if MCN_TEST(MODEL_CHIP_NAMESPACE,tofino)
#define MODEL_CHIP_PROPER_NAME Tofino
#define MODEL_CHIP_TEST_NAMESPACE tofino_test
#undef BFN_DISABLE_TOFINOXX
#define BFN_DISABLE_TOFINOXX(name)  MCN_CONCAT(DISABLED_,name)
// tofinoB0
#elif MCN_TEST(MODEL_CHIP_NAMESPACE,tofinoB0)
#define MODEL_CHIP_PROPER_NAME TofinoB0
#define MODEL_CHIP_TEST_NAMESPACE tofinoB0_test
#undef BFN_DISABLE_TOFINOXX
#define BFN_DISABLE_TOFINOXX(name)  MCN_CONCAT(DISABLED_,name)
// jbay
#elif MCN_TEST(MODEL_CHIP_NAMESPACE,jbay)
#define MODEL_CHIP_PROPER_NAME Jbay
#define MODEL_CHIP_TEST_NAMESPACE jbay_test
#define MODEL_CHIP_JBAY_OR_LATER
#define MODEL_CHIP_JBAYXX
#undef BFN_DISABLE_JBAY
#define BFN_DISABLE_JBAY(name)  MCN_CONCAT(DISABLED_,name)
// jbayB0
#elif MCN_TEST(MODEL_CHIP_NAMESPACE,jbayB0)
#define MODEL_CHIP_PROPER_NAME JbayB0
#define MODEL_CHIP_TEST_NAMESPACE jbayB0_test
#define MODEL_CHIP_JBAY_OR_LATER
#define MODEL_CHIP_JBAYXX
#else
#error Unknown MODEL_CHIP_NAMESPACE specified
#endif

// Macro to create a qualified test name
#define BFN_TEST_NAME(n) MCN_CONCAT(n,MODEL_CHIP_PROPER_NAME)

#ifdef __cplusplus
// Ensure namespaces exist
namespace MODEL_CHIP_NAMESPACE      { }
namespace MODEL_CHIP_TEST_NAMESPACE { }
#endif /* __cplusplus */


#endif /* _UTESTS_TEST_NAMESPACE_ */
