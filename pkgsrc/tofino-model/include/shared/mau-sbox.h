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

#ifndef _SHARED_MAU_SBOX_H_
#define _SHARED_MAU_SBOX_H_

#include <cstdint>
#include <string>
#include <cstdint>
#include <rmt-defs.h>
#include <mau-defs.h>
#include <mau-object.h>
#include <pipe.h>
#include <cache-id.h>
#include <phv.h>
#include <mau-lookup-result.h>
#include <mau-input.h>
#include <mau-dependencies.h>
#include <mau-result-bus.h>
#include <mau-addr-dist.h>
#include <mau-instr-store.h>
#include <mau-op-handler.h>
#include <mau-memory.h>
#include <mau-logical-row.h>
#include <mau-sram-column.h>
#include <mau-sram-row.h>
#include <mau-sram.h>
#include <mau-logical-tcam.h>
#include <mau-tcam-row.h>
#include <mau-tcam.h>

#include <register_includes/selector_alu_ctl.h>
#include <register_includes/exactmatch_row_hashadr_xbar_ctl_array.h>
#include <register_includes/mau_selector_action_adr_shift_array.h>
#include <register_includes/mau_map_and_alu_row_addrmap.h>
#include <register_includes/meter_alu_group_data_delay_ctl.h>


namespace MODEL_CHIP_NAMESPACE {
  class MauSbox {

    public:
      MauSbox() { }

      ~MauSbox() {}

      uint16_t  sps14(uint16_t  hash);
      uint16_t  sps15(uint16_t  hash);
      uint32_t  sps18(uint32_t  hash);

    private:
      uint8_t sbox3x3(uint8_t value);
      uint8_t sbox4x4(uint8_t value);

  };


}

#endif
