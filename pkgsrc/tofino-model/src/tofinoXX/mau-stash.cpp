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

#include <string>
#include <rmt-log.h>
#include <rmt-object-manager.h>
#include <mau-object.h>
#include <mau-stash.h>
#include <mau-sram-row.h>

namespace MODEL_CHIP_NAMESPACE {


void MauStash::set_match_output(Phv *phv, const int which_bus, const int index, const int which_stash) {
  RMT_ASSERT(index<kEntries);

  BitVector<kMatchOutputBusWidth> busVal;

  busVal.set_word(entries_[index].get_word(0,64),0,64);
  busVal.set32(2, mau_stash_reg_.get_match_address(index));

  row_->set_match_output_bus(which_bus,busVal,888);
}

}
