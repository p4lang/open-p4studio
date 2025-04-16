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

#ifndef _util_h_
#define _util_h_

#include <mcn_test.h>
#include <rmt-defs.h>

#if MCN_TEST(MODEL_CHIP_NAMESPACE,tofino) || MCN_TEST(MODEL_CHIP_NAMESPACE,tofinoB0)
#define PKG_SIZE 1
#elif MCN_TEST(MODEL_CHIP_NAMESPACE,jbay) || MCN_TEST(MODEL_CHIP_NAMESPACE,jbayB0)
#define PKG_SIZE 1
#elif MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd0)
#define PKG_SIZE 2
#elif MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd1)
#define PKG_SIZE 1
#else
#error "unknown model"
#define STRINGIFY_RAW(X)  #X
#define STRINGIFY(X) STRINGIFY_RAW(X)
#pragma message "model: " STRINGIFY(MODEL_CHIP_NAMESPACE)
#endif

#define MAX_PIPE_COUNT ((PKG_SIZE) * RmtDefs::kPipesMax)
#define ALL_PIPES ((1U << MAX_PIPE_COUNT) - 1)
#define PIPE2DIE(logical_pipe) ((logical_pipe) / RmtDefs::kPipesMax)
#define PIPE_WITHIN_DIE(logical_pipe) ((logical_pipe) % RmtDefs::kPipesMax)
#define ASIC_PORT(rmt_port) ((rmt_port) % RmtDefsShared::kPortsTotal)
#define PORT2DIE(rmt_port) ((rmt_port) / RmtDefsShared::kPortsTotal)

#endif /* _util_h_ */
