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

#ifndef _JBAYB0_RMT_DEFS_
#define _JBAYB0_RMT_DEFS_

#include <rmt-defs-jbayXX.h>

namespace jbayB0 {

class RmtDefs : public RmtDefsJbayXX {
 public:
  static constexpr uint8_t kChipType = model_core::ChipType::kJbayB0;

  RmtDefs()          {}
  virtual ~RmtDefs() {}

};

}

#endif // _JBAYB0_RMT_DEFS_
