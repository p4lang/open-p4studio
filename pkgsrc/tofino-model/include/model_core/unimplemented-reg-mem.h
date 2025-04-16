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

#ifndef  _MODEL_CORE_REG_MEM_H_
#define  _MODEL_CORE_REG_MEM_H_

#include <utility>
#include <unordered_map>

#include <model_core/register_block.h>

namespace model_core {

class UnimplementedReg : public model_core::RegisterBlock<RegisterCallback> {
 public:
  UnimplementedReg(int chipIndex, size_t start, size_t size, std::string name, uint32_t default_val=0u)
      : RegisterBlock(chipIndex, start, size, false, 0, 0, name), default_val_{default_val} {
    // printf("%s: start=0x%lx size=0x%lx\n", name.c_str(), start, size);
  }
  virtual ~UnimplementedReg() { }

  bool read(uint32_t offset, uint32_t* data) const {
    auto elem = reg_map_.find(offset);  // elem == end  ==> offset not yet written to
    *data = (elem == reg_map_.end()) ?default_val_ :elem->second;
    return true;
  }
  bool write(uint32_t offset, uint32_t data) {
    if (data != default_val_) reg_map_[offset] = data;
    return true;
  }
  std::string to_string(bool print_zeros = false, std::string indent_string = "") const {
    return "";
  }
  std::string to_string(uint32_t offset, bool print_zeros = false, std::string indent_string = "") const {
    return "";
  }

 private:
  uint32_t                               default_val_;
  std::unordered_map<uint32_t, uint32_t> reg_map_;
};


class UnimplementedMem : public model_core::RegisterBlockIndirect<RegisterCallback> {
 public:
  UnimplementedMem(int chipIndex, uint64_t start, uint64_t size, std::string name)
      : RegisterBlockIndirect(chipIndex, start, size, false, 0, 0, name) {
    // printf("%s: start=0x%lx size=0x%lx\n", name.c_str(), start, size);
  }
  virtual ~UnimplementedMem() { }

  bool read(uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T) const {
    auto elem = reg_map_.find(offset);  // elem == end  ==> offset not yet written to
    if (elem != reg_map_.end()) {
      *data0 = elem->second.first;
      *data1 = elem->second.second;
    }
    return true;
  }
  bool write(uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T) {
    reg_map_[offset] = std::make_pair(data0, data1);
    return true;
  }
  std::string to_string(bool print_zeros = false, std::string indent_string = "") const {
    return "";
  }
  std::string to_string(uint64_t offset, bool print_zeros = false, std::string indent_string = "") const {
    return "";
  }

 private:
  std::unordered_map< uint64_t, std::pair<uint64_t,uint64_t> > reg_map_;
};


}  // namespace model_core

#endif // _MODEL_CORE_UNIMPLEMENTED_REG_MEM_H_
