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

// MauMapram - TofinoB0 specific code
#include <mau.h>
#include <mau-mapram.h>

namespace tofinoB0 {

uint64_t MauMapram::get_write_mask(uint64_t data0, uint64_t data1) {
  // Return inverse of bits [21:11] as write mask 
  return (~data0 >> kMapramWidth) & kMapramMask;
}


}
