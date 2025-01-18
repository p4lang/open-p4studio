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

#ifndef _TOFINOXX_MAU_HASH_DISTRIBUTION_CHIP_REGS_H_
#define _TOFINOXX_MAU_HASH_DISTRIBUTION_CHIP_REGS_H_

#include <rmt-defs.h>
#include <mau-defs.h>
#include <mau-object.h>

namespace MODEL_CHIP_NAMESPACE {

  class MauHashDistributionChipRegs : public MauObject {

 public:
    MauHashDistributionChipRegs(RmtObjectManager *om, int pipeIndex, int mauIndex, Mau *mau) :
        MauObject(om, pipeIndex, mauIndex, mau) {
    }
    ~MauHashDistributionChipRegs() { }


 public:
    inline int selector_mod_shift_fixup(int alu) {
      return 0;
    }
    inline bool selector_mod_is_vpn(int alu, uint8_t addr_type) {
      return false;
    }
    inline uint32_t selector_dividend(int alu, uint8_t addr_type, uint32_t hash_input) {
      // Return low 10b of hash_input
      return (hash_input >> 0) & 0x3FFu;
    }
    inline uint8_t selector_divisor(int alu, uint8_t addr_type, uint32_t selector_len) {
      // Return low 5b of selector_len
      return static_cast<uint8_t>((selector_len >> 0) & 0x1Fu);
    }

  };
}

#endif // _TOFINOXX_MAU_HASH_DISTRIBUTION_CHIP_REGS_H_
