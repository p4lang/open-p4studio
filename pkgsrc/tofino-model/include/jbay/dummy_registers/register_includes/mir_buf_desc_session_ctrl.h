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


#ifndef __REGISTER_INCLUDES_MIR_BUF_DESC_SESSION_CTRL_H__
#define __REGISTER_INCLUDES_MIR_BUF_DESC_SESSION_CTRL_H__


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

class MirBufDescSessionCtrl : public model_core::RegisterBlock<RegisterCallback> {
public:
  MirBufDescSessionCtrl(
      int chipNumber, int index_pipe_addrmap, int index_mir_buf_desc_norm_desc_grp, RegisterCallback& write_callback = 0, RegisterCallback& read_callback = 0
  )
    : RegisterBlock(chipNumber, StartOffset(index_pipe_addrmap, index_mir_buf_desc_norm_desc_grp), 4, false, write_callback, read_callback, std::string("MirBufDescSessionCtrl")+":"+boost::lexical_cast<std::string>(index_pipe_addrmap) + "," + boost::lexical_cast<std::string>(index_mir_buf_desc_norm_desc_grp))
    {
    }
  MirBufDescSessionCtrl(
      
  )
    : RegisterBlock(0, 0, 0, false, 0, 0, "MirBufDescSessionCtrl")
    {
    }
public:





  uint8_t &norm_ingr_ena() { return norm_ingr_ena_; }





  uint8_t &norm_egr_ena() { return norm_egr_ena_; }






  uint8_t &norm_max_entries() { return norm_max_entries_; }






  uint16_t &norm_trunc_size() { return norm_trunc_size_; }

  bool read(
      uint32_t offset, uint32_t* data
      ) const {
    if (read_callback_) read_callback_();
    *data = (norm_ingr_ena_ & 0x1);
    *data |= ((norm_egr_ena_ & 0x1) << 1);
    *data |= ((norm_max_entries_ & 0x3f) << 8);
    *data |= ((norm_trunc_size_ & 0x3fff) << 16);
    return true;
  }


  bool write(
      uint32_t offset, uint32_t data
      ) {
    norm_ingr_ena_ = (data & 0x1);
    norm_egr_ena_ = ((data >> 1) & 0x1);
    norm_max_entries_ = ((data >> 8) & 0x3f);
    norm_trunc_size_ = ((data >> 16) & 0x3fff);
    if (write_callback_) write_callback_();
    return true;
  }

  void reset(
      
      ) {
    norm_ingr_ena_ = 0x0;
    norm_egr_ena_ = 0x0;
    norm_max_entries_ = 0x0;
    norm_trunc_size_ = 0x0;
    if (write_callback_) write_callback_();
  }

  std::string to_string(
      uint32_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    r += indent_string + std::string("MirBufDescSessionCtrl") + ":\n";
    r += indent_string + "  " + std::string("norm_ingr_ena") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(norm_ingr_ena_) ) + "\n";
    all_zeros &= (0 == norm_ingr_ena_);
    r += indent_string + "  " + std::string("norm_egr_ena") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(norm_egr_ena_) ) + "\n";
    all_zeros &= (0 == norm_egr_ena_);
    r += indent_string + "  " + std::string("norm_max_entries") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(norm_max_entries_) ) + "\n";
    all_zeros &= (0 == norm_max_entries_);
    r += indent_string + "  " + std::string("norm_trunc_size") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(norm_trunc_size_) ) + "\n";
    all_zeros &= (0 == norm_trunc_size_);
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
    r += indent_string + std::string("MirBufDescSessionCtrl") + ":\n";
    r += indent_string + "  " + std::string("norm_ingr_ena") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(norm_ingr_ena_) ) + "\n";
    all_zeros &= (0 == norm_ingr_ena_);
    r += indent_string + "  " + std::string("norm_egr_ena") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(norm_egr_ena_) ) + "\n";
    all_zeros &= (0 == norm_egr_ena_);
    r += indent_string + "  " + std::string("norm_max_entries") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(norm_max_entries_) ) + "\n";
    all_zeros &= (0 == norm_max_entries_);
    r += indent_string + "  " + std::string("norm_trunc_size") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(norm_trunc_size_) ) + "\n";
    all_zeros &= (0 == norm_trunc_size_);
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  uint8_t norm_ingr_ena_;
  uint8_t norm_egr_ena_;
  uint8_t norm_max_entries_;
  uint16_t norm_trunc_size_;
private:
  int StartOffset(
      int index_pipe_addrmap, int index_mir_buf_desc_norm_desc_grp
      ) {
    int offset=0;
    offset += 0x2000000; // to get to pipes
    assert(index_pipe_addrmap < 4);
    offset += index_pipe_addrmap * 0x800000; // pipe_addrmap[]
    offset += 0x780000; // to get to deparser
    offset += 0x20000; // to get to mirror
    assert(index_mir_buf_desc_norm_desc_grp < 1024);
    offset += index_mir_buf_desc_norm_desc_grp * 0x20; // mir_buf_desc_norm_desc_grp[]
    offset += 0x14; // to get to norm_desc_grp_session_ctrl
    return offset;
  }

};










  }; // namespace register_classes
}; // namespace jbay

#endif // __REGISTER_INCLUDES_MIR_BUF_DESC_SESSION_CTRL_H__
