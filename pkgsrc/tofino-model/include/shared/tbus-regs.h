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

#ifndef _SHARED_TBUS_REGS
#define _SHARED_TBUS_REGS

#include <unordered_map>
#include <model_core/register_block.h>

namespace MODEL_CHIP_NAMESPACE {
  class TbusRegs : public model_core::RegisterBlock<RegisterCallback> {
    public:
      TbusRegs(int chipIndex) : RegisterBlock(chipIndex, 0x180000, 0x000090, false, 0, 0, "FakeTbusRegs") {
      }
      virtual ~TbusRegs() {
      }

      bool read(uint32_t offset, uint32_t* data) const {
        //std::unordered_map<uint32_t, uint32_t> elem = tbus_regs_.find(offset);
        auto elem = tbus_regs_.find(offset);
        if (elem == tbus_regs_.end()) {
          *data = BAD_DATA_WORD;
        } else {
          *data = elem->second;
        }
        return true;
      }
      bool write(uint32_t offset, uint32_t data) {
        tbus_regs_[offset] = data;
        return true;
      }

      std::string to_string(bool print_zeros = false, std::string indent_string = "") const {
        return "";
      }

      std::string to_string(uint32_t offset,bool print_zeros = false, std::string indent_string = "") const {
      return "";
    }

  private:
    std::unordered_map<uint32_t, uint32_t> tbus_regs_;
  };
}

#endif

