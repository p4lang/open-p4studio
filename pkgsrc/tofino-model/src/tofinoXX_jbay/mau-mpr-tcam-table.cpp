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

#include <mau.h>

namespace MODEL_CHIP_NAMESPACE {

uint8_t MauMprTcamTable::get_powered_ltcams(uint32_t powered_lts) {
  RMT_ASSERT(mau_ != nullptr);
  uint32_t powered_ltcams = 0u;
  for (int lt = 0; lt < kLogicalTables; lt++) {
    if (((powered_lts >> lt) & 1) == 1) {
      MauLogicalTable *ltab = mau_->logical_table_lookup(lt);
      RMT_ASSERT(ltab != nullptr);
      powered_ltcams |= static_cast<uint32_t>(ltab->logical_tcams());
    }
  }
  RMT_ASSERT((powered_ltcams & ~0xFFu) == 0u);
  return static_cast<uint8_t>(powered_ltcams);
}

}
