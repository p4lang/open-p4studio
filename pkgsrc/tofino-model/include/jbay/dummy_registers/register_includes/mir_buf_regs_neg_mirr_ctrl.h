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


#ifndef __REGISTER_INCLUDES_MIR_BUF_REGS_NEG_MIRR_CTRL_H__
#define __REGISTER_INCLUDES_MIR_BUF_REGS_NEG_MIRR_CTRL_H__


#include <cstdint>
#include <cassert>
#include <array>
#include <vector>
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <model_core/register_block.h>
#include <shared/bitvector.h>












namespace jbay {
  namespace register_classes {

class MirBufRegsNegMirrCtrl : public model_core::RegisterBlock<RegisterCallback> {
public:
  MirBufRegsNegMirrCtrl(
      int chipNumber, int index_pipe_addrmap, RegisterCallback& write_callback = 0, RegisterCallback& read_callback = 0
  )
    : RegisterBlock(chipNumber, StartOffset(index_pipe_addrmap), 4, false, write_callback, read_callback, std::string("MirBufRegsNegMirrCtrl")+":"+boost::lexical_cast<std::string>(index_pipe_addrmap))
    {
    }
  MirBufRegsNegMirrCtrl(
      
  )
    : RegisterBlock(0, 0, 0, false, 0, 0, "MirBufRegsNegMirrCtrl")
    {
    }
public:







  uint8_t &negmir_min_entries() { return negmir_min_entries_; }








  uint8_t &negmir_max_entries() { return negmir_max_entries_; }





  uint16_t &negmir_sid() { return negmir_sid_; }

  bool read(
      uint32_t offset, uint32_t* data
      ) const {
    if (read_callback_) read_callback_();
    *data = negmir_min_entries_;
    *data |= (negmir_max_entries_ << 8);
    *data |= ((negmir_sid_ & 0x3ff) << 16);
    return true;
  }


  bool write(
      uint32_t offset, uint32_t data
      ) {
    negmir_min_entries_ = data;
    negmir_max_entries_ = (data >> 8);
    negmir_sid_ = ((data >> 16) & 0x3ff);
    if (write_callback_) write_callback_();
    return true;
  }

  void reset(
      
      ) {
    negmir_min_entries_ = 0x8;
    negmir_max_entries_ = 0x20;
    negmir_sid_ = 0x3ff;
    if (write_callback_) write_callback_();
  }

  std::string to_string(
      uint32_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    r += indent_string + std::string("MirBufRegsNegMirrCtrl") + ":\n";
    r += indent_string + "  " + std::string("negmir_min_entries") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(negmir_min_entries_) ) + "\n";
    all_zeros &= (0 == negmir_min_entries_);
    r += indent_string + "  " + std::string("negmir_max_entries") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(negmir_max_entries_) ) + "\n";
    all_zeros &= (0 == negmir_max_entries_);
    r += indent_string + "  " + std::string("negmir_sid") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(negmir_sid_) ) + "\n";
    all_zeros &= (0 == negmir_sid_);
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

  std::string to_string(
      bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    r += indent_string + std::string("MirBufRegsNegMirrCtrl") + ":\n";
    r += indent_string + "  " + std::string("negmir_min_entries") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(negmir_min_entries_) ) + "\n";
    all_zeros &= (0 == negmir_min_entries_);
    r += indent_string + "  " + std::string("negmir_max_entries") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(negmir_max_entries_) ) + "\n";
    all_zeros &= (0 == negmir_max_entries_);
    r += indent_string + "  " + std::string("negmir_sid") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(negmir_sid_) ) + "\n";
    all_zeros &= (0 == negmir_sid_);
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  uint8_t negmir_min_entries_;
  uint8_t negmir_max_entries_;
  uint16_t negmir_sid_;
private:
  int StartOffset(
      int index_pipe_addrmap
      ) {
    int offset=0;
    offset += 0x2000000; // to get to pipes
    assert(index_pipe_addrmap < 4);
    offset += index_pipe_addrmap * 0x800000; // pipe_addrmap[]
    offset += 0x780000; // to get to deparser
    offset += 0x20000; // to get to mirror
    offset += 0x8000; // to get to mir_buf_regs
    offset += 0x8; // to get to mir_glb_group_neg_mirr_ctrl
    return offset;
  }

};









  }; // namespace register_classes
}; // namespace jbay

#endif // __REGISTER_INCLUDES_MIR_BUF_REGS_NEG_MIRR_CTRL_H__
