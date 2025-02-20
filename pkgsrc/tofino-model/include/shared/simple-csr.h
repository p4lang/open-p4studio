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

#ifndef _SHARED_SIMPLE_CSR_H_
#define _SHARED_SIMPLE_CSR_H_

#include <cassert>
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <mem_utils.h>
#include <model_core/register_block.h>


namespace MODEL_CHIP_NAMESPACE {
namespace register_classes {

class SimpleCsr : public model_core::RegisterBlock<RegisterCallback> {

 public:
  SimpleCsr(int chipNumber, int addr, const char *name,
            RegisterCallback& write_callback = 0, RegisterCallback& read_callback = 0)
      : RegisterBlock(chipNumber, addr, 4, false, write_callback, read_callback, std::string(name)),
        value_(0u), addr_(addr), name_(name) {
    assert(addr >= 0);
  }
  SimpleCsr()
      : RegisterBlock(0, 0, 0, false, 0, 0, "simple-csr"),
        value_(0u), addr_(0), name_("") {
  }

 public:
  uint32_t    value() const { return value_; }
  int         addr()  const { return addr_; }
  const char *name()  const { return name_; }

  bool read(uint32_t offset, uint32_t* data) const {
    if (read_callback_) read_callback_();
    *data = value_;
    return true;
  }
  bool write(uint32_t offset, uint32_t data) {
    value_ = data;
    if (write_callback_) write_callback_();
    return true;
  }

  void reset(uint32_t data=0u) {
    value_ = data;
    if (write_callback_) write_callback_();
  }

  std::string to_string(uint32_t offset, bool print_zeros = false, std::string indent_string = "") const {
    std::string r("");
    bool all_zeros=true;
    r += indent_string + std::string(name_) + ":\n";
    r += indent_string + "  " + std::string("val") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(value_) ) + "\n";
    all_zeros &= (0 == value_);
    if (all_zeros && !print_zeros) {
      return("");
    } else {
      return r;
    }
  }
  std::string to_string(bool print_zeros = false, std::string indent_string = "") const {
    std::string r("");
    bool all_zeros=true;
    r += indent_string + std::string(name_) + ":\n";
    r += indent_string + "  " + std::string("val") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(value_) ) + "\n";
    all_zeros &= (0 == value_);
    if (all_zeros && !print_zeros) {
      return("");
    } else {
      return r;
    }
  }

 private:
  uint32_t    value_;
  int         addr_;
  const char *name_;
};


} // namespace register_classes
} // namespace MODEL_CHIP_NAMESPACE

#endif // _SHARED_SIMPLE_CSR_H_
