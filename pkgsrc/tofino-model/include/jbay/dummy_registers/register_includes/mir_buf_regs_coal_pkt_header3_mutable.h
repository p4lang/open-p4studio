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


#ifndef __REGISTER_INCLUDES_MIR_BUF_REGS_COAL_PKT_HEADER3_MUTABLE_H__
#define __REGISTER_INCLUDES_MIR_BUF_REGS_COAL_PKT_HEADER3_MUTABLE_H__


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

class MirBufRegsCoalPktHeader3Mutable : public model_core::RegisterBlock<RegisterCallback> {
public:
  MirBufRegsCoalPktHeader3Mutable(
      int chipNumber, int index_pipe_addrmap, int index_mir_buf_regs_coal_desc_grp, RegisterCallback& write_callback = 0, RegisterCallback& read_callback = 0
  )
    : RegisterBlock(chipNumber, StartOffset(index_pipe_addrmap, index_mir_buf_regs_coal_desc_grp), 4, true, write_callback, read_callback, std::string("MirBufRegsCoalPktHeader3Mutable")+":"+boost::lexical_cast<std::string>(index_pipe_addrmap) + "," + boost::lexical_cast<std::string>(index_mir_buf_regs_coal_desc_grp))
    {
    }
  MirBufRegsCoalPktHeader3Mutable(
      
  )
    : RegisterBlock(0, 0, 0, true, 0, 0, "MirBufRegsCoalPktHeader3Mutable")
    {
    }
public:





  uint32_t coal_pkt_hdr3() { return coal_pkt_hdr3_; }
  void coal_pkt_hdr3(const uint32_t &v) { coal_pkt_hdr3_=v; }

  bool read(
      uint32_t offset, uint32_t* data
      ) const {
    if (read_callback_) read_callback_();
    *data = coal_pkt_hdr3_;
    return true;
  }


  bool write(
      uint32_t offset, uint32_t data
      ) {
    coal_pkt_hdr3_ = data;
    if (write_callback_) write_callback_();
    return true;
  }

  void reset(
      
      ) {
    coal_pkt_hdr3_ = 0x0;
    if (write_callback_) write_callback_();
  }

  std::string to_string(
      uint32_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    r += indent_string + std::string("MirBufRegsCoalPktHeader3Mutable") + ":\n";
    r += indent_string + "  " + std::string("coal_pkt_hdr3") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(coal_pkt_hdr3_) ) + "\n";
    all_zeros &= (0 == coal_pkt_hdr3_);
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
    r += indent_string + std::string("MirBufRegsCoalPktHeader3Mutable") + ":\n";
    r += indent_string + "  " + std::string("coal_pkt_hdr3") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(coal_pkt_hdr3_) ) + "\n";
    all_zeros &= (0 == coal_pkt_hdr3_);
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  uint32_t coal_pkt_hdr3_;
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
    offset += 0x214; // to get to coal_desc_grp_coal_pkt_header3
    return offset;
  }

};










  }; // namespace register_classes
}; // namespace jbay

#endif // __REGISTER_INCLUDES_MIR_BUF_REGS_COAL_PKT_HEADER3_MUTABLE_H__
