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

#ifndef _JBAY_SHARED_MAU_CHIP_METER_ALU_
#define _JBAY_SHARED_MAU_CHIP_METER_ALU_

#include <register_includes/meter_alu_group_action_ctl.h>
#include <register_includes/meter_ctl.h>

namespace MODEL_CHIP_NAMESPACE {

class MauChipMeterAlu {
 public:
  static bool get_right_action_override( register_classes::MeterAluGroupActionCtl&
                                         meter_alu_group_action_ctl) {
    return meter_alu_group_action_ctl.right_action_override();
  }
  static bool get_meter_lpf_sat_ctl( register_classes::MeterCtl& meter_ctl) {
    return meter_ctl.meter_lpf_sat_ctl();
  }

};

}

#endif
