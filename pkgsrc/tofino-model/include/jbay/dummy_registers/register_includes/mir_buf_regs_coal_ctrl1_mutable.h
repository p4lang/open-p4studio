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


#ifndef __REGISTER_INCLUDES_MIR_BUF_REGS_COAL_CTRL1_MUTABLE_H__
#define __REGISTER_INCLUDES_MIR_BUF_REGS_COAL_CTRL1_MUTABLE_H__


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

class MirBufRegsCoalCtrl1Mutable : public model_core::RegisterBlock<RegisterCallback> {
public:
  MirBufRegsCoalCtrl1Mutable(
      int chipNumber, int index_pipe_addrmap, int index_mir_buf_regs_coal_desc_grp, RegisterCallback& write_callback = 0, RegisterCallback& read_callback = 0
  )
    : RegisterBlock(chipNumber, StartOffset(index_pipe_addrmap, index_mir_buf_regs_coal_desc_grp), 4, true, write_callback, read_callback, std::string("MirBufRegsCoalCtrl1Mutable")+":"+boost::lexical_cast<std::string>(index_pipe_addrmap) + "," + boost::lexical_cast<std::string>(index_mir_buf_regs_coal_desc_grp))
    {
    }
  MirBufRegsCoalCtrl1Mutable(
      
  )
    : RegisterBlock(0, 0, 0, true, 0, 0, "MirBufRegsCoalCtrl1Mutable")
    {
    }
public:






  uint16_t coal_extract_length() { return coal_extract_length_; }
  void coal_extract_length(const uint16_t &v) { coal_extract_length_=v; }






  uint8_t coal_sflow_type() { return coal_sflow_type_; }
  void coal_sflow_type(const uint8_t &v) { coal_sflow_type_=v; }







  uint8_t coal_ext_mode() { return coal_ext_mode_; }
  void coal_ext_mode(const uint8_t &v) { coal_ext_mode_=v; }






  uint8_t coal_min() { return coal_min_; }
  void coal_min(const uint8_t &v) { coal_min_=v; }






  uint8_t coal_max() { return coal_max_; }
  void coal_max(const uint8_t &v) { coal_max_=v; }

  bool read(
      uint32_t offset, uint32_t* data
      ) const {
    if (read_callback_) read_callback_();
    *data = (coal_extract_length_ & 0x3ff);
    *data |= ((coal_sflow_type_ & 0x1) << 12);
    *data |= ((coal_ext_mode_ & 0x1) << 13);
    *data |= (coal_min_ << 16);
    *data |= (coal_max_ << 24);
    return true;
  }


  bool write(
      uint32_t offset, uint32_t data
      ) {
    coal_extract_length_ = (data & 0x3ff);
    coal_sflow_type_ = ((data >> 12) & 0x1);
    coal_ext_mode_ = ((data >> 13) & 0x1);
    coal_min_ = (data >> 16);
    coal_max_ = (data >> 24);
    if (write_callback_) write_callback_();
    return true;
  }

  void reset(
      
      ) {
    coal_extract_length_ = 0x0;
    coal_sflow_type_ = 0x0;
    coal_ext_mode_ = 0x0;
    coal_min_ = 0x0;
    coal_max_ = 0x0;
    if (write_callback_) write_callback_();
  }

  std::string to_string(
      uint32_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    r += indent_string + std::string("MirBufRegsCoalCtrl1Mutable") + ":\n";
    r += indent_string + "  " + std::string("coal_extract_length") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(coal_extract_length_) ) + "\n";
    all_zeros &= (0 == coal_extract_length_);
    r += indent_string + "  " + std::string("coal_sflow_type") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(coal_sflow_type_) ) + "\n";
    all_zeros &= (0 == coal_sflow_type_);
    r += indent_string + "  " + std::string("coal_ext_mode") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(coal_ext_mode_) ) + "\n";
    all_zeros &= (0 == coal_ext_mode_);
    r += indent_string + "  " + std::string("coal_min") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(coal_min_) ) + "\n";
    all_zeros &= (0 == coal_min_);
    r += indent_string + "  " + std::string("coal_max") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(coal_max_) ) + "\n";
    all_zeros &= (0 == coal_max_);
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
    r += indent_string + std::string("MirBufRegsCoalCtrl1Mutable") + ":\n";
    r += indent_string + "  " + std::string("coal_extract_length") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(coal_extract_length_) ) + "\n";
    all_zeros &= (0 == coal_extract_length_);
    r += indent_string + "  " + std::string("coal_sflow_type") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(coal_sflow_type_) ) + "\n";
    all_zeros &= (0 == coal_sflow_type_);
    r += indent_string + "  " + std::string("coal_ext_mode") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(coal_ext_mode_) ) + "\n";
    all_zeros &= (0 == coal_ext_mode_);
    r += indent_string + "  " + std::string("coal_min") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(coal_min_) ) + "\n";
    all_zeros &= (0 == coal_min_);
    r += indent_string + "  " + std::string("coal_max") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(coal_max_) ) + "\n";
    all_zeros &= (0 == coal_max_);
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  uint16_t coal_extract_length_;
  uint8_t coal_sflow_type_;
  uint8_t coal_ext_mode_;
  uint8_t coal_min_;
  uint8_t coal_max_;
private:
  int StartOffset(
      int index_pipe_addrmap, int index_mir_buf_regs_coal_desc_grp
      ) {
    int offset=0;
    offset += 0x2000000; // to get to pipes
    assert(index_pipe_addrmap < 4);
    offset += index_pipe_addrmap * 0x800000; // pipe_addrmap[]
    offset += 0x780000; // to get to deparser
    offset += 0x20000; // to get to mirror
    offset += 0x8000; // to get to mir_buf_regs
    assert(index_mir_buf_regs_coal_desc_grp < 8);
    offset += index_mir_buf_regs_coal_desc_grp * 0x1c; // mir_buf_regs_coal_desc_grp[]
    offset += 0x204; // to get to coal_desc_grp_coal_ctrl1
    return offset;
  }

};









  }; // namespace register_classes
}; // namespace jbay

#endif // __REGISTER_INCLUDES_MIR_BUF_REGS_COAL_CTRL1_MUTABLE_H__
