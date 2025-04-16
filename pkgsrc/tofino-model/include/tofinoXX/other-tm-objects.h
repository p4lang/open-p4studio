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

#ifndef _SHARED_OTHER_TM_OBJECTS_
#define _SHARED_OTHER_TM_OBJECTS_
#include <rmt-log.h>
#include <rmt-object-manager.h>

namespace MODEL_CHIP_NAMESPACE {

/**
 * Provide dummy classes with no-op method implementations for components that
 * do not exist in Tofino. These enable calling code to be shared between chip
 * types.
 */

class TmSchModelWrapper : public DefaultLogger {
 public:
  TmSchModelWrapper(RmtObjectManager *om, uint8_t pipe_index) 
    : DefaultLogger(om, RmtTypes::kRmtTypeTm) {}
  void run_sch_model() {}
};

}

#endif // _SHARED_OTHER_TM_OBJECTS_
