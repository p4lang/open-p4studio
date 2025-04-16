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


#ifndef __REGISTER_INCLUDES_MIR_BUF_REGS_COAL_CTRL0_H__
#define __REGISTER_INCLUDES_MIR_BUF_REGS_COAL_CTRL0_H__


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

class MirBufRegsCoalCtrl0 : public model_core::RegisterBlock<RegisterCallback> {
public:
  MirBufRegsCoalCtrl0(
      int chipNumber, int index_pipe_addrmap, int index_mir_buf_regs_coal_desc_grp, RegisterCallback& write_callback = 0, RegisterCallback& read_callback = 0
  )
    : RegisterBlock(chipNumber, StartOffset(index_pipe_addrmap, index_mir_buf_regs_coal_desc_grp), 4, false, write_callback, read_callback, std::string("MirBufRegsCoalCtrl0")+":"+boost::lexical_cast<std::string>(index_pipe_addrmap) + "," + boost::lexical_cast<std::string>(index_mir_buf_regs_coal_desc_grp))
    {
    }
  MirBufRegsCoalCtrl0(
      
  )
    : RegisterBlock(0, 0, 0, false, 0, 0, "MirBufRegsCoalCtrl0")
    {
    }
public:





  uint8_t &coal_ena() { return coal_ena_; }






  uint8_t &coal_pkthdr_length() { return coal_pkthdr_length_; }





  uint8_t &coal_timeout() { return coal_timeout_; }






  uint16_t &coal_minpkt_size() { return coal_minpkt_size_; }





  uint8_t &coal_vid() { return coal_vid_; }

  bool read(
      uint32_t offset, uint32_t* data
      ) const {
    if (read_callback_) read_callback_();
    *data = (coal_ena_ & 0x1);
    *data |= ((coal_pkthdr_length_ & 0x1f) << 2);
    *data |= (coal_timeout_ << 8);
    *data |= ((coal_minpkt_size_ & 0x3fff) << 16);
    *data |= ((coal_vid_ & 0x3) << 30);
    return true;
  }


  bool write(
      uint32_t offset, uint32_t data
      ) {
    coal_ena_ = (data & 0x1);
    coal_pkthdr_length_ = ((data >> 2) & 0x1f);
    coal_timeout_ = (data >> 8);
    coal_minpkt_size_ = ((data >> 16) & 0x3fff);
    coal_vid_ = ((data >> 30) & 0x3);
    if (write_callback_) write_callback_();
    return true;
  }

  void reset(
      
      ) {
    coal_ena_ = 0x0;
    coal_pkthdr_length_ = 0x0;
    coal_timeout_ = 0x0;
    coal_minpkt_size_ = 0x0;
    coal_vid_ = 0x0;
    if (write_callback_) write_callback_();
  }

  std::string to_string(
      uint32_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    r += indent_string + std::string("MirBufRegsCoalCtrl0") + ":\n";
    r += indent_string + "  " + std::string("coal_ena") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(coal_ena_) ) + "\n";
    all_zeros &= (0 == coal_ena_);
    r += indent_string + "  " + std::string("coal_pkthdr_length") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(coal_pkthdr_length_) ) + "\n";
    all_zeros &= (0 == coal_pkthdr_length_);
    r += indent_string + "  " + std::string("coal_timeout") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(coal_timeout_) ) + "\n";
    all_zeros &= (0 == coal_timeout_);
    r += indent_string + "  " + std::string("coal_minpkt_size") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(coal_minpkt_size_) ) + "\n";
    all_zeros &= (0 == coal_minpkt_size_);
    r += indent_string + "  " + std::string("coal_vid") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(coal_vid_) ) + "\n";
    all_zeros &= (0 == coal_vid_);
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
    r += indent_string + std::string("MirBufRegsCoalCtrl0") + ":\n";
    r += indent_string + "  " + std::string("coal_ena") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(coal_ena_) ) + "\n";
    all_zeros &= (0 == coal_ena_);
    r += indent_string + "  " + std::string("coal_pkthdr_length") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(coal_pkthdr_length_) ) + "\n";
    all_zeros &= (0 == coal_pkthdr_length_);
    r += indent_string + "  " + std::string("coal_timeout") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(coal_timeout_) ) + "\n";
    all_zeros &= (0 == coal_timeout_);
    r += indent_string + "  " + std::string("coal_minpkt_size") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(coal_minpkt_size_) ) + "\n";
    all_zeros &= (0 == coal_minpkt_size_);
    r += indent_string + "  " + std::string("coal_vid") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(coal_vid_) ) + "\n";
    all_zeros &= (0 == coal_vid_);
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  uint8_t coal_ena_;
  uint8_t coal_pkthdr_length_;
  uint8_t coal_timeout_;
  uint16_t coal_minpkt_size_;
  uint8_t coal_vid_;
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
    offset += 0x200; // to get to coal_desc_grp_coal_ctrl0
    return offset;
  }

};









  }; // namespace register_classes
}; // namespace jbay

#endif // __REGISTER_INCLUDES_MIR_BUF_REGS_COAL_CTRL0_H__
