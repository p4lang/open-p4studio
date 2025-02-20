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

#ifndef _TOFINOB0_RMT_DEFS_
#define _TOFINOB0_RMT_DEFS_

#include <model_core/model.h>
#include <rmt-defs-tofinoXX.h>

namespace tofinoB0 {

class RmtDefs : public RmtDefsTofinoXX {
 public:
    static constexpr uint8_t kChipType = model_core::ChipType::kTofinoB0;
};

}

#endif // _TOFINOB0_RMT_DEFS_
