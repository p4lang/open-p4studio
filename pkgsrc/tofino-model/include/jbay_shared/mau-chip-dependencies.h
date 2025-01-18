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

#ifndef _JBAY_MAU_CHIP_DEPENDENCIES_
#define _JBAY_MAU_CHIP_DEPENDENCIES_

// Chip specific dependency code

#include <rmt-defs.h>
#include <mau-defs.h>

// Reg defs auto-generated from Semifore
#include <register_includes/meter_alu_group_action_ctl_array.h>
#include <register_includes/pred_ghost_thread.h>


namespace MODEL_CHIP_NAMESPACE {

class MauChipDependencies {

public:
  MauChipDependencies(int chipIndex, int pipeIndex, int mauIndex, Mau *mau)
      : meter_alu_group_action_ctl_(default_adapter(meter_alu_group_action_ctl_, chipIndex, pipeIndex, mauIndex)),
        pred_ghost_thread_(default_adapter(pred_ghost_thread_, chipIndex, pipeIndex, mauIndex))

  {
    meter_alu_group_action_ctl_.reset();
    pred_ghost_thread_.reset();
  }
  ~MauChipDependencies() { }

  bool get_right_action_override(int alu) {
    return ((meter_alu_group_action_ctl_.right_action_override(alu) & 1) == 1);
  }
  uint16_t get_ghost_table_thread() {
    return pred_ghost_thread_.pred_ghost_thread();
  }

 private:
  register_classes::MeterAluGroupActionCtlArray  meter_alu_group_action_ctl_;
  register_classes::PredGhostThread              pred_ghost_thread_;

};


}

#endif // _JBAY_MAU_CHIP_DEPENDENCIES_
