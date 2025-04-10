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

#ifndef _TOFINOXX_MAU_CHIP_DEPENDENCIES_
#define _TOFINOXX_MAU_CHIP_DEPENDENCIES_

// Chip specific dependency code

#include <rmt-defs.h>
#include <mau-defs.h>


namespace MODEL_CHIP_NAMESPACE {

class MauChipDependencies {

public:
  MauChipDependencies(int chipIndex, int pipeIndex, int mauIndex, Mau *mau) {
  }
  ~MauChipDependencies() { }

  bool     get_right_action_override(int alu) { return false; }
  uint16_t get_ghost_table_thread()           { return 0; }

 private:

};


}

#endif // _TOFINOXX_MAU_CHIP_DEPENDENCIES_
