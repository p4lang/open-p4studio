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

#ifndef _SHARED_TM_REGS
#define _SHARED_TM_REGS

#include <unordered_map>
#include <model_core/register_block.h>
#include <register_utils.h>

namespace MODEL_CHIP_NAMESPACE {
  class TmRegs : public model_core::RegisterBlock<RegisterCallback> {
    static_assert( RegisterUtils::kPreStartAddress > RegisterUtils::kTmStartAddress,
                   "Code assumes PRE starts after TM") ;
    static constexpr size_t kStart = RegisterUtils::kTmStartAddress;
    static constexpr size_t kSize  = RegisterUtils::kPreStartAddress - kStart;
    public:
    TmRegs(int chipIndex) : RegisterBlock(chipIndex, kStart, kSize, false, 0, 0, "FakeTmRegs") {
      }
      virtual ~TmRegs() {
      }

      bool read(uint32_t offset, uint32_t* data) const {
        //std::unordered_map<uint32_t, uint32_t> elem = tm_regs_.find(offset);
        auto elem = tm_regs_.find(offset);
        if (elem == tm_regs_.end()) {
          // offset not yet written to
          *data = RegisterUtils::kTmDefaultRegVal;
        } else {
          *data = elem->second;
        }
        return true;
      }
      bool write(uint32_t offset, uint32_t data) {
        tm_regs_[offset] = data;
        return true;
      }

      std::string to_string(bool print_zeros = false, std::string indent_string = "") const {
        return "";
      }

      std::string to_string(uint32_t offset,bool print_zeros = false, std::string indent_string = "") const {
      return "";
    }

  private:
    std::unordered_map<uint32_t, uint32_t> tm_regs_;
  };
}

// Need another block of TM registers after leaving hole for PRE registers.
// PRE  is modeled hence register should not be faked.

namespace MODEL_CHIP_NAMESPACE {
  class TmRegs2 : public model_core::RegisterBlock<RegisterCallback> {
    static_assert( RegisterUtils::kPscStartAddress > RegisterUtils::kPreStartAddress,
                   "Code assumes PSC starts after PRE") ;
    static_assert( RegisterUtils::kTmLastAddress > RegisterUtils::kPscStartAddress,
                   "Code assumes PSC starts before TM end") ;
    static constexpr size_t kStart = RegisterUtils::kPscStartAddress;
    static constexpr size_t kSize  = RegisterUtils::kTmLastAddress - kStart + 4;
    public:
    TmRegs2(int chipIndex) : RegisterBlock(chipIndex, kStart, kSize, false, 0, 0, "FakeTmRegs2") {
      }
      virtual ~TmRegs2() {
      }

      bool read(uint32_t offset, uint32_t* data) const {
        //std::unordered_map<uint32_t, uint32_t> elem = tm_regs_.find(offset);
        auto elem = tm_regs_.find(offset);
        if (elem == tm_regs_.end()) {
          // offset not yet written to
          *data = RegisterUtils::kTmDefaultRegVal;
        } else {
          *data = elem->second;
        }
        return true;
      }
      bool write(uint32_t offset, uint32_t data) {
        tm_regs_[offset] = data;
        return true;
      }

      std::string to_string(bool print_zeros = false, std::string indent_string = "") const {
        return "";
      }

      std::string to_string(uint32_t offset,bool print_zeros = false, std::string indent_string = "") const {
      return "";
    }

  private:
    std::unordered_map<uint32_t, uint32_t> tm_regs_;
  };
}

#endif
