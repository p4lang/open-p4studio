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

#include <cstdio>
#include "../../include/model_core/model.h"

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;

int main(int argc, char* argv[]) {
  GLOBAL_MODEL.reset(new model_core::Model(1,NULL));
  bool ok = GLOBAL_MODEL.get()->CreateChip(0, model_core::ChipType::kRsvd3);
  assert(ok);
  printf("Successfully created rsvd1 using libdv-pre.a\n");
  int chip=0;
  uint64_t address=0;
  uint64_t data0=0;
  uint64_t data1=0;
  uint64_t timestamp=0;
  GLOBAL_MODEL->IndirectWrite(chip, address, data0, data1, timestamp);
  return 0;
}
