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
#include <mau-addr-dist.h>
#include <mau-chip-addr-dist.h>


namespace MODEL_CHIP_NAMESPACE {

void MauChipAddrDist::sweep_write_callback(int alu) {
  RMT_ASSERT((alu >= 0) && (alu < static_cast<int>(MauDefs::kNumMeterAlus)));
  if (ctor_running_) return;
  // Take copy of prev per-alu values
  int prev_subw_shift = subw_shift_[alu];
  int prev_sweep_op4  = sweep_op4_[alu];
  // Update with latest values
  subw_shift_[alu] = get_meter_sweep_subword_shift(alu);
  sweep_op4_[alu] = get_meter_sweep_op4(alu);
  // If any change upcall superclass meter_sweep_change_callback
  if ((subw_shift_[alu] != prev_subw_shift) ||
      (sweep_op4_[alu] != prev_sweep_op4))
    mau_addr_dist_->meter_sweep_change_callback(alu);
}

}
