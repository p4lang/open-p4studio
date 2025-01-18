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



#ifndef ___TOFINO_REGISTER_INCLUDES_MODEL_MEM_H__
#define ___TOFINO_REGISTER_INCLUDES_MODEL_MEM_H__
#include <cstdint>
#include <cassert>
#include <array>
#include <vector>
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <model_core/register_block.h>
#include <shared/bitvector.h>
namespace tofino {
  namespace memory_classes {







class PrsrPoCsumCtrlRow : public model_core::RegisterBlockIndirect<RegisterCallback> {
public:
  enum PipeAddrmapEnum {
    kEPrsr,
    kIPrsr
  };
  enum PrsrMemMainRspecEnum {
    kPoCsumCtrl_0Row,
    kPoCsumCtrl_1Row
  };
public:
  PrsrPoCsumCtrlRow(
      int chipNumber, int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, PrsrMemMainRspecEnum selector_prsr_mem_main_rspec, int index_prsr_po_csum_ctrl_row, RegisterCallback& write_callback = 0, RegisterCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, selector_prsr_mem_main_rspec, index_prsr_po_csum_ctrl_row), 1, false, write_callback, read_callback, std::string("PrsrPoCsumCtrlRow")+":"+boost::lexical_cast<std::string>(index_pipe_addrmap) + "," + boost::lexical_cast<std::string>(selector_pipe_addrmap) + "," + boost::lexical_cast<std::string>(index_prsr_mem_main_rspec) + "," + boost::lexical_cast<std::string>(selector_prsr_mem_main_rspec) + "," + boost::lexical_cast<std::string>(index_prsr_po_csum_ctrl_row))
    {
    }
  PrsrPoCsumCtrlRow(
      
  )
    : RegisterBlockIndirect(0, 0, 0, false, 0, 0, "PrsrPoCsumCtrlRow")
    {
    }
public:





  uint16_t &add() { return add_; }







  uint8_t &swap(int j0) { return swap_[j0]; }






  uint8_t &shr() { return shr_; }







  uint8_t &mask(int j0) { return mask_[j0]; }






  uint8_t &dst_bit_hdr_end_pos() { return dst_bit_hdr_end_pos_; }






  uint16_t &dst() { return dst_; }







  uint8_t &hdr_end() { return hdr_end_; }






  uint8_t &type() { return type_; }





  uint8_t &start() { return start_; }







  uint8_t &zeros_as_ones() { return zeros_as_ones_; }






  uint8_t &zeros_as_ones_pos() { return zeros_as_ones_pos_; }

  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    if (read_callback_) read_callback_();
    *data0 = static_cast<uint64_t>(add_);
    *data0 |= ((static_cast<uint64_t>(swap_[0]) & 0x1) << 16);
    *data0 |= ((static_cast<uint64_t>(swap_[1]) & 0x1) << 17);
    *data0 |= ((static_cast<uint64_t>(swap_[2]) & 0x1) << 18);
    *data0 |= ((static_cast<uint64_t>(swap_[3]) & 0x1) << 19);
    *data0 |= ((static_cast<uint64_t>(swap_[4]) & 0x1) << 20);
    *data0 |= ((static_cast<uint64_t>(swap_[5]) & 0x1) << 21);
    *data0 |= ((static_cast<uint64_t>(swap_[6]) & 0x1) << 22);
    *data0 |= ((static_cast<uint64_t>(swap_[7]) & 0x1) << 23);
    *data0 |= ((static_cast<uint64_t>(swap_[8]) & 0x1) << 24);
    *data0 |= ((static_cast<uint64_t>(swap_[9]) & 0x1) << 25);
    *data0 |= ((static_cast<uint64_t>(swap_[10]) & 0x1) << 26);
    *data0 |= ((static_cast<uint64_t>(swap_[11]) & 0x1) << 27);
    *data0 |= ((static_cast<uint64_t>(swap_[12]) & 0x1) << 28);
    *data0 |= ((static_cast<uint64_t>(swap_[13]) & 0x1) << 29);
    *data0 |= ((static_cast<uint64_t>(swap_[14]) & 0x1) << 30);
    *data0 |= ((static_cast<uint64_t>(swap_[15]) & 0x1) << 31);
    *data0 |= ((static_cast<uint64_t>(swap_[16]) & 0x1) << 32);
    *data0 |= ((static_cast<uint64_t>(shr_) & 0x1) << 33);
    *data0 |= ((static_cast<uint64_t>(mask_[0]) & 0x1) << 34);
    *data0 |= ((static_cast<uint64_t>(mask_[1]) & 0x1) << 35);
    *data0 |= ((static_cast<uint64_t>(mask_[2]) & 0x1) << 36);
    *data0 |= ((static_cast<uint64_t>(mask_[3]) & 0x1) << 37);
    *data0 |= ((static_cast<uint64_t>(mask_[4]) & 0x1) << 38);
    *data0 |= ((static_cast<uint64_t>(mask_[5]) & 0x1) << 39);
    *data0 |= ((static_cast<uint64_t>(mask_[6]) & 0x1) << 40);
    *data0 |= ((static_cast<uint64_t>(mask_[7]) & 0x1) << 41);
    *data0 |= ((static_cast<uint64_t>(mask_[8]) & 0x1) << 42);
    *data0 |= ((static_cast<uint64_t>(mask_[9]) & 0x1) << 43);
    *data0 |= ((static_cast<uint64_t>(mask_[10]) & 0x1) << 44);
    *data0 |= ((static_cast<uint64_t>(mask_[11]) & 0x1) << 45);
    *data0 |= ((static_cast<uint64_t>(mask_[12]) & 0x1) << 46);
    *data0 |= ((static_cast<uint64_t>(mask_[13]) & 0x1) << 47);
    *data0 |= ((static_cast<uint64_t>(mask_[14]) & 0x1) << 48);
    *data0 |= ((static_cast<uint64_t>(mask_[15]) & 0x1) << 49);
    *data0 |= ((static_cast<uint64_t>(mask_[16]) & 0x1) << 50);
    *data0 |= ((static_cast<uint64_t>(mask_[17]) & 0x1) << 51);
    *data0 |= ((static_cast<uint64_t>(mask_[18]) & 0x1) << 52);
    *data0 |= ((static_cast<uint64_t>(mask_[19]) & 0x1) << 53);
    *data0 |= ((static_cast<uint64_t>(mask_[20]) & 0x1) << 54);
    *data0 |= ((static_cast<uint64_t>(mask_[21]) & 0x1) << 55);
    *data0 |= ((static_cast<uint64_t>(mask_[22]) & 0x1) << 56);
    *data0 |= ((static_cast<uint64_t>(mask_[23]) & 0x1) << 57);
    *data0 |= ((static_cast<uint64_t>(mask_[24]) & 0x1) << 58);
    *data0 |= ((static_cast<uint64_t>(mask_[25]) & 0x1) << 59);
    *data0 |= ((static_cast<uint64_t>(mask_[26]) & 0x1) << 60);
    *data0 |= ((static_cast<uint64_t>(mask_[27]) & 0x1) << 61);
    *data0 |= ((static_cast<uint64_t>(mask_[28]) & 0x1) << 62);
    *data0 |= ((static_cast<uint64_t>(mask_[29]) & 0x1) << 63);
    *data1 = (static_cast<uint64_t>(mask_[30]) & 0x1);
    *data1 |= ((static_cast<uint64_t>(mask_[31]) & 0x1) << 1);
    *data1 |= ((static_cast<uint64_t>(dst_bit_hdr_end_pos_) & 0x1f) << 2);
    *data1 |= ((static_cast<uint64_t>(dst_) & 0x1ff) << 7);
    *data1 |= ((static_cast<uint64_t>(hdr_end_) & 0x1) << 16);
    *data1 |= ((static_cast<uint64_t>(type_) & 0x1) << 17);
    *data1 |= ((static_cast<uint64_t>(start_) & 0x1) << 18);
    *data1 |= ((static_cast<uint64_t>(zeros_as_ones_) & 0x1) << 19);
    *data1 |= ((static_cast<uint64_t>(zeros_as_ones_pos_) & 0x1f) << 20);
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    add_ = data0;
    swap_[0] = ((data0 >> 16) & 0x1);
    swap_[1] = ((data0 >> 17) & 0x1);
    swap_[2] = ((data0 >> 18) & 0x1);
    swap_[3] = ((data0 >> 19) & 0x1);
    swap_[4] = ((data0 >> 20) & 0x1);
    swap_[5] = ((data0 >> 21) & 0x1);
    swap_[6] = ((data0 >> 22) & 0x1);
    swap_[7] = ((data0 >> 23) & 0x1);
    swap_[8] = ((data0 >> 24) & 0x1);
    swap_[9] = ((data0 >> 25) & 0x1);
    swap_[10] = ((data0 >> 26) & 0x1);
    swap_[11] = ((data0 >> 27) & 0x1);
    swap_[12] = ((data0 >> 28) & 0x1);
    swap_[13] = ((data0 >> 29) & 0x1);
    swap_[14] = ((data0 >> 30) & 0x1);
    swap_[15] = ((data0 >> 31) & 0x1);
    swap_[16] = ((data0 >> 32) & 0x1);
    shr_ = ((data0 >> 33) & 0x1);
    mask_[0] = ((data0 >> 34) & 0x1);
    mask_[1] = ((data0 >> 35) & 0x1);
    mask_[2] = ((data0 >> 36) & 0x1);
    mask_[3] = ((data0 >> 37) & 0x1);
    mask_[4] = ((data0 >> 38) & 0x1);
    mask_[5] = ((data0 >> 39) & 0x1);
    mask_[6] = ((data0 >> 40) & 0x1);
    mask_[7] = ((data0 >> 41) & 0x1);
    mask_[8] = ((data0 >> 42) & 0x1);
    mask_[9] = ((data0 >> 43) & 0x1);
    mask_[10] = ((data0 >> 44) & 0x1);
    mask_[11] = ((data0 >> 45) & 0x1);
    mask_[12] = ((data0 >> 46) & 0x1);
    mask_[13] = ((data0 >> 47) & 0x1);
    mask_[14] = ((data0 >> 48) & 0x1);
    mask_[15] = ((data0 >> 49) & 0x1);
    mask_[16] = ((data0 >> 50) & 0x1);
    mask_[17] = ((data0 >> 51) & 0x1);
    mask_[18] = ((data0 >> 52) & 0x1);
    mask_[19] = ((data0 >> 53) & 0x1);
    mask_[20] = ((data0 >> 54) & 0x1);
    mask_[21] = ((data0 >> 55) & 0x1);
    mask_[22] = ((data0 >> 56) & 0x1);
    mask_[23] = ((data0 >> 57) & 0x1);
    mask_[24] = ((data0 >> 58) & 0x1);
    mask_[25] = ((data0 >> 59) & 0x1);
    mask_[26] = ((data0 >> 60) & 0x1);
    mask_[27] = ((data0 >> 61) & 0x1);
    mask_[28] = ((data0 >> 62) & 0x1);
    mask_[29] = ((data0 >> 63) & 0x1);
    mask_[30] = (data1 & 0x1);
    mask_[31] = ((data1 >> 1) & 0x1);
    dst_bit_hdr_end_pos_ = ((data1 >> 2) & 0x1f);
    dst_ = ((data1 >> 7) & 0x1ff);
    hdr_end_ = ((data1 >> 16) & 0x1);
    type_ = ((data1 >> 17) & 0x1);
    start_ = ((data1 >> 18) & 0x1);
    zeros_as_ones_ = ((data1 >> 19) & 0x1);
    zeros_as_ones_pos_ = ((data1 >> 20) & 0x1f);
    if (write_callback_) write_callback_();
    return true;
  }

  void reset(
      
      ) {
    add_ = 0;
    swap_[0] = 0;
    swap_[1] = 0;
    swap_[2] = 0;
    swap_[3] = 0;
    swap_[4] = 0;
    swap_[5] = 0;
    swap_[6] = 0;
    swap_[7] = 0;
    swap_[8] = 0;
    swap_[9] = 0;
    swap_[10] = 0;
    swap_[11] = 0;
    swap_[12] = 0;
    swap_[13] = 0;
    swap_[14] = 0;
    swap_[15] = 0;
    swap_[16] = 0;
    shr_ = 0;
    mask_[0] = 0;
    mask_[1] = 0;
    mask_[2] = 0;
    mask_[3] = 0;
    mask_[4] = 0;
    mask_[5] = 0;
    mask_[6] = 0;
    mask_[7] = 0;
    mask_[8] = 0;
    mask_[9] = 0;
    mask_[10] = 0;
    mask_[11] = 0;
    mask_[12] = 0;
    mask_[13] = 0;
    mask_[14] = 0;
    mask_[15] = 0;
    mask_[16] = 0;
    mask_[17] = 0;
    mask_[18] = 0;
    mask_[19] = 0;
    mask_[20] = 0;
    mask_[21] = 0;
    mask_[22] = 0;
    mask_[23] = 0;
    mask_[24] = 0;
    mask_[25] = 0;
    mask_[26] = 0;
    mask_[27] = 0;
    mask_[28] = 0;
    mask_[29] = 0;
    mask_[30] = 0;
    mask_[31] = 0;
    dst_bit_hdr_end_pos_ = 0;
    dst_ = 0;
    hdr_end_ = 0;
    type_ = 0;
    start_ = 0;
    zeros_as_ones_ = 0;
    zeros_as_ones_pos_ = 0;
    if (write_callback_) write_callback_();
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    r += indent_string + std::string("PrsrPoCsumCtrlRow") + ":\n";
    r += indent_string + "  " + std::string("add") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(add_) ) + "\n";
    all_zeros &= (0 == add_);
    for (uint32_t f=0;f<17;++f) {
      r += indent_string + "  " + std::string("swap") + "["+boost::lexical_cast<std::string>(f)+"]"+ ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(swap_[f]) ) + "\n";
      all_zeros &= (0 == swap_[f]);
    }
    r += indent_string + "  " + std::string("shr") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(shr_) ) + "\n";
    all_zeros &= (0 == shr_);
    for (uint32_t f=0;f<32;++f) {
      r += indent_string + "  " + std::string("mask") + "["+boost::lexical_cast<std::string>(f)+"]"+ ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(mask_[f]) ) + "\n";
      all_zeros &= (0 == mask_[f]);
    }
    r += indent_string + "  " + std::string("dst_bit_hdr_end_pos") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(dst_bit_hdr_end_pos_) ) + "\n";
    all_zeros &= (0 == dst_bit_hdr_end_pos_);
    r += indent_string + "  " + std::string("dst") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(dst_) ) + "\n";
    all_zeros &= (0 == dst_);
    r += indent_string + "  " + std::string("hdr_end") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(hdr_end_) ) + "\n";
    all_zeros &= (0 == hdr_end_);
    r += indent_string + "  " + std::string("type") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(type_) ) + "\n";
    all_zeros &= (0 == type_);
    r += indent_string + "  " + std::string("start") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(start_) ) + "\n";
    all_zeros &= (0 == start_);
    r += indent_string + "  " + std::string("zeros_as_ones") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(zeros_as_ones_) ) + "\n";
    all_zeros &= (0 == zeros_as_ones_);
    r += indent_string + "  " + std::string("zeros_as_ones_pos") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(zeros_as_ones_pos_) ) + "\n";
    all_zeros &= (0 == zeros_as_ones_pos_);
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
    r += indent_string + std::string("PrsrPoCsumCtrlRow") + ":\n";
    r += indent_string + "  " + std::string("add") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(add_) ) + "\n";
    all_zeros &= (0 == add_);
    for (uint32_t f=0;f<17;++f) {
      r += indent_string + "  " + std::string("swap") + "["+boost::lexical_cast<std::string>(f)+"]"+ ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(swap_[f]) ) + "\n";
      all_zeros &= (0 == swap_[f]);
    }
    r += indent_string + "  " + std::string("shr") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(shr_) ) + "\n";
    all_zeros &= (0 == shr_);
    for (uint32_t f=0;f<32;++f) {
      r += indent_string + "  " + std::string("mask") + "["+boost::lexical_cast<std::string>(f)+"]"+ ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(mask_[f]) ) + "\n";
      all_zeros &= (0 == mask_[f]);
    }
    r += indent_string + "  " + std::string("dst_bit_hdr_end_pos") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(dst_bit_hdr_end_pos_) ) + "\n";
    all_zeros &= (0 == dst_bit_hdr_end_pos_);
    r += indent_string + "  " + std::string("dst") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(dst_) ) + "\n";
    all_zeros &= (0 == dst_);
    r += indent_string + "  " + std::string("hdr_end") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(hdr_end_) ) + "\n";
    all_zeros &= (0 == hdr_end_);
    r += indent_string + "  " + std::string("type") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(type_) ) + "\n";
    all_zeros &= (0 == type_);
    r += indent_string + "  " + std::string("start") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(start_) ) + "\n";
    all_zeros &= (0 == start_);
    r += indent_string + "  " + std::string("zeros_as_ones") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(zeros_as_ones_) ) + "\n";
    all_zeros &= (0 == zeros_as_ones_);
    r += indent_string + "  " + std::string("zeros_as_ones_pos") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(zeros_as_ones_pos_) ) + "\n";
    all_zeros &= (0 == zeros_as_ones_pos_);
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  uint16_t add_;
  std::array< uint8_t, 17 > swap_;
  uint8_t shr_;
  std::array< uint8_t, 32 > mask_;
  uint8_t dst_bit_hdr_end_pos_;
  uint16_t dst_;
  uint8_t hdr_end_;
  uint8_t type_;
  uint8_t start_;
  uint8_t zeros_as_ones_;
  uint8_t zeros_as_ones_pos_;
private:
  static uint64_t StartOffset(
      int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, PrsrMemMainRspecEnum selector_prsr_mem_main_rspec, int index_prsr_po_csum_ctrl_row
      ) {
    uint64_t offset=0;
    offset += 0x20000000000; // to get to pipes
    assert(index_pipe_addrmap < 4);
    offset += index_pipe_addrmap * 0x2000000000; // pipe_addrmap[]
    switch (selector_pipe_addrmap) {
      case kEPrsr:
        offset += 0x1c80040000; // to get to e_prsr
        assert(index_prsr_mem_main_rspec < 18);
        offset += index_prsr_mem_main_rspec * 0x2000; // prsr_mem_main_rspec[]
        switch (selector_prsr_mem_main_rspec) {
          case kPoCsumCtrl_0Row:
            offset += 0x620; // to get to po_csum_ctrl_0_row
            assert(index_prsr_po_csum_ctrl_row < 32);
            offset += index_prsr_po_csum_ctrl_row * 0x1; // prsr_po_csum_ctrl_row[]
            break;
          case kPoCsumCtrl_1Row:
            offset += 0x640; // to get to po_csum_ctrl_1_row
            assert(index_prsr_po_csum_ctrl_row < 32);
            offset += index_prsr_po_csum_ctrl_row * 0x1; // prsr_po_csum_ctrl_row[]
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kIPrsr:
        offset += 0x1c80000000; // to get to i_prsr
        assert(index_prsr_mem_main_rspec < 18);
        offset += index_prsr_mem_main_rspec * 0x2000; // prsr_mem_main_rspec[]
        switch (selector_prsr_mem_main_rspec) {
          case kPoCsumCtrl_0Row:
            offset += 0x620; // to get to po_csum_ctrl_0_row
            assert(index_prsr_po_csum_ctrl_row < 32);
            offset += index_prsr_po_csum_ctrl_row * 0x1; // prsr_po_csum_ctrl_row[]
            break;
          case kPoCsumCtrl_1Row:
            offset += 0x640; // to get to po_csum_ctrl_1_row
            assert(index_prsr_po_csum_ctrl_row < 32);
            offset += index_prsr_po_csum_ctrl_row * 0x1; // prsr_po_csum_ctrl_row[]
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
    return offset;
  }

};







class PrsrPoCsumCtrlRowMutable : public model_core::RegisterBlockIndirect<RegisterCallback> {
public:
  enum PipeAddrmapEnum {
    kEPrsr,
    kIPrsr
  };
  enum PrsrMemMainRspecEnum {
    kPoCsumCtrl_0Row,
    kPoCsumCtrl_1Row
  };
public:
  PrsrPoCsumCtrlRowMutable(
      int chipNumber, int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, PrsrMemMainRspecEnum selector_prsr_mem_main_rspec, int index_prsr_po_csum_ctrl_row, RegisterCallback& write_callback = 0, RegisterCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, selector_prsr_mem_main_rspec, index_prsr_po_csum_ctrl_row), 1, true, write_callback, read_callback, std::string("PrsrPoCsumCtrlRowMutable")+":"+boost::lexical_cast<std::string>(index_pipe_addrmap) + "," + boost::lexical_cast<std::string>(selector_pipe_addrmap) + "," + boost::lexical_cast<std::string>(index_prsr_mem_main_rspec) + "," + boost::lexical_cast<std::string>(selector_prsr_mem_main_rspec) + "," + boost::lexical_cast<std::string>(index_prsr_po_csum_ctrl_row))
    {
    }
  PrsrPoCsumCtrlRowMutable(
      
  )
    : RegisterBlockIndirect(0, 0, 0, true, 0, 0, "PrsrPoCsumCtrlRowMutable")
    {
    }
public:





  uint16_t add() { return add_; }
  void add(const uint16_t &v) { add_=v; }







  uint8_t swap(int j0) { return swap_[j0]; }
  void swap(int j0,const uint8_t &v) { swap_[j0]=v; }






  uint8_t shr() { return shr_; }
  void shr(const uint8_t &v) { shr_=v; }







  uint8_t mask(int j0) { return mask_[j0]; }
  void mask(int j0,const uint8_t &v) { mask_[j0]=v; }






  uint8_t dst_bit_hdr_end_pos() { return dst_bit_hdr_end_pos_; }
  void dst_bit_hdr_end_pos(const uint8_t &v) { dst_bit_hdr_end_pos_=v; }






  uint16_t dst() { return dst_; }
  void dst(const uint16_t &v) { dst_=v; }







  uint8_t hdr_end() { return hdr_end_; }
  void hdr_end(const uint8_t &v) { hdr_end_=v; }






  uint8_t type() { return type_; }
  void type(const uint8_t &v) { type_=v; }





  uint8_t start() { return start_; }
  void start(const uint8_t &v) { start_=v; }







  uint8_t zeros_as_ones() { return zeros_as_ones_; }
  void zeros_as_ones(const uint8_t &v) { zeros_as_ones_=v; }






  uint8_t zeros_as_ones_pos() { return zeros_as_ones_pos_; }
  void zeros_as_ones_pos(const uint8_t &v) { zeros_as_ones_pos_=v; }

  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    if (read_callback_) read_callback_();
    *data0 = static_cast<uint64_t>(add_);
    *data0 |= ((static_cast<uint64_t>(swap_[0]) & 0x1) << 16);
    *data0 |= ((static_cast<uint64_t>(swap_[1]) & 0x1) << 17);
    *data0 |= ((static_cast<uint64_t>(swap_[2]) & 0x1) << 18);
    *data0 |= ((static_cast<uint64_t>(swap_[3]) & 0x1) << 19);
    *data0 |= ((static_cast<uint64_t>(swap_[4]) & 0x1) << 20);
    *data0 |= ((static_cast<uint64_t>(swap_[5]) & 0x1) << 21);
    *data0 |= ((static_cast<uint64_t>(swap_[6]) & 0x1) << 22);
    *data0 |= ((static_cast<uint64_t>(swap_[7]) & 0x1) << 23);
    *data0 |= ((static_cast<uint64_t>(swap_[8]) & 0x1) << 24);
    *data0 |= ((static_cast<uint64_t>(swap_[9]) & 0x1) << 25);
    *data0 |= ((static_cast<uint64_t>(swap_[10]) & 0x1) << 26);
    *data0 |= ((static_cast<uint64_t>(swap_[11]) & 0x1) << 27);
    *data0 |= ((static_cast<uint64_t>(swap_[12]) & 0x1) << 28);
    *data0 |= ((static_cast<uint64_t>(swap_[13]) & 0x1) << 29);
    *data0 |= ((static_cast<uint64_t>(swap_[14]) & 0x1) << 30);
    *data0 |= ((static_cast<uint64_t>(swap_[15]) & 0x1) << 31);
    *data0 |= ((static_cast<uint64_t>(swap_[16]) & 0x1) << 32);
    *data0 |= ((static_cast<uint64_t>(shr_) & 0x1) << 33);
    *data0 |= ((static_cast<uint64_t>(mask_[0]) & 0x1) << 34);
    *data0 |= ((static_cast<uint64_t>(mask_[1]) & 0x1) << 35);
    *data0 |= ((static_cast<uint64_t>(mask_[2]) & 0x1) << 36);
    *data0 |= ((static_cast<uint64_t>(mask_[3]) & 0x1) << 37);
    *data0 |= ((static_cast<uint64_t>(mask_[4]) & 0x1) << 38);
    *data0 |= ((static_cast<uint64_t>(mask_[5]) & 0x1) << 39);
    *data0 |= ((static_cast<uint64_t>(mask_[6]) & 0x1) << 40);
    *data0 |= ((static_cast<uint64_t>(mask_[7]) & 0x1) << 41);
    *data0 |= ((static_cast<uint64_t>(mask_[8]) & 0x1) << 42);
    *data0 |= ((static_cast<uint64_t>(mask_[9]) & 0x1) << 43);
    *data0 |= ((static_cast<uint64_t>(mask_[10]) & 0x1) << 44);
    *data0 |= ((static_cast<uint64_t>(mask_[11]) & 0x1) << 45);
    *data0 |= ((static_cast<uint64_t>(mask_[12]) & 0x1) << 46);
    *data0 |= ((static_cast<uint64_t>(mask_[13]) & 0x1) << 47);
    *data0 |= ((static_cast<uint64_t>(mask_[14]) & 0x1) << 48);
    *data0 |= ((static_cast<uint64_t>(mask_[15]) & 0x1) << 49);
    *data0 |= ((static_cast<uint64_t>(mask_[16]) & 0x1) << 50);
    *data0 |= ((static_cast<uint64_t>(mask_[17]) & 0x1) << 51);
    *data0 |= ((static_cast<uint64_t>(mask_[18]) & 0x1) << 52);
    *data0 |= ((static_cast<uint64_t>(mask_[19]) & 0x1) << 53);
    *data0 |= ((static_cast<uint64_t>(mask_[20]) & 0x1) << 54);
    *data0 |= ((static_cast<uint64_t>(mask_[21]) & 0x1) << 55);
    *data0 |= ((static_cast<uint64_t>(mask_[22]) & 0x1) << 56);
    *data0 |= ((static_cast<uint64_t>(mask_[23]) & 0x1) << 57);
    *data0 |= ((static_cast<uint64_t>(mask_[24]) & 0x1) << 58);
    *data0 |= ((static_cast<uint64_t>(mask_[25]) & 0x1) << 59);
    *data0 |= ((static_cast<uint64_t>(mask_[26]) & 0x1) << 60);
    *data0 |= ((static_cast<uint64_t>(mask_[27]) & 0x1) << 61);
    *data0 |= ((static_cast<uint64_t>(mask_[28]) & 0x1) << 62);
    *data0 |= ((static_cast<uint64_t>(mask_[29]) & 0x1) << 63);
    *data1 = (static_cast<uint64_t>(mask_[30]) & 0x1);
    *data1 |= ((static_cast<uint64_t>(mask_[31]) & 0x1) << 1);
    *data1 |= ((static_cast<uint64_t>(dst_bit_hdr_end_pos_) & 0x1f) << 2);
    *data1 |= ((static_cast<uint64_t>(dst_) & 0x1ff) << 7);
    *data1 |= ((static_cast<uint64_t>(hdr_end_) & 0x1) << 16);
    *data1 |= ((static_cast<uint64_t>(type_) & 0x1) << 17);
    *data1 |= ((static_cast<uint64_t>(start_) & 0x1) << 18);
    *data1 |= ((static_cast<uint64_t>(zeros_as_ones_) & 0x1) << 19);
    *data1 |= ((static_cast<uint64_t>(zeros_as_ones_pos_) & 0x1f) << 20);
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    add_ = data0;
    swap_[0] = ((data0 >> 16) & 0x1);
    swap_[1] = ((data0 >> 17) & 0x1);
    swap_[2] = ((data0 >> 18) & 0x1);
    swap_[3] = ((data0 >> 19) & 0x1);
    swap_[4] = ((data0 >> 20) & 0x1);
    swap_[5] = ((data0 >> 21) & 0x1);
    swap_[6] = ((data0 >> 22) & 0x1);
    swap_[7] = ((data0 >> 23) & 0x1);
    swap_[8] = ((data0 >> 24) & 0x1);
    swap_[9] = ((data0 >> 25) & 0x1);
    swap_[10] = ((data0 >> 26) & 0x1);
    swap_[11] = ((data0 >> 27) & 0x1);
    swap_[12] = ((data0 >> 28) & 0x1);
    swap_[13] = ((data0 >> 29) & 0x1);
    swap_[14] = ((data0 >> 30) & 0x1);
    swap_[15] = ((data0 >> 31) & 0x1);
    swap_[16] = ((data0 >> 32) & 0x1);
    shr_ = ((data0 >> 33) & 0x1);
    mask_[0] = ((data0 >> 34) & 0x1);
    mask_[1] = ((data0 >> 35) & 0x1);
    mask_[2] = ((data0 >> 36) & 0x1);
    mask_[3] = ((data0 >> 37) & 0x1);
    mask_[4] = ((data0 >> 38) & 0x1);
    mask_[5] = ((data0 >> 39) & 0x1);
    mask_[6] = ((data0 >> 40) & 0x1);
    mask_[7] = ((data0 >> 41) & 0x1);
    mask_[8] = ((data0 >> 42) & 0x1);
    mask_[9] = ((data0 >> 43) & 0x1);
    mask_[10] = ((data0 >> 44) & 0x1);
    mask_[11] = ((data0 >> 45) & 0x1);
    mask_[12] = ((data0 >> 46) & 0x1);
    mask_[13] = ((data0 >> 47) & 0x1);
    mask_[14] = ((data0 >> 48) & 0x1);
    mask_[15] = ((data0 >> 49) & 0x1);
    mask_[16] = ((data0 >> 50) & 0x1);
    mask_[17] = ((data0 >> 51) & 0x1);
    mask_[18] = ((data0 >> 52) & 0x1);
    mask_[19] = ((data0 >> 53) & 0x1);
    mask_[20] = ((data0 >> 54) & 0x1);
    mask_[21] = ((data0 >> 55) & 0x1);
    mask_[22] = ((data0 >> 56) & 0x1);
    mask_[23] = ((data0 >> 57) & 0x1);
    mask_[24] = ((data0 >> 58) & 0x1);
    mask_[25] = ((data0 >> 59) & 0x1);
    mask_[26] = ((data0 >> 60) & 0x1);
    mask_[27] = ((data0 >> 61) & 0x1);
    mask_[28] = ((data0 >> 62) & 0x1);
    mask_[29] = ((data0 >> 63) & 0x1);
    mask_[30] = (data1 & 0x1);
    mask_[31] = ((data1 >> 1) & 0x1);
    dst_bit_hdr_end_pos_ = ((data1 >> 2) & 0x1f);
    dst_ = ((data1 >> 7) & 0x1ff);
    hdr_end_ = ((data1 >> 16) & 0x1);
    type_ = ((data1 >> 17) & 0x1);
    start_ = ((data1 >> 18) & 0x1);
    zeros_as_ones_ = ((data1 >> 19) & 0x1);
    zeros_as_ones_pos_ = ((data1 >> 20) & 0x1f);
    if (write_callback_) write_callback_();
    return true;
  }

  void reset(
      
      ) {
    add_ = 0;
    swap_[0] = 0;
    swap_[1] = 0;
    swap_[2] = 0;
    swap_[3] = 0;
    swap_[4] = 0;
    swap_[5] = 0;
    swap_[6] = 0;
    swap_[7] = 0;
    swap_[8] = 0;
    swap_[9] = 0;
    swap_[10] = 0;
    swap_[11] = 0;
    swap_[12] = 0;
    swap_[13] = 0;
    swap_[14] = 0;
    swap_[15] = 0;
    swap_[16] = 0;
    shr_ = 0;
    mask_[0] = 0;
    mask_[1] = 0;
    mask_[2] = 0;
    mask_[3] = 0;
    mask_[4] = 0;
    mask_[5] = 0;
    mask_[6] = 0;
    mask_[7] = 0;
    mask_[8] = 0;
    mask_[9] = 0;
    mask_[10] = 0;
    mask_[11] = 0;
    mask_[12] = 0;
    mask_[13] = 0;
    mask_[14] = 0;
    mask_[15] = 0;
    mask_[16] = 0;
    mask_[17] = 0;
    mask_[18] = 0;
    mask_[19] = 0;
    mask_[20] = 0;
    mask_[21] = 0;
    mask_[22] = 0;
    mask_[23] = 0;
    mask_[24] = 0;
    mask_[25] = 0;
    mask_[26] = 0;
    mask_[27] = 0;
    mask_[28] = 0;
    mask_[29] = 0;
    mask_[30] = 0;
    mask_[31] = 0;
    dst_bit_hdr_end_pos_ = 0;
    dst_ = 0;
    hdr_end_ = 0;
    type_ = 0;
    start_ = 0;
    zeros_as_ones_ = 0;
    zeros_as_ones_pos_ = 0;
    if (write_callback_) write_callback_();
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    r += indent_string + std::string("PrsrPoCsumCtrlRowMutable") + ":\n";
    r += indent_string + "  " + std::string("add") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(add_) ) + "\n";
    all_zeros &= (0 == add_);
    for (uint32_t f=0;f<17;++f) {
      r += indent_string + "  " + std::string("swap") + "["+boost::lexical_cast<std::string>(f)+"]"+ ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(swap_[f]) ) + "\n";
      all_zeros &= (0 == swap_[f]);
    }
    r += indent_string + "  " + std::string("shr") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(shr_) ) + "\n";
    all_zeros &= (0 == shr_);
    for (uint32_t f=0;f<32;++f) {
      r += indent_string + "  " + std::string("mask") + "["+boost::lexical_cast<std::string>(f)+"]"+ ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(mask_[f]) ) + "\n";
      all_zeros &= (0 == mask_[f]);
    }
    r += indent_string + "  " + std::string("dst_bit_hdr_end_pos") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(dst_bit_hdr_end_pos_) ) + "\n";
    all_zeros &= (0 == dst_bit_hdr_end_pos_);
    r += indent_string + "  " + std::string("dst") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(dst_) ) + "\n";
    all_zeros &= (0 == dst_);
    r += indent_string + "  " + std::string("hdr_end") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(hdr_end_) ) + "\n";
    all_zeros &= (0 == hdr_end_);
    r += indent_string + "  " + std::string("type") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(type_) ) + "\n";
    all_zeros &= (0 == type_);
    r += indent_string + "  " + std::string("start") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(start_) ) + "\n";
    all_zeros &= (0 == start_);
    r += indent_string + "  " + std::string("zeros_as_ones") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(zeros_as_ones_) ) + "\n";
    all_zeros &= (0 == zeros_as_ones_);
    r += indent_string + "  " + std::string("zeros_as_ones_pos") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(zeros_as_ones_pos_) ) + "\n";
    all_zeros &= (0 == zeros_as_ones_pos_);
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
    r += indent_string + std::string("PrsrPoCsumCtrlRowMutable") + ":\n";
    r += indent_string + "  " + std::string("add") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(add_) ) + "\n";
    all_zeros &= (0 == add_);
    for (uint32_t f=0;f<17;++f) {
      r += indent_string + "  " + std::string("swap") + "["+boost::lexical_cast<std::string>(f)+"]"+ ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(swap_[f]) ) + "\n";
      all_zeros &= (0 == swap_[f]);
    }
    r += indent_string + "  " + std::string("shr") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(shr_) ) + "\n";
    all_zeros &= (0 == shr_);
    for (uint32_t f=0;f<32;++f) {
      r += indent_string + "  " + std::string("mask") + "["+boost::lexical_cast<std::string>(f)+"]"+ ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(mask_[f]) ) + "\n";
      all_zeros &= (0 == mask_[f]);
    }
    r += indent_string + "  " + std::string("dst_bit_hdr_end_pos") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(dst_bit_hdr_end_pos_) ) + "\n";
    all_zeros &= (0 == dst_bit_hdr_end_pos_);
    r += indent_string + "  " + std::string("dst") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(dst_) ) + "\n";
    all_zeros &= (0 == dst_);
    r += indent_string + "  " + std::string("hdr_end") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(hdr_end_) ) + "\n";
    all_zeros &= (0 == hdr_end_);
    r += indent_string + "  " + std::string("type") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(type_) ) + "\n";
    all_zeros &= (0 == type_);
    r += indent_string + "  " + std::string("start") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(start_) ) + "\n";
    all_zeros &= (0 == start_);
    r += indent_string + "  " + std::string("zeros_as_ones") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(zeros_as_ones_) ) + "\n";
    all_zeros &= (0 == zeros_as_ones_);
    r += indent_string + "  " + std::string("zeros_as_ones_pos") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(zeros_as_ones_pos_) ) + "\n";
    all_zeros &= (0 == zeros_as_ones_pos_);
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  uint16_t add_;
  std::array< uint8_t, 17 > swap_;
  uint8_t shr_;
  std::array< uint8_t, 32 > mask_;
  uint8_t dst_bit_hdr_end_pos_;
  uint16_t dst_;
  uint8_t hdr_end_;
  uint8_t type_;
  uint8_t start_;
  uint8_t zeros_as_ones_;
  uint8_t zeros_as_ones_pos_;
private:
  static uint64_t StartOffset(
      int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, PrsrMemMainRspecEnum selector_prsr_mem_main_rspec, int index_prsr_po_csum_ctrl_row
      ) {
    uint64_t offset=0;
    offset += 0x20000000000; // to get to pipes
    assert(index_pipe_addrmap < 4);
    offset += index_pipe_addrmap * 0x2000000000; // pipe_addrmap[]
    switch (selector_pipe_addrmap) {
      case kEPrsr:
        offset += 0x1c80040000; // to get to e_prsr
        assert(index_prsr_mem_main_rspec < 18);
        offset += index_prsr_mem_main_rspec * 0x2000; // prsr_mem_main_rspec[]
        switch (selector_prsr_mem_main_rspec) {
          case kPoCsumCtrl_0Row:
            offset += 0x620; // to get to po_csum_ctrl_0_row
            assert(index_prsr_po_csum_ctrl_row < 32);
            offset += index_prsr_po_csum_ctrl_row * 0x1; // prsr_po_csum_ctrl_row[]
            break;
          case kPoCsumCtrl_1Row:
            offset += 0x640; // to get to po_csum_ctrl_1_row
            assert(index_prsr_po_csum_ctrl_row < 32);
            offset += index_prsr_po_csum_ctrl_row * 0x1; // prsr_po_csum_ctrl_row[]
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kIPrsr:
        offset += 0x1c80000000; // to get to i_prsr
        assert(index_prsr_mem_main_rspec < 18);
        offset += index_prsr_mem_main_rspec * 0x2000; // prsr_mem_main_rspec[]
        switch (selector_prsr_mem_main_rspec) {
          case kPoCsumCtrl_0Row:
            offset += 0x620; // to get to po_csum_ctrl_0_row
            assert(index_prsr_po_csum_ctrl_row < 32);
            offset += index_prsr_po_csum_ctrl_row * 0x1; // prsr_po_csum_ctrl_row[]
            break;
          case kPoCsumCtrl_1Row:
            offset += 0x640; // to get to po_csum_ctrl_1_row
            assert(index_prsr_po_csum_ctrl_row < 32);
            offset += index_prsr_po_csum_ctrl_row * 0x1; // prsr_po_csum_ctrl_row[]
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
    return offset;
  }

};







class PrsrPoCsumCtrlRowArray : public model_core::RegisterBlockIndirect<RegisterArrayCallback> {
public:
  enum PipeAddrmapEnum {
    kEPrsr,
    kIPrsr
  };
  enum PrsrMemMainRspecEnum {
    kPoCsumCtrl_0Row,
    kPoCsumCtrl_1Row
  };
public:
  PrsrPoCsumCtrlRowArray(
      int chipNumber, int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, PrsrMemMainRspecEnum selector_prsr_mem_main_rspec, RegisterArrayCallback& write_callback = 0, RegisterArrayCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, selector_prsr_mem_main_rspec), 1 * ArraySize(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, selector_prsr_mem_main_rspec, -1), false, write_callback, read_callback, std::string("PrsrPoCsumCtrlRowArray")+":"+boost::lexical_cast<std::string>(index_pipe_addrmap) + "," + boost::lexical_cast<std::string>(selector_pipe_addrmap) + "," + boost::lexical_cast<std::string>(index_prsr_mem_main_rspec) + "," + boost::lexical_cast<std::string>(selector_prsr_mem_main_rspec)),
    array(RealArraySize(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, selector_prsr_mem_main_rspec, -1)),
    size0_(ArraySize(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, selector_prsr_mem_main_rspec, 0)),
    real_size0_(RealArraySize(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, selector_prsr_mem_main_rspec, 0))
    {
    }
public:





  uint16_t &add(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].add();
  }







  uint8_t &swap(uint32_t a0,int j0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].swap(j0);
  }






  uint8_t &shr(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].shr();
  }







  uint8_t &mask(uint32_t a0,int j0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].mask(j0);
  }






  uint8_t &dst_bit_hdr_end_pos(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].dst_bit_hdr_end_pos();
  }






  uint16_t &dst(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].dst();
  }







  uint8_t &hdr_end(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].hdr_end();
  }






  uint8_t &type(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].type();
  }





  uint8_t &start(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].start();
  }







  uint8_t &zeros_as_ones(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].zeros_as_ones();
  }






  uint8_t &zeros_as_ones_pos(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].zeros_as_ones_pos();
  }
  bool calculate_index(
      uint64_t* offset, uint32_t* a0
      ) const {
    int i = (*offset)/1;
    (*offset) -= (i*1);
    uint32_t t = i;
    *a0 = t;
    bool in_bounds = true;
    in_bounds &= CheckArrayBounds(*a0,real_size0_,size0_,*offset,0,true);
    return in_bounds;
  }


  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    if (read_callback_) read_callback_(a0);
    array[a0].read(offset,data0,data1,T);
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    array[a0].write(offset,data0,data1,T);
    if (write_callback_) write_callback_(a0);
    return true;
  }

  void reset(
      
      ) {
    for (uint32_t i=0;i<array.size();++i) {
      array[i].reset();
      int t = i;
      int a0 = t;
      if (write_callback_) write_callback_(a0);
    }
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    all_zeros=false;
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return "OUT_OF_BOUNDS";
    std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
    if (! r_s.empty()) {
      r += indent_string + std::string("PrsrPoCsumCtrlRowArray") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
    }
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
    all_zeros=false;
    for (uint32_t a0=0;a0<real_size0_;++a0) {
      std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
      if (! r_s.empty()) {
        r += indent_string + std::string("PrsrPoCsumCtrlRowArray") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
      }
    }
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  std::vector<PrsrPoCsumCtrlRow> array;
  uint32_t size0_;
  uint32_t real_size0_;
private:
  static uint64_t StartOffset(
      int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, PrsrMemMainRspecEnum selector_prsr_mem_main_rspec
      ) {
    uint64_t offset=0;
    offset += 0x20000000000; // to get to pipes
    assert(index_pipe_addrmap < 4);
    offset += index_pipe_addrmap * 0x2000000000; // pipe_addrmap[]
    switch (selector_pipe_addrmap) {
      case kEPrsr:
        offset += 0x1c80040000; // to get to e_prsr
        assert(index_prsr_mem_main_rspec < 18);
        offset += index_prsr_mem_main_rspec * 0x2000; // prsr_mem_main_rspec[]
        switch (selector_prsr_mem_main_rspec) {
          case kPoCsumCtrl_0Row:
            offset += 0x620; // to get to po_csum_ctrl_0_row
            break;
          case kPoCsumCtrl_1Row:
            offset += 0x640; // to get to po_csum_ctrl_1_row
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kIPrsr:
        offset += 0x1c80000000; // to get to i_prsr
        assert(index_prsr_mem_main_rspec < 18);
        offset += index_prsr_mem_main_rspec * 0x2000; // prsr_mem_main_rspec[]
        switch (selector_prsr_mem_main_rspec) {
          case kPoCsumCtrl_0Row:
            offset += 0x620; // to get to po_csum_ctrl_0_row
            break;
          case kPoCsumCtrl_1Row:
            offset += 0x640; // to get to po_csum_ctrl_1_row
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
    return offset;
  }

  static int ArraySize(
      int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, PrsrMemMainRspecEnum selector_prsr_mem_main_rspec, int dimension
      ) {
    switch (selector_pipe_addrmap) {
      case kEPrsr:
        switch (selector_prsr_mem_main_rspec) {
          case kPoCsumCtrl_0Row:
            switch (dimension) {
              case -1:
                return 32;
                break;
              case 0:
                return 32;
                break;
              default:
                assert(0);
                break;
            }
            break;
          case kPoCsumCtrl_1Row:
            switch (dimension) {
              case -1:
                return 32;
                break;
              case 0:
                return 32;
                break;
              default:
                assert(0);
                break;
            }
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kIPrsr:
        switch (selector_prsr_mem_main_rspec) {
          case kPoCsumCtrl_0Row:
            switch (dimension) {
              case -1:
                return 32;
                break;
              case 0:
                return 32;
                break;
              default:
                assert(0);
                break;
            }
            break;
          case kPoCsumCtrl_1Row:
            switch (dimension) {
              case -1:
                return 32;
                break;
              case 0:
                return 32;
                break;
              default:
                assert(0);
                break;
            }
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
  }

  static int RealArraySize(
      int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, PrsrMemMainRspecEnum selector_prsr_mem_main_rspec, int dimension
      ) {
    switch (selector_pipe_addrmap) {
      case kEPrsr:
        switch (selector_prsr_mem_main_rspec) {
          case kPoCsumCtrl_0Row:
            switch (dimension) {
              case -1:
                return 32;
                break;
              case 0:
                return 32;
                break;
              default:
                assert(0);
                break;
            }
            break;
          case kPoCsumCtrl_1Row:
            switch (dimension) {
              case -1:
                return 32;
                break;
              case 0:
                return 32;
                break;
              default:
                assert(0);
                break;
            }
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kIPrsr:
        switch (selector_prsr_mem_main_rspec) {
          case kPoCsumCtrl_0Row:
            switch (dimension) {
              case -1:
                return 32;
                break;
              case 0:
                return 32;
                break;
              default:
                assert(0);
                break;
            }
            break;
          case kPoCsumCtrl_1Row:
            switch (dimension) {
              case -1:
                return 32;
                break;
              case 0:
                return 32;
                break;
              default:
                assert(0);
                break;
            }
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
  }

};







class PrsrPoCsumCtrlRowArrayMutable : public model_core::RegisterBlockIndirect<RegisterArrayCallback> {
public:
  enum PipeAddrmapEnum {
    kEPrsr,
    kIPrsr
  };
  enum PrsrMemMainRspecEnum {
    kPoCsumCtrl_0Row,
    kPoCsumCtrl_1Row
  };
public:
  PrsrPoCsumCtrlRowArrayMutable(
      int chipNumber, int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, PrsrMemMainRspecEnum selector_prsr_mem_main_rspec, RegisterArrayCallback& write_callback = 0, RegisterArrayCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, selector_prsr_mem_main_rspec), 1 * ArraySize(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, selector_prsr_mem_main_rspec, -1), true, write_callback, read_callback, std::string("PrsrPoCsumCtrlRowArrayMutable")+":"+boost::lexical_cast<std::string>(index_pipe_addrmap) + "," + boost::lexical_cast<std::string>(selector_pipe_addrmap) + "," + boost::lexical_cast<std::string>(index_prsr_mem_main_rspec) + "," + boost::lexical_cast<std::string>(selector_prsr_mem_main_rspec)),
    array(RealArraySize(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, selector_prsr_mem_main_rspec, -1)),
    size0_(ArraySize(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, selector_prsr_mem_main_rspec, 0)),
    real_size0_(RealArraySize(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, selector_prsr_mem_main_rspec, 0))
    {
    }
public:





  uint16_t add(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].add();
  }
  void add(uint32_t a0,const uint16_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].add(v);
  }







  uint8_t swap(uint32_t a0,int j0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].swap(j0);
  }
  void swap(uint32_t a0,int j0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].swap(j0,v);
  }






  uint8_t shr(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].shr();
  }
  void shr(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].shr(v);
  }







  uint8_t mask(uint32_t a0,int j0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].mask(j0);
  }
  void mask(uint32_t a0,int j0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].mask(j0,v);
  }






  uint8_t dst_bit_hdr_end_pos(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].dst_bit_hdr_end_pos();
  }
  void dst_bit_hdr_end_pos(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].dst_bit_hdr_end_pos(v);
  }






  uint16_t dst(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].dst();
  }
  void dst(uint32_t a0,const uint16_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].dst(v);
  }







  uint8_t hdr_end(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].hdr_end();
  }
  void hdr_end(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].hdr_end(v);
  }






  uint8_t type(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].type();
  }
  void type(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].type(v);
  }





  uint8_t start(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].start();
  }
  void start(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].start(v);
  }







  uint8_t zeros_as_ones(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].zeros_as_ones();
  }
  void zeros_as_ones(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].zeros_as_ones(v);
  }






  uint8_t zeros_as_ones_pos(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].zeros_as_ones_pos();
  }
  void zeros_as_ones_pos(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].zeros_as_ones_pos(v);
  }
  bool calculate_index(
      uint64_t* offset, uint32_t* a0
      ) const {
    int i = (*offset)/1;
    (*offset) -= (i*1);
    uint32_t t = i;
    *a0 = t;
    bool in_bounds = true;
    in_bounds &= CheckArrayBounds(*a0,real_size0_,size0_,*offset,0,true);
    return in_bounds;
  }


  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    if (read_callback_) read_callback_(a0);
    array[a0].read(offset,data0,data1,T);
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    array[a0].write(offset,data0,data1,T);
    if (write_callback_) write_callback_(a0);
    return true;
  }

  void reset(
      
      ) {
    for (uint32_t i=0;i<array.size();++i) {
      array[i].reset();
      int t = i;
      int a0 = t;
      if (write_callback_) write_callback_(a0);
    }
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    all_zeros=false;
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return "OUT_OF_BOUNDS";
    std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
    if (! r_s.empty()) {
      r += indent_string + std::string("PrsrPoCsumCtrlRowArrayMutable") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
    }
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
    all_zeros=false;
    for (uint32_t a0=0;a0<real_size0_;++a0) {
      std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
      if (! r_s.empty()) {
        r += indent_string + std::string("PrsrPoCsumCtrlRowArrayMutable") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
      }
    }
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  std::vector<PrsrPoCsumCtrlRowMutable> array;
  uint32_t size0_;
  uint32_t real_size0_;
private:
  static uint64_t StartOffset(
      int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, PrsrMemMainRspecEnum selector_prsr_mem_main_rspec
      ) {
    uint64_t offset=0;
    offset += 0x20000000000; // to get to pipes
    assert(index_pipe_addrmap < 4);
    offset += index_pipe_addrmap * 0x2000000000; // pipe_addrmap[]
    switch (selector_pipe_addrmap) {
      case kEPrsr:
        offset += 0x1c80040000; // to get to e_prsr
        assert(index_prsr_mem_main_rspec < 18);
        offset += index_prsr_mem_main_rspec * 0x2000; // prsr_mem_main_rspec[]
        switch (selector_prsr_mem_main_rspec) {
          case kPoCsumCtrl_0Row:
            offset += 0x620; // to get to po_csum_ctrl_0_row
            break;
          case kPoCsumCtrl_1Row:
            offset += 0x640; // to get to po_csum_ctrl_1_row
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kIPrsr:
        offset += 0x1c80000000; // to get to i_prsr
        assert(index_prsr_mem_main_rspec < 18);
        offset += index_prsr_mem_main_rspec * 0x2000; // prsr_mem_main_rspec[]
        switch (selector_prsr_mem_main_rspec) {
          case kPoCsumCtrl_0Row:
            offset += 0x620; // to get to po_csum_ctrl_0_row
            break;
          case kPoCsumCtrl_1Row:
            offset += 0x640; // to get to po_csum_ctrl_1_row
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
    return offset;
  }

  static int ArraySize(
      int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, PrsrMemMainRspecEnum selector_prsr_mem_main_rspec, int dimension
      ) {
    switch (selector_pipe_addrmap) {
      case kEPrsr:
        switch (selector_prsr_mem_main_rspec) {
          case kPoCsumCtrl_0Row:
            switch (dimension) {
              case -1:
                return 32;
                break;
              case 0:
                return 32;
                break;
              default:
                assert(0);
                break;
            }
            break;
          case kPoCsumCtrl_1Row:
            switch (dimension) {
              case -1:
                return 32;
                break;
              case 0:
                return 32;
                break;
              default:
                assert(0);
                break;
            }
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kIPrsr:
        switch (selector_prsr_mem_main_rspec) {
          case kPoCsumCtrl_0Row:
            switch (dimension) {
              case -1:
                return 32;
                break;
              case 0:
                return 32;
                break;
              default:
                assert(0);
                break;
            }
            break;
          case kPoCsumCtrl_1Row:
            switch (dimension) {
              case -1:
                return 32;
                break;
              case 0:
                return 32;
                break;
              default:
                assert(0);
                break;
            }
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
  }

  static int RealArraySize(
      int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, PrsrMemMainRspecEnum selector_prsr_mem_main_rspec, int dimension
      ) {
    switch (selector_pipe_addrmap) {
      case kEPrsr:
        switch (selector_prsr_mem_main_rspec) {
          case kPoCsumCtrl_0Row:
            switch (dimension) {
              case -1:
                return 32;
                break;
              case 0:
                return 32;
                break;
              default:
                assert(0);
                break;
            }
            break;
          case kPoCsumCtrl_1Row:
            switch (dimension) {
              case -1:
                return 32;
                break;
              case 0:
                return 32;
                break;
              default:
                assert(0);
                break;
            }
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kIPrsr:
        switch (selector_prsr_mem_main_rspec) {
          case kPoCsumCtrl_0Row:
            switch (dimension) {
              case -1:
                return 32;
                break;
              case 0:
                return 32;
                break;
              default:
                assert(0);
                break;
            }
            break;
          case kPoCsumCtrl_1Row:
            switch (dimension) {
              case -1:
                return 32;
                break;
              case 0:
                return 32;
                break;
              default:
                assert(0);
                break;
            }
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
  }

};








class PrsrMlCtrInitRamM : public model_core::RegisterBlockIndirect<RegisterCallback> {
public:
  enum PipeAddrmapEnum {
    kEPrsr,
    kIPrsr
  };
public:
  PrsrMlCtrInitRamM(
      int chipNumber, int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, int index_prsr_ml_ctr_init_ram_m, RegisterCallback& write_callback = 0, RegisterCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, index_prsr_ml_ctr_init_ram_m), 1, false, write_callback, read_callback, std::string("PrsrMlCtrInitRamM")+":"+boost::lexical_cast<std::string>(index_pipe_addrmap) + "," + boost::lexical_cast<std::string>(selector_pipe_addrmap) + "," + boost::lexical_cast<std::string>(index_prsr_mem_main_rspec) + "," + boost::lexical_cast<std::string>(index_prsr_ml_ctr_init_ram_m))
    {
    }
  PrsrMlCtrInitRamM(
      
  )
    : RegisterBlockIndirect(0, 0, 0, false, 0, 0, "PrsrMlCtrInitRamM")
    {
    }
public:





  uint8_t &add() { return add_; }





  uint8_t &mask() { return mask_; }





  uint8_t &rotate() { return rotate_; }





  uint8_t &max() { return max_; }





  uint8_t &src() { return src_; }

  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    if (read_callback_) read_callback_();
    *data0 = static_cast<uint64_t>(add_);
    *data0 |= ((static_cast<uint64_t>(mask_) & 0x7) << 8);
    *data0 |= ((static_cast<uint64_t>(rotate_) & 0x7) << 11);
    *data0 |= (static_cast<uint64_t>(max_) << 14);
    *data0 |= ((static_cast<uint64_t>(src_) & 0x3) << 22);
    *data1 = 0;
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    add_ = data0;
    mask_ = ((data0 >> 8) & 0x7);
    rotate_ = ((data0 >> 11) & 0x7);
    max_ = (data0 >> 14);
    src_ = ((data0 >> 22) & 0x3);
    if (write_callback_) write_callback_();
    return true;
  }

  void reset(
      
      ) {
    add_ = 0;
    mask_ = 0;
    rotate_ = 0;
    max_ = 0;
    src_ = 0;
    if (write_callback_) write_callback_();
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    r += indent_string + std::string("PrsrMlCtrInitRamM") + ":\n";
    r += indent_string + "  " + std::string("add") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(add_) ) + "\n";
    all_zeros &= (0 == add_);
    r += indent_string + "  " + std::string("mask") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(mask_) ) + "\n";
    all_zeros &= (0 == mask_);
    r += indent_string + "  " + std::string("rotate") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(rotate_) ) + "\n";
    all_zeros &= (0 == rotate_);
    r += indent_string + "  " + std::string("max") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(max_) ) + "\n";
    all_zeros &= (0 == max_);
    r += indent_string + "  " + std::string("src") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(src_) ) + "\n";
    all_zeros &= (0 == src_);
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
    r += indent_string + std::string("PrsrMlCtrInitRamM") + ":\n";
    r += indent_string + "  " + std::string("add") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(add_) ) + "\n";
    all_zeros &= (0 == add_);
    r += indent_string + "  " + std::string("mask") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(mask_) ) + "\n";
    all_zeros &= (0 == mask_);
    r += indent_string + "  " + std::string("rotate") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(rotate_) ) + "\n";
    all_zeros &= (0 == rotate_);
    r += indent_string + "  " + std::string("max") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(max_) ) + "\n";
    all_zeros &= (0 == max_);
    r += indent_string + "  " + std::string("src") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(src_) ) + "\n";
    all_zeros &= (0 == src_);
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  uint8_t add_;
  uint8_t mask_;
  uint8_t rotate_;
  uint8_t max_;
  uint8_t src_;
private:
  static uint64_t StartOffset(
      int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, int index_prsr_ml_ctr_init_ram_m
      ) {
    uint64_t offset=0;
    offset += 0x20000000000; // to get to pipes
    assert(index_pipe_addrmap < 4);
    offset += index_pipe_addrmap * 0x2000000000; // pipe_addrmap[]
    switch (selector_pipe_addrmap) {
      case kEPrsr:
        offset += 0x1c80040000; // to get to e_prsr
        assert(index_prsr_mem_main_rspec < 18);
        offset += index_prsr_mem_main_rspec * 0x2000; // prsr_mem_main_rspec[]
        offset += 0x600; // to get to ml_ctr_init_ram
        assert(index_prsr_ml_ctr_init_ram_m < 16);
        offset += index_prsr_ml_ctr_init_ram_m * 0x1; // prsr_ml_ctr_init_ram_m[]
        break;
      case kIPrsr:
        offset += 0x1c80000000; // to get to i_prsr
        assert(index_prsr_mem_main_rspec < 18);
        offset += index_prsr_mem_main_rspec * 0x2000; // prsr_mem_main_rspec[]
        offset += 0x600; // to get to ml_ctr_init_ram
        assert(index_prsr_ml_ctr_init_ram_m < 16);
        offset += index_prsr_ml_ctr_init_ram_m * 0x1; // prsr_ml_ctr_init_ram_m[]
        break;
      default:
        assert(0);
        break;
    }
    return offset;
  }

};








class PrsrMlCtrInitRamMMutable : public model_core::RegisterBlockIndirect<RegisterCallback> {
public:
  enum PipeAddrmapEnum {
    kEPrsr,
    kIPrsr
  };
public:
  PrsrMlCtrInitRamMMutable(
      int chipNumber, int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, int index_prsr_ml_ctr_init_ram_m, RegisterCallback& write_callback = 0, RegisterCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, index_prsr_ml_ctr_init_ram_m), 1, true, write_callback, read_callback, std::string("PrsrMlCtrInitRamMMutable")+":"+boost::lexical_cast<std::string>(index_pipe_addrmap) + "," + boost::lexical_cast<std::string>(selector_pipe_addrmap) + "," + boost::lexical_cast<std::string>(index_prsr_mem_main_rspec) + "," + boost::lexical_cast<std::string>(index_prsr_ml_ctr_init_ram_m))
    {
    }
  PrsrMlCtrInitRamMMutable(
      
  )
    : RegisterBlockIndirect(0, 0, 0, true, 0, 0, "PrsrMlCtrInitRamMMutable")
    {
    }
public:





  uint8_t add() { return add_; }
  void add(const uint8_t &v) { add_=v; }





  uint8_t mask() { return mask_; }
  void mask(const uint8_t &v) { mask_=v; }





  uint8_t rotate() { return rotate_; }
  void rotate(const uint8_t &v) { rotate_=v; }





  uint8_t max() { return max_; }
  void max(const uint8_t &v) { max_=v; }





  uint8_t src() { return src_; }
  void src(const uint8_t &v) { src_=v; }

  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    if (read_callback_) read_callback_();
    *data0 = static_cast<uint64_t>(add_);
    *data0 |= ((static_cast<uint64_t>(mask_) & 0x7) << 8);
    *data0 |= ((static_cast<uint64_t>(rotate_) & 0x7) << 11);
    *data0 |= (static_cast<uint64_t>(max_) << 14);
    *data0 |= ((static_cast<uint64_t>(src_) & 0x3) << 22);
    *data1 = 0;
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    add_ = data0;
    mask_ = ((data0 >> 8) & 0x7);
    rotate_ = ((data0 >> 11) & 0x7);
    max_ = (data0 >> 14);
    src_ = ((data0 >> 22) & 0x3);
    if (write_callback_) write_callback_();
    return true;
  }

  void reset(
      
      ) {
    add_ = 0;
    mask_ = 0;
    rotate_ = 0;
    max_ = 0;
    src_ = 0;
    if (write_callback_) write_callback_();
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    r += indent_string + std::string("PrsrMlCtrInitRamMMutable") + ":\n";
    r += indent_string + "  " + std::string("add") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(add_) ) + "\n";
    all_zeros &= (0 == add_);
    r += indent_string + "  " + std::string("mask") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(mask_) ) + "\n";
    all_zeros &= (0 == mask_);
    r += indent_string + "  " + std::string("rotate") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(rotate_) ) + "\n";
    all_zeros &= (0 == rotate_);
    r += indent_string + "  " + std::string("max") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(max_) ) + "\n";
    all_zeros &= (0 == max_);
    r += indent_string + "  " + std::string("src") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(src_) ) + "\n";
    all_zeros &= (0 == src_);
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
    r += indent_string + std::string("PrsrMlCtrInitRamMMutable") + ":\n";
    r += indent_string + "  " + std::string("add") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(add_) ) + "\n";
    all_zeros &= (0 == add_);
    r += indent_string + "  " + std::string("mask") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(mask_) ) + "\n";
    all_zeros &= (0 == mask_);
    r += indent_string + "  " + std::string("rotate") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(rotate_) ) + "\n";
    all_zeros &= (0 == rotate_);
    r += indent_string + "  " + std::string("max") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(max_) ) + "\n";
    all_zeros &= (0 == max_);
    r += indent_string + "  " + std::string("src") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(src_) ) + "\n";
    all_zeros &= (0 == src_);
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  uint8_t add_;
  uint8_t mask_;
  uint8_t rotate_;
  uint8_t max_;
  uint8_t src_;
private:
  static uint64_t StartOffset(
      int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, int index_prsr_ml_ctr_init_ram_m
      ) {
    uint64_t offset=0;
    offset += 0x20000000000; // to get to pipes
    assert(index_pipe_addrmap < 4);
    offset += index_pipe_addrmap * 0x2000000000; // pipe_addrmap[]
    switch (selector_pipe_addrmap) {
      case kEPrsr:
        offset += 0x1c80040000; // to get to e_prsr
        assert(index_prsr_mem_main_rspec < 18);
        offset += index_prsr_mem_main_rspec * 0x2000; // prsr_mem_main_rspec[]
        offset += 0x600; // to get to ml_ctr_init_ram
        assert(index_prsr_ml_ctr_init_ram_m < 16);
        offset += index_prsr_ml_ctr_init_ram_m * 0x1; // prsr_ml_ctr_init_ram_m[]
        break;
      case kIPrsr:
        offset += 0x1c80000000; // to get to i_prsr
        assert(index_prsr_mem_main_rspec < 18);
        offset += index_prsr_mem_main_rspec * 0x2000; // prsr_mem_main_rspec[]
        offset += 0x600; // to get to ml_ctr_init_ram
        assert(index_prsr_ml_ctr_init_ram_m < 16);
        offset += index_prsr_ml_ctr_init_ram_m * 0x1; // prsr_ml_ctr_init_ram_m[]
        break;
      default:
        assert(0);
        break;
    }
    return offset;
  }

};








class PrsrMlCtrInitRamMArray : public model_core::RegisterBlockIndirect<RegisterArrayCallback> {
public:
  enum PipeAddrmapEnum {
    kEPrsr,
    kIPrsr
  };
public:
  PrsrMlCtrInitRamMArray(
      int chipNumber, int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, RegisterArrayCallback& write_callback = 0, RegisterArrayCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec), 1 * ArraySize(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, -1), false, write_callback, read_callback, std::string("PrsrMlCtrInitRamMArray")+":"+boost::lexical_cast<std::string>(index_pipe_addrmap) + "," + boost::lexical_cast<std::string>(selector_pipe_addrmap) + "," + boost::lexical_cast<std::string>(index_prsr_mem_main_rspec)),
    array(RealArraySize(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, -1)),
    size0_(ArraySize(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, 0)),
    real_size0_(RealArraySize(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, 0))
    {
    }
public:





  uint8_t &add(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].add();
  }





  uint8_t &mask(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].mask();
  }





  uint8_t &rotate(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].rotate();
  }





  uint8_t &max(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].max();
  }





  uint8_t &src(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].src();
  }
  bool calculate_index(
      uint64_t* offset, uint32_t* a0
      ) const {
    int i = (*offset)/1;
    (*offset) -= (i*1);
    uint32_t t = i;
    *a0 = t;
    bool in_bounds = true;
    in_bounds &= CheckArrayBounds(*a0,real_size0_,size0_,*offset,0,true);
    return in_bounds;
  }


  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    if (read_callback_) read_callback_(a0);
    array[a0].read(offset,data0,data1,T);
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    array[a0].write(offset,data0,data1,T);
    if (write_callback_) write_callback_(a0);
    return true;
  }

  void reset(
      
      ) {
    for (uint32_t i=0;i<array.size();++i) {
      array[i].reset();
      int t = i;
      int a0 = t;
      if (write_callback_) write_callback_(a0);
    }
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    all_zeros=false;
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return "OUT_OF_BOUNDS";
    std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
    if (! r_s.empty()) {
      r += indent_string + std::string("PrsrMlCtrInitRamMArray") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
    }
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
    all_zeros=false;
    for (uint32_t a0=0;a0<real_size0_;++a0) {
      std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
      if (! r_s.empty()) {
        r += indent_string + std::string("PrsrMlCtrInitRamMArray") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
      }
    }
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  std::vector<PrsrMlCtrInitRamM> array;
  uint32_t size0_;
  uint32_t real_size0_;
private:
  static uint64_t StartOffset(
      int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec
      ) {
    uint64_t offset=0;
    offset += 0x20000000000; // to get to pipes
    assert(index_pipe_addrmap < 4);
    offset += index_pipe_addrmap * 0x2000000000; // pipe_addrmap[]
    switch (selector_pipe_addrmap) {
      case kEPrsr:
        offset += 0x1c80040000; // to get to e_prsr
        assert(index_prsr_mem_main_rspec < 18);
        offset += index_prsr_mem_main_rspec * 0x2000; // prsr_mem_main_rspec[]
        offset += 0x600; // to get to ml_ctr_init_ram
        break;
      case kIPrsr:
        offset += 0x1c80000000; // to get to i_prsr
        assert(index_prsr_mem_main_rspec < 18);
        offset += index_prsr_mem_main_rspec * 0x2000; // prsr_mem_main_rspec[]
        offset += 0x600; // to get to ml_ctr_init_ram
        break;
      default:
        assert(0);
        break;
    }
    return offset;
  }

  static int ArraySize(
      int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, int dimension
      ) {
    switch (selector_pipe_addrmap) {
      case kEPrsr:
        switch (dimension) {
          case -1:
            return 16;
            break;
          case 0:
            return 16;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kIPrsr:
        switch (dimension) {
          case -1:
            return 16;
            break;
          case 0:
            return 16;
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
  }

  static int RealArraySize(
      int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, int dimension
      ) {
    switch (selector_pipe_addrmap) {
      case kEPrsr:
        switch (dimension) {
          case -1:
            return 16;
            break;
          case 0:
            return 16;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kIPrsr:
        switch (dimension) {
          case -1:
            return 16;
            break;
          case 0:
            return 16;
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
  }

};








class PrsrMlCtrInitRamMArrayMutable : public model_core::RegisterBlockIndirect<RegisterArrayCallback> {
public:
  enum PipeAddrmapEnum {
    kEPrsr,
    kIPrsr
  };
public:
  PrsrMlCtrInitRamMArrayMutable(
      int chipNumber, int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, RegisterArrayCallback& write_callback = 0, RegisterArrayCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec), 1 * ArraySize(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, -1), true, write_callback, read_callback, std::string("PrsrMlCtrInitRamMArrayMutable")+":"+boost::lexical_cast<std::string>(index_pipe_addrmap) + "," + boost::lexical_cast<std::string>(selector_pipe_addrmap) + "," + boost::lexical_cast<std::string>(index_prsr_mem_main_rspec)),
    array(RealArraySize(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, -1)),
    size0_(ArraySize(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, 0)),
    real_size0_(RealArraySize(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, 0))
    {
    }
public:





  uint8_t add(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].add();
  }
  void add(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].add(v);
  }





  uint8_t mask(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].mask();
  }
  void mask(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].mask(v);
  }





  uint8_t rotate(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].rotate();
  }
  void rotate(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].rotate(v);
  }





  uint8_t max(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].max();
  }
  void max(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].max(v);
  }





  uint8_t src(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].src();
  }
  void src(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].src(v);
  }
  bool calculate_index(
      uint64_t* offset, uint32_t* a0
      ) const {
    int i = (*offset)/1;
    (*offset) -= (i*1);
    uint32_t t = i;
    *a0 = t;
    bool in_bounds = true;
    in_bounds &= CheckArrayBounds(*a0,real_size0_,size0_,*offset,0,true);
    return in_bounds;
  }


  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    if (read_callback_) read_callback_(a0);
    array[a0].read(offset,data0,data1,T);
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    array[a0].write(offset,data0,data1,T);
    if (write_callback_) write_callback_(a0);
    return true;
  }

  void reset(
      
      ) {
    for (uint32_t i=0;i<array.size();++i) {
      array[i].reset();
      int t = i;
      int a0 = t;
      if (write_callback_) write_callback_(a0);
    }
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    all_zeros=false;
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return "OUT_OF_BOUNDS";
    std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
    if (! r_s.empty()) {
      r += indent_string + std::string("PrsrMlCtrInitRamMArrayMutable") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
    }
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
    all_zeros=false;
    for (uint32_t a0=0;a0<real_size0_;++a0) {
      std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
      if (! r_s.empty()) {
        r += indent_string + std::string("PrsrMlCtrInitRamMArrayMutable") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
      }
    }
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  std::vector<PrsrMlCtrInitRamMMutable> array;
  uint32_t size0_;
  uint32_t real_size0_;
private:
  static uint64_t StartOffset(
      int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec
      ) {
    uint64_t offset=0;
    offset += 0x20000000000; // to get to pipes
    assert(index_pipe_addrmap < 4);
    offset += index_pipe_addrmap * 0x2000000000; // pipe_addrmap[]
    switch (selector_pipe_addrmap) {
      case kEPrsr:
        offset += 0x1c80040000; // to get to e_prsr
        assert(index_prsr_mem_main_rspec < 18);
        offset += index_prsr_mem_main_rspec * 0x2000; // prsr_mem_main_rspec[]
        offset += 0x600; // to get to ml_ctr_init_ram
        break;
      case kIPrsr:
        offset += 0x1c80000000; // to get to i_prsr
        assert(index_prsr_mem_main_rspec < 18);
        offset += index_prsr_mem_main_rspec * 0x2000; // prsr_mem_main_rspec[]
        offset += 0x600; // to get to ml_ctr_init_ram
        break;
      default:
        assert(0);
        break;
    }
    return offset;
  }

  static int ArraySize(
      int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, int dimension
      ) {
    switch (selector_pipe_addrmap) {
      case kEPrsr:
        switch (dimension) {
          case -1:
            return 16;
            break;
          case 0:
            return 16;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kIPrsr:
        switch (dimension) {
          case -1:
            return 16;
            break;
          case 0:
            return 16;
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
  }

  static int RealArraySize(
      int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, int dimension
      ) {
    switch (selector_pipe_addrmap) {
      case kEPrsr:
        switch (dimension) {
          case -1:
            return 16;
            break;
          case 0:
            return 16;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kIPrsr:
        switch (dimension) {
          case -1:
            return 16;
            break;
          case 0:
            return 16;
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
  }

};








class PrsrPoActionRow : public model_core::RegisterBlockIndirect<RegisterCallback> {
public:
  enum PipeAddrmapEnum {
    kEPrsr,
    kIPrsr
  };
public:
  PrsrPoActionRow(
      int chipNumber, int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, int index_prsr_po_action_row, RegisterCallback& write_callback = 0, RegisterCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, index_prsr_po_action_row), 2, false, write_callback, read_callback, std::string("PrsrPoActionRow")+":"+boost::lexical_cast<std::string>(index_pipe_addrmap) + "," + boost::lexical_cast<std::string>(selector_pipe_addrmap) + "," + boost::lexical_cast<std::string>(index_prsr_mem_main_rspec) + "," + boost::lexical_cast<std::string>(index_prsr_po_action_row))
    {
    }
  PrsrPoActionRow(
      
  )
    : RegisterBlockIndirect(0, 0, 0, false, 0, 0, "PrsrPoActionRow")
    {
    }
public:






  uint8_t &csum_addr(int j0) { return csum_addr_[j0]; }






  uint8_t &csum_en(int j0) { return csum_en_[j0]; }






  uint8_t &dst_offset_inc() { return dst_offset_inc_; }






  uint8_t &dst_offset_rst() { return dst_offset_rst_; }






  uint8_t &phv_32b_offset_add_dst_0() { return phv_32b_offset_add_dst_0_; }






  uint8_t &phv_32b_offset_add_dst_1() { return phv_32b_offset_add_dst_1_; }






  uint8_t &phv_32b_offset_add_dst_2() { return phv_32b_offset_add_dst_2_; }






  uint8_t &phv_32b_offset_add_dst_3() { return phv_32b_offset_add_dst_3_; }






  uint8_t &phv_16b_offset_add_dst_0() { return phv_16b_offset_add_dst_0_; }






  uint8_t &phv_16b_offset_add_dst_1() { return phv_16b_offset_add_dst_1_; }






  uint8_t &phv_16b_offset_add_dst_2() { return phv_16b_offset_add_dst_2_; }






  uint8_t &phv_16b_offset_add_dst_3() { return phv_16b_offset_add_dst_3_; }






  uint8_t &phv_8b_offset_add_dst_0() { return phv_8b_offset_add_dst_0_; }






  uint8_t &phv_8b_offset_add_dst_1() { return phv_8b_offset_add_dst_1_; }






  uint8_t &phv_8b_offset_add_dst_2() { return phv_8b_offset_add_dst_2_; }






  uint8_t &phv_8b_offset_add_dst_3() { return phv_8b_offset_add_dst_3_; }






  uint8_t &phv_32b_offset_rot_imm_0() { return phv_32b_offset_rot_imm_0_; }






  uint8_t &phv_32b_offset_rot_imm_1() { return phv_32b_offset_rot_imm_1_; }






  uint8_t &phv_16b_offset_rot_imm_0() { return phv_16b_offset_rot_imm_0_; }






  uint8_t &phv_16b_offset_rot_imm_1() { return phv_16b_offset_rot_imm_1_; }






  uint8_t &phv_8b_offset_rot_imm_0() { return phv_8b_offset_rot_imm_0_; }






  uint8_t &phv_8b_offset_rot_imm_1() { return phv_8b_offset_rot_imm_1_; }






  uint8_t &phv_8b_offset_rot_imm_2() { return phv_8b_offset_rot_imm_2_; }






  uint8_t &phv_8b_offset_rot_imm_3() { return phv_8b_offset_rot_imm_3_; }





  uint16_t &phv_32b_dst_0() { return phv_32b_dst_0_; }





  uint16_t &phv_32b_dst_1() { return phv_32b_dst_1_; }





  uint16_t &phv_32b_dst_2() { return phv_32b_dst_2_; }





  uint16_t &phv_32b_dst_3() { return phv_32b_dst_3_; }





  uint16_t &phv_16b_dst_0() { return phv_16b_dst_0_; }





  uint16_t &phv_16b_dst_1() { return phv_16b_dst_1_; }





  uint16_t &phv_16b_dst_2() { return phv_16b_dst_2_; }





  uint16_t &phv_16b_dst_3() { return phv_16b_dst_3_; }





  uint16_t &phv_8b_dst_0() { return phv_8b_dst_0_; }





  uint16_t &phv_8b_dst_1() { return phv_8b_dst_1_; }





  uint16_t &phv_8b_dst_2() { return phv_8b_dst_2_; }





  uint16_t &phv_8b_dst_3() { return phv_8b_dst_3_; }






  uint8_t &phv_32b_src_0() { return phv_32b_src_0_; }






  uint8_t &phv_32b_src_1() { return phv_32b_src_1_; }






  uint8_t &phv_32b_src_2() { return phv_32b_src_2_; }






  uint8_t &phv_32b_src_3() { return phv_32b_src_3_; }






  uint8_t &phv_16b_src_0() { return phv_16b_src_0_; }






  uint8_t &phv_16b_src_1() { return phv_16b_src_1_; }






  uint8_t &phv_16b_src_2() { return phv_16b_src_2_; }






  uint8_t &phv_16b_src_3() { return phv_16b_src_3_; }






  uint8_t &phv_8b_src_0() { return phv_8b_src_0_; }






  uint8_t &phv_8b_src_1() { return phv_8b_src_1_; }






  uint8_t &phv_8b_src_2() { return phv_8b_src_2_; }






  uint8_t &phv_8b_src_3() { return phv_8b_src_3_; }





  uint8_t &phv_32b_src_type_0() { return phv_32b_src_type_0_; }





  uint8_t &phv_32b_src_type_1() { return phv_32b_src_type_1_; }





  uint8_t &phv_16b_src_type_0() { return phv_16b_src_type_0_; }





  uint8_t &phv_16b_src_type_1() { return phv_16b_src_type_1_; }





  uint8_t &phv_8b_src_type_0() { return phv_8b_src_type_0_; }





  uint8_t &phv_8b_src_type_1() { return phv_8b_src_type_1_; }





  uint8_t &phv_8b_src_type_2() { return phv_8b_src_type_2_; }





  uint8_t &phv_8b_src_type_3() { return phv_8b_src_type_3_; }






  uint8_t &pri_upd_type() { return pri_upd_type_; }






  uint8_t &pri_upd_src() { return pri_upd_src_; }







  uint8_t &pri_upd_en_shr() { return pri_upd_en_shr_; }






  uint8_t &pri_upd_val_mask() { return pri_upd_val_mask_; }

  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    if (offset < 0x1) {
      if (read_callback_) read_callback_();
      *data0 = (static_cast<uint64_t>(csum_addr_[0]) & 0x1f);
      *data0 |= ((static_cast<uint64_t>(csum_addr_[1]) & 0x1f) << 5);
      *data0 |= ((static_cast<uint64_t>(csum_en_[0]) & 0x1) << 10);
      *data0 |= ((static_cast<uint64_t>(csum_en_[1]) & 0x1) << 11);
      *data0 |= ((static_cast<uint64_t>(dst_offset_inc_) & 0x1f) << 12);
      *data0 |= ((static_cast<uint64_t>(dst_offset_rst_) & 0x1) << 17);
      *data0 |= ((static_cast<uint64_t>(phv_32b_offset_add_dst_0_) & 0x1) << 18);
      *data0 |= ((static_cast<uint64_t>(phv_32b_offset_add_dst_1_) & 0x1) << 19);
      *data0 |= ((static_cast<uint64_t>(phv_32b_offset_add_dst_2_) & 0x1) << 20);
      *data0 |= ((static_cast<uint64_t>(phv_32b_offset_add_dst_3_) & 0x1) << 21);
      *data0 |= ((static_cast<uint64_t>(phv_16b_offset_add_dst_0_) & 0x1) << 22);
      *data0 |= ((static_cast<uint64_t>(phv_16b_offset_add_dst_1_) & 0x1) << 23);
      *data0 |= ((static_cast<uint64_t>(phv_16b_offset_add_dst_2_) & 0x1) << 24);
      *data0 |= ((static_cast<uint64_t>(phv_16b_offset_add_dst_3_) & 0x1) << 25);
      *data0 |= ((static_cast<uint64_t>(phv_8b_offset_add_dst_0_) & 0x1) << 26);
      *data0 |= ((static_cast<uint64_t>(phv_8b_offset_add_dst_1_) & 0x1) << 27);
      *data0 |= ((static_cast<uint64_t>(phv_8b_offset_add_dst_2_) & 0x1) << 28);
      *data0 |= ((static_cast<uint64_t>(phv_8b_offset_add_dst_3_) & 0x1) << 29);
      *data0 |= ((static_cast<uint64_t>(phv_32b_offset_rot_imm_0_) & 0x1) << 30);
      *data0 |= ((static_cast<uint64_t>(phv_32b_offset_rot_imm_1_) & 0x1) << 31);
      *data0 |= ((static_cast<uint64_t>(phv_16b_offset_rot_imm_0_) & 0x1) << 32);
      *data0 |= ((static_cast<uint64_t>(phv_16b_offset_rot_imm_1_) & 0x1) << 33);
      *data0 |= ((static_cast<uint64_t>(phv_8b_offset_rot_imm_0_) & 0x1) << 34);
      *data0 |= ((static_cast<uint64_t>(phv_8b_offset_rot_imm_1_) & 0x1) << 35);
      *data0 |= ((static_cast<uint64_t>(phv_8b_offset_rot_imm_2_) & 0x1) << 36);
      *data0 |= ((static_cast<uint64_t>(phv_8b_offset_rot_imm_3_) & 0x1) << 37);
      *data0 |= ((static_cast<uint64_t>(phv_32b_dst_0_) & 0x1ff) << 38);
      *data0 |= ((static_cast<uint64_t>(phv_32b_dst_1_) & 0x1ff) << 47);
      *data0 |= ((static_cast<uint64_t>(phv_32b_dst_2_) & 0xff) << 56);
      *data1 = ((static_cast<uint64_t>(phv_32b_dst_2_) & 0x100) >> 8);
      *data1 |= ((static_cast<uint64_t>(phv_32b_dst_3_) & 0x1ff) << 1);
      *data1 |= ((static_cast<uint64_t>(phv_16b_dst_0_) & 0x1ff) << 10);
      *data1 |= ((static_cast<uint64_t>(phv_16b_dst_1_) & 0x1ff) << 19);
      *data1 |= ((static_cast<uint64_t>(phv_16b_dst_2_) & 0x1ff) << 28);
      *data1 |= ((static_cast<uint64_t>(phv_16b_dst_3_) & 0x1ff) << 37);
      *data1 |= ((static_cast<uint64_t>(phv_8b_dst_0_) & 0x1ff) << 46);
      *data1 |= ((static_cast<uint64_t>(phv_8b_dst_1_) & 0x1ff) << 55);
    }
    else if (offset >= 0x1 && offset < 0x2) {
      if (read_callback_) read_callback_();
      *data0 = (static_cast<uint64_t>(phv_8b_dst_2_) & 0x1ff);
      *data0 |= ((static_cast<uint64_t>(phv_8b_dst_3_) & 0x1ff) << 9);
      *data0 |= (static_cast<uint64_t>(phv_32b_src_0_) << 18);
      *data0 |= (static_cast<uint64_t>(phv_32b_src_1_) << 26);
      *data0 |= ((static_cast<uint64_t>(phv_32b_src_2_) & 0x3f) << 34);
      *data0 |= ((static_cast<uint64_t>(phv_32b_src_3_) & 0x3f) << 40);
      *data0 |= (static_cast<uint64_t>(phv_16b_src_0_) << 46);
      *data0 |= (static_cast<uint64_t>(phv_16b_src_1_) << 54);
      *data0 |= ((static_cast<uint64_t>(phv_16b_src_2_) & 0x3) << 62);
      *data1 = ((static_cast<uint64_t>(phv_16b_src_2_) & 0x3c) >> 2);
      *data1 |= ((static_cast<uint64_t>(phv_16b_src_3_) & 0x3f) << 4);
      *data1 |= (static_cast<uint64_t>(phv_8b_src_0_) << 10);
      *data1 |= (static_cast<uint64_t>(phv_8b_src_1_) << 18);
      *data1 |= (static_cast<uint64_t>(phv_8b_src_2_) << 26);
      *data1 |= (static_cast<uint64_t>(phv_8b_src_3_) << 34);
      *data1 |= ((static_cast<uint64_t>(phv_32b_src_type_0_) & 0x1) << 42);
      *data1 |= ((static_cast<uint64_t>(phv_32b_src_type_1_) & 0x1) << 43);
      *data1 |= ((static_cast<uint64_t>(phv_16b_src_type_0_) & 0x1) << 44);
      *data1 |= ((static_cast<uint64_t>(phv_16b_src_type_1_) & 0x1) << 45);
      *data1 |= ((static_cast<uint64_t>(phv_8b_src_type_0_) & 0x1) << 46);
      *data1 |= ((static_cast<uint64_t>(phv_8b_src_type_1_) & 0x1) << 47);
      *data1 |= ((static_cast<uint64_t>(phv_8b_src_type_2_) & 0x1) << 48);
      *data1 |= ((static_cast<uint64_t>(phv_8b_src_type_3_) & 0x1) << 49);
      *data1 |= ((static_cast<uint64_t>(pri_upd_type_) & 0x1) << 50);
      *data1 |= ((static_cast<uint64_t>(pri_upd_src_) & 0x1f) << 51);
      *data1 |= ((static_cast<uint64_t>(pri_upd_en_shr_) & 0xf) << 56);
      *data1 |= ((static_cast<uint64_t>(pri_upd_val_mask_) & 0x7) << 60);
    }
    else {
      assert(0);
    }
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    if (offset < 0x1) {
      csum_addr_[0] = (data0 & 0x1f);
      csum_addr_[1] = ((data0 >> 5) & 0x1f);
      csum_en_[0] = ((data0 >> 10) & 0x1);
      csum_en_[1] = ((data0 >> 11) & 0x1);
      dst_offset_inc_ = ((data0 >> 12) & 0x1f);
      dst_offset_rst_ = ((data0 >> 17) & 0x1);
      phv_32b_offset_add_dst_0_ = ((data0 >> 18) & 0x1);
      phv_32b_offset_add_dst_1_ = ((data0 >> 19) & 0x1);
      phv_32b_offset_add_dst_2_ = ((data0 >> 20) & 0x1);
      phv_32b_offset_add_dst_3_ = ((data0 >> 21) & 0x1);
      phv_16b_offset_add_dst_0_ = ((data0 >> 22) & 0x1);
      phv_16b_offset_add_dst_1_ = ((data0 >> 23) & 0x1);
      phv_16b_offset_add_dst_2_ = ((data0 >> 24) & 0x1);
      phv_16b_offset_add_dst_3_ = ((data0 >> 25) & 0x1);
      phv_8b_offset_add_dst_0_ = ((data0 >> 26) & 0x1);
      phv_8b_offset_add_dst_1_ = ((data0 >> 27) & 0x1);
      phv_8b_offset_add_dst_2_ = ((data0 >> 28) & 0x1);
      phv_8b_offset_add_dst_3_ = ((data0 >> 29) & 0x1);
      phv_32b_offset_rot_imm_0_ = ((data0 >> 30) & 0x1);
      phv_32b_offset_rot_imm_1_ = ((data0 >> 31) & 0x1);
      phv_16b_offset_rot_imm_0_ = ((data0 >> 32) & 0x1);
      phv_16b_offset_rot_imm_1_ = ((data0 >> 33) & 0x1);
      phv_8b_offset_rot_imm_0_ = ((data0 >> 34) & 0x1);
      phv_8b_offset_rot_imm_1_ = ((data0 >> 35) & 0x1);
      phv_8b_offset_rot_imm_2_ = ((data0 >> 36) & 0x1);
      phv_8b_offset_rot_imm_3_ = ((data0 >> 37) & 0x1);
      phv_32b_dst_0_ = ((data0 >> 38) & 0x1ff);
      phv_32b_dst_1_ = ((data0 >> 47) & 0x1ff);
      phv_32b_dst_2_ = (((data0 >> 56) & 0xff) | (phv_32b_dst_2_ & 0x100));
      phv_32b_dst_2_ = (((data1 << 8) & 0x100) | (phv_32b_dst_2_ & 0xff));
      phv_32b_dst_3_ = ((data1 >> 1) & 0x1ff);
      phv_16b_dst_0_ = ((data1 >> 10) & 0x1ff);
      phv_16b_dst_1_ = ((data1 >> 19) & 0x1ff);
      phv_16b_dst_2_ = ((data1 >> 28) & 0x1ff);
      phv_16b_dst_3_ = ((data1 >> 37) & 0x1ff);
      phv_8b_dst_0_ = ((data1 >> 46) & 0x1ff);
      phv_8b_dst_1_ = ((data1 >> 55) & 0x1ff);
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x1 && offset < 0x2) {
      phv_8b_dst_2_ = (data0 & 0x1ff);
      phv_8b_dst_3_ = ((data0 >> 9) & 0x1ff);
      phv_32b_src_0_ = (data0 >> 18);
      phv_32b_src_1_ = (data0 >> 26);
      phv_32b_src_2_ = ((data0 >> 34) & 0x3f);
      phv_32b_src_3_ = ((data0 >> 40) & 0x3f);
      phv_16b_src_0_ = (data0 >> 46);
      phv_16b_src_1_ = (data0 >> 54);
      phv_16b_src_2_ = (((data0 >> 62) & 0x3) | (phv_16b_src_2_ & 0x3c));
      phv_16b_src_2_ = (((data1 << 2) & 0x3c) | (phv_16b_src_2_ & 0x3));
      phv_16b_src_3_ = ((data1 >> 4) & 0x3f);
      phv_8b_src_0_ = (data1 >> 10);
      phv_8b_src_1_ = (data1 >> 18);
      phv_8b_src_2_ = (data1 >> 26);
      phv_8b_src_3_ = (data1 >> 34);
      phv_32b_src_type_0_ = ((data1 >> 42) & 0x1);
      phv_32b_src_type_1_ = ((data1 >> 43) & 0x1);
      phv_16b_src_type_0_ = ((data1 >> 44) & 0x1);
      phv_16b_src_type_1_ = ((data1 >> 45) & 0x1);
      phv_8b_src_type_0_ = ((data1 >> 46) & 0x1);
      phv_8b_src_type_1_ = ((data1 >> 47) & 0x1);
      phv_8b_src_type_2_ = ((data1 >> 48) & 0x1);
      phv_8b_src_type_3_ = ((data1 >> 49) & 0x1);
      pri_upd_type_ = ((data1 >> 50) & 0x1);
      pri_upd_src_ = ((data1 >> 51) & 0x1f);
      pri_upd_en_shr_ = ((data1 >> 56) & 0xf);
      pri_upd_val_mask_ = ((data1 >> 60) & 0x7);
      if (write_callback_) write_callback_();
    }
    else {
      assert(0);
    }
    return true;
  }

  void reset(
      
      ) {
    csum_addr_[0] = 0;
    csum_addr_[1] = 0;
    csum_en_[0] = 0;
    csum_en_[1] = 0;
    dst_offset_inc_ = 0;
    dst_offset_rst_ = 0;
    phv_32b_offset_add_dst_0_ = 0;
    phv_32b_offset_add_dst_1_ = 0;
    phv_32b_offset_add_dst_2_ = 0;
    phv_32b_offset_add_dst_3_ = 0;
    phv_16b_offset_add_dst_0_ = 0;
    phv_16b_offset_add_dst_1_ = 0;
    phv_16b_offset_add_dst_2_ = 0;
    phv_16b_offset_add_dst_3_ = 0;
    phv_8b_offset_add_dst_0_ = 0;
    phv_8b_offset_add_dst_1_ = 0;
    phv_8b_offset_add_dst_2_ = 0;
    phv_8b_offset_add_dst_3_ = 0;
    phv_32b_offset_rot_imm_0_ = 0;
    phv_32b_offset_rot_imm_1_ = 0;
    phv_16b_offset_rot_imm_0_ = 0;
    phv_16b_offset_rot_imm_1_ = 0;
    phv_8b_offset_rot_imm_0_ = 0;
    phv_8b_offset_rot_imm_1_ = 0;
    phv_8b_offset_rot_imm_2_ = 0;
    phv_8b_offset_rot_imm_3_ = 0;
    phv_32b_dst_0_ = 0;
    phv_32b_dst_1_ = 0;
    phv_32b_dst_2_ = 0;
    phv_32b_dst_3_ = 0;
    phv_16b_dst_0_ = 0;
    phv_16b_dst_1_ = 0;
    phv_16b_dst_2_ = 0;
    phv_16b_dst_3_ = 0;
    phv_8b_dst_0_ = 0;
    phv_8b_dst_1_ = 0;
    phv_8b_dst_2_ = 0;
    phv_8b_dst_3_ = 0;
    phv_32b_src_0_ = 0;
    phv_32b_src_1_ = 0;
    phv_32b_src_2_ = 0;
    phv_32b_src_3_ = 0;
    phv_16b_src_0_ = 0;
    phv_16b_src_1_ = 0;
    phv_16b_src_2_ = 0;
    phv_16b_src_3_ = 0;
    phv_8b_src_0_ = 0;
    phv_8b_src_1_ = 0;
    phv_8b_src_2_ = 0;
    phv_8b_src_3_ = 0;
    phv_32b_src_type_0_ = 0;
    phv_32b_src_type_1_ = 0;
    phv_16b_src_type_0_ = 0;
    phv_16b_src_type_1_ = 0;
    phv_8b_src_type_0_ = 0;
    phv_8b_src_type_1_ = 0;
    phv_8b_src_type_2_ = 0;
    phv_8b_src_type_3_ = 0;
    pri_upd_type_ = 0;
    pri_upd_src_ = 0;
    pri_upd_en_shr_ = 0;
    pri_upd_val_mask_ = 0;
    if (write_callback_) write_callback_();
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    r += indent_string + std::string("PrsrPoActionRow") + ":\n";
    for (uint32_t f=0;f<2;++f) {
      r += indent_string + "  " + std::string("csum_addr") + "["+boost::lexical_cast<std::string>(f)+"]"+ ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(csum_addr_[f]) ) + "\n";
      all_zeros &= (0 == csum_addr_[f]);
    }
    for (uint32_t f=0;f<2;++f) {
      r += indent_string + "  " + std::string("csum_en") + "["+boost::lexical_cast<std::string>(f)+"]"+ ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(csum_en_[f]) ) + "\n";
      all_zeros &= (0 == csum_en_[f]);
    }
    r += indent_string + "  " + std::string("dst_offset_inc") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(dst_offset_inc_) ) + "\n";
    all_zeros &= (0 == dst_offset_inc_);
    r += indent_string + "  " + std::string("dst_offset_rst") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(dst_offset_rst_) ) + "\n";
    all_zeros &= (0 == dst_offset_rst_);
    r += indent_string + "  " + std::string("phv_32b_offset_add_dst_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_offset_add_dst_0_) ) + "\n";
    all_zeros &= (0 == phv_32b_offset_add_dst_0_);
    r += indent_string + "  " + std::string("phv_32b_offset_add_dst_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_offset_add_dst_1_) ) + "\n";
    all_zeros &= (0 == phv_32b_offset_add_dst_1_);
    r += indent_string + "  " + std::string("phv_32b_offset_add_dst_2") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_offset_add_dst_2_) ) + "\n";
    all_zeros &= (0 == phv_32b_offset_add_dst_2_);
    r += indent_string + "  " + std::string("phv_32b_offset_add_dst_3") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_offset_add_dst_3_) ) + "\n";
    all_zeros &= (0 == phv_32b_offset_add_dst_3_);
    r += indent_string + "  " + std::string("phv_16b_offset_add_dst_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_offset_add_dst_0_) ) + "\n";
    all_zeros &= (0 == phv_16b_offset_add_dst_0_);
    r += indent_string + "  " + std::string("phv_16b_offset_add_dst_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_offset_add_dst_1_) ) + "\n";
    all_zeros &= (0 == phv_16b_offset_add_dst_1_);
    r += indent_string + "  " + std::string("phv_16b_offset_add_dst_2") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_offset_add_dst_2_) ) + "\n";
    all_zeros &= (0 == phv_16b_offset_add_dst_2_);
    r += indent_string + "  " + std::string("phv_16b_offset_add_dst_3") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_offset_add_dst_3_) ) + "\n";
    all_zeros &= (0 == phv_16b_offset_add_dst_3_);
    r += indent_string + "  " + std::string("phv_8b_offset_add_dst_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_offset_add_dst_0_) ) + "\n";
    all_zeros &= (0 == phv_8b_offset_add_dst_0_);
    r += indent_string + "  " + std::string("phv_8b_offset_add_dst_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_offset_add_dst_1_) ) + "\n";
    all_zeros &= (0 == phv_8b_offset_add_dst_1_);
    r += indent_string + "  " + std::string("phv_8b_offset_add_dst_2") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_offset_add_dst_2_) ) + "\n";
    all_zeros &= (0 == phv_8b_offset_add_dst_2_);
    r += indent_string + "  " + std::string("phv_8b_offset_add_dst_3") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_offset_add_dst_3_) ) + "\n";
    all_zeros &= (0 == phv_8b_offset_add_dst_3_);
    r += indent_string + "  " + std::string("phv_32b_offset_rot_imm_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_offset_rot_imm_0_) ) + "\n";
    all_zeros &= (0 == phv_32b_offset_rot_imm_0_);
    r += indent_string + "  " + std::string("phv_32b_offset_rot_imm_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_offset_rot_imm_1_) ) + "\n";
    all_zeros &= (0 == phv_32b_offset_rot_imm_1_);
    r += indent_string + "  " + std::string("phv_16b_offset_rot_imm_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_offset_rot_imm_0_) ) + "\n";
    all_zeros &= (0 == phv_16b_offset_rot_imm_0_);
    r += indent_string + "  " + std::string("phv_16b_offset_rot_imm_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_offset_rot_imm_1_) ) + "\n";
    all_zeros &= (0 == phv_16b_offset_rot_imm_1_);
    r += indent_string + "  " + std::string("phv_8b_offset_rot_imm_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_offset_rot_imm_0_) ) + "\n";
    all_zeros &= (0 == phv_8b_offset_rot_imm_0_);
    r += indent_string + "  " + std::string("phv_8b_offset_rot_imm_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_offset_rot_imm_1_) ) + "\n";
    all_zeros &= (0 == phv_8b_offset_rot_imm_1_);
    r += indent_string + "  " + std::string("phv_8b_offset_rot_imm_2") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_offset_rot_imm_2_) ) + "\n";
    all_zeros &= (0 == phv_8b_offset_rot_imm_2_);
    r += indent_string + "  " + std::string("phv_8b_offset_rot_imm_3") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_offset_rot_imm_3_) ) + "\n";
    all_zeros &= (0 == phv_8b_offset_rot_imm_3_);
    r += indent_string + "  " + std::string("phv_32b_dst_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_dst_0_) ) + "\n";
    all_zeros &= (0 == phv_32b_dst_0_);
    r += indent_string + "  " + std::string("phv_32b_dst_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_dst_1_) ) + "\n";
    all_zeros &= (0 == phv_32b_dst_1_);
    r += indent_string + "  " + std::string("phv_32b_dst_2") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_dst_2_) ) + "\n";
    all_zeros &= (0 == phv_32b_dst_2_);
    r += indent_string + "  " + std::string("phv_32b_dst_3") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_dst_3_) ) + "\n";
    all_zeros &= (0 == phv_32b_dst_3_);
    r += indent_string + "  " + std::string("phv_16b_dst_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_dst_0_) ) + "\n";
    all_zeros &= (0 == phv_16b_dst_0_);
    r += indent_string + "  " + std::string("phv_16b_dst_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_dst_1_) ) + "\n";
    all_zeros &= (0 == phv_16b_dst_1_);
    r += indent_string + "  " + std::string("phv_16b_dst_2") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_dst_2_) ) + "\n";
    all_zeros &= (0 == phv_16b_dst_2_);
    r += indent_string + "  " + std::string("phv_16b_dst_3") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_dst_3_) ) + "\n";
    all_zeros &= (0 == phv_16b_dst_3_);
    r += indent_string + "  " + std::string("phv_8b_dst_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_dst_0_) ) + "\n";
    all_zeros &= (0 == phv_8b_dst_0_);
    r += indent_string + "  " + std::string("phv_8b_dst_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_dst_1_) ) + "\n";
    all_zeros &= (0 == phv_8b_dst_1_);
    r += indent_string + "  " + std::string("phv_8b_dst_2") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_dst_2_) ) + "\n";
    all_zeros &= (0 == phv_8b_dst_2_);
    r += indent_string + "  " + std::string("phv_8b_dst_3") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_dst_3_) ) + "\n";
    all_zeros &= (0 == phv_8b_dst_3_);
    r += indent_string + "  " + std::string("phv_32b_src_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_src_0_) ) + "\n";
    all_zeros &= (0 == phv_32b_src_0_);
    r += indent_string + "  " + std::string("phv_32b_src_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_src_1_) ) + "\n";
    all_zeros &= (0 == phv_32b_src_1_);
    r += indent_string + "  " + std::string("phv_32b_src_2") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_src_2_) ) + "\n";
    all_zeros &= (0 == phv_32b_src_2_);
    r += indent_string + "  " + std::string("phv_32b_src_3") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_src_3_) ) + "\n";
    all_zeros &= (0 == phv_32b_src_3_);
    r += indent_string + "  " + std::string("phv_16b_src_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_src_0_) ) + "\n";
    all_zeros &= (0 == phv_16b_src_0_);
    r += indent_string + "  " + std::string("phv_16b_src_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_src_1_) ) + "\n";
    all_zeros &= (0 == phv_16b_src_1_);
    r += indent_string + "  " + std::string("phv_16b_src_2") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_src_2_) ) + "\n";
    all_zeros &= (0 == phv_16b_src_2_);
    r += indent_string + "  " + std::string("phv_16b_src_3") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_src_3_) ) + "\n";
    all_zeros &= (0 == phv_16b_src_3_);
    r += indent_string + "  " + std::string("phv_8b_src_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_src_0_) ) + "\n";
    all_zeros &= (0 == phv_8b_src_0_);
    r += indent_string + "  " + std::string("phv_8b_src_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_src_1_) ) + "\n";
    all_zeros &= (0 == phv_8b_src_1_);
    r += indent_string + "  " + std::string("phv_8b_src_2") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_src_2_) ) + "\n";
    all_zeros &= (0 == phv_8b_src_2_);
    r += indent_string + "  " + std::string("phv_8b_src_3") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_src_3_) ) + "\n";
    all_zeros &= (0 == phv_8b_src_3_);
    r += indent_string + "  " + std::string("phv_32b_src_type_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_src_type_0_) ) + "\n";
    all_zeros &= (0 == phv_32b_src_type_0_);
    r += indent_string + "  " + std::string("phv_32b_src_type_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_src_type_1_) ) + "\n";
    all_zeros &= (0 == phv_32b_src_type_1_);
    r += indent_string + "  " + std::string("phv_16b_src_type_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_src_type_0_) ) + "\n";
    all_zeros &= (0 == phv_16b_src_type_0_);
    r += indent_string + "  " + std::string("phv_16b_src_type_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_src_type_1_) ) + "\n";
    all_zeros &= (0 == phv_16b_src_type_1_);
    r += indent_string + "  " + std::string("phv_8b_src_type_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_src_type_0_) ) + "\n";
    all_zeros &= (0 == phv_8b_src_type_0_);
    r += indent_string + "  " + std::string("phv_8b_src_type_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_src_type_1_) ) + "\n";
    all_zeros &= (0 == phv_8b_src_type_1_);
    r += indent_string + "  " + std::string("phv_8b_src_type_2") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_src_type_2_) ) + "\n";
    all_zeros &= (0 == phv_8b_src_type_2_);
    r += indent_string + "  " + std::string("phv_8b_src_type_3") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_src_type_3_) ) + "\n";
    all_zeros &= (0 == phv_8b_src_type_3_);
    r += indent_string + "  " + std::string("pri_upd_type") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(pri_upd_type_) ) + "\n";
    all_zeros &= (0 == pri_upd_type_);
    r += indent_string + "  " + std::string("pri_upd_src") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(pri_upd_src_) ) + "\n";
    all_zeros &= (0 == pri_upd_src_);
    r += indent_string + "  " + std::string("pri_upd_en_shr") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(pri_upd_en_shr_) ) + "\n";
    all_zeros &= (0 == pri_upd_en_shr_);
    r += indent_string + "  " + std::string("pri_upd_val_mask") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(pri_upd_val_mask_) ) + "\n";
    all_zeros &= (0 == pri_upd_val_mask_);
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
    r += indent_string + std::string("PrsrPoActionRow") + ":\n";
    for (uint32_t f=0;f<2;++f) {
      r += indent_string + "  " + std::string("csum_addr") + "["+boost::lexical_cast<std::string>(f)+"]"+ ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(csum_addr_[f]) ) + "\n";
      all_zeros &= (0 == csum_addr_[f]);
    }
    for (uint32_t f=0;f<2;++f) {
      r += indent_string + "  " + std::string("csum_en") + "["+boost::lexical_cast<std::string>(f)+"]"+ ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(csum_en_[f]) ) + "\n";
      all_zeros &= (0 == csum_en_[f]);
    }
    r += indent_string + "  " + std::string("dst_offset_inc") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(dst_offset_inc_) ) + "\n";
    all_zeros &= (0 == dst_offset_inc_);
    r += indent_string + "  " + std::string("dst_offset_rst") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(dst_offset_rst_) ) + "\n";
    all_zeros &= (0 == dst_offset_rst_);
    r += indent_string + "  " + std::string("phv_32b_offset_add_dst_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_offset_add_dst_0_) ) + "\n";
    all_zeros &= (0 == phv_32b_offset_add_dst_0_);
    r += indent_string + "  " + std::string("phv_32b_offset_add_dst_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_offset_add_dst_1_) ) + "\n";
    all_zeros &= (0 == phv_32b_offset_add_dst_1_);
    r += indent_string + "  " + std::string("phv_32b_offset_add_dst_2") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_offset_add_dst_2_) ) + "\n";
    all_zeros &= (0 == phv_32b_offset_add_dst_2_);
    r += indent_string + "  " + std::string("phv_32b_offset_add_dst_3") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_offset_add_dst_3_) ) + "\n";
    all_zeros &= (0 == phv_32b_offset_add_dst_3_);
    r += indent_string + "  " + std::string("phv_16b_offset_add_dst_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_offset_add_dst_0_) ) + "\n";
    all_zeros &= (0 == phv_16b_offset_add_dst_0_);
    r += indent_string + "  " + std::string("phv_16b_offset_add_dst_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_offset_add_dst_1_) ) + "\n";
    all_zeros &= (0 == phv_16b_offset_add_dst_1_);
    r += indent_string + "  " + std::string("phv_16b_offset_add_dst_2") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_offset_add_dst_2_) ) + "\n";
    all_zeros &= (0 == phv_16b_offset_add_dst_2_);
    r += indent_string + "  " + std::string("phv_16b_offset_add_dst_3") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_offset_add_dst_3_) ) + "\n";
    all_zeros &= (0 == phv_16b_offset_add_dst_3_);
    r += indent_string + "  " + std::string("phv_8b_offset_add_dst_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_offset_add_dst_0_) ) + "\n";
    all_zeros &= (0 == phv_8b_offset_add_dst_0_);
    r += indent_string + "  " + std::string("phv_8b_offset_add_dst_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_offset_add_dst_1_) ) + "\n";
    all_zeros &= (0 == phv_8b_offset_add_dst_1_);
    r += indent_string + "  " + std::string("phv_8b_offset_add_dst_2") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_offset_add_dst_2_) ) + "\n";
    all_zeros &= (0 == phv_8b_offset_add_dst_2_);
    r += indent_string + "  " + std::string("phv_8b_offset_add_dst_3") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_offset_add_dst_3_) ) + "\n";
    all_zeros &= (0 == phv_8b_offset_add_dst_3_);
    r += indent_string + "  " + std::string("phv_32b_offset_rot_imm_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_offset_rot_imm_0_) ) + "\n";
    all_zeros &= (0 == phv_32b_offset_rot_imm_0_);
    r += indent_string + "  " + std::string("phv_32b_offset_rot_imm_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_offset_rot_imm_1_) ) + "\n";
    all_zeros &= (0 == phv_32b_offset_rot_imm_1_);
    r += indent_string + "  " + std::string("phv_16b_offset_rot_imm_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_offset_rot_imm_0_) ) + "\n";
    all_zeros &= (0 == phv_16b_offset_rot_imm_0_);
    r += indent_string + "  " + std::string("phv_16b_offset_rot_imm_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_offset_rot_imm_1_) ) + "\n";
    all_zeros &= (0 == phv_16b_offset_rot_imm_1_);
    r += indent_string + "  " + std::string("phv_8b_offset_rot_imm_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_offset_rot_imm_0_) ) + "\n";
    all_zeros &= (0 == phv_8b_offset_rot_imm_0_);
    r += indent_string + "  " + std::string("phv_8b_offset_rot_imm_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_offset_rot_imm_1_) ) + "\n";
    all_zeros &= (0 == phv_8b_offset_rot_imm_1_);
    r += indent_string + "  " + std::string("phv_8b_offset_rot_imm_2") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_offset_rot_imm_2_) ) + "\n";
    all_zeros &= (0 == phv_8b_offset_rot_imm_2_);
    r += indent_string + "  " + std::string("phv_8b_offset_rot_imm_3") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_offset_rot_imm_3_) ) + "\n";
    all_zeros &= (0 == phv_8b_offset_rot_imm_3_);
    r += indent_string + "  " + std::string("phv_32b_dst_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_dst_0_) ) + "\n";
    all_zeros &= (0 == phv_32b_dst_0_);
    r += indent_string + "  " + std::string("phv_32b_dst_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_dst_1_) ) + "\n";
    all_zeros &= (0 == phv_32b_dst_1_);
    r += indent_string + "  " + std::string("phv_32b_dst_2") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_dst_2_) ) + "\n";
    all_zeros &= (0 == phv_32b_dst_2_);
    r += indent_string + "  " + std::string("phv_32b_dst_3") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_dst_3_) ) + "\n";
    all_zeros &= (0 == phv_32b_dst_3_);
    r += indent_string + "  " + std::string("phv_16b_dst_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_dst_0_) ) + "\n";
    all_zeros &= (0 == phv_16b_dst_0_);
    r += indent_string + "  " + std::string("phv_16b_dst_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_dst_1_) ) + "\n";
    all_zeros &= (0 == phv_16b_dst_1_);
    r += indent_string + "  " + std::string("phv_16b_dst_2") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_dst_2_) ) + "\n";
    all_zeros &= (0 == phv_16b_dst_2_);
    r += indent_string + "  " + std::string("phv_16b_dst_3") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_dst_3_) ) + "\n";
    all_zeros &= (0 == phv_16b_dst_3_);
    r += indent_string + "  " + std::string("phv_8b_dst_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_dst_0_) ) + "\n";
    all_zeros &= (0 == phv_8b_dst_0_);
    r += indent_string + "  " + std::string("phv_8b_dst_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_dst_1_) ) + "\n";
    all_zeros &= (0 == phv_8b_dst_1_);
    r += indent_string + "  " + std::string("phv_8b_dst_2") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_dst_2_) ) + "\n";
    all_zeros &= (0 == phv_8b_dst_2_);
    r += indent_string + "  " + std::string("phv_8b_dst_3") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_dst_3_) ) + "\n";
    all_zeros &= (0 == phv_8b_dst_3_);
    r += indent_string + "  " + std::string("phv_32b_src_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_src_0_) ) + "\n";
    all_zeros &= (0 == phv_32b_src_0_);
    r += indent_string + "  " + std::string("phv_32b_src_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_src_1_) ) + "\n";
    all_zeros &= (0 == phv_32b_src_1_);
    r += indent_string + "  " + std::string("phv_32b_src_2") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_src_2_) ) + "\n";
    all_zeros &= (0 == phv_32b_src_2_);
    r += indent_string + "  " + std::string("phv_32b_src_3") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_src_3_) ) + "\n";
    all_zeros &= (0 == phv_32b_src_3_);
    r += indent_string + "  " + std::string("phv_16b_src_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_src_0_) ) + "\n";
    all_zeros &= (0 == phv_16b_src_0_);
    r += indent_string + "  " + std::string("phv_16b_src_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_src_1_) ) + "\n";
    all_zeros &= (0 == phv_16b_src_1_);
    r += indent_string + "  " + std::string("phv_16b_src_2") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_src_2_) ) + "\n";
    all_zeros &= (0 == phv_16b_src_2_);
    r += indent_string + "  " + std::string("phv_16b_src_3") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_src_3_) ) + "\n";
    all_zeros &= (0 == phv_16b_src_3_);
    r += indent_string + "  " + std::string("phv_8b_src_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_src_0_) ) + "\n";
    all_zeros &= (0 == phv_8b_src_0_);
    r += indent_string + "  " + std::string("phv_8b_src_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_src_1_) ) + "\n";
    all_zeros &= (0 == phv_8b_src_1_);
    r += indent_string + "  " + std::string("phv_8b_src_2") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_src_2_) ) + "\n";
    all_zeros &= (0 == phv_8b_src_2_);
    r += indent_string + "  " + std::string("phv_8b_src_3") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_src_3_) ) + "\n";
    all_zeros &= (0 == phv_8b_src_3_);
    r += indent_string + "  " + std::string("phv_32b_src_type_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_src_type_0_) ) + "\n";
    all_zeros &= (0 == phv_32b_src_type_0_);
    r += indent_string + "  " + std::string("phv_32b_src_type_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_src_type_1_) ) + "\n";
    all_zeros &= (0 == phv_32b_src_type_1_);
    r += indent_string + "  " + std::string("phv_16b_src_type_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_src_type_0_) ) + "\n";
    all_zeros &= (0 == phv_16b_src_type_0_);
    r += indent_string + "  " + std::string("phv_16b_src_type_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_src_type_1_) ) + "\n";
    all_zeros &= (0 == phv_16b_src_type_1_);
    r += indent_string + "  " + std::string("phv_8b_src_type_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_src_type_0_) ) + "\n";
    all_zeros &= (0 == phv_8b_src_type_0_);
    r += indent_string + "  " + std::string("phv_8b_src_type_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_src_type_1_) ) + "\n";
    all_zeros &= (0 == phv_8b_src_type_1_);
    r += indent_string + "  " + std::string("phv_8b_src_type_2") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_src_type_2_) ) + "\n";
    all_zeros &= (0 == phv_8b_src_type_2_);
    r += indent_string + "  " + std::string("phv_8b_src_type_3") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_src_type_3_) ) + "\n";
    all_zeros &= (0 == phv_8b_src_type_3_);
    r += indent_string + "  " + std::string("pri_upd_type") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(pri_upd_type_) ) + "\n";
    all_zeros &= (0 == pri_upd_type_);
    r += indent_string + "  " + std::string("pri_upd_src") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(pri_upd_src_) ) + "\n";
    all_zeros &= (0 == pri_upd_src_);
    r += indent_string + "  " + std::string("pri_upd_en_shr") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(pri_upd_en_shr_) ) + "\n";
    all_zeros &= (0 == pri_upd_en_shr_);
    r += indent_string + "  " + std::string("pri_upd_val_mask") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(pri_upd_val_mask_) ) + "\n";
    all_zeros &= (0 == pri_upd_val_mask_);
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  std::array< uint8_t, 2 > csum_addr_;
  std::array< uint8_t, 2 > csum_en_;
  uint8_t dst_offset_inc_;
  uint8_t dst_offset_rst_;
  uint8_t phv_32b_offset_add_dst_0_;
  uint8_t phv_32b_offset_add_dst_1_;
  uint8_t phv_32b_offset_add_dst_2_;
  uint8_t phv_32b_offset_add_dst_3_;
  uint8_t phv_16b_offset_add_dst_0_;
  uint8_t phv_16b_offset_add_dst_1_;
  uint8_t phv_16b_offset_add_dst_2_;
  uint8_t phv_16b_offset_add_dst_3_;
  uint8_t phv_8b_offset_add_dst_0_;
  uint8_t phv_8b_offset_add_dst_1_;
  uint8_t phv_8b_offset_add_dst_2_;
  uint8_t phv_8b_offset_add_dst_3_;
  uint8_t phv_32b_offset_rot_imm_0_;
  uint8_t phv_32b_offset_rot_imm_1_;
  uint8_t phv_16b_offset_rot_imm_0_;
  uint8_t phv_16b_offset_rot_imm_1_;
  uint8_t phv_8b_offset_rot_imm_0_;
  uint8_t phv_8b_offset_rot_imm_1_;
  uint8_t phv_8b_offset_rot_imm_2_;
  uint8_t phv_8b_offset_rot_imm_3_;
  uint16_t phv_32b_dst_0_;
  uint16_t phv_32b_dst_1_;
  uint16_t phv_32b_dst_2_;
  uint16_t phv_32b_dst_3_;
  uint16_t phv_16b_dst_0_;
  uint16_t phv_16b_dst_1_;
  uint16_t phv_16b_dst_2_;
  uint16_t phv_16b_dst_3_;
  uint16_t phv_8b_dst_0_;
  uint16_t phv_8b_dst_1_;
  uint16_t phv_8b_dst_2_;
  uint16_t phv_8b_dst_3_;
  uint8_t phv_32b_src_0_;
  uint8_t phv_32b_src_1_;
  uint8_t phv_32b_src_2_;
  uint8_t phv_32b_src_3_;
  uint8_t phv_16b_src_0_;
  uint8_t phv_16b_src_1_;
  uint8_t phv_16b_src_2_;
  uint8_t phv_16b_src_3_;
  uint8_t phv_8b_src_0_;
  uint8_t phv_8b_src_1_;
  uint8_t phv_8b_src_2_;
  uint8_t phv_8b_src_3_;
  uint8_t phv_32b_src_type_0_;
  uint8_t phv_32b_src_type_1_;
  uint8_t phv_16b_src_type_0_;
  uint8_t phv_16b_src_type_1_;
  uint8_t phv_8b_src_type_0_;
  uint8_t phv_8b_src_type_1_;
  uint8_t phv_8b_src_type_2_;
  uint8_t phv_8b_src_type_3_;
  uint8_t pri_upd_type_;
  uint8_t pri_upd_src_;
  uint8_t pri_upd_en_shr_;
  uint8_t pri_upd_val_mask_;
private:
  static uint64_t StartOffset(
      int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, int index_prsr_po_action_row
      ) {
    uint64_t offset=0;
    offset += 0x20000000000; // to get to pipes
    assert(index_pipe_addrmap < 4);
    offset += index_pipe_addrmap * 0x2000000000; // pipe_addrmap[]
    switch (selector_pipe_addrmap) {
      case kEPrsr:
        offset += 0x1c80040000; // to get to e_prsr
        assert(index_prsr_mem_main_rspec < 18);
        offset += index_prsr_mem_main_rspec * 0x2000; // prsr_mem_main_rspec[]
        offset += 0x400; // to get to po_action_row
        assert(index_prsr_po_action_row < 256);
        offset += index_prsr_po_action_row * 0x2; // prsr_po_action_row[]
        break;
      case kIPrsr:
        offset += 0x1c80000000; // to get to i_prsr
        assert(index_prsr_mem_main_rspec < 18);
        offset += index_prsr_mem_main_rspec * 0x2000; // prsr_mem_main_rspec[]
        offset += 0x400; // to get to po_action_row
        assert(index_prsr_po_action_row < 256);
        offset += index_prsr_po_action_row * 0x2; // prsr_po_action_row[]
        break;
      default:
        assert(0);
        break;
    }
    return offset;
  }

};








class PrsrPoActionRowMutable : public model_core::RegisterBlockIndirect<RegisterCallback> {
public:
  enum PipeAddrmapEnum {
    kEPrsr,
    kIPrsr
  };
public:
  PrsrPoActionRowMutable(
      int chipNumber, int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, int index_prsr_po_action_row, RegisterCallback& write_callback = 0, RegisterCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, index_prsr_po_action_row), 2, true, write_callback, read_callback, std::string("PrsrPoActionRowMutable")+":"+boost::lexical_cast<std::string>(index_pipe_addrmap) + "," + boost::lexical_cast<std::string>(selector_pipe_addrmap) + "," + boost::lexical_cast<std::string>(index_prsr_mem_main_rspec) + "," + boost::lexical_cast<std::string>(index_prsr_po_action_row))
    {
    }
  PrsrPoActionRowMutable(
      
  )
    : RegisterBlockIndirect(0, 0, 0, true, 0, 0, "PrsrPoActionRowMutable")
    {
    }
public:






  uint8_t csum_addr(int j0) { return csum_addr_[j0]; }
  void csum_addr(int j0,const uint8_t &v) { csum_addr_[j0]=v; }






  uint8_t csum_en(int j0) { return csum_en_[j0]; }
  void csum_en(int j0,const uint8_t &v) { csum_en_[j0]=v; }






  uint8_t dst_offset_inc() { return dst_offset_inc_; }
  void dst_offset_inc(const uint8_t &v) { dst_offset_inc_=v; }






  uint8_t dst_offset_rst() { return dst_offset_rst_; }
  void dst_offset_rst(const uint8_t &v) { dst_offset_rst_=v; }






  uint8_t phv_32b_offset_add_dst_0() { return phv_32b_offset_add_dst_0_; }
  void phv_32b_offset_add_dst_0(const uint8_t &v) { phv_32b_offset_add_dst_0_=v; }






  uint8_t phv_32b_offset_add_dst_1() { return phv_32b_offset_add_dst_1_; }
  void phv_32b_offset_add_dst_1(const uint8_t &v) { phv_32b_offset_add_dst_1_=v; }






  uint8_t phv_32b_offset_add_dst_2() { return phv_32b_offset_add_dst_2_; }
  void phv_32b_offset_add_dst_2(const uint8_t &v) { phv_32b_offset_add_dst_2_=v; }






  uint8_t phv_32b_offset_add_dst_3() { return phv_32b_offset_add_dst_3_; }
  void phv_32b_offset_add_dst_3(const uint8_t &v) { phv_32b_offset_add_dst_3_=v; }






  uint8_t phv_16b_offset_add_dst_0() { return phv_16b_offset_add_dst_0_; }
  void phv_16b_offset_add_dst_0(const uint8_t &v) { phv_16b_offset_add_dst_0_=v; }






  uint8_t phv_16b_offset_add_dst_1() { return phv_16b_offset_add_dst_1_; }
  void phv_16b_offset_add_dst_1(const uint8_t &v) { phv_16b_offset_add_dst_1_=v; }






  uint8_t phv_16b_offset_add_dst_2() { return phv_16b_offset_add_dst_2_; }
  void phv_16b_offset_add_dst_2(const uint8_t &v) { phv_16b_offset_add_dst_2_=v; }






  uint8_t phv_16b_offset_add_dst_3() { return phv_16b_offset_add_dst_3_; }
  void phv_16b_offset_add_dst_3(const uint8_t &v) { phv_16b_offset_add_dst_3_=v; }






  uint8_t phv_8b_offset_add_dst_0() { return phv_8b_offset_add_dst_0_; }
  void phv_8b_offset_add_dst_0(const uint8_t &v) { phv_8b_offset_add_dst_0_=v; }






  uint8_t phv_8b_offset_add_dst_1() { return phv_8b_offset_add_dst_1_; }
  void phv_8b_offset_add_dst_1(const uint8_t &v) { phv_8b_offset_add_dst_1_=v; }






  uint8_t phv_8b_offset_add_dst_2() { return phv_8b_offset_add_dst_2_; }
  void phv_8b_offset_add_dst_2(const uint8_t &v) { phv_8b_offset_add_dst_2_=v; }






  uint8_t phv_8b_offset_add_dst_3() { return phv_8b_offset_add_dst_3_; }
  void phv_8b_offset_add_dst_3(const uint8_t &v) { phv_8b_offset_add_dst_3_=v; }






  uint8_t phv_32b_offset_rot_imm_0() { return phv_32b_offset_rot_imm_0_; }
  void phv_32b_offset_rot_imm_0(const uint8_t &v) { phv_32b_offset_rot_imm_0_=v; }






  uint8_t phv_32b_offset_rot_imm_1() { return phv_32b_offset_rot_imm_1_; }
  void phv_32b_offset_rot_imm_1(const uint8_t &v) { phv_32b_offset_rot_imm_1_=v; }






  uint8_t phv_16b_offset_rot_imm_0() { return phv_16b_offset_rot_imm_0_; }
  void phv_16b_offset_rot_imm_0(const uint8_t &v) { phv_16b_offset_rot_imm_0_=v; }






  uint8_t phv_16b_offset_rot_imm_1() { return phv_16b_offset_rot_imm_1_; }
  void phv_16b_offset_rot_imm_1(const uint8_t &v) { phv_16b_offset_rot_imm_1_=v; }






  uint8_t phv_8b_offset_rot_imm_0() { return phv_8b_offset_rot_imm_0_; }
  void phv_8b_offset_rot_imm_0(const uint8_t &v) { phv_8b_offset_rot_imm_0_=v; }






  uint8_t phv_8b_offset_rot_imm_1() { return phv_8b_offset_rot_imm_1_; }
  void phv_8b_offset_rot_imm_1(const uint8_t &v) { phv_8b_offset_rot_imm_1_=v; }






  uint8_t phv_8b_offset_rot_imm_2() { return phv_8b_offset_rot_imm_2_; }
  void phv_8b_offset_rot_imm_2(const uint8_t &v) { phv_8b_offset_rot_imm_2_=v; }






  uint8_t phv_8b_offset_rot_imm_3() { return phv_8b_offset_rot_imm_3_; }
  void phv_8b_offset_rot_imm_3(const uint8_t &v) { phv_8b_offset_rot_imm_3_=v; }





  uint16_t phv_32b_dst_0() { return phv_32b_dst_0_; }
  void phv_32b_dst_0(const uint16_t &v) { phv_32b_dst_0_=v; }





  uint16_t phv_32b_dst_1() { return phv_32b_dst_1_; }
  void phv_32b_dst_1(const uint16_t &v) { phv_32b_dst_1_=v; }





  uint16_t phv_32b_dst_2() { return phv_32b_dst_2_; }
  void phv_32b_dst_2(const uint16_t &v) { phv_32b_dst_2_=v; }





  uint16_t phv_32b_dst_3() { return phv_32b_dst_3_; }
  void phv_32b_dst_3(const uint16_t &v) { phv_32b_dst_3_=v; }





  uint16_t phv_16b_dst_0() { return phv_16b_dst_0_; }
  void phv_16b_dst_0(const uint16_t &v) { phv_16b_dst_0_=v; }





  uint16_t phv_16b_dst_1() { return phv_16b_dst_1_; }
  void phv_16b_dst_1(const uint16_t &v) { phv_16b_dst_1_=v; }





  uint16_t phv_16b_dst_2() { return phv_16b_dst_2_; }
  void phv_16b_dst_2(const uint16_t &v) { phv_16b_dst_2_=v; }





  uint16_t phv_16b_dst_3() { return phv_16b_dst_3_; }
  void phv_16b_dst_3(const uint16_t &v) { phv_16b_dst_3_=v; }





  uint16_t phv_8b_dst_0() { return phv_8b_dst_0_; }
  void phv_8b_dst_0(const uint16_t &v) { phv_8b_dst_0_=v; }





  uint16_t phv_8b_dst_1() { return phv_8b_dst_1_; }
  void phv_8b_dst_1(const uint16_t &v) { phv_8b_dst_1_=v; }





  uint16_t phv_8b_dst_2() { return phv_8b_dst_2_; }
  void phv_8b_dst_2(const uint16_t &v) { phv_8b_dst_2_=v; }





  uint16_t phv_8b_dst_3() { return phv_8b_dst_3_; }
  void phv_8b_dst_3(const uint16_t &v) { phv_8b_dst_3_=v; }






  uint8_t phv_32b_src_0() { return phv_32b_src_0_; }
  void phv_32b_src_0(const uint8_t &v) { phv_32b_src_0_=v; }






  uint8_t phv_32b_src_1() { return phv_32b_src_1_; }
  void phv_32b_src_1(const uint8_t &v) { phv_32b_src_1_=v; }






  uint8_t phv_32b_src_2() { return phv_32b_src_2_; }
  void phv_32b_src_2(const uint8_t &v) { phv_32b_src_2_=v; }






  uint8_t phv_32b_src_3() { return phv_32b_src_3_; }
  void phv_32b_src_3(const uint8_t &v) { phv_32b_src_3_=v; }






  uint8_t phv_16b_src_0() { return phv_16b_src_0_; }
  void phv_16b_src_0(const uint8_t &v) { phv_16b_src_0_=v; }






  uint8_t phv_16b_src_1() { return phv_16b_src_1_; }
  void phv_16b_src_1(const uint8_t &v) { phv_16b_src_1_=v; }






  uint8_t phv_16b_src_2() { return phv_16b_src_2_; }
  void phv_16b_src_2(const uint8_t &v) { phv_16b_src_2_=v; }






  uint8_t phv_16b_src_3() { return phv_16b_src_3_; }
  void phv_16b_src_3(const uint8_t &v) { phv_16b_src_3_=v; }






  uint8_t phv_8b_src_0() { return phv_8b_src_0_; }
  void phv_8b_src_0(const uint8_t &v) { phv_8b_src_0_=v; }






  uint8_t phv_8b_src_1() { return phv_8b_src_1_; }
  void phv_8b_src_1(const uint8_t &v) { phv_8b_src_1_=v; }






  uint8_t phv_8b_src_2() { return phv_8b_src_2_; }
  void phv_8b_src_2(const uint8_t &v) { phv_8b_src_2_=v; }






  uint8_t phv_8b_src_3() { return phv_8b_src_3_; }
  void phv_8b_src_3(const uint8_t &v) { phv_8b_src_3_=v; }





  uint8_t phv_32b_src_type_0() { return phv_32b_src_type_0_; }
  void phv_32b_src_type_0(const uint8_t &v) { phv_32b_src_type_0_=v; }





  uint8_t phv_32b_src_type_1() { return phv_32b_src_type_1_; }
  void phv_32b_src_type_1(const uint8_t &v) { phv_32b_src_type_1_=v; }





  uint8_t phv_16b_src_type_0() { return phv_16b_src_type_0_; }
  void phv_16b_src_type_0(const uint8_t &v) { phv_16b_src_type_0_=v; }





  uint8_t phv_16b_src_type_1() { return phv_16b_src_type_1_; }
  void phv_16b_src_type_1(const uint8_t &v) { phv_16b_src_type_1_=v; }





  uint8_t phv_8b_src_type_0() { return phv_8b_src_type_0_; }
  void phv_8b_src_type_0(const uint8_t &v) { phv_8b_src_type_0_=v; }





  uint8_t phv_8b_src_type_1() { return phv_8b_src_type_1_; }
  void phv_8b_src_type_1(const uint8_t &v) { phv_8b_src_type_1_=v; }





  uint8_t phv_8b_src_type_2() { return phv_8b_src_type_2_; }
  void phv_8b_src_type_2(const uint8_t &v) { phv_8b_src_type_2_=v; }





  uint8_t phv_8b_src_type_3() { return phv_8b_src_type_3_; }
  void phv_8b_src_type_3(const uint8_t &v) { phv_8b_src_type_3_=v; }






  uint8_t pri_upd_type() { return pri_upd_type_; }
  void pri_upd_type(const uint8_t &v) { pri_upd_type_=v; }






  uint8_t pri_upd_src() { return pri_upd_src_; }
  void pri_upd_src(const uint8_t &v) { pri_upd_src_=v; }







  uint8_t pri_upd_en_shr() { return pri_upd_en_shr_; }
  void pri_upd_en_shr(const uint8_t &v) { pri_upd_en_shr_=v; }






  uint8_t pri_upd_val_mask() { return pri_upd_val_mask_; }
  void pri_upd_val_mask(const uint8_t &v) { pri_upd_val_mask_=v; }

  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    if (offset < 0x1) {
      if (read_callback_) read_callback_();
      *data0 = (static_cast<uint64_t>(csum_addr_[0]) & 0x1f);
      *data0 |= ((static_cast<uint64_t>(csum_addr_[1]) & 0x1f) << 5);
      *data0 |= ((static_cast<uint64_t>(csum_en_[0]) & 0x1) << 10);
      *data0 |= ((static_cast<uint64_t>(csum_en_[1]) & 0x1) << 11);
      *data0 |= ((static_cast<uint64_t>(dst_offset_inc_) & 0x1f) << 12);
      *data0 |= ((static_cast<uint64_t>(dst_offset_rst_) & 0x1) << 17);
      *data0 |= ((static_cast<uint64_t>(phv_32b_offset_add_dst_0_) & 0x1) << 18);
      *data0 |= ((static_cast<uint64_t>(phv_32b_offset_add_dst_1_) & 0x1) << 19);
      *data0 |= ((static_cast<uint64_t>(phv_32b_offset_add_dst_2_) & 0x1) << 20);
      *data0 |= ((static_cast<uint64_t>(phv_32b_offset_add_dst_3_) & 0x1) << 21);
      *data0 |= ((static_cast<uint64_t>(phv_16b_offset_add_dst_0_) & 0x1) << 22);
      *data0 |= ((static_cast<uint64_t>(phv_16b_offset_add_dst_1_) & 0x1) << 23);
      *data0 |= ((static_cast<uint64_t>(phv_16b_offset_add_dst_2_) & 0x1) << 24);
      *data0 |= ((static_cast<uint64_t>(phv_16b_offset_add_dst_3_) & 0x1) << 25);
      *data0 |= ((static_cast<uint64_t>(phv_8b_offset_add_dst_0_) & 0x1) << 26);
      *data0 |= ((static_cast<uint64_t>(phv_8b_offset_add_dst_1_) & 0x1) << 27);
      *data0 |= ((static_cast<uint64_t>(phv_8b_offset_add_dst_2_) & 0x1) << 28);
      *data0 |= ((static_cast<uint64_t>(phv_8b_offset_add_dst_3_) & 0x1) << 29);
      *data0 |= ((static_cast<uint64_t>(phv_32b_offset_rot_imm_0_) & 0x1) << 30);
      *data0 |= ((static_cast<uint64_t>(phv_32b_offset_rot_imm_1_) & 0x1) << 31);
      *data0 |= ((static_cast<uint64_t>(phv_16b_offset_rot_imm_0_) & 0x1) << 32);
      *data0 |= ((static_cast<uint64_t>(phv_16b_offset_rot_imm_1_) & 0x1) << 33);
      *data0 |= ((static_cast<uint64_t>(phv_8b_offset_rot_imm_0_) & 0x1) << 34);
      *data0 |= ((static_cast<uint64_t>(phv_8b_offset_rot_imm_1_) & 0x1) << 35);
      *data0 |= ((static_cast<uint64_t>(phv_8b_offset_rot_imm_2_) & 0x1) << 36);
      *data0 |= ((static_cast<uint64_t>(phv_8b_offset_rot_imm_3_) & 0x1) << 37);
      *data0 |= ((static_cast<uint64_t>(phv_32b_dst_0_) & 0x1ff) << 38);
      *data0 |= ((static_cast<uint64_t>(phv_32b_dst_1_) & 0x1ff) << 47);
      *data0 |= ((static_cast<uint64_t>(phv_32b_dst_2_) & 0xff) << 56);
      *data1 = ((static_cast<uint64_t>(phv_32b_dst_2_) & 0x100) >> 8);
      *data1 |= ((static_cast<uint64_t>(phv_32b_dst_3_) & 0x1ff) << 1);
      *data1 |= ((static_cast<uint64_t>(phv_16b_dst_0_) & 0x1ff) << 10);
      *data1 |= ((static_cast<uint64_t>(phv_16b_dst_1_) & 0x1ff) << 19);
      *data1 |= ((static_cast<uint64_t>(phv_16b_dst_2_) & 0x1ff) << 28);
      *data1 |= ((static_cast<uint64_t>(phv_16b_dst_3_) & 0x1ff) << 37);
      *data1 |= ((static_cast<uint64_t>(phv_8b_dst_0_) & 0x1ff) << 46);
      *data1 |= ((static_cast<uint64_t>(phv_8b_dst_1_) & 0x1ff) << 55);
    }
    else if (offset >= 0x1 && offset < 0x2) {
      if (read_callback_) read_callback_();
      *data0 = (static_cast<uint64_t>(phv_8b_dst_2_) & 0x1ff);
      *data0 |= ((static_cast<uint64_t>(phv_8b_dst_3_) & 0x1ff) << 9);
      *data0 |= (static_cast<uint64_t>(phv_32b_src_0_) << 18);
      *data0 |= (static_cast<uint64_t>(phv_32b_src_1_) << 26);
      *data0 |= ((static_cast<uint64_t>(phv_32b_src_2_) & 0x3f) << 34);
      *data0 |= ((static_cast<uint64_t>(phv_32b_src_3_) & 0x3f) << 40);
      *data0 |= (static_cast<uint64_t>(phv_16b_src_0_) << 46);
      *data0 |= (static_cast<uint64_t>(phv_16b_src_1_) << 54);
      *data0 |= ((static_cast<uint64_t>(phv_16b_src_2_) & 0x3) << 62);
      *data1 = ((static_cast<uint64_t>(phv_16b_src_2_) & 0x3c) >> 2);
      *data1 |= ((static_cast<uint64_t>(phv_16b_src_3_) & 0x3f) << 4);
      *data1 |= (static_cast<uint64_t>(phv_8b_src_0_) << 10);
      *data1 |= (static_cast<uint64_t>(phv_8b_src_1_) << 18);
      *data1 |= (static_cast<uint64_t>(phv_8b_src_2_) << 26);
      *data1 |= (static_cast<uint64_t>(phv_8b_src_3_) << 34);
      *data1 |= ((static_cast<uint64_t>(phv_32b_src_type_0_) & 0x1) << 42);
      *data1 |= ((static_cast<uint64_t>(phv_32b_src_type_1_) & 0x1) << 43);
      *data1 |= ((static_cast<uint64_t>(phv_16b_src_type_0_) & 0x1) << 44);
      *data1 |= ((static_cast<uint64_t>(phv_16b_src_type_1_) & 0x1) << 45);
      *data1 |= ((static_cast<uint64_t>(phv_8b_src_type_0_) & 0x1) << 46);
      *data1 |= ((static_cast<uint64_t>(phv_8b_src_type_1_) & 0x1) << 47);
      *data1 |= ((static_cast<uint64_t>(phv_8b_src_type_2_) & 0x1) << 48);
      *data1 |= ((static_cast<uint64_t>(phv_8b_src_type_3_) & 0x1) << 49);
      *data1 |= ((static_cast<uint64_t>(pri_upd_type_) & 0x1) << 50);
      *data1 |= ((static_cast<uint64_t>(pri_upd_src_) & 0x1f) << 51);
      *data1 |= ((static_cast<uint64_t>(pri_upd_en_shr_) & 0xf) << 56);
      *data1 |= ((static_cast<uint64_t>(pri_upd_val_mask_) & 0x7) << 60);
    }
    else {
      assert(0);
    }
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    if (offset < 0x1) {
      csum_addr_[0] = (data0 & 0x1f);
      csum_addr_[1] = ((data0 >> 5) & 0x1f);
      csum_en_[0] = ((data0 >> 10) & 0x1);
      csum_en_[1] = ((data0 >> 11) & 0x1);
      dst_offset_inc_ = ((data0 >> 12) & 0x1f);
      dst_offset_rst_ = ((data0 >> 17) & 0x1);
      phv_32b_offset_add_dst_0_ = ((data0 >> 18) & 0x1);
      phv_32b_offset_add_dst_1_ = ((data0 >> 19) & 0x1);
      phv_32b_offset_add_dst_2_ = ((data0 >> 20) & 0x1);
      phv_32b_offset_add_dst_3_ = ((data0 >> 21) & 0x1);
      phv_16b_offset_add_dst_0_ = ((data0 >> 22) & 0x1);
      phv_16b_offset_add_dst_1_ = ((data0 >> 23) & 0x1);
      phv_16b_offset_add_dst_2_ = ((data0 >> 24) & 0x1);
      phv_16b_offset_add_dst_3_ = ((data0 >> 25) & 0x1);
      phv_8b_offset_add_dst_0_ = ((data0 >> 26) & 0x1);
      phv_8b_offset_add_dst_1_ = ((data0 >> 27) & 0x1);
      phv_8b_offset_add_dst_2_ = ((data0 >> 28) & 0x1);
      phv_8b_offset_add_dst_3_ = ((data0 >> 29) & 0x1);
      phv_32b_offset_rot_imm_0_ = ((data0 >> 30) & 0x1);
      phv_32b_offset_rot_imm_1_ = ((data0 >> 31) & 0x1);
      phv_16b_offset_rot_imm_0_ = ((data0 >> 32) & 0x1);
      phv_16b_offset_rot_imm_1_ = ((data0 >> 33) & 0x1);
      phv_8b_offset_rot_imm_0_ = ((data0 >> 34) & 0x1);
      phv_8b_offset_rot_imm_1_ = ((data0 >> 35) & 0x1);
      phv_8b_offset_rot_imm_2_ = ((data0 >> 36) & 0x1);
      phv_8b_offset_rot_imm_3_ = ((data0 >> 37) & 0x1);
      phv_32b_dst_0_ = ((data0 >> 38) & 0x1ff);
      phv_32b_dst_1_ = ((data0 >> 47) & 0x1ff);
      phv_32b_dst_2_ = (((data0 >> 56) & 0xff) | (phv_32b_dst_2_ & 0x100));
      phv_32b_dst_2_ = (((data1 << 8) & 0x100) | (phv_32b_dst_2_ & 0xff));
      phv_32b_dst_3_ = ((data1 >> 1) & 0x1ff);
      phv_16b_dst_0_ = ((data1 >> 10) & 0x1ff);
      phv_16b_dst_1_ = ((data1 >> 19) & 0x1ff);
      phv_16b_dst_2_ = ((data1 >> 28) & 0x1ff);
      phv_16b_dst_3_ = ((data1 >> 37) & 0x1ff);
      phv_8b_dst_0_ = ((data1 >> 46) & 0x1ff);
      phv_8b_dst_1_ = ((data1 >> 55) & 0x1ff);
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x1 && offset < 0x2) {
      phv_8b_dst_2_ = (data0 & 0x1ff);
      phv_8b_dst_3_ = ((data0 >> 9) & 0x1ff);
      phv_32b_src_0_ = (data0 >> 18);
      phv_32b_src_1_ = (data0 >> 26);
      phv_32b_src_2_ = ((data0 >> 34) & 0x3f);
      phv_32b_src_3_ = ((data0 >> 40) & 0x3f);
      phv_16b_src_0_ = (data0 >> 46);
      phv_16b_src_1_ = (data0 >> 54);
      phv_16b_src_2_ = (((data0 >> 62) & 0x3) | (phv_16b_src_2_ & 0x3c));
      phv_16b_src_2_ = (((data1 << 2) & 0x3c) | (phv_16b_src_2_ & 0x3));
      phv_16b_src_3_ = ((data1 >> 4) & 0x3f);
      phv_8b_src_0_ = (data1 >> 10);
      phv_8b_src_1_ = (data1 >> 18);
      phv_8b_src_2_ = (data1 >> 26);
      phv_8b_src_3_ = (data1 >> 34);
      phv_32b_src_type_0_ = ((data1 >> 42) & 0x1);
      phv_32b_src_type_1_ = ((data1 >> 43) & 0x1);
      phv_16b_src_type_0_ = ((data1 >> 44) & 0x1);
      phv_16b_src_type_1_ = ((data1 >> 45) & 0x1);
      phv_8b_src_type_0_ = ((data1 >> 46) & 0x1);
      phv_8b_src_type_1_ = ((data1 >> 47) & 0x1);
      phv_8b_src_type_2_ = ((data1 >> 48) & 0x1);
      phv_8b_src_type_3_ = ((data1 >> 49) & 0x1);
      pri_upd_type_ = ((data1 >> 50) & 0x1);
      pri_upd_src_ = ((data1 >> 51) & 0x1f);
      pri_upd_en_shr_ = ((data1 >> 56) & 0xf);
      pri_upd_val_mask_ = ((data1 >> 60) & 0x7);
      if (write_callback_) write_callback_();
    }
    else {
      assert(0);
    }
    return true;
  }

  void reset(
      
      ) {
    csum_addr_[0] = 0;
    csum_addr_[1] = 0;
    csum_en_[0] = 0;
    csum_en_[1] = 0;
    dst_offset_inc_ = 0;
    dst_offset_rst_ = 0;
    phv_32b_offset_add_dst_0_ = 0;
    phv_32b_offset_add_dst_1_ = 0;
    phv_32b_offset_add_dst_2_ = 0;
    phv_32b_offset_add_dst_3_ = 0;
    phv_16b_offset_add_dst_0_ = 0;
    phv_16b_offset_add_dst_1_ = 0;
    phv_16b_offset_add_dst_2_ = 0;
    phv_16b_offset_add_dst_3_ = 0;
    phv_8b_offset_add_dst_0_ = 0;
    phv_8b_offset_add_dst_1_ = 0;
    phv_8b_offset_add_dst_2_ = 0;
    phv_8b_offset_add_dst_3_ = 0;
    phv_32b_offset_rot_imm_0_ = 0;
    phv_32b_offset_rot_imm_1_ = 0;
    phv_16b_offset_rot_imm_0_ = 0;
    phv_16b_offset_rot_imm_1_ = 0;
    phv_8b_offset_rot_imm_0_ = 0;
    phv_8b_offset_rot_imm_1_ = 0;
    phv_8b_offset_rot_imm_2_ = 0;
    phv_8b_offset_rot_imm_3_ = 0;
    phv_32b_dst_0_ = 0;
    phv_32b_dst_1_ = 0;
    phv_32b_dst_2_ = 0;
    phv_32b_dst_3_ = 0;
    phv_16b_dst_0_ = 0;
    phv_16b_dst_1_ = 0;
    phv_16b_dst_2_ = 0;
    phv_16b_dst_3_ = 0;
    phv_8b_dst_0_ = 0;
    phv_8b_dst_1_ = 0;
    phv_8b_dst_2_ = 0;
    phv_8b_dst_3_ = 0;
    phv_32b_src_0_ = 0;
    phv_32b_src_1_ = 0;
    phv_32b_src_2_ = 0;
    phv_32b_src_3_ = 0;
    phv_16b_src_0_ = 0;
    phv_16b_src_1_ = 0;
    phv_16b_src_2_ = 0;
    phv_16b_src_3_ = 0;
    phv_8b_src_0_ = 0;
    phv_8b_src_1_ = 0;
    phv_8b_src_2_ = 0;
    phv_8b_src_3_ = 0;
    phv_32b_src_type_0_ = 0;
    phv_32b_src_type_1_ = 0;
    phv_16b_src_type_0_ = 0;
    phv_16b_src_type_1_ = 0;
    phv_8b_src_type_0_ = 0;
    phv_8b_src_type_1_ = 0;
    phv_8b_src_type_2_ = 0;
    phv_8b_src_type_3_ = 0;
    pri_upd_type_ = 0;
    pri_upd_src_ = 0;
    pri_upd_en_shr_ = 0;
    pri_upd_val_mask_ = 0;
    if (write_callback_) write_callback_();
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    r += indent_string + std::string("PrsrPoActionRowMutable") + ":\n";
    for (uint32_t f=0;f<2;++f) {
      r += indent_string + "  " + std::string("csum_addr") + "["+boost::lexical_cast<std::string>(f)+"]"+ ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(csum_addr_[f]) ) + "\n";
      all_zeros &= (0 == csum_addr_[f]);
    }
    for (uint32_t f=0;f<2;++f) {
      r += indent_string + "  " + std::string("csum_en") + "["+boost::lexical_cast<std::string>(f)+"]"+ ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(csum_en_[f]) ) + "\n";
      all_zeros &= (0 == csum_en_[f]);
    }
    r += indent_string + "  " + std::string("dst_offset_inc") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(dst_offset_inc_) ) + "\n";
    all_zeros &= (0 == dst_offset_inc_);
    r += indent_string + "  " + std::string("dst_offset_rst") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(dst_offset_rst_) ) + "\n";
    all_zeros &= (0 == dst_offset_rst_);
    r += indent_string + "  " + std::string("phv_32b_offset_add_dst_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_offset_add_dst_0_) ) + "\n";
    all_zeros &= (0 == phv_32b_offset_add_dst_0_);
    r += indent_string + "  " + std::string("phv_32b_offset_add_dst_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_offset_add_dst_1_) ) + "\n";
    all_zeros &= (0 == phv_32b_offset_add_dst_1_);
    r += indent_string + "  " + std::string("phv_32b_offset_add_dst_2") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_offset_add_dst_2_) ) + "\n";
    all_zeros &= (0 == phv_32b_offset_add_dst_2_);
    r += indent_string + "  " + std::string("phv_32b_offset_add_dst_3") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_offset_add_dst_3_) ) + "\n";
    all_zeros &= (0 == phv_32b_offset_add_dst_3_);
    r += indent_string + "  " + std::string("phv_16b_offset_add_dst_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_offset_add_dst_0_) ) + "\n";
    all_zeros &= (0 == phv_16b_offset_add_dst_0_);
    r += indent_string + "  " + std::string("phv_16b_offset_add_dst_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_offset_add_dst_1_) ) + "\n";
    all_zeros &= (0 == phv_16b_offset_add_dst_1_);
    r += indent_string + "  " + std::string("phv_16b_offset_add_dst_2") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_offset_add_dst_2_) ) + "\n";
    all_zeros &= (0 == phv_16b_offset_add_dst_2_);
    r += indent_string + "  " + std::string("phv_16b_offset_add_dst_3") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_offset_add_dst_3_) ) + "\n";
    all_zeros &= (0 == phv_16b_offset_add_dst_3_);
    r += indent_string + "  " + std::string("phv_8b_offset_add_dst_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_offset_add_dst_0_) ) + "\n";
    all_zeros &= (0 == phv_8b_offset_add_dst_0_);
    r += indent_string + "  " + std::string("phv_8b_offset_add_dst_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_offset_add_dst_1_) ) + "\n";
    all_zeros &= (0 == phv_8b_offset_add_dst_1_);
    r += indent_string + "  " + std::string("phv_8b_offset_add_dst_2") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_offset_add_dst_2_) ) + "\n";
    all_zeros &= (0 == phv_8b_offset_add_dst_2_);
    r += indent_string + "  " + std::string("phv_8b_offset_add_dst_3") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_offset_add_dst_3_) ) + "\n";
    all_zeros &= (0 == phv_8b_offset_add_dst_3_);
    r += indent_string + "  " + std::string("phv_32b_offset_rot_imm_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_offset_rot_imm_0_) ) + "\n";
    all_zeros &= (0 == phv_32b_offset_rot_imm_0_);
    r += indent_string + "  " + std::string("phv_32b_offset_rot_imm_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_offset_rot_imm_1_) ) + "\n";
    all_zeros &= (0 == phv_32b_offset_rot_imm_1_);
    r += indent_string + "  " + std::string("phv_16b_offset_rot_imm_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_offset_rot_imm_0_) ) + "\n";
    all_zeros &= (0 == phv_16b_offset_rot_imm_0_);
    r += indent_string + "  " + std::string("phv_16b_offset_rot_imm_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_offset_rot_imm_1_) ) + "\n";
    all_zeros &= (0 == phv_16b_offset_rot_imm_1_);
    r += indent_string + "  " + std::string("phv_8b_offset_rot_imm_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_offset_rot_imm_0_) ) + "\n";
    all_zeros &= (0 == phv_8b_offset_rot_imm_0_);
    r += indent_string + "  " + std::string("phv_8b_offset_rot_imm_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_offset_rot_imm_1_) ) + "\n";
    all_zeros &= (0 == phv_8b_offset_rot_imm_1_);
    r += indent_string + "  " + std::string("phv_8b_offset_rot_imm_2") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_offset_rot_imm_2_) ) + "\n";
    all_zeros &= (0 == phv_8b_offset_rot_imm_2_);
    r += indent_string + "  " + std::string("phv_8b_offset_rot_imm_3") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_offset_rot_imm_3_) ) + "\n";
    all_zeros &= (0 == phv_8b_offset_rot_imm_3_);
    r += indent_string + "  " + std::string("phv_32b_dst_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_dst_0_) ) + "\n";
    all_zeros &= (0 == phv_32b_dst_0_);
    r += indent_string + "  " + std::string("phv_32b_dst_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_dst_1_) ) + "\n";
    all_zeros &= (0 == phv_32b_dst_1_);
    r += indent_string + "  " + std::string("phv_32b_dst_2") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_dst_2_) ) + "\n";
    all_zeros &= (0 == phv_32b_dst_2_);
    r += indent_string + "  " + std::string("phv_32b_dst_3") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_dst_3_) ) + "\n";
    all_zeros &= (0 == phv_32b_dst_3_);
    r += indent_string + "  " + std::string("phv_16b_dst_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_dst_0_) ) + "\n";
    all_zeros &= (0 == phv_16b_dst_0_);
    r += indent_string + "  " + std::string("phv_16b_dst_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_dst_1_) ) + "\n";
    all_zeros &= (0 == phv_16b_dst_1_);
    r += indent_string + "  " + std::string("phv_16b_dst_2") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_dst_2_) ) + "\n";
    all_zeros &= (0 == phv_16b_dst_2_);
    r += indent_string + "  " + std::string("phv_16b_dst_3") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_dst_3_) ) + "\n";
    all_zeros &= (0 == phv_16b_dst_3_);
    r += indent_string + "  " + std::string("phv_8b_dst_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_dst_0_) ) + "\n";
    all_zeros &= (0 == phv_8b_dst_0_);
    r += indent_string + "  " + std::string("phv_8b_dst_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_dst_1_) ) + "\n";
    all_zeros &= (0 == phv_8b_dst_1_);
    r += indent_string + "  " + std::string("phv_8b_dst_2") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_dst_2_) ) + "\n";
    all_zeros &= (0 == phv_8b_dst_2_);
    r += indent_string + "  " + std::string("phv_8b_dst_3") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_dst_3_) ) + "\n";
    all_zeros &= (0 == phv_8b_dst_3_);
    r += indent_string + "  " + std::string("phv_32b_src_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_src_0_) ) + "\n";
    all_zeros &= (0 == phv_32b_src_0_);
    r += indent_string + "  " + std::string("phv_32b_src_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_src_1_) ) + "\n";
    all_zeros &= (0 == phv_32b_src_1_);
    r += indent_string + "  " + std::string("phv_32b_src_2") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_src_2_) ) + "\n";
    all_zeros &= (0 == phv_32b_src_2_);
    r += indent_string + "  " + std::string("phv_32b_src_3") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_src_3_) ) + "\n";
    all_zeros &= (0 == phv_32b_src_3_);
    r += indent_string + "  " + std::string("phv_16b_src_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_src_0_) ) + "\n";
    all_zeros &= (0 == phv_16b_src_0_);
    r += indent_string + "  " + std::string("phv_16b_src_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_src_1_) ) + "\n";
    all_zeros &= (0 == phv_16b_src_1_);
    r += indent_string + "  " + std::string("phv_16b_src_2") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_src_2_) ) + "\n";
    all_zeros &= (0 == phv_16b_src_2_);
    r += indent_string + "  " + std::string("phv_16b_src_3") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_src_3_) ) + "\n";
    all_zeros &= (0 == phv_16b_src_3_);
    r += indent_string + "  " + std::string("phv_8b_src_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_src_0_) ) + "\n";
    all_zeros &= (0 == phv_8b_src_0_);
    r += indent_string + "  " + std::string("phv_8b_src_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_src_1_) ) + "\n";
    all_zeros &= (0 == phv_8b_src_1_);
    r += indent_string + "  " + std::string("phv_8b_src_2") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_src_2_) ) + "\n";
    all_zeros &= (0 == phv_8b_src_2_);
    r += indent_string + "  " + std::string("phv_8b_src_3") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_src_3_) ) + "\n";
    all_zeros &= (0 == phv_8b_src_3_);
    r += indent_string + "  " + std::string("phv_32b_src_type_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_src_type_0_) ) + "\n";
    all_zeros &= (0 == phv_32b_src_type_0_);
    r += indent_string + "  " + std::string("phv_32b_src_type_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_src_type_1_) ) + "\n";
    all_zeros &= (0 == phv_32b_src_type_1_);
    r += indent_string + "  " + std::string("phv_16b_src_type_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_src_type_0_) ) + "\n";
    all_zeros &= (0 == phv_16b_src_type_0_);
    r += indent_string + "  " + std::string("phv_16b_src_type_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_src_type_1_) ) + "\n";
    all_zeros &= (0 == phv_16b_src_type_1_);
    r += indent_string + "  " + std::string("phv_8b_src_type_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_src_type_0_) ) + "\n";
    all_zeros &= (0 == phv_8b_src_type_0_);
    r += indent_string + "  " + std::string("phv_8b_src_type_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_src_type_1_) ) + "\n";
    all_zeros &= (0 == phv_8b_src_type_1_);
    r += indent_string + "  " + std::string("phv_8b_src_type_2") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_src_type_2_) ) + "\n";
    all_zeros &= (0 == phv_8b_src_type_2_);
    r += indent_string + "  " + std::string("phv_8b_src_type_3") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_src_type_3_) ) + "\n";
    all_zeros &= (0 == phv_8b_src_type_3_);
    r += indent_string + "  " + std::string("pri_upd_type") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(pri_upd_type_) ) + "\n";
    all_zeros &= (0 == pri_upd_type_);
    r += indent_string + "  " + std::string("pri_upd_src") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(pri_upd_src_) ) + "\n";
    all_zeros &= (0 == pri_upd_src_);
    r += indent_string + "  " + std::string("pri_upd_en_shr") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(pri_upd_en_shr_) ) + "\n";
    all_zeros &= (0 == pri_upd_en_shr_);
    r += indent_string + "  " + std::string("pri_upd_val_mask") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(pri_upd_val_mask_) ) + "\n";
    all_zeros &= (0 == pri_upd_val_mask_);
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
    r += indent_string + std::string("PrsrPoActionRowMutable") + ":\n";
    for (uint32_t f=0;f<2;++f) {
      r += indent_string + "  " + std::string("csum_addr") + "["+boost::lexical_cast<std::string>(f)+"]"+ ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(csum_addr_[f]) ) + "\n";
      all_zeros &= (0 == csum_addr_[f]);
    }
    for (uint32_t f=0;f<2;++f) {
      r += indent_string + "  " + std::string("csum_en") + "["+boost::lexical_cast<std::string>(f)+"]"+ ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(csum_en_[f]) ) + "\n";
      all_zeros &= (0 == csum_en_[f]);
    }
    r += indent_string + "  " + std::string("dst_offset_inc") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(dst_offset_inc_) ) + "\n";
    all_zeros &= (0 == dst_offset_inc_);
    r += indent_string + "  " + std::string("dst_offset_rst") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(dst_offset_rst_) ) + "\n";
    all_zeros &= (0 == dst_offset_rst_);
    r += indent_string + "  " + std::string("phv_32b_offset_add_dst_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_offset_add_dst_0_) ) + "\n";
    all_zeros &= (0 == phv_32b_offset_add_dst_0_);
    r += indent_string + "  " + std::string("phv_32b_offset_add_dst_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_offset_add_dst_1_) ) + "\n";
    all_zeros &= (0 == phv_32b_offset_add_dst_1_);
    r += indent_string + "  " + std::string("phv_32b_offset_add_dst_2") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_offset_add_dst_2_) ) + "\n";
    all_zeros &= (0 == phv_32b_offset_add_dst_2_);
    r += indent_string + "  " + std::string("phv_32b_offset_add_dst_3") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_offset_add_dst_3_) ) + "\n";
    all_zeros &= (0 == phv_32b_offset_add_dst_3_);
    r += indent_string + "  " + std::string("phv_16b_offset_add_dst_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_offset_add_dst_0_) ) + "\n";
    all_zeros &= (0 == phv_16b_offset_add_dst_0_);
    r += indent_string + "  " + std::string("phv_16b_offset_add_dst_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_offset_add_dst_1_) ) + "\n";
    all_zeros &= (0 == phv_16b_offset_add_dst_1_);
    r += indent_string + "  " + std::string("phv_16b_offset_add_dst_2") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_offset_add_dst_2_) ) + "\n";
    all_zeros &= (0 == phv_16b_offset_add_dst_2_);
    r += indent_string + "  " + std::string("phv_16b_offset_add_dst_3") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_offset_add_dst_3_) ) + "\n";
    all_zeros &= (0 == phv_16b_offset_add_dst_3_);
    r += indent_string + "  " + std::string("phv_8b_offset_add_dst_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_offset_add_dst_0_) ) + "\n";
    all_zeros &= (0 == phv_8b_offset_add_dst_0_);
    r += indent_string + "  " + std::string("phv_8b_offset_add_dst_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_offset_add_dst_1_) ) + "\n";
    all_zeros &= (0 == phv_8b_offset_add_dst_1_);
    r += indent_string + "  " + std::string("phv_8b_offset_add_dst_2") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_offset_add_dst_2_) ) + "\n";
    all_zeros &= (0 == phv_8b_offset_add_dst_2_);
    r += indent_string + "  " + std::string("phv_8b_offset_add_dst_3") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_offset_add_dst_3_) ) + "\n";
    all_zeros &= (0 == phv_8b_offset_add_dst_3_);
    r += indent_string + "  " + std::string("phv_32b_offset_rot_imm_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_offset_rot_imm_0_) ) + "\n";
    all_zeros &= (0 == phv_32b_offset_rot_imm_0_);
    r += indent_string + "  " + std::string("phv_32b_offset_rot_imm_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_offset_rot_imm_1_) ) + "\n";
    all_zeros &= (0 == phv_32b_offset_rot_imm_1_);
    r += indent_string + "  " + std::string("phv_16b_offset_rot_imm_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_offset_rot_imm_0_) ) + "\n";
    all_zeros &= (0 == phv_16b_offset_rot_imm_0_);
    r += indent_string + "  " + std::string("phv_16b_offset_rot_imm_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_offset_rot_imm_1_) ) + "\n";
    all_zeros &= (0 == phv_16b_offset_rot_imm_1_);
    r += indent_string + "  " + std::string("phv_8b_offset_rot_imm_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_offset_rot_imm_0_) ) + "\n";
    all_zeros &= (0 == phv_8b_offset_rot_imm_0_);
    r += indent_string + "  " + std::string("phv_8b_offset_rot_imm_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_offset_rot_imm_1_) ) + "\n";
    all_zeros &= (0 == phv_8b_offset_rot_imm_1_);
    r += indent_string + "  " + std::string("phv_8b_offset_rot_imm_2") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_offset_rot_imm_2_) ) + "\n";
    all_zeros &= (0 == phv_8b_offset_rot_imm_2_);
    r += indent_string + "  " + std::string("phv_8b_offset_rot_imm_3") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_offset_rot_imm_3_) ) + "\n";
    all_zeros &= (0 == phv_8b_offset_rot_imm_3_);
    r += indent_string + "  " + std::string("phv_32b_dst_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_dst_0_) ) + "\n";
    all_zeros &= (0 == phv_32b_dst_0_);
    r += indent_string + "  " + std::string("phv_32b_dst_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_dst_1_) ) + "\n";
    all_zeros &= (0 == phv_32b_dst_1_);
    r += indent_string + "  " + std::string("phv_32b_dst_2") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_dst_2_) ) + "\n";
    all_zeros &= (0 == phv_32b_dst_2_);
    r += indent_string + "  " + std::string("phv_32b_dst_3") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_dst_3_) ) + "\n";
    all_zeros &= (0 == phv_32b_dst_3_);
    r += indent_string + "  " + std::string("phv_16b_dst_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_dst_0_) ) + "\n";
    all_zeros &= (0 == phv_16b_dst_0_);
    r += indent_string + "  " + std::string("phv_16b_dst_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_dst_1_) ) + "\n";
    all_zeros &= (0 == phv_16b_dst_1_);
    r += indent_string + "  " + std::string("phv_16b_dst_2") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_dst_2_) ) + "\n";
    all_zeros &= (0 == phv_16b_dst_2_);
    r += indent_string + "  " + std::string("phv_16b_dst_3") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_dst_3_) ) + "\n";
    all_zeros &= (0 == phv_16b_dst_3_);
    r += indent_string + "  " + std::string("phv_8b_dst_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_dst_0_) ) + "\n";
    all_zeros &= (0 == phv_8b_dst_0_);
    r += indent_string + "  " + std::string("phv_8b_dst_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_dst_1_) ) + "\n";
    all_zeros &= (0 == phv_8b_dst_1_);
    r += indent_string + "  " + std::string("phv_8b_dst_2") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_dst_2_) ) + "\n";
    all_zeros &= (0 == phv_8b_dst_2_);
    r += indent_string + "  " + std::string("phv_8b_dst_3") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_dst_3_) ) + "\n";
    all_zeros &= (0 == phv_8b_dst_3_);
    r += indent_string + "  " + std::string("phv_32b_src_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_src_0_) ) + "\n";
    all_zeros &= (0 == phv_32b_src_0_);
    r += indent_string + "  " + std::string("phv_32b_src_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_src_1_) ) + "\n";
    all_zeros &= (0 == phv_32b_src_1_);
    r += indent_string + "  " + std::string("phv_32b_src_2") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_src_2_) ) + "\n";
    all_zeros &= (0 == phv_32b_src_2_);
    r += indent_string + "  " + std::string("phv_32b_src_3") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_src_3_) ) + "\n";
    all_zeros &= (0 == phv_32b_src_3_);
    r += indent_string + "  " + std::string("phv_16b_src_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_src_0_) ) + "\n";
    all_zeros &= (0 == phv_16b_src_0_);
    r += indent_string + "  " + std::string("phv_16b_src_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_src_1_) ) + "\n";
    all_zeros &= (0 == phv_16b_src_1_);
    r += indent_string + "  " + std::string("phv_16b_src_2") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_src_2_) ) + "\n";
    all_zeros &= (0 == phv_16b_src_2_);
    r += indent_string + "  " + std::string("phv_16b_src_3") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_src_3_) ) + "\n";
    all_zeros &= (0 == phv_16b_src_3_);
    r += indent_string + "  " + std::string("phv_8b_src_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_src_0_) ) + "\n";
    all_zeros &= (0 == phv_8b_src_0_);
    r += indent_string + "  " + std::string("phv_8b_src_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_src_1_) ) + "\n";
    all_zeros &= (0 == phv_8b_src_1_);
    r += indent_string + "  " + std::string("phv_8b_src_2") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_src_2_) ) + "\n";
    all_zeros &= (0 == phv_8b_src_2_);
    r += indent_string + "  " + std::string("phv_8b_src_3") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_src_3_) ) + "\n";
    all_zeros &= (0 == phv_8b_src_3_);
    r += indent_string + "  " + std::string("phv_32b_src_type_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_src_type_0_) ) + "\n";
    all_zeros &= (0 == phv_32b_src_type_0_);
    r += indent_string + "  " + std::string("phv_32b_src_type_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_32b_src_type_1_) ) + "\n";
    all_zeros &= (0 == phv_32b_src_type_1_);
    r += indent_string + "  " + std::string("phv_16b_src_type_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_src_type_0_) ) + "\n";
    all_zeros &= (0 == phv_16b_src_type_0_);
    r += indent_string + "  " + std::string("phv_16b_src_type_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_16b_src_type_1_) ) + "\n";
    all_zeros &= (0 == phv_16b_src_type_1_);
    r += indent_string + "  " + std::string("phv_8b_src_type_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_src_type_0_) ) + "\n";
    all_zeros &= (0 == phv_8b_src_type_0_);
    r += indent_string + "  " + std::string("phv_8b_src_type_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_src_type_1_) ) + "\n";
    all_zeros &= (0 == phv_8b_src_type_1_);
    r += indent_string + "  " + std::string("phv_8b_src_type_2") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_src_type_2_) ) + "\n";
    all_zeros &= (0 == phv_8b_src_type_2_);
    r += indent_string + "  " + std::string("phv_8b_src_type_3") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(phv_8b_src_type_3_) ) + "\n";
    all_zeros &= (0 == phv_8b_src_type_3_);
    r += indent_string + "  " + std::string("pri_upd_type") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(pri_upd_type_) ) + "\n";
    all_zeros &= (0 == pri_upd_type_);
    r += indent_string + "  " + std::string("pri_upd_src") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(pri_upd_src_) ) + "\n";
    all_zeros &= (0 == pri_upd_src_);
    r += indent_string + "  " + std::string("pri_upd_en_shr") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(pri_upd_en_shr_) ) + "\n";
    all_zeros &= (0 == pri_upd_en_shr_);
    r += indent_string + "  " + std::string("pri_upd_val_mask") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(pri_upd_val_mask_) ) + "\n";
    all_zeros &= (0 == pri_upd_val_mask_);
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  std::array< uint8_t, 2 > csum_addr_;
  std::array< uint8_t, 2 > csum_en_;
  uint8_t dst_offset_inc_;
  uint8_t dst_offset_rst_;
  uint8_t phv_32b_offset_add_dst_0_;
  uint8_t phv_32b_offset_add_dst_1_;
  uint8_t phv_32b_offset_add_dst_2_;
  uint8_t phv_32b_offset_add_dst_3_;
  uint8_t phv_16b_offset_add_dst_0_;
  uint8_t phv_16b_offset_add_dst_1_;
  uint8_t phv_16b_offset_add_dst_2_;
  uint8_t phv_16b_offset_add_dst_3_;
  uint8_t phv_8b_offset_add_dst_0_;
  uint8_t phv_8b_offset_add_dst_1_;
  uint8_t phv_8b_offset_add_dst_2_;
  uint8_t phv_8b_offset_add_dst_3_;
  uint8_t phv_32b_offset_rot_imm_0_;
  uint8_t phv_32b_offset_rot_imm_1_;
  uint8_t phv_16b_offset_rot_imm_0_;
  uint8_t phv_16b_offset_rot_imm_1_;
  uint8_t phv_8b_offset_rot_imm_0_;
  uint8_t phv_8b_offset_rot_imm_1_;
  uint8_t phv_8b_offset_rot_imm_2_;
  uint8_t phv_8b_offset_rot_imm_3_;
  uint16_t phv_32b_dst_0_;
  uint16_t phv_32b_dst_1_;
  uint16_t phv_32b_dst_2_;
  uint16_t phv_32b_dst_3_;
  uint16_t phv_16b_dst_0_;
  uint16_t phv_16b_dst_1_;
  uint16_t phv_16b_dst_2_;
  uint16_t phv_16b_dst_3_;
  uint16_t phv_8b_dst_0_;
  uint16_t phv_8b_dst_1_;
  uint16_t phv_8b_dst_2_;
  uint16_t phv_8b_dst_3_;
  uint8_t phv_32b_src_0_;
  uint8_t phv_32b_src_1_;
  uint8_t phv_32b_src_2_;
  uint8_t phv_32b_src_3_;
  uint8_t phv_16b_src_0_;
  uint8_t phv_16b_src_1_;
  uint8_t phv_16b_src_2_;
  uint8_t phv_16b_src_3_;
  uint8_t phv_8b_src_0_;
  uint8_t phv_8b_src_1_;
  uint8_t phv_8b_src_2_;
  uint8_t phv_8b_src_3_;
  uint8_t phv_32b_src_type_0_;
  uint8_t phv_32b_src_type_1_;
  uint8_t phv_16b_src_type_0_;
  uint8_t phv_16b_src_type_1_;
  uint8_t phv_8b_src_type_0_;
  uint8_t phv_8b_src_type_1_;
  uint8_t phv_8b_src_type_2_;
  uint8_t phv_8b_src_type_3_;
  uint8_t pri_upd_type_;
  uint8_t pri_upd_src_;
  uint8_t pri_upd_en_shr_;
  uint8_t pri_upd_val_mask_;
private:
  static uint64_t StartOffset(
      int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, int index_prsr_po_action_row
      ) {
    uint64_t offset=0;
    offset += 0x20000000000; // to get to pipes
    assert(index_pipe_addrmap < 4);
    offset += index_pipe_addrmap * 0x2000000000; // pipe_addrmap[]
    switch (selector_pipe_addrmap) {
      case kEPrsr:
        offset += 0x1c80040000; // to get to e_prsr
        assert(index_prsr_mem_main_rspec < 18);
        offset += index_prsr_mem_main_rspec * 0x2000; // prsr_mem_main_rspec[]
        offset += 0x400; // to get to po_action_row
        assert(index_prsr_po_action_row < 256);
        offset += index_prsr_po_action_row * 0x2; // prsr_po_action_row[]
        break;
      case kIPrsr:
        offset += 0x1c80000000; // to get to i_prsr
        assert(index_prsr_mem_main_rspec < 18);
        offset += index_prsr_mem_main_rspec * 0x2000; // prsr_mem_main_rspec[]
        offset += 0x400; // to get to po_action_row
        assert(index_prsr_po_action_row < 256);
        offset += index_prsr_po_action_row * 0x2; // prsr_po_action_row[]
        break;
      default:
        assert(0);
        break;
    }
    return offset;
  }

};








class PrsrPoActionRowArray : public model_core::RegisterBlockIndirect<RegisterArrayCallback> {
public:
  enum PipeAddrmapEnum {
    kEPrsr,
    kIPrsr
  };
public:
  PrsrPoActionRowArray(
      int chipNumber, int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, RegisterArrayCallback& write_callback = 0, RegisterArrayCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec), 2 * ArraySize(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, -1), false, write_callback, read_callback, std::string("PrsrPoActionRowArray")+":"+boost::lexical_cast<std::string>(index_pipe_addrmap) + "," + boost::lexical_cast<std::string>(selector_pipe_addrmap) + "," + boost::lexical_cast<std::string>(index_prsr_mem_main_rspec)),
    array(RealArraySize(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, -1)),
    size0_(ArraySize(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, 0)),
    real_size0_(RealArraySize(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, 0))
    {
    }
public:






  uint8_t &csum_addr(uint32_t a0,int j0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].csum_addr(j0);
  }






  uint8_t &csum_en(uint32_t a0,int j0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].csum_en(j0);
  }






  uint8_t &dst_offset_inc(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].dst_offset_inc();
  }






  uint8_t &dst_offset_rst(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].dst_offset_rst();
  }






  uint8_t &phv_32b_offset_add_dst_0(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_32b_offset_add_dst_0();
  }






  uint8_t &phv_32b_offset_add_dst_1(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_32b_offset_add_dst_1();
  }






  uint8_t &phv_32b_offset_add_dst_2(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_32b_offset_add_dst_2();
  }






  uint8_t &phv_32b_offset_add_dst_3(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_32b_offset_add_dst_3();
  }






  uint8_t &phv_16b_offset_add_dst_0(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_16b_offset_add_dst_0();
  }






  uint8_t &phv_16b_offset_add_dst_1(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_16b_offset_add_dst_1();
  }






  uint8_t &phv_16b_offset_add_dst_2(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_16b_offset_add_dst_2();
  }






  uint8_t &phv_16b_offset_add_dst_3(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_16b_offset_add_dst_3();
  }






  uint8_t &phv_8b_offset_add_dst_0(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_8b_offset_add_dst_0();
  }






  uint8_t &phv_8b_offset_add_dst_1(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_8b_offset_add_dst_1();
  }






  uint8_t &phv_8b_offset_add_dst_2(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_8b_offset_add_dst_2();
  }






  uint8_t &phv_8b_offset_add_dst_3(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_8b_offset_add_dst_3();
  }






  uint8_t &phv_32b_offset_rot_imm_0(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_32b_offset_rot_imm_0();
  }






  uint8_t &phv_32b_offset_rot_imm_1(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_32b_offset_rot_imm_1();
  }






  uint8_t &phv_16b_offset_rot_imm_0(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_16b_offset_rot_imm_0();
  }






  uint8_t &phv_16b_offset_rot_imm_1(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_16b_offset_rot_imm_1();
  }






  uint8_t &phv_8b_offset_rot_imm_0(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_8b_offset_rot_imm_0();
  }






  uint8_t &phv_8b_offset_rot_imm_1(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_8b_offset_rot_imm_1();
  }






  uint8_t &phv_8b_offset_rot_imm_2(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_8b_offset_rot_imm_2();
  }






  uint8_t &phv_8b_offset_rot_imm_3(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_8b_offset_rot_imm_3();
  }





  uint16_t &phv_32b_dst_0(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_32b_dst_0();
  }





  uint16_t &phv_32b_dst_1(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_32b_dst_1();
  }





  uint16_t &phv_32b_dst_2(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_32b_dst_2();
  }





  uint16_t &phv_32b_dst_3(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_32b_dst_3();
  }





  uint16_t &phv_16b_dst_0(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_16b_dst_0();
  }





  uint16_t &phv_16b_dst_1(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_16b_dst_1();
  }





  uint16_t &phv_16b_dst_2(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_16b_dst_2();
  }





  uint16_t &phv_16b_dst_3(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_16b_dst_3();
  }





  uint16_t &phv_8b_dst_0(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_8b_dst_0();
  }





  uint16_t &phv_8b_dst_1(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_8b_dst_1();
  }





  uint16_t &phv_8b_dst_2(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_8b_dst_2();
  }





  uint16_t &phv_8b_dst_3(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_8b_dst_3();
  }






  uint8_t &phv_32b_src_0(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_32b_src_0();
  }






  uint8_t &phv_32b_src_1(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_32b_src_1();
  }






  uint8_t &phv_32b_src_2(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_32b_src_2();
  }






  uint8_t &phv_32b_src_3(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_32b_src_3();
  }






  uint8_t &phv_16b_src_0(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_16b_src_0();
  }






  uint8_t &phv_16b_src_1(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_16b_src_1();
  }






  uint8_t &phv_16b_src_2(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_16b_src_2();
  }






  uint8_t &phv_16b_src_3(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_16b_src_3();
  }






  uint8_t &phv_8b_src_0(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_8b_src_0();
  }






  uint8_t &phv_8b_src_1(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_8b_src_1();
  }






  uint8_t &phv_8b_src_2(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_8b_src_2();
  }






  uint8_t &phv_8b_src_3(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_8b_src_3();
  }





  uint8_t &phv_32b_src_type_0(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_32b_src_type_0();
  }





  uint8_t &phv_32b_src_type_1(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_32b_src_type_1();
  }





  uint8_t &phv_16b_src_type_0(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_16b_src_type_0();
  }





  uint8_t &phv_16b_src_type_1(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_16b_src_type_1();
  }





  uint8_t &phv_8b_src_type_0(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_8b_src_type_0();
  }





  uint8_t &phv_8b_src_type_1(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_8b_src_type_1();
  }





  uint8_t &phv_8b_src_type_2(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_8b_src_type_2();
  }





  uint8_t &phv_8b_src_type_3(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_8b_src_type_3();
  }






  uint8_t &pri_upd_type(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].pri_upd_type();
  }






  uint8_t &pri_upd_src(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].pri_upd_src();
  }







  uint8_t &pri_upd_en_shr(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].pri_upd_en_shr();
  }






  uint8_t &pri_upd_val_mask(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].pri_upd_val_mask();
  }
  bool calculate_index(
      uint64_t* offset, uint32_t* a0
      ) const {
    int i = (*offset)/2;
    (*offset) -= (i*2);
    uint32_t t = i;
    *a0 = t;
    bool in_bounds = true;
    in_bounds &= CheckArrayBounds(*a0,real_size0_,size0_,*offset,0,true);
    return in_bounds;
  }


  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    if (read_callback_) read_callback_(a0);
    array[a0].read(offset,data0,data1,T);
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    array[a0].write(offset,data0,data1,T);
    if (write_callback_) write_callback_(a0);
    return true;
  }

  void reset(
      
      ) {
    for (uint32_t i=0;i<array.size();++i) {
      array[i].reset();
      int t = i;
      int a0 = t;
      if (write_callback_) write_callback_(a0);
    }
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    all_zeros=false;
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return "OUT_OF_BOUNDS";
    std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
    if (! r_s.empty()) {
      r += indent_string + std::string("PrsrPoActionRowArray") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
    }
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
    all_zeros=false;
    for (uint32_t a0=0;a0<real_size0_;++a0) {
      std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
      if (! r_s.empty()) {
        r += indent_string + std::string("PrsrPoActionRowArray") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
      }
    }
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  std::vector<PrsrPoActionRow> array;
  uint32_t size0_;
  uint32_t real_size0_;
private:
  static uint64_t StartOffset(
      int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec
      ) {
    uint64_t offset=0;
    offset += 0x20000000000; // to get to pipes
    assert(index_pipe_addrmap < 4);
    offset += index_pipe_addrmap * 0x2000000000; // pipe_addrmap[]
    switch (selector_pipe_addrmap) {
      case kEPrsr:
        offset += 0x1c80040000; // to get to e_prsr
        assert(index_prsr_mem_main_rspec < 18);
        offset += index_prsr_mem_main_rspec * 0x2000; // prsr_mem_main_rspec[]
        offset += 0x400; // to get to po_action_row
        break;
      case kIPrsr:
        offset += 0x1c80000000; // to get to i_prsr
        assert(index_prsr_mem_main_rspec < 18);
        offset += index_prsr_mem_main_rspec * 0x2000; // prsr_mem_main_rspec[]
        offset += 0x400; // to get to po_action_row
        break;
      default:
        assert(0);
        break;
    }
    return offset;
  }

  static int ArraySize(
      int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, int dimension
      ) {
    switch (selector_pipe_addrmap) {
      case kEPrsr:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kIPrsr:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
  }

  static int RealArraySize(
      int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, int dimension
      ) {
    switch (selector_pipe_addrmap) {
      case kEPrsr:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kIPrsr:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
  }

};








class PrsrPoActionRowArrayMutable : public model_core::RegisterBlockIndirect<RegisterArrayCallback> {
public:
  enum PipeAddrmapEnum {
    kEPrsr,
    kIPrsr
  };
public:
  PrsrPoActionRowArrayMutable(
      int chipNumber, int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, RegisterArrayCallback& write_callback = 0, RegisterArrayCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec), 2 * ArraySize(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, -1), true, write_callback, read_callback, std::string("PrsrPoActionRowArrayMutable")+":"+boost::lexical_cast<std::string>(index_pipe_addrmap) + "," + boost::lexical_cast<std::string>(selector_pipe_addrmap) + "," + boost::lexical_cast<std::string>(index_prsr_mem_main_rspec)),
    array(RealArraySize(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, -1)),
    size0_(ArraySize(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, 0)),
    real_size0_(RealArraySize(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, 0))
    {
    }
public:






  uint8_t csum_addr(uint32_t a0,int j0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].csum_addr(j0);
  }
  void csum_addr(uint32_t a0,int j0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].csum_addr(j0,v);
  }






  uint8_t csum_en(uint32_t a0,int j0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].csum_en(j0);
  }
  void csum_en(uint32_t a0,int j0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].csum_en(j0,v);
  }






  uint8_t dst_offset_inc(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].dst_offset_inc();
  }
  void dst_offset_inc(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].dst_offset_inc(v);
  }






  uint8_t dst_offset_rst(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].dst_offset_rst();
  }
  void dst_offset_rst(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].dst_offset_rst(v);
  }






  uint8_t phv_32b_offset_add_dst_0(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_32b_offset_add_dst_0();
  }
  void phv_32b_offset_add_dst_0(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_32b_offset_add_dst_0(v);
  }






  uint8_t phv_32b_offset_add_dst_1(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_32b_offset_add_dst_1();
  }
  void phv_32b_offset_add_dst_1(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_32b_offset_add_dst_1(v);
  }






  uint8_t phv_32b_offset_add_dst_2(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_32b_offset_add_dst_2();
  }
  void phv_32b_offset_add_dst_2(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_32b_offset_add_dst_2(v);
  }






  uint8_t phv_32b_offset_add_dst_3(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_32b_offset_add_dst_3();
  }
  void phv_32b_offset_add_dst_3(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_32b_offset_add_dst_3(v);
  }






  uint8_t phv_16b_offset_add_dst_0(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_16b_offset_add_dst_0();
  }
  void phv_16b_offset_add_dst_0(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_16b_offset_add_dst_0(v);
  }






  uint8_t phv_16b_offset_add_dst_1(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_16b_offset_add_dst_1();
  }
  void phv_16b_offset_add_dst_1(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_16b_offset_add_dst_1(v);
  }






  uint8_t phv_16b_offset_add_dst_2(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_16b_offset_add_dst_2();
  }
  void phv_16b_offset_add_dst_2(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_16b_offset_add_dst_2(v);
  }






  uint8_t phv_16b_offset_add_dst_3(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_16b_offset_add_dst_3();
  }
  void phv_16b_offset_add_dst_3(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_16b_offset_add_dst_3(v);
  }






  uint8_t phv_8b_offset_add_dst_0(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_8b_offset_add_dst_0();
  }
  void phv_8b_offset_add_dst_0(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_8b_offset_add_dst_0(v);
  }






  uint8_t phv_8b_offset_add_dst_1(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_8b_offset_add_dst_1();
  }
  void phv_8b_offset_add_dst_1(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_8b_offset_add_dst_1(v);
  }






  uint8_t phv_8b_offset_add_dst_2(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_8b_offset_add_dst_2();
  }
  void phv_8b_offset_add_dst_2(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_8b_offset_add_dst_2(v);
  }






  uint8_t phv_8b_offset_add_dst_3(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_8b_offset_add_dst_3();
  }
  void phv_8b_offset_add_dst_3(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_8b_offset_add_dst_3(v);
  }






  uint8_t phv_32b_offset_rot_imm_0(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_32b_offset_rot_imm_0();
  }
  void phv_32b_offset_rot_imm_0(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_32b_offset_rot_imm_0(v);
  }






  uint8_t phv_32b_offset_rot_imm_1(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_32b_offset_rot_imm_1();
  }
  void phv_32b_offset_rot_imm_1(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_32b_offset_rot_imm_1(v);
  }






  uint8_t phv_16b_offset_rot_imm_0(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_16b_offset_rot_imm_0();
  }
  void phv_16b_offset_rot_imm_0(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_16b_offset_rot_imm_0(v);
  }






  uint8_t phv_16b_offset_rot_imm_1(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_16b_offset_rot_imm_1();
  }
  void phv_16b_offset_rot_imm_1(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_16b_offset_rot_imm_1(v);
  }






  uint8_t phv_8b_offset_rot_imm_0(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_8b_offset_rot_imm_0();
  }
  void phv_8b_offset_rot_imm_0(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_8b_offset_rot_imm_0(v);
  }






  uint8_t phv_8b_offset_rot_imm_1(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_8b_offset_rot_imm_1();
  }
  void phv_8b_offset_rot_imm_1(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_8b_offset_rot_imm_1(v);
  }






  uint8_t phv_8b_offset_rot_imm_2(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_8b_offset_rot_imm_2();
  }
  void phv_8b_offset_rot_imm_2(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_8b_offset_rot_imm_2(v);
  }






  uint8_t phv_8b_offset_rot_imm_3(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_8b_offset_rot_imm_3();
  }
  void phv_8b_offset_rot_imm_3(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_8b_offset_rot_imm_3(v);
  }





  uint16_t phv_32b_dst_0(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_32b_dst_0();
  }
  void phv_32b_dst_0(uint32_t a0,const uint16_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_32b_dst_0(v);
  }





  uint16_t phv_32b_dst_1(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_32b_dst_1();
  }
  void phv_32b_dst_1(uint32_t a0,const uint16_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_32b_dst_1(v);
  }





  uint16_t phv_32b_dst_2(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_32b_dst_2();
  }
  void phv_32b_dst_2(uint32_t a0,const uint16_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_32b_dst_2(v);
  }





  uint16_t phv_32b_dst_3(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_32b_dst_3();
  }
  void phv_32b_dst_3(uint32_t a0,const uint16_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_32b_dst_3(v);
  }





  uint16_t phv_16b_dst_0(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_16b_dst_0();
  }
  void phv_16b_dst_0(uint32_t a0,const uint16_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_16b_dst_0(v);
  }





  uint16_t phv_16b_dst_1(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_16b_dst_1();
  }
  void phv_16b_dst_1(uint32_t a0,const uint16_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_16b_dst_1(v);
  }





  uint16_t phv_16b_dst_2(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_16b_dst_2();
  }
  void phv_16b_dst_2(uint32_t a0,const uint16_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_16b_dst_2(v);
  }





  uint16_t phv_16b_dst_3(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_16b_dst_3();
  }
  void phv_16b_dst_3(uint32_t a0,const uint16_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_16b_dst_3(v);
  }





  uint16_t phv_8b_dst_0(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_8b_dst_0();
  }
  void phv_8b_dst_0(uint32_t a0,const uint16_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_8b_dst_0(v);
  }





  uint16_t phv_8b_dst_1(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_8b_dst_1();
  }
  void phv_8b_dst_1(uint32_t a0,const uint16_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_8b_dst_1(v);
  }





  uint16_t phv_8b_dst_2(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_8b_dst_2();
  }
  void phv_8b_dst_2(uint32_t a0,const uint16_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_8b_dst_2(v);
  }





  uint16_t phv_8b_dst_3(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_8b_dst_3();
  }
  void phv_8b_dst_3(uint32_t a0,const uint16_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_8b_dst_3(v);
  }






  uint8_t phv_32b_src_0(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_32b_src_0();
  }
  void phv_32b_src_0(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_32b_src_0(v);
  }






  uint8_t phv_32b_src_1(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_32b_src_1();
  }
  void phv_32b_src_1(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_32b_src_1(v);
  }






  uint8_t phv_32b_src_2(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_32b_src_2();
  }
  void phv_32b_src_2(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_32b_src_2(v);
  }






  uint8_t phv_32b_src_3(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_32b_src_3();
  }
  void phv_32b_src_3(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_32b_src_3(v);
  }






  uint8_t phv_16b_src_0(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_16b_src_0();
  }
  void phv_16b_src_0(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_16b_src_0(v);
  }






  uint8_t phv_16b_src_1(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_16b_src_1();
  }
  void phv_16b_src_1(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_16b_src_1(v);
  }






  uint8_t phv_16b_src_2(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_16b_src_2();
  }
  void phv_16b_src_2(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_16b_src_2(v);
  }






  uint8_t phv_16b_src_3(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_16b_src_3();
  }
  void phv_16b_src_3(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_16b_src_3(v);
  }






  uint8_t phv_8b_src_0(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_8b_src_0();
  }
  void phv_8b_src_0(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_8b_src_0(v);
  }






  uint8_t phv_8b_src_1(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_8b_src_1();
  }
  void phv_8b_src_1(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_8b_src_1(v);
  }






  uint8_t phv_8b_src_2(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_8b_src_2();
  }
  void phv_8b_src_2(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_8b_src_2(v);
  }






  uint8_t phv_8b_src_3(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_8b_src_3();
  }
  void phv_8b_src_3(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_8b_src_3(v);
  }





  uint8_t phv_32b_src_type_0(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_32b_src_type_0();
  }
  void phv_32b_src_type_0(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_32b_src_type_0(v);
  }





  uint8_t phv_32b_src_type_1(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_32b_src_type_1();
  }
  void phv_32b_src_type_1(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_32b_src_type_1(v);
  }





  uint8_t phv_16b_src_type_0(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_16b_src_type_0();
  }
  void phv_16b_src_type_0(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_16b_src_type_0(v);
  }





  uint8_t phv_16b_src_type_1(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_16b_src_type_1();
  }
  void phv_16b_src_type_1(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_16b_src_type_1(v);
  }





  uint8_t phv_8b_src_type_0(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_8b_src_type_0();
  }
  void phv_8b_src_type_0(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_8b_src_type_0(v);
  }





  uint8_t phv_8b_src_type_1(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_8b_src_type_1();
  }
  void phv_8b_src_type_1(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_8b_src_type_1(v);
  }





  uint8_t phv_8b_src_type_2(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_8b_src_type_2();
  }
  void phv_8b_src_type_2(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_8b_src_type_2(v);
  }





  uint8_t phv_8b_src_type_3(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].phv_8b_src_type_3();
  }
  void phv_8b_src_type_3(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].phv_8b_src_type_3(v);
  }






  uint8_t pri_upd_type(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].pri_upd_type();
  }
  void pri_upd_type(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].pri_upd_type(v);
  }






  uint8_t pri_upd_src(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].pri_upd_src();
  }
  void pri_upd_src(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].pri_upd_src(v);
  }







  uint8_t pri_upd_en_shr(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].pri_upd_en_shr();
  }
  void pri_upd_en_shr(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].pri_upd_en_shr(v);
  }






  uint8_t pri_upd_val_mask(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].pri_upd_val_mask();
  }
  void pri_upd_val_mask(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].pri_upd_val_mask(v);
  }
  bool calculate_index(
      uint64_t* offset, uint32_t* a0
      ) const {
    int i = (*offset)/2;
    (*offset) -= (i*2);
    uint32_t t = i;
    *a0 = t;
    bool in_bounds = true;
    in_bounds &= CheckArrayBounds(*a0,real_size0_,size0_,*offset,0,true);
    return in_bounds;
  }


  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    if (read_callback_) read_callback_(a0);
    array[a0].read(offset,data0,data1,T);
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    array[a0].write(offset,data0,data1,T);
    if (write_callback_) write_callback_(a0);
    return true;
  }

  void reset(
      
      ) {
    for (uint32_t i=0;i<array.size();++i) {
      array[i].reset();
      int t = i;
      int a0 = t;
      if (write_callback_) write_callback_(a0);
    }
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    all_zeros=false;
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return "OUT_OF_BOUNDS";
    std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
    if (! r_s.empty()) {
      r += indent_string + std::string("PrsrPoActionRowArrayMutable") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
    }
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
    all_zeros=false;
    for (uint32_t a0=0;a0<real_size0_;++a0) {
      std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
      if (! r_s.empty()) {
        r += indent_string + std::string("PrsrPoActionRowArrayMutable") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
      }
    }
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  std::vector<PrsrPoActionRowMutable> array;
  uint32_t size0_;
  uint32_t real_size0_;
private:
  static uint64_t StartOffset(
      int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec
      ) {
    uint64_t offset=0;
    offset += 0x20000000000; // to get to pipes
    assert(index_pipe_addrmap < 4);
    offset += index_pipe_addrmap * 0x2000000000; // pipe_addrmap[]
    switch (selector_pipe_addrmap) {
      case kEPrsr:
        offset += 0x1c80040000; // to get to e_prsr
        assert(index_prsr_mem_main_rspec < 18);
        offset += index_prsr_mem_main_rspec * 0x2000; // prsr_mem_main_rspec[]
        offset += 0x400; // to get to po_action_row
        break;
      case kIPrsr:
        offset += 0x1c80000000; // to get to i_prsr
        assert(index_prsr_mem_main_rspec < 18);
        offset += index_prsr_mem_main_rspec * 0x2000; // prsr_mem_main_rspec[]
        offset += 0x400; // to get to po_action_row
        break;
      default:
        assert(0);
        break;
    }
    return offset;
  }

  static int ArraySize(
      int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, int dimension
      ) {
    switch (selector_pipe_addrmap) {
      case kEPrsr:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kIPrsr:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
  }

  static int RealArraySize(
      int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, int dimension
      ) {
    switch (selector_pipe_addrmap) {
      case kEPrsr:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kIPrsr:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
  }

};








class PrsrMlEaRow : public model_core::RegisterBlockIndirect<RegisterCallback> {
public:
  enum PipeAddrmapEnum {
    kEPrsr,
    kIPrsr
  };
public:
  PrsrMlEaRow(
      int chipNumber, int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, int index_prsr_ml_ea_row, RegisterCallback& write_callback = 0, RegisterCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, index_prsr_ml_ea_row), 1, false, write_callback, read_callback, std::string("PrsrMlEaRow")+":"+boost::lexical_cast<std::string>(index_pipe_addrmap) + "," + boost::lexical_cast<std::string>(selector_pipe_addrmap) + "," + boost::lexical_cast<std::string>(index_prsr_mem_main_rspec) + "," + boost::lexical_cast<std::string>(index_prsr_ml_ea_row))
    {
    }
  PrsrMlEaRow(
      
  )
    : RegisterBlockIndirect(0, 0, 0, false, 0, 0, "PrsrMlEaRow")
    {
    }
public:







  uint8_t &ctr_amt_idx() { return ctr_amt_idx_; }





  uint8_t &ctr_ld_src() { return ctr_ld_src_; }





  uint8_t &ctr_load() { return ctr_load_; }






  uint8_t &done() { return done_; }





  uint8_t &shift_amt() { return shift_amt_; }






  uint8_t &lookup_offset_16() { return lookup_offset_16_; }








  uint8_t &lookup_offset_8(int j0) { return lookup_offset_8_[j0]; }




  uint8_t &ld_lookup_16() { return ld_lookup_16_; }






  uint8_t &ld_lookup_8(int j0) { return ld_lookup_8_[j0]; }





  uint8_t &nxt_state_mask() { return nxt_state_mask_; }




  uint8_t &nxt_state() { return nxt_state_; }






  uint8_t &buf_req() { return buf_req_; }

  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    if (read_callback_) read_callback_();
    *data0 = static_cast<uint64_t>(ctr_amt_idx_);
    *data0 |= ((static_cast<uint64_t>(ctr_ld_src_) & 0x1) << 8);
    *data0 |= ((static_cast<uint64_t>(ctr_load_) & 0x1) << 9);
    *data0 |= ((static_cast<uint64_t>(done_) & 0x1) << 10);
    *data0 |= ((static_cast<uint64_t>(shift_amt_) & 0x3f) << 11);
    *data0 |= ((static_cast<uint64_t>(lookup_offset_16_) & 0x3f) << 17);
    *data0 |= ((static_cast<uint64_t>(lookup_offset_8_[0]) & 0x3f) << 23);
    *data0 |= ((static_cast<uint64_t>(lookup_offset_8_[1]) & 0x3f) << 29);
    *data0 |= ((static_cast<uint64_t>(ld_lookup_16_) & 0x1) << 35);
    *data0 |= ((static_cast<uint64_t>(ld_lookup_8_[0]) & 0x1) << 36);
    *data0 |= ((static_cast<uint64_t>(ld_lookup_8_[1]) & 0x1) << 37);
    *data0 |= (static_cast<uint64_t>(nxt_state_mask_) << 38);
    *data0 |= (static_cast<uint64_t>(nxt_state_) << 46);
    *data0 |= ((static_cast<uint64_t>(buf_req_) & 0x3f) << 54);
    *data1 = 0;
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    ctr_amt_idx_ = data0;
    ctr_ld_src_ = ((data0 >> 8) & 0x1);
    ctr_load_ = ((data0 >> 9) & 0x1);
    done_ = ((data0 >> 10) & 0x1);
    shift_amt_ = ((data0 >> 11) & 0x3f);
    lookup_offset_16_ = ((data0 >> 17) & 0x3f);
    lookup_offset_8_[0] = ((data0 >> 23) & 0x3f);
    lookup_offset_8_[1] = ((data0 >> 29) & 0x3f);
    ld_lookup_16_ = ((data0 >> 35) & 0x1);
    ld_lookup_8_[0] = ((data0 >> 36) & 0x1);
    ld_lookup_8_[1] = ((data0 >> 37) & 0x1);
    nxt_state_mask_ = (data0 >> 38);
    nxt_state_ = (data0 >> 46);
    buf_req_ = ((data0 >> 54) & 0x3f);
    if (write_callback_) write_callback_();
    return true;
  }

  void reset(
      
      ) {
    ctr_amt_idx_ = 0;
    ctr_ld_src_ = 0;
    ctr_load_ = 0;
    done_ = 0;
    shift_amt_ = 0;
    lookup_offset_16_ = 0;
    lookup_offset_8_[0] = 0;
    lookup_offset_8_[1] = 0;
    ld_lookup_16_ = 0;
    ld_lookup_8_[0] = 0;
    ld_lookup_8_[1] = 0;
    nxt_state_mask_ = 0;
    nxt_state_ = 0;
    buf_req_ = 0;
    if (write_callback_) write_callback_();
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    r += indent_string + std::string("PrsrMlEaRow") + ":\n";
    r += indent_string + "  " + std::string("ctr_amt_idx") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(ctr_amt_idx_) ) + "\n";
    all_zeros &= (0 == ctr_amt_idx_);
    r += indent_string + "  " + std::string("ctr_ld_src") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(ctr_ld_src_) ) + "\n";
    all_zeros &= (0 == ctr_ld_src_);
    r += indent_string + "  " + std::string("ctr_load") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(ctr_load_) ) + "\n";
    all_zeros &= (0 == ctr_load_);
    r += indent_string + "  " + std::string("done") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(done_) ) + "\n";
    all_zeros &= (0 == done_);
    r += indent_string + "  " + std::string("shift_amt") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(shift_amt_) ) + "\n";
    all_zeros &= (0 == shift_amt_);
    r += indent_string + "  " + std::string("lookup_offset_16") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(lookup_offset_16_) ) + "\n";
    all_zeros &= (0 == lookup_offset_16_);
    for (uint32_t f=0;f<2;++f) {
      r += indent_string + "  " + std::string("lookup_offset_8") + "["+boost::lexical_cast<std::string>(f)+"]"+ ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(lookup_offset_8_[f]) ) + "\n";
      all_zeros &= (0 == lookup_offset_8_[f]);
    }
    r += indent_string + "  " + std::string("ld_lookup_16") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(ld_lookup_16_) ) + "\n";
    all_zeros &= (0 == ld_lookup_16_);
    for (uint32_t f=0;f<2;++f) {
      r += indent_string + "  " + std::string("ld_lookup_8") + "["+boost::lexical_cast<std::string>(f)+"]"+ ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(ld_lookup_8_[f]) ) + "\n";
      all_zeros &= (0 == ld_lookup_8_[f]);
    }
    r += indent_string + "  " + std::string("nxt_state_mask") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(nxt_state_mask_) ) + "\n";
    all_zeros &= (0 == nxt_state_mask_);
    r += indent_string + "  " + std::string("nxt_state") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(nxt_state_) ) + "\n";
    all_zeros &= (0 == nxt_state_);
    r += indent_string + "  " + std::string("buf_req") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(buf_req_) ) + "\n";
    all_zeros &= (0 == buf_req_);
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
    r += indent_string + std::string("PrsrMlEaRow") + ":\n";
    r += indent_string + "  " + std::string("ctr_amt_idx") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(ctr_amt_idx_) ) + "\n";
    all_zeros &= (0 == ctr_amt_idx_);
    r += indent_string + "  " + std::string("ctr_ld_src") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(ctr_ld_src_) ) + "\n";
    all_zeros &= (0 == ctr_ld_src_);
    r += indent_string + "  " + std::string("ctr_load") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(ctr_load_) ) + "\n";
    all_zeros &= (0 == ctr_load_);
    r += indent_string + "  " + std::string("done") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(done_) ) + "\n";
    all_zeros &= (0 == done_);
    r += indent_string + "  " + std::string("shift_amt") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(shift_amt_) ) + "\n";
    all_zeros &= (0 == shift_amt_);
    r += indent_string + "  " + std::string("lookup_offset_16") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(lookup_offset_16_) ) + "\n";
    all_zeros &= (0 == lookup_offset_16_);
    for (uint32_t f=0;f<2;++f) {
      r += indent_string + "  " + std::string("lookup_offset_8") + "["+boost::lexical_cast<std::string>(f)+"]"+ ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(lookup_offset_8_[f]) ) + "\n";
      all_zeros &= (0 == lookup_offset_8_[f]);
    }
    r += indent_string + "  " + std::string("ld_lookup_16") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(ld_lookup_16_) ) + "\n";
    all_zeros &= (0 == ld_lookup_16_);
    for (uint32_t f=0;f<2;++f) {
      r += indent_string + "  " + std::string("ld_lookup_8") + "["+boost::lexical_cast<std::string>(f)+"]"+ ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(ld_lookup_8_[f]) ) + "\n";
      all_zeros &= (0 == ld_lookup_8_[f]);
    }
    r += indent_string + "  " + std::string("nxt_state_mask") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(nxt_state_mask_) ) + "\n";
    all_zeros &= (0 == nxt_state_mask_);
    r += indent_string + "  " + std::string("nxt_state") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(nxt_state_) ) + "\n";
    all_zeros &= (0 == nxt_state_);
    r += indent_string + "  " + std::string("buf_req") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(buf_req_) ) + "\n";
    all_zeros &= (0 == buf_req_);
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  uint8_t ctr_amt_idx_;
  uint8_t ctr_ld_src_;
  uint8_t ctr_load_;
  uint8_t done_;
  uint8_t shift_amt_;
  uint8_t lookup_offset_16_;
  std::array< uint8_t, 2 > lookup_offset_8_;
  uint8_t ld_lookup_16_;
  std::array< uint8_t, 2 > ld_lookup_8_;
  uint8_t nxt_state_mask_;
  uint8_t nxt_state_;
  uint8_t buf_req_;
private:
  static uint64_t StartOffset(
      int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, int index_prsr_ml_ea_row
      ) {
    uint64_t offset=0;
    offset += 0x20000000000; // to get to pipes
    assert(index_pipe_addrmap < 4);
    offset += index_pipe_addrmap * 0x2000000000; // pipe_addrmap[]
    switch (selector_pipe_addrmap) {
      case kEPrsr:
        offset += 0x1c80040000; // to get to e_prsr
        assert(index_prsr_mem_main_rspec < 18);
        offset += index_prsr_mem_main_rspec * 0x2000; // prsr_mem_main_rspec[]
        offset += 0x200; // to get to ml_ea_row
        assert(index_prsr_ml_ea_row < 256);
        offset += index_prsr_ml_ea_row * 0x1; // prsr_ml_ea_row[]
        break;
      case kIPrsr:
        offset += 0x1c80000000; // to get to i_prsr
        assert(index_prsr_mem_main_rspec < 18);
        offset += index_prsr_mem_main_rspec * 0x2000; // prsr_mem_main_rspec[]
        offset += 0x200; // to get to ml_ea_row
        assert(index_prsr_ml_ea_row < 256);
        offset += index_prsr_ml_ea_row * 0x1; // prsr_ml_ea_row[]
        break;
      default:
        assert(0);
        break;
    }
    return offset;
  }

};








class PrsrMlEaRowMutable : public model_core::RegisterBlockIndirect<RegisterCallback> {
public:
  enum PipeAddrmapEnum {
    kEPrsr,
    kIPrsr
  };
public:
  PrsrMlEaRowMutable(
      int chipNumber, int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, int index_prsr_ml_ea_row, RegisterCallback& write_callback = 0, RegisterCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, index_prsr_ml_ea_row), 1, true, write_callback, read_callback, std::string("PrsrMlEaRowMutable")+":"+boost::lexical_cast<std::string>(index_pipe_addrmap) + "," + boost::lexical_cast<std::string>(selector_pipe_addrmap) + "," + boost::lexical_cast<std::string>(index_prsr_mem_main_rspec) + "," + boost::lexical_cast<std::string>(index_prsr_ml_ea_row))
    {
    }
  PrsrMlEaRowMutable(
      
  )
    : RegisterBlockIndirect(0, 0, 0, true, 0, 0, "PrsrMlEaRowMutable")
    {
    }
public:







  uint8_t ctr_amt_idx() { return ctr_amt_idx_; }
  void ctr_amt_idx(const uint8_t &v) { ctr_amt_idx_=v; }





  uint8_t ctr_ld_src() { return ctr_ld_src_; }
  void ctr_ld_src(const uint8_t &v) { ctr_ld_src_=v; }





  uint8_t ctr_load() { return ctr_load_; }
  void ctr_load(const uint8_t &v) { ctr_load_=v; }






  uint8_t done() { return done_; }
  void done(const uint8_t &v) { done_=v; }





  uint8_t shift_amt() { return shift_amt_; }
  void shift_amt(const uint8_t &v) { shift_amt_=v; }






  uint8_t lookup_offset_16() { return lookup_offset_16_; }
  void lookup_offset_16(const uint8_t &v) { lookup_offset_16_=v; }








  uint8_t lookup_offset_8(int j0) { return lookup_offset_8_[j0]; }
  void lookup_offset_8(int j0,const uint8_t &v) { lookup_offset_8_[j0]=v; }




  uint8_t ld_lookup_16() { return ld_lookup_16_; }
  void ld_lookup_16(const uint8_t &v) { ld_lookup_16_=v; }






  uint8_t ld_lookup_8(int j0) { return ld_lookup_8_[j0]; }
  void ld_lookup_8(int j0,const uint8_t &v) { ld_lookup_8_[j0]=v; }





  uint8_t nxt_state_mask() { return nxt_state_mask_; }
  void nxt_state_mask(const uint8_t &v) { nxt_state_mask_=v; }




  uint8_t nxt_state() { return nxt_state_; }
  void nxt_state(const uint8_t &v) { nxt_state_=v; }






  uint8_t buf_req() { return buf_req_; }
  void buf_req(const uint8_t &v) { buf_req_=v; }

  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    if (read_callback_) read_callback_();
    *data0 = static_cast<uint64_t>(ctr_amt_idx_);
    *data0 |= ((static_cast<uint64_t>(ctr_ld_src_) & 0x1) << 8);
    *data0 |= ((static_cast<uint64_t>(ctr_load_) & 0x1) << 9);
    *data0 |= ((static_cast<uint64_t>(done_) & 0x1) << 10);
    *data0 |= ((static_cast<uint64_t>(shift_amt_) & 0x3f) << 11);
    *data0 |= ((static_cast<uint64_t>(lookup_offset_16_) & 0x3f) << 17);
    *data0 |= ((static_cast<uint64_t>(lookup_offset_8_[0]) & 0x3f) << 23);
    *data0 |= ((static_cast<uint64_t>(lookup_offset_8_[1]) & 0x3f) << 29);
    *data0 |= ((static_cast<uint64_t>(ld_lookup_16_) & 0x1) << 35);
    *data0 |= ((static_cast<uint64_t>(ld_lookup_8_[0]) & 0x1) << 36);
    *data0 |= ((static_cast<uint64_t>(ld_lookup_8_[1]) & 0x1) << 37);
    *data0 |= (static_cast<uint64_t>(nxt_state_mask_) << 38);
    *data0 |= (static_cast<uint64_t>(nxt_state_) << 46);
    *data0 |= ((static_cast<uint64_t>(buf_req_) & 0x3f) << 54);
    *data1 = 0;
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    ctr_amt_idx_ = data0;
    ctr_ld_src_ = ((data0 >> 8) & 0x1);
    ctr_load_ = ((data0 >> 9) & 0x1);
    done_ = ((data0 >> 10) & 0x1);
    shift_amt_ = ((data0 >> 11) & 0x3f);
    lookup_offset_16_ = ((data0 >> 17) & 0x3f);
    lookup_offset_8_[0] = ((data0 >> 23) & 0x3f);
    lookup_offset_8_[1] = ((data0 >> 29) & 0x3f);
    ld_lookup_16_ = ((data0 >> 35) & 0x1);
    ld_lookup_8_[0] = ((data0 >> 36) & 0x1);
    ld_lookup_8_[1] = ((data0 >> 37) & 0x1);
    nxt_state_mask_ = (data0 >> 38);
    nxt_state_ = (data0 >> 46);
    buf_req_ = ((data0 >> 54) & 0x3f);
    if (write_callback_) write_callback_();
    return true;
  }

  void reset(
      
      ) {
    ctr_amt_idx_ = 0;
    ctr_ld_src_ = 0;
    ctr_load_ = 0;
    done_ = 0;
    shift_amt_ = 0;
    lookup_offset_16_ = 0;
    lookup_offset_8_[0] = 0;
    lookup_offset_8_[1] = 0;
    ld_lookup_16_ = 0;
    ld_lookup_8_[0] = 0;
    ld_lookup_8_[1] = 0;
    nxt_state_mask_ = 0;
    nxt_state_ = 0;
    buf_req_ = 0;
    if (write_callback_) write_callback_();
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    r += indent_string + std::string("PrsrMlEaRowMutable") + ":\n";
    r += indent_string + "  " + std::string("ctr_amt_idx") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(ctr_amt_idx_) ) + "\n";
    all_zeros &= (0 == ctr_amt_idx_);
    r += indent_string + "  " + std::string("ctr_ld_src") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(ctr_ld_src_) ) + "\n";
    all_zeros &= (0 == ctr_ld_src_);
    r += indent_string + "  " + std::string("ctr_load") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(ctr_load_) ) + "\n";
    all_zeros &= (0 == ctr_load_);
    r += indent_string + "  " + std::string("done") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(done_) ) + "\n";
    all_zeros &= (0 == done_);
    r += indent_string + "  " + std::string("shift_amt") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(shift_amt_) ) + "\n";
    all_zeros &= (0 == shift_amt_);
    r += indent_string + "  " + std::string("lookup_offset_16") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(lookup_offset_16_) ) + "\n";
    all_zeros &= (0 == lookup_offset_16_);
    for (uint32_t f=0;f<2;++f) {
      r += indent_string + "  " + std::string("lookup_offset_8") + "["+boost::lexical_cast<std::string>(f)+"]"+ ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(lookup_offset_8_[f]) ) + "\n";
      all_zeros &= (0 == lookup_offset_8_[f]);
    }
    r += indent_string + "  " + std::string("ld_lookup_16") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(ld_lookup_16_) ) + "\n";
    all_zeros &= (0 == ld_lookup_16_);
    for (uint32_t f=0;f<2;++f) {
      r += indent_string + "  " + std::string("ld_lookup_8") + "["+boost::lexical_cast<std::string>(f)+"]"+ ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(ld_lookup_8_[f]) ) + "\n";
      all_zeros &= (0 == ld_lookup_8_[f]);
    }
    r += indent_string + "  " + std::string("nxt_state_mask") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(nxt_state_mask_) ) + "\n";
    all_zeros &= (0 == nxt_state_mask_);
    r += indent_string + "  " + std::string("nxt_state") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(nxt_state_) ) + "\n";
    all_zeros &= (0 == nxt_state_);
    r += indent_string + "  " + std::string("buf_req") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(buf_req_) ) + "\n";
    all_zeros &= (0 == buf_req_);
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
    r += indent_string + std::string("PrsrMlEaRowMutable") + ":\n";
    r += indent_string + "  " + std::string("ctr_amt_idx") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(ctr_amt_idx_) ) + "\n";
    all_zeros &= (0 == ctr_amt_idx_);
    r += indent_string + "  " + std::string("ctr_ld_src") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(ctr_ld_src_) ) + "\n";
    all_zeros &= (0 == ctr_ld_src_);
    r += indent_string + "  " + std::string("ctr_load") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(ctr_load_) ) + "\n";
    all_zeros &= (0 == ctr_load_);
    r += indent_string + "  " + std::string("done") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(done_) ) + "\n";
    all_zeros &= (0 == done_);
    r += indent_string + "  " + std::string("shift_amt") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(shift_amt_) ) + "\n";
    all_zeros &= (0 == shift_amt_);
    r += indent_string + "  " + std::string("lookup_offset_16") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(lookup_offset_16_) ) + "\n";
    all_zeros &= (0 == lookup_offset_16_);
    for (uint32_t f=0;f<2;++f) {
      r += indent_string + "  " + std::string("lookup_offset_8") + "["+boost::lexical_cast<std::string>(f)+"]"+ ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(lookup_offset_8_[f]) ) + "\n";
      all_zeros &= (0 == lookup_offset_8_[f]);
    }
    r += indent_string + "  " + std::string("ld_lookup_16") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(ld_lookup_16_) ) + "\n";
    all_zeros &= (0 == ld_lookup_16_);
    for (uint32_t f=0;f<2;++f) {
      r += indent_string + "  " + std::string("ld_lookup_8") + "["+boost::lexical_cast<std::string>(f)+"]"+ ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(ld_lookup_8_[f]) ) + "\n";
      all_zeros &= (0 == ld_lookup_8_[f]);
    }
    r += indent_string + "  " + std::string("nxt_state_mask") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(nxt_state_mask_) ) + "\n";
    all_zeros &= (0 == nxt_state_mask_);
    r += indent_string + "  " + std::string("nxt_state") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(nxt_state_) ) + "\n";
    all_zeros &= (0 == nxt_state_);
    r += indent_string + "  " + std::string("buf_req") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(buf_req_) ) + "\n";
    all_zeros &= (0 == buf_req_);
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  uint8_t ctr_amt_idx_;
  uint8_t ctr_ld_src_;
  uint8_t ctr_load_;
  uint8_t done_;
  uint8_t shift_amt_;
  uint8_t lookup_offset_16_;
  std::array< uint8_t, 2 > lookup_offset_8_;
  uint8_t ld_lookup_16_;
  std::array< uint8_t, 2 > ld_lookup_8_;
  uint8_t nxt_state_mask_;
  uint8_t nxt_state_;
  uint8_t buf_req_;
private:
  static uint64_t StartOffset(
      int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, int index_prsr_ml_ea_row
      ) {
    uint64_t offset=0;
    offset += 0x20000000000; // to get to pipes
    assert(index_pipe_addrmap < 4);
    offset += index_pipe_addrmap * 0x2000000000; // pipe_addrmap[]
    switch (selector_pipe_addrmap) {
      case kEPrsr:
        offset += 0x1c80040000; // to get to e_prsr
        assert(index_prsr_mem_main_rspec < 18);
        offset += index_prsr_mem_main_rspec * 0x2000; // prsr_mem_main_rspec[]
        offset += 0x200; // to get to ml_ea_row
        assert(index_prsr_ml_ea_row < 256);
        offset += index_prsr_ml_ea_row * 0x1; // prsr_ml_ea_row[]
        break;
      case kIPrsr:
        offset += 0x1c80000000; // to get to i_prsr
        assert(index_prsr_mem_main_rspec < 18);
        offset += index_prsr_mem_main_rspec * 0x2000; // prsr_mem_main_rspec[]
        offset += 0x200; // to get to ml_ea_row
        assert(index_prsr_ml_ea_row < 256);
        offset += index_prsr_ml_ea_row * 0x1; // prsr_ml_ea_row[]
        break;
      default:
        assert(0);
        break;
    }
    return offset;
  }

};








class PrsrMlEaRowArray : public model_core::RegisterBlockIndirect<RegisterArrayCallback> {
public:
  enum PipeAddrmapEnum {
    kEPrsr,
    kIPrsr
  };
public:
  PrsrMlEaRowArray(
      int chipNumber, int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, RegisterArrayCallback& write_callback = 0, RegisterArrayCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec), 1 * ArraySize(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, -1), false, write_callback, read_callback, std::string("PrsrMlEaRowArray")+":"+boost::lexical_cast<std::string>(index_pipe_addrmap) + "," + boost::lexical_cast<std::string>(selector_pipe_addrmap) + "," + boost::lexical_cast<std::string>(index_prsr_mem_main_rspec)),
    array(RealArraySize(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, -1)),
    size0_(ArraySize(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, 0)),
    real_size0_(RealArraySize(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, 0))
    {
    }
public:







  uint8_t &ctr_amt_idx(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].ctr_amt_idx();
  }





  uint8_t &ctr_ld_src(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].ctr_ld_src();
  }





  uint8_t &ctr_load(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].ctr_load();
  }






  uint8_t &done(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].done();
  }





  uint8_t &shift_amt(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].shift_amt();
  }






  uint8_t &lookup_offset_16(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].lookup_offset_16();
  }








  uint8_t &lookup_offset_8(uint32_t a0,int j0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].lookup_offset_8(j0);
  }




  uint8_t &ld_lookup_16(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].ld_lookup_16();
  }






  uint8_t &ld_lookup_8(uint32_t a0,int j0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].ld_lookup_8(j0);
  }





  uint8_t &nxt_state_mask(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].nxt_state_mask();
  }




  uint8_t &nxt_state(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].nxt_state();
  }






  uint8_t &buf_req(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].buf_req();
  }
  bool calculate_index(
      uint64_t* offset, uint32_t* a0
      ) const {
    int i = (*offset)/1;
    (*offset) -= (i*1);
    uint32_t t = i;
    *a0 = t;
    bool in_bounds = true;
    in_bounds &= CheckArrayBounds(*a0,real_size0_,size0_,*offset,0,true);
    return in_bounds;
  }


  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    if (read_callback_) read_callback_(a0);
    array[a0].read(offset,data0,data1,T);
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    array[a0].write(offset,data0,data1,T);
    if (write_callback_) write_callback_(a0);
    return true;
  }

  void reset(
      
      ) {
    for (uint32_t i=0;i<array.size();++i) {
      array[i].reset();
      int t = i;
      int a0 = t;
      if (write_callback_) write_callback_(a0);
    }
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    all_zeros=false;
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return "OUT_OF_BOUNDS";
    std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
    if (! r_s.empty()) {
      r += indent_string + std::string("PrsrMlEaRowArray") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
    }
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
    all_zeros=false;
    for (uint32_t a0=0;a0<real_size0_;++a0) {
      std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
      if (! r_s.empty()) {
        r += indent_string + std::string("PrsrMlEaRowArray") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
      }
    }
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  std::vector<PrsrMlEaRow> array;
  uint32_t size0_;
  uint32_t real_size0_;
private:
  static uint64_t StartOffset(
      int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec
      ) {
    uint64_t offset=0;
    offset += 0x20000000000; // to get to pipes
    assert(index_pipe_addrmap < 4);
    offset += index_pipe_addrmap * 0x2000000000; // pipe_addrmap[]
    switch (selector_pipe_addrmap) {
      case kEPrsr:
        offset += 0x1c80040000; // to get to e_prsr
        assert(index_prsr_mem_main_rspec < 18);
        offset += index_prsr_mem_main_rspec * 0x2000; // prsr_mem_main_rspec[]
        offset += 0x200; // to get to ml_ea_row
        break;
      case kIPrsr:
        offset += 0x1c80000000; // to get to i_prsr
        assert(index_prsr_mem_main_rspec < 18);
        offset += index_prsr_mem_main_rspec * 0x2000; // prsr_mem_main_rspec[]
        offset += 0x200; // to get to ml_ea_row
        break;
      default:
        assert(0);
        break;
    }
    return offset;
  }

  static int ArraySize(
      int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, int dimension
      ) {
    switch (selector_pipe_addrmap) {
      case kEPrsr:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kIPrsr:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
  }

  static int RealArraySize(
      int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, int dimension
      ) {
    switch (selector_pipe_addrmap) {
      case kEPrsr:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kIPrsr:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
  }

};








class PrsrMlEaRowArrayMutable : public model_core::RegisterBlockIndirect<RegisterArrayCallback> {
public:
  enum PipeAddrmapEnum {
    kEPrsr,
    kIPrsr
  };
public:
  PrsrMlEaRowArrayMutable(
      int chipNumber, int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, RegisterArrayCallback& write_callback = 0, RegisterArrayCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec), 1 * ArraySize(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, -1), true, write_callback, read_callback, std::string("PrsrMlEaRowArrayMutable")+":"+boost::lexical_cast<std::string>(index_pipe_addrmap) + "," + boost::lexical_cast<std::string>(selector_pipe_addrmap) + "," + boost::lexical_cast<std::string>(index_prsr_mem_main_rspec)),
    array(RealArraySize(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, -1)),
    size0_(ArraySize(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, 0)),
    real_size0_(RealArraySize(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, 0))
    {
    }
public:







  uint8_t ctr_amt_idx(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].ctr_amt_idx();
  }
  void ctr_amt_idx(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].ctr_amt_idx(v);
  }





  uint8_t ctr_ld_src(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].ctr_ld_src();
  }
  void ctr_ld_src(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].ctr_ld_src(v);
  }





  uint8_t ctr_load(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].ctr_load();
  }
  void ctr_load(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].ctr_load(v);
  }






  uint8_t done(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].done();
  }
  void done(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].done(v);
  }





  uint8_t shift_amt(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].shift_amt();
  }
  void shift_amt(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].shift_amt(v);
  }






  uint8_t lookup_offset_16(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].lookup_offset_16();
  }
  void lookup_offset_16(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].lookup_offset_16(v);
  }








  uint8_t lookup_offset_8(uint32_t a0,int j0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].lookup_offset_8(j0);
  }
  void lookup_offset_8(uint32_t a0,int j0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].lookup_offset_8(j0,v);
  }




  uint8_t ld_lookup_16(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].ld_lookup_16();
  }
  void ld_lookup_16(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].ld_lookup_16(v);
  }






  uint8_t ld_lookup_8(uint32_t a0,int j0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].ld_lookup_8(j0);
  }
  void ld_lookup_8(uint32_t a0,int j0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].ld_lookup_8(j0,v);
  }





  uint8_t nxt_state_mask(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].nxt_state_mask();
  }
  void nxt_state_mask(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].nxt_state_mask(v);
  }




  uint8_t nxt_state(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].nxt_state();
  }
  void nxt_state(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].nxt_state(v);
  }






  uint8_t buf_req(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].buf_req();
  }
  void buf_req(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].buf_req(v);
  }
  bool calculate_index(
      uint64_t* offset, uint32_t* a0
      ) const {
    int i = (*offset)/1;
    (*offset) -= (i*1);
    uint32_t t = i;
    *a0 = t;
    bool in_bounds = true;
    in_bounds &= CheckArrayBounds(*a0,real_size0_,size0_,*offset,0,true);
    return in_bounds;
  }


  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    if (read_callback_) read_callback_(a0);
    array[a0].read(offset,data0,data1,T);
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    array[a0].write(offset,data0,data1,T);
    if (write_callback_) write_callback_(a0);
    return true;
  }

  void reset(
      
      ) {
    for (uint32_t i=0;i<array.size();++i) {
      array[i].reset();
      int t = i;
      int a0 = t;
      if (write_callback_) write_callback_(a0);
    }
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    all_zeros=false;
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return "OUT_OF_BOUNDS";
    std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
    if (! r_s.empty()) {
      r += indent_string + std::string("PrsrMlEaRowArrayMutable") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
    }
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
    all_zeros=false;
    for (uint32_t a0=0;a0<real_size0_;++a0) {
      std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
      if (! r_s.empty()) {
        r += indent_string + std::string("PrsrMlEaRowArrayMutable") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
      }
    }
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  std::vector<PrsrMlEaRowMutable> array;
  uint32_t size0_;
  uint32_t real_size0_;
private:
  static uint64_t StartOffset(
      int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec
      ) {
    uint64_t offset=0;
    offset += 0x20000000000; // to get to pipes
    assert(index_pipe_addrmap < 4);
    offset += index_pipe_addrmap * 0x2000000000; // pipe_addrmap[]
    switch (selector_pipe_addrmap) {
      case kEPrsr:
        offset += 0x1c80040000; // to get to e_prsr
        assert(index_prsr_mem_main_rspec < 18);
        offset += index_prsr_mem_main_rspec * 0x2000; // prsr_mem_main_rspec[]
        offset += 0x200; // to get to ml_ea_row
        break;
      case kIPrsr:
        offset += 0x1c80000000; // to get to i_prsr
        assert(index_prsr_mem_main_rspec < 18);
        offset += index_prsr_mem_main_rspec * 0x2000; // prsr_mem_main_rspec[]
        offset += 0x200; // to get to ml_ea_row
        break;
      default:
        assert(0);
        break;
    }
    return offset;
  }

  static int ArraySize(
      int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, int dimension
      ) {
    switch (selector_pipe_addrmap) {
      case kEPrsr:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kIPrsr:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
  }

  static int RealArraySize(
      int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, int dimension
      ) {
    switch (selector_pipe_addrmap) {
      case kEPrsr:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kIPrsr:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
  }

};








class PrsrMlTcamRow : public model_core::RegisterBlockIndirect<RegisterCallback> {
public:
  enum PipeAddrmapEnum {
    kEPrsr,
    kIPrsr
  };
  enum PrsrMemMainRspecEnum {
    kMlTcamRowWord0,
    kMlTcamRowWord1
  };
public:
  PrsrMlTcamRow(
      int chipNumber, int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, PrsrMemMainRspecEnum selector_prsr_mem_main_rspec, int index_prsr_ml_tcam_row, RegisterCallback& write_callback = 0, RegisterCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, selector_prsr_mem_main_rspec, index_prsr_ml_tcam_row), 1, false, write_callback, read_callback, std::string("PrsrMlTcamRow")+":"+boost::lexical_cast<std::string>(index_pipe_addrmap) + "," + boost::lexical_cast<std::string>(selector_pipe_addrmap) + "," + boost::lexical_cast<std::string>(index_prsr_mem_main_rspec) + "," + boost::lexical_cast<std::string>(selector_prsr_mem_main_rspec) + "," + boost::lexical_cast<std::string>(index_prsr_ml_tcam_row))
    {
    }
  PrsrMlTcamRow(
      
  )
    : RegisterBlockIndirect(0, 0, 0, false, 0, 0, "PrsrMlTcamRow")
    {
    }
public:




  uint16_t &lookup_16() { return lookup_16_; }






  uint8_t &lookup_8(int j0) { return lookup_8_[j0]; }




  uint8_t &curr_state() { return curr_state_; }





  uint8_t &ctr_zero() { return ctr_zero_; }





  uint8_t &ctr_neg() { return ctr_neg_; }





  uint8_t &ver_0() { return ver_0_; }





  uint8_t &ver_1() { return ver_1_; }

  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    if (read_callback_) read_callback_();
    *data0 = static_cast<uint64_t>(lookup_16_);
    *data0 |= (static_cast<uint64_t>(lookup_8_[0]) << 16);
    *data0 |= (static_cast<uint64_t>(lookup_8_[1]) << 24);
    *data0 |= (static_cast<uint64_t>(curr_state_) << 32);
    *data0 |= ((static_cast<uint64_t>(ctr_zero_) & 0x1) << 40);
    *data0 |= ((static_cast<uint64_t>(ctr_neg_) & 0x1) << 41);
    *data0 |= ((static_cast<uint64_t>(ver_0_) & 0x1) << 42);
    *data0 |= ((static_cast<uint64_t>(ver_1_) & 0x1) << 43);
    *data1 = 0;
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    lookup_16_ = data0;
    lookup_8_[0] = (data0 >> 16);
    lookup_8_[1] = (data0 >> 24);
    curr_state_ = (data0 >> 32);
    ctr_zero_ = ((data0 >> 40) & 0x1);
    ctr_neg_ = ((data0 >> 41) & 0x1);
    ver_0_ = ((data0 >> 42) & 0x1);
    ver_1_ = ((data0 >> 43) & 0x1);
    if (write_callback_) write_callback_();
    return true;
  }

  void reset(
      
      ) {
    lookup_16_ = 0;
    lookup_8_[0] = 0;
    lookup_8_[1] = 0;
    curr_state_ = 0;
    ctr_zero_ = 0;
    ctr_neg_ = 0;
    ver_0_ = 0;
    ver_1_ = 0;
    if (write_callback_) write_callback_();
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    r += indent_string + std::string("PrsrMlTcamRow") + ":\n";
    r += indent_string + "  " + std::string("lookup_16") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(lookup_16_) ) + "\n";
    all_zeros &= (0 == lookup_16_);
    for (uint32_t f=0;f<2;++f) {
      r += indent_string + "  " + std::string("lookup_8") + "["+boost::lexical_cast<std::string>(f)+"]"+ ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(lookup_8_[f]) ) + "\n";
      all_zeros &= (0 == lookup_8_[f]);
    }
    r += indent_string + "  " + std::string("curr_state") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(curr_state_) ) + "\n";
    all_zeros &= (0 == curr_state_);
    r += indent_string + "  " + std::string("ctr_zero") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(ctr_zero_) ) + "\n";
    all_zeros &= (0 == ctr_zero_);
    r += indent_string + "  " + std::string("ctr_neg") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(ctr_neg_) ) + "\n";
    all_zeros &= (0 == ctr_neg_);
    r += indent_string + "  " + std::string("ver_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(ver_0_) ) + "\n";
    all_zeros &= (0 == ver_0_);
    r += indent_string + "  " + std::string("ver_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(ver_1_) ) + "\n";
    all_zeros &= (0 == ver_1_);
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
    r += indent_string + std::string("PrsrMlTcamRow") + ":\n";
    r += indent_string + "  " + std::string("lookup_16") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(lookup_16_) ) + "\n";
    all_zeros &= (0 == lookup_16_);
    for (uint32_t f=0;f<2;++f) {
      r += indent_string + "  " + std::string("lookup_8") + "["+boost::lexical_cast<std::string>(f)+"]"+ ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(lookup_8_[f]) ) + "\n";
      all_zeros &= (0 == lookup_8_[f]);
    }
    r += indent_string + "  " + std::string("curr_state") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(curr_state_) ) + "\n";
    all_zeros &= (0 == curr_state_);
    r += indent_string + "  " + std::string("ctr_zero") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(ctr_zero_) ) + "\n";
    all_zeros &= (0 == ctr_zero_);
    r += indent_string + "  " + std::string("ctr_neg") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(ctr_neg_) ) + "\n";
    all_zeros &= (0 == ctr_neg_);
    r += indent_string + "  " + std::string("ver_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(ver_0_) ) + "\n";
    all_zeros &= (0 == ver_0_);
    r += indent_string + "  " + std::string("ver_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(ver_1_) ) + "\n";
    all_zeros &= (0 == ver_1_);
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  uint16_t lookup_16_;
  std::array< uint8_t, 2 > lookup_8_;
  uint8_t curr_state_;
  uint8_t ctr_zero_;
  uint8_t ctr_neg_;
  uint8_t ver_0_;
  uint8_t ver_1_;
private:
  static uint64_t StartOffset(
      int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, PrsrMemMainRspecEnum selector_prsr_mem_main_rspec, int index_prsr_ml_tcam_row
      ) {
    uint64_t offset=0;
    offset += 0x20000000000; // to get to pipes
    assert(index_pipe_addrmap < 4);
    offset += index_pipe_addrmap * 0x2000000000; // pipe_addrmap[]
    switch (selector_pipe_addrmap) {
      case kEPrsr:
        offset += 0x1c80040000; // to get to e_prsr
        assert(index_prsr_mem_main_rspec < 18);
        offset += index_prsr_mem_main_rspec * 0x2000; // prsr_mem_main_rspec[]
        switch (selector_prsr_mem_main_rspec) {
          case kMlTcamRowWord0:
            assert(index_prsr_ml_tcam_row < 256);
            offset += index_prsr_ml_tcam_row * 0x1; // prsr_ml_tcam_row[]
            break;
          case kMlTcamRowWord1:
            offset += 0x100; // to get to ml_tcam_row_word1
            assert(index_prsr_ml_tcam_row < 256);
            offset += index_prsr_ml_tcam_row * 0x1; // prsr_ml_tcam_row[]
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kIPrsr:
        offset += 0x1c80000000; // to get to i_prsr
        assert(index_prsr_mem_main_rspec < 18);
        offset += index_prsr_mem_main_rspec * 0x2000; // prsr_mem_main_rspec[]
        switch (selector_prsr_mem_main_rspec) {
          case kMlTcamRowWord0:
            assert(index_prsr_ml_tcam_row < 256);
            offset += index_prsr_ml_tcam_row * 0x1; // prsr_ml_tcam_row[]
            break;
          case kMlTcamRowWord1:
            offset += 0x100; // to get to ml_tcam_row_word1
            assert(index_prsr_ml_tcam_row < 256);
            offset += index_prsr_ml_tcam_row * 0x1; // prsr_ml_tcam_row[]
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
    return offset;
  }

};








class PrsrMlTcamRowMutable : public model_core::RegisterBlockIndirect<RegisterCallback> {
public:
  enum PipeAddrmapEnum {
    kEPrsr,
    kIPrsr
  };
  enum PrsrMemMainRspecEnum {
    kMlTcamRowWord0,
    kMlTcamRowWord1
  };
public:
  PrsrMlTcamRowMutable(
      int chipNumber, int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, PrsrMemMainRspecEnum selector_prsr_mem_main_rspec, int index_prsr_ml_tcam_row, RegisterCallback& write_callback = 0, RegisterCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, selector_prsr_mem_main_rspec, index_prsr_ml_tcam_row), 1, true, write_callback, read_callback, std::string("PrsrMlTcamRowMutable")+":"+boost::lexical_cast<std::string>(index_pipe_addrmap) + "," + boost::lexical_cast<std::string>(selector_pipe_addrmap) + "," + boost::lexical_cast<std::string>(index_prsr_mem_main_rspec) + "," + boost::lexical_cast<std::string>(selector_prsr_mem_main_rspec) + "," + boost::lexical_cast<std::string>(index_prsr_ml_tcam_row))
    {
    }
  PrsrMlTcamRowMutable(
      
  )
    : RegisterBlockIndirect(0, 0, 0, true, 0, 0, "PrsrMlTcamRowMutable")
    {
    }
public:




  uint16_t lookup_16() { return lookup_16_; }
  void lookup_16(const uint16_t &v) { lookup_16_=v; }






  uint8_t lookup_8(int j0) { return lookup_8_[j0]; }
  void lookup_8(int j0,const uint8_t &v) { lookup_8_[j0]=v; }




  uint8_t curr_state() { return curr_state_; }
  void curr_state(const uint8_t &v) { curr_state_=v; }





  uint8_t ctr_zero() { return ctr_zero_; }
  void ctr_zero(const uint8_t &v) { ctr_zero_=v; }





  uint8_t ctr_neg() { return ctr_neg_; }
  void ctr_neg(const uint8_t &v) { ctr_neg_=v; }





  uint8_t ver_0() { return ver_0_; }
  void ver_0(const uint8_t &v) { ver_0_=v; }





  uint8_t ver_1() { return ver_1_; }
  void ver_1(const uint8_t &v) { ver_1_=v; }

  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    if (read_callback_) read_callback_();
    *data0 = static_cast<uint64_t>(lookup_16_);
    *data0 |= (static_cast<uint64_t>(lookup_8_[0]) << 16);
    *data0 |= (static_cast<uint64_t>(lookup_8_[1]) << 24);
    *data0 |= (static_cast<uint64_t>(curr_state_) << 32);
    *data0 |= ((static_cast<uint64_t>(ctr_zero_) & 0x1) << 40);
    *data0 |= ((static_cast<uint64_t>(ctr_neg_) & 0x1) << 41);
    *data0 |= ((static_cast<uint64_t>(ver_0_) & 0x1) << 42);
    *data0 |= ((static_cast<uint64_t>(ver_1_) & 0x1) << 43);
    *data1 = 0;
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    lookup_16_ = data0;
    lookup_8_[0] = (data0 >> 16);
    lookup_8_[1] = (data0 >> 24);
    curr_state_ = (data0 >> 32);
    ctr_zero_ = ((data0 >> 40) & 0x1);
    ctr_neg_ = ((data0 >> 41) & 0x1);
    ver_0_ = ((data0 >> 42) & 0x1);
    ver_1_ = ((data0 >> 43) & 0x1);
    if (write_callback_) write_callback_();
    return true;
  }

  void reset(
      
      ) {
    lookup_16_ = 0;
    lookup_8_[0] = 0;
    lookup_8_[1] = 0;
    curr_state_ = 0;
    ctr_zero_ = 0;
    ctr_neg_ = 0;
    ver_0_ = 0;
    ver_1_ = 0;
    if (write_callback_) write_callback_();
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    r += indent_string + std::string("PrsrMlTcamRowMutable") + ":\n";
    r += indent_string + "  " + std::string("lookup_16") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(lookup_16_) ) + "\n";
    all_zeros &= (0 == lookup_16_);
    for (uint32_t f=0;f<2;++f) {
      r += indent_string + "  " + std::string("lookup_8") + "["+boost::lexical_cast<std::string>(f)+"]"+ ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(lookup_8_[f]) ) + "\n";
      all_zeros &= (0 == lookup_8_[f]);
    }
    r += indent_string + "  " + std::string("curr_state") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(curr_state_) ) + "\n";
    all_zeros &= (0 == curr_state_);
    r += indent_string + "  " + std::string("ctr_zero") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(ctr_zero_) ) + "\n";
    all_zeros &= (0 == ctr_zero_);
    r += indent_string + "  " + std::string("ctr_neg") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(ctr_neg_) ) + "\n";
    all_zeros &= (0 == ctr_neg_);
    r += indent_string + "  " + std::string("ver_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(ver_0_) ) + "\n";
    all_zeros &= (0 == ver_0_);
    r += indent_string + "  " + std::string("ver_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(ver_1_) ) + "\n";
    all_zeros &= (0 == ver_1_);
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
    r += indent_string + std::string("PrsrMlTcamRowMutable") + ":\n";
    r += indent_string + "  " + std::string("lookup_16") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(lookup_16_) ) + "\n";
    all_zeros &= (0 == lookup_16_);
    for (uint32_t f=0;f<2;++f) {
      r += indent_string + "  " + std::string("lookup_8") + "["+boost::lexical_cast<std::string>(f)+"]"+ ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(lookup_8_[f]) ) + "\n";
      all_zeros &= (0 == lookup_8_[f]);
    }
    r += indent_string + "  " + std::string("curr_state") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(curr_state_) ) + "\n";
    all_zeros &= (0 == curr_state_);
    r += indent_string + "  " + std::string("ctr_zero") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(ctr_zero_) ) + "\n";
    all_zeros &= (0 == ctr_zero_);
    r += indent_string + "  " + std::string("ctr_neg") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(ctr_neg_) ) + "\n";
    all_zeros &= (0 == ctr_neg_);
    r += indent_string + "  " + std::string("ver_0") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(ver_0_) ) + "\n";
    all_zeros &= (0 == ver_0_);
    r += indent_string + "  " + std::string("ver_1") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(ver_1_) ) + "\n";
    all_zeros &= (0 == ver_1_);
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  uint16_t lookup_16_;
  std::array< uint8_t, 2 > lookup_8_;
  uint8_t curr_state_;
  uint8_t ctr_zero_;
  uint8_t ctr_neg_;
  uint8_t ver_0_;
  uint8_t ver_1_;
private:
  static uint64_t StartOffset(
      int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, PrsrMemMainRspecEnum selector_prsr_mem_main_rspec, int index_prsr_ml_tcam_row
      ) {
    uint64_t offset=0;
    offset += 0x20000000000; // to get to pipes
    assert(index_pipe_addrmap < 4);
    offset += index_pipe_addrmap * 0x2000000000; // pipe_addrmap[]
    switch (selector_pipe_addrmap) {
      case kEPrsr:
        offset += 0x1c80040000; // to get to e_prsr
        assert(index_prsr_mem_main_rspec < 18);
        offset += index_prsr_mem_main_rspec * 0x2000; // prsr_mem_main_rspec[]
        switch (selector_prsr_mem_main_rspec) {
          case kMlTcamRowWord0:
            assert(index_prsr_ml_tcam_row < 256);
            offset += index_prsr_ml_tcam_row * 0x1; // prsr_ml_tcam_row[]
            break;
          case kMlTcamRowWord1:
            offset += 0x100; // to get to ml_tcam_row_word1
            assert(index_prsr_ml_tcam_row < 256);
            offset += index_prsr_ml_tcam_row * 0x1; // prsr_ml_tcam_row[]
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kIPrsr:
        offset += 0x1c80000000; // to get to i_prsr
        assert(index_prsr_mem_main_rspec < 18);
        offset += index_prsr_mem_main_rspec * 0x2000; // prsr_mem_main_rspec[]
        switch (selector_prsr_mem_main_rspec) {
          case kMlTcamRowWord0:
            assert(index_prsr_ml_tcam_row < 256);
            offset += index_prsr_ml_tcam_row * 0x1; // prsr_ml_tcam_row[]
            break;
          case kMlTcamRowWord1:
            offset += 0x100; // to get to ml_tcam_row_word1
            assert(index_prsr_ml_tcam_row < 256);
            offset += index_prsr_ml_tcam_row * 0x1; // prsr_ml_tcam_row[]
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
    return offset;
  }

};








class PrsrMlTcamRowArray : public model_core::RegisterBlockIndirect<RegisterArrayCallback> {
public:
  enum PipeAddrmapEnum {
    kEPrsr,
    kIPrsr
  };
  enum PrsrMemMainRspecEnum {
    kMlTcamRowWord0,
    kMlTcamRowWord1
  };
public:
  PrsrMlTcamRowArray(
      int chipNumber, int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, PrsrMemMainRspecEnum selector_prsr_mem_main_rspec, RegisterArrayCallback& write_callback = 0, RegisterArrayCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, selector_prsr_mem_main_rspec), 1 * ArraySize(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, selector_prsr_mem_main_rspec, -1), false, write_callback, read_callback, std::string("PrsrMlTcamRowArray")+":"+boost::lexical_cast<std::string>(index_pipe_addrmap) + "," + boost::lexical_cast<std::string>(selector_pipe_addrmap) + "," + boost::lexical_cast<std::string>(index_prsr_mem_main_rspec) + "," + boost::lexical_cast<std::string>(selector_prsr_mem_main_rspec)),
    array(RealArraySize(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, selector_prsr_mem_main_rspec, -1)),
    size0_(ArraySize(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, selector_prsr_mem_main_rspec, 0)),
    real_size0_(RealArraySize(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, selector_prsr_mem_main_rspec, 0))
    {
    }
public:




  uint16_t &lookup_16(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].lookup_16();
  }






  uint8_t &lookup_8(uint32_t a0,int j0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].lookup_8(j0);
  }




  uint8_t &curr_state(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].curr_state();
  }





  uint8_t &ctr_zero(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].ctr_zero();
  }





  uint8_t &ctr_neg(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].ctr_neg();
  }





  uint8_t &ver_0(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].ver_0();
  }





  uint8_t &ver_1(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].ver_1();
  }
  bool calculate_index(
      uint64_t* offset, uint32_t* a0
      ) const {
    int i = (*offset)/1;
    (*offset) -= (i*1);
    uint32_t t = i;
    *a0 = t;
    bool in_bounds = true;
    in_bounds &= CheckArrayBounds(*a0,real_size0_,size0_,*offset,0,true);
    return in_bounds;
  }


  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    if (read_callback_) read_callback_(a0);
    array[a0].read(offset,data0,data1,T);
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    array[a0].write(offset,data0,data1,T);
    if (write_callback_) write_callback_(a0);
    return true;
  }

  void reset(
      
      ) {
    for (uint32_t i=0;i<array.size();++i) {
      array[i].reset();
      int t = i;
      int a0 = t;
      if (write_callback_) write_callback_(a0);
    }
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    all_zeros=false;
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return "OUT_OF_BOUNDS";
    std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
    if (! r_s.empty()) {
      r += indent_string + std::string("PrsrMlTcamRowArray") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
    }
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
    all_zeros=false;
    for (uint32_t a0=0;a0<real_size0_;++a0) {
      std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
      if (! r_s.empty()) {
        r += indent_string + std::string("PrsrMlTcamRowArray") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
      }
    }
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  std::vector<PrsrMlTcamRow> array;
  uint32_t size0_;
  uint32_t real_size0_;
private:
  static uint64_t StartOffset(
      int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, PrsrMemMainRspecEnum selector_prsr_mem_main_rspec
      ) {
    uint64_t offset=0;
    offset += 0x20000000000; // to get to pipes
    assert(index_pipe_addrmap < 4);
    offset += index_pipe_addrmap * 0x2000000000; // pipe_addrmap[]
    switch (selector_pipe_addrmap) {
      case kEPrsr:
        offset += 0x1c80040000; // to get to e_prsr
        assert(index_prsr_mem_main_rspec < 18);
        offset += index_prsr_mem_main_rspec * 0x2000; // prsr_mem_main_rspec[]
        switch (selector_prsr_mem_main_rspec) {
          case kMlTcamRowWord0:
            break;
          case kMlTcamRowWord1:
            offset += 0x100; // to get to ml_tcam_row_word1
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kIPrsr:
        offset += 0x1c80000000; // to get to i_prsr
        assert(index_prsr_mem_main_rspec < 18);
        offset += index_prsr_mem_main_rspec * 0x2000; // prsr_mem_main_rspec[]
        switch (selector_prsr_mem_main_rspec) {
          case kMlTcamRowWord0:
            break;
          case kMlTcamRowWord1:
            offset += 0x100; // to get to ml_tcam_row_word1
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
    return offset;
  }

  static int ArraySize(
      int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, PrsrMemMainRspecEnum selector_prsr_mem_main_rspec, int dimension
      ) {
    switch (selector_pipe_addrmap) {
      case kEPrsr:
        switch (selector_prsr_mem_main_rspec) {
          case kMlTcamRowWord0:
            switch (dimension) {
              case -1:
                return 256;
                break;
              case 0:
                return 256;
                break;
              default:
                assert(0);
                break;
            }
            break;
          case kMlTcamRowWord1:
            switch (dimension) {
              case -1:
                return 256;
                break;
              case 0:
                return 256;
                break;
              default:
                assert(0);
                break;
            }
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kIPrsr:
        switch (selector_prsr_mem_main_rspec) {
          case kMlTcamRowWord0:
            switch (dimension) {
              case -1:
                return 256;
                break;
              case 0:
                return 256;
                break;
              default:
                assert(0);
                break;
            }
            break;
          case kMlTcamRowWord1:
            switch (dimension) {
              case -1:
                return 256;
                break;
              case 0:
                return 256;
                break;
              default:
                assert(0);
                break;
            }
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
  }

  static int RealArraySize(
      int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, PrsrMemMainRspecEnum selector_prsr_mem_main_rspec, int dimension
      ) {
    switch (selector_pipe_addrmap) {
      case kEPrsr:
        switch (selector_prsr_mem_main_rspec) {
          case kMlTcamRowWord0:
            switch (dimension) {
              case -1:
                return 256;
                break;
              case 0:
                return 256;
                break;
              default:
                assert(0);
                break;
            }
            break;
          case kMlTcamRowWord1:
            switch (dimension) {
              case -1:
                return 256;
                break;
              case 0:
                return 256;
                break;
              default:
                assert(0);
                break;
            }
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kIPrsr:
        switch (selector_prsr_mem_main_rspec) {
          case kMlTcamRowWord0:
            switch (dimension) {
              case -1:
                return 256;
                break;
              case 0:
                return 256;
                break;
              default:
                assert(0);
                break;
            }
            break;
          case kMlTcamRowWord1:
            switch (dimension) {
              case -1:
                return 256;
                break;
              case 0:
                return 256;
                break;
              default:
                assert(0);
                break;
            }
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
  }

};








class PrsrMlTcamRowArrayMutable : public model_core::RegisterBlockIndirect<RegisterArrayCallback> {
public:
  enum PipeAddrmapEnum {
    kEPrsr,
    kIPrsr
  };
  enum PrsrMemMainRspecEnum {
    kMlTcamRowWord0,
    kMlTcamRowWord1
  };
public:
  PrsrMlTcamRowArrayMutable(
      int chipNumber, int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, PrsrMemMainRspecEnum selector_prsr_mem_main_rspec, RegisterArrayCallback& write_callback = 0, RegisterArrayCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, selector_prsr_mem_main_rspec), 1 * ArraySize(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, selector_prsr_mem_main_rspec, -1), true, write_callback, read_callback, std::string("PrsrMlTcamRowArrayMutable")+":"+boost::lexical_cast<std::string>(index_pipe_addrmap) + "," + boost::lexical_cast<std::string>(selector_pipe_addrmap) + "," + boost::lexical_cast<std::string>(index_prsr_mem_main_rspec) + "," + boost::lexical_cast<std::string>(selector_prsr_mem_main_rspec)),
    array(RealArraySize(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, selector_prsr_mem_main_rspec, -1)),
    size0_(ArraySize(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, selector_prsr_mem_main_rspec, 0)),
    real_size0_(RealArraySize(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec, selector_prsr_mem_main_rspec, 0))
    {
    }
public:




  uint16_t lookup_16(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].lookup_16();
  }
  void lookup_16(uint32_t a0,const uint16_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].lookup_16(v);
  }






  uint8_t lookup_8(uint32_t a0,int j0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].lookup_8(j0);
  }
  void lookup_8(uint32_t a0,int j0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].lookup_8(j0,v);
  }




  uint8_t curr_state(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].curr_state();
  }
  void curr_state(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].curr_state(v);
  }





  uint8_t ctr_zero(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].ctr_zero();
  }
  void ctr_zero(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].ctr_zero(v);
  }





  uint8_t ctr_neg(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].ctr_neg();
  }
  void ctr_neg(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].ctr_neg(v);
  }





  uint8_t ver_0(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].ver_0();
  }
  void ver_0(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].ver_0(v);
  }





  uint8_t ver_1(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].ver_1();
  }
  void ver_1(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].ver_1(v);
  }
  bool calculate_index(
      uint64_t* offset, uint32_t* a0
      ) const {
    int i = (*offset)/1;
    (*offset) -= (i*1);
    uint32_t t = i;
    *a0 = t;
    bool in_bounds = true;
    in_bounds &= CheckArrayBounds(*a0,real_size0_,size0_,*offset,0,true);
    return in_bounds;
  }


  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    if (read_callback_) read_callback_(a0);
    array[a0].read(offset,data0,data1,T);
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    array[a0].write(offset,data0,data1,T);
    if (write_callback_) write_callback_(a0);
    return true;
  }

  void reset(
      
      ) {
    for (uint32_t i=0;i<array.size();++i) {
      array[i].reset();
      int t = i;
      int a0 = t;
      if (write_callback_) write_callback_(a0);
    }
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    all_zeros=false;
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return "OUT_OF_BOUNDS";
    std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
    if (! r_s.empty()) {
      r += indent_string + std::string("PrsrMlTcamRowArrayMutable") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
    }
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
    all_zeros=false;
    for (uint32_t a0=0;a0<real_size0_;++a0) {
      std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
      if (! r_s.empty()) {
        r += indent_string + std::string("PrsrMlTcamRowArrayMutable") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
      }
    }
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  std::vector<PrsrMlTcamRowMutable> array;
  uint32_t size0_;
  uint32_t real_size0_;
private:
  static uint64_t StartOffset(
      int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, PrsrMemMainRspecEnum selector_prsr_mem_main_rspec
      ) {
    uint64_t offset=0;
    offset += 0x20000000000; // to get to pipes
    assert(index_pipe_addrmap < 4);
    offset += index_pipe_addrmap * 0x2000000000; // pipe_addrmap[]
    switch (selector_pipe_addrmap) {
      case kEPrsr:
        offset += 0x1c80040000; // to get to e_prsr
        assert(index_prsr_mem_main_rspec < 18);
        offset += index_prsr_mem_main_rspec * 0x2000; // prsr_mem_main_rspec[]
        switch (selector_prsr_mem_main_rspec) {
          case kMlTcamRowWord0:
            break;
          case kMlTcamRowWord1:
            offset += 0x100; // to get to ml_tcam_row_word1
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kIPrsr:
        offset += 0x1c80000000; // to get to i_prsr
        assert(index_prsr_mem_main_rspec < 18);
        offset += index_prsr_mem_main_rspec * 0x2000; // prsr_mem_main_rspec[]
        switch (selector_prsr_mem_main_rspec) {
          case kMlTcamRowWord0:
            break;
          case kMlTcamRowWord1:
            offset += 0x100; // to get to ml_tcam_row_word1
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
    return offset;
  }

  static int ArraySize(
      int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, PrsrMemMainRspecEnum selector_prsr_mem_main_rspec, int dimension
      ) {
    switch (selector_pipe_addrmap) {
      case kEPrsr:
        switch (selector_prsr_mem_main_rspec) {
          case kMlTcamRowWord0:
            switch (dimension) {
              case -1:
                return 256;
                break;
              case 0:
                return 256;
                break;
              default:
                assert(0);
                break;
            }
            break;
          case kMlTcamRowWord1:
            switch (dimension) {
              case -1:
                return 256;
                break;
              case 0:
                return 256;
                break;
              default:
                assert(0);
                break;
            }
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kIPrsr:
        switch (selector_prsr_mem_main_rspec) {
          case kMlTcamRowWord0:
            switch (dimension) {
              case -1:
                return 256;
                break;
              case 0:
                return 256;
                break;
              default:
                assert(0);
                break;
            }
            break;
          case kMlTcamRowWord1:
            switch (dimension) {
              case -1:
                return 256;
                break;
              case 0:
                return 256;
                break;
              default:
                assert(0);
                break;
            }
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
  }

  static int RealArraySize(
      int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, PrsrMemMainRspecEnum selector_prsr_mem_main_rspec, int dimension
      ) {
    switch (selector_pipe_addrmap) {
      case kEPrsr:
        switch (selector_prsr_mem_main_rspec) {
          case kMlTcamRowWord0:
            switch (dimension) {
              case -1:
                return 256;
                break;
              case 0:
                return 256;
                break;
              default:
                assert(0);
                break;
            }
            break;
          case kMlTcamRowWord1:
            switch (dimension) {
              case -1:
                return 256;
                break;
              case 0:
                return 256;
                break;
              default:
                assert(0);
                break;
            }
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kIPrsr:
        switch (selector_prsr_mem_main_rspec) {
          case kMlTcamRowWord0:
            switch (dimension) {
              case -1:
                return 256;
                break;
              case 0:
                return 256;
                break;
              default:
                assert(0);
                break;
            }
            break;
          case kMlTcamRowWord1:
            switch (dimension) {
              case -1:
                return 256;
                break;
              case 0:
                return 256;
                break;
              default:
                assert(0);
                break;
            }
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
  }

};




class PrsrMemMainRspec : public model_core::RegisterBlockIndirect<RegisterCallback> {
public:
  enum PipeAddrmapEnum {
    kEPrsr,
    kIPrsr
  };
public:
  PrsrMemMainRspec(
      int chipNumber, int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, RegisterCallback& write_callback = 0, RegisterCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec), 1632, false, write_callback, read_callback, std::string("PrsrMemMainRspec")+":"+boost::lexical_cast<std::string>(index_pipe_addrmap) + "," + boost::lexical_cast<std::string>(selector_pipe_addrmap) + "," + boost::lexical_cast<std::string>(index_prsr_mem_main_rspec))
    {
    }
  PrsrMemMainRspec(
      
  )
    : RegisterBlockIndirect(0, 0, 0, false, 0, 0, "PrsrMemMainRspec")
    {
    }
public:







  PrsrMlTcamRow &ml_tcam_row_word0(int j0) { return ml_tcam_row_word0_[j0]; }







  PrsrMlTcamRow &ml_tcam_row_word1(int j0) { return ml_tcam_row_word1_[j0]; }







  PrsrMlEaRow &ml_ea_row(int j0) { return ml_ea_row_[j0]; }







  PrsrPoActionRow &po_action_row(int j0) { return po_action_row_[j0]; }







  PrsrMlCtrInitRamM &ml_ctr_init_ram(int j0) { return ml_ctr_init_ram_[j0]; }






  PrsrPoCsumCtrlRow &po_csum_ctrl_0_row(int j0) { return po_csum_ctrl_0_row_[j0]; }






  PrsrPoCsumCtrlRow &po_csum_ctrl_1_row(int j0) { return po_csum_ctrl_1_row_[j0]; }

  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    if (offset < 0x100) {
      offset -= 0x0;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      ml_tcam_row_word0_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x100 && offset < 0x200) {
      offset -= 0x100;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      ml_tcam_row_word1_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x200 && offset < 0x300) {
      offset -= 0x200;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      ml_ea_row_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x400 && offset < 0x600) {
      offset -= 0x400;
      int i0 = offset / 0x2;
      offset  -= i0 * 0x2;
      if (read_callback_) read_callback_();
      po_action_row_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x600 && offset < 0x610) {
      offset -= 0x600;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      ml_ctr_init_ram_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x620 && offset < 0x640) {
      offset -= 0x620;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      po_csum_ctrl_0_row_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x640 && offset < 0x660) {
      offset -= 0x640;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      po_csum_ctrl_1_row_[ i0 ].read( offset, data0,data1,T );
    }
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    if (offset < 0x100) {
      offset -= 0x0;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      ml_tcam_row_word0_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x100 && offset < 0x200) {
      offset -= 0x100;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      ml_tcam_row_word1_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x200 && offset < 0x300) {
      offset -= 0x200;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      ml_ea_row_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x400 && offset < 0x600) {
      offset -= 0x400;
      int i0 = offset / 0x2;
      offset  -= i0 * 0x2;
      po_action_row_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x600 && offset < 0x610) {
      offset -= 0x600;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      ml_ctr_init_ram_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x620 && offset < 0x640) {
      offset -= 0x620;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      po_csum_ctrl_0_row_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x640 && offset < 0x660) {
      offset -= 0x640;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      po_csum_ctrl_1_row_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    return true;
  }

  void reset(
      
      ) {
    for (auto &f0 : ml_tcam_row_word0_) {
      f0.reset();
    }
    for (auto &f0 : ml_tcam_row_word1_) {
      f0.reset();
    }
    for (auto &f0 : ml_ea_row_) {
      f0.reset();
    }
    for (auto &f0 : po_action_row_) {
      f0.reset();
    }
    for (auto &f0 : ml_ctr_init_ram_) {
      f0.reset();
    }
    for (auto &f0 : po_csum_ctrl_0_row_) {
      f0.reset();
    }
    for (auto &f0 : po_csum_ctrl_1_row_) {
      f0.reset();
    }
    if (write_callback_) write_callback_();
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    all_zeros=false;
    if (offset < 0x100) {
      offset -= 0x0;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += ml_tcam_row_word0_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x100 && offset < 0x200) {
      offset -= 0x100;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += ml_tcam_row_word1_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x200 && offset < 0x300) {
      offset -= 0x200;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += ml_ea_row_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x400 && offset < 0x600) {
      offset -= 0x400;
      int i0 = offset / 0x2;
      offset  -= i0 * 0x2;
      r += po_action_row_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x600 && offset < 0x610) {
      offset -= 0x600;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += ml_ctr_init_ram_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x620 && offset < 0x640) {
      offset -= 0x620;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += po_csum_ctrl_0_row_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x640 && offset < 0x660) {
      offset -= 0x640;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += po_csum_ctrl_1_row_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
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
    all_zeros=false;
    for (uint32_t a0=0;a0<256;++a0) {
      r += ml_tcam_row_word0_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<256;++a0) {
      r += ml_tcam_row_word1_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<256;++a0) {
      r += ml_ea_row_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<256;++a0) {
      r += po_action_row_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<16;++a0) {
      r += ml_ctr_init_ram_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<32;++a0) {
      r += po_csum_ctrl_0_row_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<32;++a0) {
      r += po_csum_ctrl_1_row_[a0].to_string(print_zeros,indent_string) ;
    }
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  std::array< PrsrMlTcamRow, 256 > ml_tcam_row_word0_;
  std::array< PrsrMlTcamRow, 256 > ml_tcam_row_word1_;
  std::array< PrsrMlEaRow, 256 > ml_ea_row_;
  std::array< PrsrPoActionRow, 256 > po_action_row_;
  std::array< PrsrMlCtrInitRamM, 16 > ml_ctr_init_ram_;
  std::array< PrsrPoCsumCtrlRow, 32 > po_csum_ctrl_0_row_;
  std::array< PrsrPoCsumCtrlRow, 32 > po_csum_ctrl_1_row_;
private:
  static uint64_t StartOffset(
      int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec
      ) {
    uint64_t offset=0;
    offset += 0x20000000000; // to get to pipes
    assert(index_pipe_addrmap < 4);
    offset += index_pipe_addrmap * 0x2000000000; // pipe_addrmap[]
    switch (selector_pipe_addrmap) {
      case kEPrsr:
        offset += 0x1c80040000; // to get to e_prsr
        assert(index_prsr_mem_main_rspec < 18);
        offset += index_prsr_mem_main_rspec * 0x2000; // prsr_mem_main_rspec[]
        break;
      case kIPrsr:
        offset += 0x1c80000000; // to get to i_prsr
        assert(index_prsr_mem_main_rspec < 18);
        offset += index_prsr_mem_main_rspec * 0x2000; // prsr_mem_main_rspec[]
        break;
      default:
        assert(0);
        break;
    }
    return offset;
  }

};




class PrsrMemMainRspecMutable : public model_core::RegisterBlockIndirect<RegisterCallback> {
public:
  enum PipeAddrmapEnum {
    kEPrsr,
    kIPrsr
  };
public:
  PrsrMemMainRspecMutable(
      int chipNumber, int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec, RegisterCallback& write_callback = 0, RegisterCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(index_pipe_addrmap, selector_pipe_addrmap, index_prsr_mem_main_rspec), 1632, true, write_callback, read_callback, std::string("PrsrMemMainRspecMutable")+":"+boost::lexical_cast<std::string>(index_pipe_addrmap) + "," + boost::lexical_cast<std::string>(selector_pipe_addrmap) + "," + boost::lexical_cast<std::string>(index_prsr_mem_main_rspec))
    {
    }
  PrsrMemMainRspecMutable(
      
  )
    : RegisterBlockIndirect(0, 0, 0, true, 0, 0, "PrsrMemMainRspecMutable")
    {
    }
public:







  PrsrMlTcamRowMutable &ml_tcam_row_word0(int j0) { return ml_tcam_row_word0_[j0]; }







  PrsrMlTcamRowMutable &ml_tcam_row_word1(int j0) { return ml_tcam_row_word1_[j0]; }







  PrsrMlEaRowMutable &ml_ea_row(int j0) { return ml_ea_row_[j0]; }







  PrsrPoActionRowMutable &po_action_row(int j0) { return po_action_row_[j0]; }







  PrsrMlCtrInitRamMMutable &ml_ctr_init_ram(int j0) { return ml_ctr_init_ram_[j0]; }






  PrsrPoCsumCtrlRowMutable &po_csum_ctrl_0_row(int j0) { return po_csum_ctrl_0_row_[j0]; }






  PrsrPoCsumCtrlRowMutable &po_csum_ctrl_1_row(int j0) { return po_csum_ctrl_1_row_[j0]; }

  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    if (offset < 0x100) {
      offset -= 0x0;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      ml_tcam_row_word0_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x100 && offset < 0x200) {
      offset -= 0x100;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      ml_tcam_row_word1_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x200 && offset < 0x300) {
      offset -= 0x200;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      ml_ea_row_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x400 && offset < 0x600) {
      offset -= 0x400;
      int i0 = offset / 0x2;
      offset  -= i0 * 0x2;
      if (read_callback_) read_callback_();
      po_action_row_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x600 && offset < 0x610) {
      offset -= 0x600;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      ml_ctr_init_ram_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x620 && offset < 0x640) {
      offset -= 0x620;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      po_csum_ctrl_0_row_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x640 && offset < 0x660) {
      offset -= 0x640;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      po_csum_ctrl_1_row_[ i0 ].read( offset, data0,data1,T );
    }
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    if (offset < 0x100) {
      offset -= 0x0;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      ml_tcam_row_word0_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x100 && offset < 0x200) {
      offset -= 0x100;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      ml_tcam_row_word1_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x200 && offset < 0x300) {
      offset -= 0x200;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      ml_ea_row_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x400 && offset < 0x600) {
      offset -= 0x400;
      int i0 = offset / 0x2;
      offset  -= i0 * 0x2;
      po_action_row_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x600 && offset < 0x610) {
      offset -= 0x600;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      ml_ctr_init_ram_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x620 && offset < 0x640) {
      offset -= 0x620;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      po_csum_ctrl_0_row_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x640 && offset < 0x660) {
      offset -= 0x640;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      po_csum_ctrl_1_row_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    return true;
  }

  void reset(
      
      ) {
    for (auto &f0 : ml_tcam_row_word0_) {
      f0.reset();
    }
    for (auto &f0 : ml_tcam_row_word1_) {
      f0.reset();
    }
    for (auto &f0 : ml_ea_row_) {
      f0.reset();
    }
    for (auto &f0 : po_action_row_) {
      f0.reset();
    }
    for (auto &f0 : ml_ctr_init_ram_) {
      f0.reset();
    }
    for (auto &f0 : po_csum_ctrl_0_row_) {
      f0.reset();
    }
    for (auto &f0 : po_csum_ctrl_1_row_) {
      f0.reset();
    }
    if (write_callback_) write_callback_();
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    all_zeros=false;
    if (offset < 0x100) {
      offset -= 0x0;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += ml_tcam_row_word0_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x100 && offset < 0x200) {
      offset -= 0x100;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += ml_tcam_row_word1_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x200 && offset < 0x300) {
      offset -= 0x200;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += ml_ea_row_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x400 && offset < 0x600) {
      offset -= 0x400;
      int i0 = offset / 0x2;
      offset  -= i0 * 0x2;
      r += po_action_row_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x600 && offset < 0x610) {
      offset -= 0x600;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += ml_ctr_init_ram_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x620 && offset < 0x640) {
      offset -= 0x620;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += po_csum_ctrl_0_row_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x640 && offset < 0x660) {
      offset -= 0x640;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += po_csum_ctrl_1_row_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
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
    all_zeros=false;
    for (uint32_t a0=0;a0<256;++a0) {
      r += ml_tcam_row_word0_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<256;++a0) {
      r += ml_tcam_row_word1_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<256;++a0) {
      r += ml_ea_row_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<256;++a0) {
      r += po_action_row_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<16;++a0) {
      r += ml_ctr_init_ram_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<32;++a0) {
      r += po_csum_ctrl_0_row_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<32;++a0) {
      r += po_csum_ctrl_1_row_[a0].to_string(print_zeros,indent_string) ;
    }
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  std::array< PrsrMlTcamRowMutable, 256 > ml_tcam_row_word0_;
  std::array< PrsrMlTcamRowMutable, 256 > ml_tcam_row_word1_;
  std::array< PrsrMlEaRowMutable, 256 > ml_ea_row_;
  std::array< PrsrPoActionRowMutable, 256 > po_action_row_;
  std::array< PrsrMlCtrInitRamMMutable, 16 > ml_ctr_init_ram_;
  std::array< PrsrPoCsumCtrlRowMutable, 32 > po_csum_ctrl_0_row_;
  std::array< PrsrPoCsumCtrlRowMutable, 32 > po_csum_ctrl_1_row_;
private:
  static uint64_t StartOffset(
      int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int index_prsr_mem_main_rspec
      ) {
    uint64_t offset=0;
    offset += 0x20000000000; // to get to pipes
    assert(index_pipe_addrmap < 4);
    offset += index_pipe_addrmap * 0x2000000000; // pipe_addrmap[]
    switch (selector_pipe_addrmap) {
      case kEPrsr:
        offset += 0x1c80040000; // to get to e_prsr
        assert(index_prsr_mem_main_rspec < 18);
        offset += index_prsr_mem_main_rspec * 0x2000; // prsr_mem_main_rspec[]
        break;
      case kIPrsr:
        offset += 0x1c80000000; // to get to i_prsr
        assert(index_prsr_mem_main_rspec < 18);
        offset += index_prsr_mem_main_rspec * 0x2000; // prsr_mem_main_rspec[]
        break;
      default:
        assert(0);
        break;
    }
    return offset;
  }

};




class PrsrMemMainRspecArray : public model_core::RegisterBlockIndirect<RegisterArrayCallback> {
public:
  enum PipeAddrmapEnum {
    kEPrsr,
    kIPrsr
  };
public:
  PrsrMemMainRspecArray(
      int chipNumber, int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, RegisterArrayCallback& write_callback = 0, RegisterArrayCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(index_pipe_addrmap, selector_pipe_addrmap), 8192 * ArraySize(index_pipe_addrmap, selector_pipe_addrmap, -1), false, write_callback, read_callback, std::string("PrsrMemMainRspecArray")+":"+boost::lexical_cast<std::string>(index_pipe_addrmap) + "," + boost::lexical_cast<std::string>(selector_pipe_addrmap)),
    array(RealArraySize(index_pipe_addrmap, selector_pipe_addrmap, -1)),
    size0_(ArraySize(index_pipe_addrmap, selector_pipe_addrmap, 0)),
    real_size0_(RealArraySize(index_pipe_addrmap, selector_pipe_addrmap, 0))
    {
    }
public:







  PrsrMlTcamRow &ml_tcam_row_word0(uint32_t a0,int j0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].ml_tcam_row_word0(j0);
  }







  PrsrMlTcamRow &ml_tcam_row_word1(uint32_t a0,int j0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].ml_tcam_row_word1(j0);
  }







  PrsrMlEaRow &ml_ea_row(uint32_t a0,int j0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].ml_ea_row(j0);
  }







  PrsrPoActionRow &po_action_row(uint32_t a0,int j0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].po_action_row(j0);
  }







  PrsrMlCtrInitRamM &ml_ctr_init_ram(uint32_t a0,int j0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].ml_ctr_init_ram(j0);
  }






  PrsrPoCsumCtrlRow &po_csum_ctrl_0_row(uint32_t a0,int j0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].po_csum_ctrl_0_row(j0);
  }






  PrsrPoCsumCtrlRow &po_csum_ctrl_1_row(uint32_t a0,int j0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].po_csum_ctrl_1_row(j0);
  }
  bool calculate_index(
      uint64_t* offset, uint32_t* a0
      ) const {
    int i = (*offset)/8192;
    (*offset) -= (i*8192);
    uint32_t t = i;
    *a0 = t;
    bool in_bounds = true;
    in_bounds &= CheckArrayBounds(*a0,real_size0_,size0_,*offset,0,true);
    return in_bounds;
  }


  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    if (read_callback_) read_callback_(a0);
    array[a0].read(offset,data0,data1,T);
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    array[a0].write(offset,data0,data1,T);
    if (write_callback_) write_callback_(a0);
    return true;
  }

  void reset(
      
      ) {
    for (uint32_t i=0;i<array.size();++i) {
      array[i].reset();
      int t = i;
      int a0 = t;
      if (write_callback_) write_callback_(a0);
    }
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    all_zeros=false;
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return "OUT_OF_BOUNDS";
    std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
    if (! r_s.empty()) {
      r += indent_string + std::string("PrsrMemMainRspecArray") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
    }
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
    all_zeros=false;
    for (uint32_t a0=0;a0<real_size0_;++a0) {
      std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
      if (! r_s.empty()) {
        r += indent_string + std::string("PrsrMemMainRspecArray") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
      }
    }
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  std::vector<PrsrMemMainRspec> array;
  uint32_t size0_;
  uint32_t real_size0_;
private:
  static uint64_t StartOffset(
      int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap
      ) {
    uint64_t offset=0;
    offset += 0x20000000000; // to get to pipes
    assert(index_pipe_addrmap < 4);
    offset += index_pipe_addrmap * 0x2000000000; // pipe_addrmap[]
    switch (selector_pipe_addrmap) {
      case kEPrsr:
        offset += 0x1c80040000; // to get to e_prsr
        break;
      case kIPrsr:
        offset += 0x1c80000000; // to get to i_prsr
        break;
      default:
        assert(0);
        break;
    }
    return offset;
  }

  static int ArraySize(
      int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int dimension
      ) {
    switch (selector_pipe_addrmap) {
      case kEPrsr:
        switch (dimension) {
          case -1:
            return 18;
            break;
          case 0:
            return 18;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kIPrsr:
        switch (dimension) {
          case -1:
            return 18;
            break;
          case 0:
            return 18;
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
  }

  static int RealArraySize(
      int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int dimension
      ) {
    switch (selector_pipe_addrmap) {
      case kEPrsr:
        switch (dimension) {
          case -1:
            return 18;
            break;
          case 0:
            return 18;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kIPrsr:
        switch (dimension) {
          case -1:
            return 18;
            break;
          case 0:
            return 18;
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
  }

};




class PrsrMemMainRspecArrayMutable : public model_core::RegisterBlockIndirect<RegisterArrayCallback> {
public:
  enum PipeAddrmapEnum {
    kEPrsr,
    kIPrsr
  };
public:
  PrsrMemMainRspecArrayMutable(
      int chipNumber, int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, RegisterArrayCallback& write_callback = 0, RegisterArrayCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(index_pipe_addrmap, selector_pipe_addrmap), 8192 * ArraySize(index_pipe_addrmap, selector_pipe_addrmap, -1), true, write_callback, read_callback, std::string("PrsrMemMainRspecArrayMutable")+":"+boost::lexical_cast<std::string>(index_pipe_addrmap) + "," + boost::lexical_cast<std::string>(selector_pipe_addrmap)),
    array(RealArraySize(index_pipe_addrmap, selector_pipe_addrmap, -1)),
    size0_(ArraySize(index_pipe_addrmap, selector_pipe_addrmap, 0)),
    real_size0_(RealArraySize(index_pipe_addrmap, selector_pipe_addrmap, 0))
    {
    }
public:







  PrsrMlTcamRowMutable &ml_tcam_row_word0(uint32_t a0,int j0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].ml_tcam_row_word0(j0);
  }







  PrsrMlTcamRowMutable &ml_tcam_row_word1(uint32_t a0,int j0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].ml_tcam_row_word1(j0);
  }







  PrsrMlEaRowMutable &ml_ea_row(uint32_t a0,int j0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].ml_ea_row(j0);
  }







  PrsrPoActionRowMutable &po_action_row(uint32_t a0,int j0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].po_action_row(j0);
  }







  PrsrMlCtrInitRamMMutable &ml_ctr_init_ram(uint32_t a0,int j0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].ml_ctr_init_ram(j0);
  }






  PrsrPoCsumCtrlRowMutable &po_csum_ctrl_0_row(uint32_t a0,int j0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].po_csum_ctrl_0_row(j0);
  }






  PrsrPoCsumCtrlRowMutable &po_csum_ctrl_1_row(uint32_t a0,int j0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].po_csum_ctrl_1_row(j0);
  }
  bool calculate_index(
      uint64_t* offset, uint32_t* a0
      ) const {
    int i = (*offset)/8192;
    (*offset) -= (i*8192);
    uint32_t t = i;
    *a0 = t;
    bool in_bounds = true;
    in_bounds &= CheckArrayBounds(*a0,real_size0_,size0_,*offset,0,true);
    return in_bounds;
  }


  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    if (read_callback_) read_callback_(a0);
    array[a0].read(offset,data0,data1,T);
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    array[a0].write(offset,data0,data1,T);
    if (write_callback_) write_callback_(a0);
    return true;
  }

  void reset(
      
      ) {
    for (uint32_t i=0;i<array.size();++i) {
      array[i].reset();
      int t = i;
      int a0 = t;
      if (write_callback_) write_callback_(a0);
    }
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    all_zeros=false;
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return "OUT_OF_BOUNDS";
    std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
    if (! r_s.empty()) {
      r += indent_string + std::string("PrsrMemMainRspecArrayMutable") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
    }
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
    all_zeros=false;
    for (uint32_t a0=0;a0<real_size0_;++a0) {
      std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
      if (! r_s.empty()) {
        r += indent_string + std::string("PrsrMemMainRspecArrayMutable") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
      }
    }
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  std::vector<PrsrMemMainRspecMutable> array;
  uint32_t size0_;
  uint32_t real_size0_;
private:
  static uint64_t StartOffset(
      int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap
      ) {
    uint64_t offset=0;
    offset += 0x20000000000; // to get to pipes
    assert(index_pipe_addrmap < 4);
    offset += index_pipe_addrmap * 0x2000000000; // pipe_addrmap[]
    switch (selector_pipe_addrmap) {
      case kEPrsr:
        offset += 0x1c80040000; // to get to e_prsr
        break;
      case kIPrsr:
        offset += 0x1c80000000; // to get to i_prsr
        break;
      default:
        assert(0);
        break;
    }
    return offset;
  }

  static int ArraySize(
      int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int dimension
      ) {
    switch (selector_pipe_addrmap) {
      case kEPrsr:
        switch (dimension) {
          case -1:
            return 18;
            break;
          case 0:
            return 18;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kIPrsr:
        switch (dimension) {
          case -1:
            return 18;
            break;
          case 0:
            return 18;
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
  }

  static int RealArraySize(
      int index_pipe_addrmap, PipeAddrmapEnum selector_pipe_addrmap, int dimension
      ) {
    switch (selector_pipe_addrmap) {
      case kEPrsr:
        switch (dimension) {
          case -1:
            return 18;
            break;
          case 0:
            return 18;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kIPrsr:
        switch (dimension) {
          case -1:
            return 18;
            break;
          case 0:
            return 18;
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
  }

};








class PartyPgrBufferMemWord : public model_core::RegisterBlockIndirect<RegisterCallback> {
public:
  PartyPgrBufferMemWord(
      int chipNumber, int index_pipe_addrmap, int index_party_pgr_buffer_mem_word, RegisterCallback& write_callback = 0, RegisterCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(index_pipe_addrmap, index_party_pgr_buffer_mem_word), 1, false, write_callback, read_callback, std::string("PartyPgrBufferMemWord")+":"+boost::lexical_cast<std::string>(index_pipe_addrmap) + "," + boost::lexical_cast<std::string>(index_party_pgr_buffer_mem_word))
    {
    }
  PartyPgrBufferMemWord(
      
  )
    : RegisterBlockIndirect(0, 0, 0, false, 0, 0, "PartyPgrBufferMemWord")
    {
    }
public:




  uint8_t &mem_word() { return mem_word_; }

  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    if (read_callback_) read_callback_();
    *data0 = (static_cast<uint64_t>(mem_word_) & 0x1);
    *data1 = 0;
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    mem_word_ = (data0 & 0x1);
    if (write_callback_) write_callback_();
    return true;
  }

  void reset(
      
      ) {
    mem_word_ = 0;
    if (write_callback_) write_callback_();
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    r += indent_string + std::string("PartyPgrBufferMemWord") + ":\n";
    r += indent_string + "  " + std::string("mem_word") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(mem_word_) ) + "\n";
    all_zeros &= (0 == mem_word_);
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
    r += indent_string + std::string("PartyPgrBufferMemWord") + ":\n";
    r += indent_string + "  " + std::string("mem_word") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(mem_word_) ) + "\n";
    all_zeros &= (0 == mem_word_);
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  uint8_t mem_word_;
private:
  static uint64_t StartOffset(
      int index_pipe_addrmap, int index_party_pgr_buffer_mem_word
      ) {
    uint64_t offset=0;
    offset += 0x20000000000; // to get to pipes
    assert(index_pipe_addrmap < 4);
    offset += index_pipe_addrmap * 0x2000000000; // pipe_addrmap[]
    offset += 0x1c8003c000; // to get to party_pgr
    assert(index_party_pgr_buffer_mem_word < 1024);
    offset += index_party_pgr_buffer_mem_word * 0x1; // party_pgr_buffer_mem_word[]
    return offset;
  }

};








class PartyPgrBufferMemWordMutable : public model_core::RegisterBlockIndirect<RegisterCallback> {
public:
  PartyPgrBufferMemWordMutable(
      int chipNumber, int index_pipe_addrmap, int index_party_pgr_buffer_mem_word, RegisterCallback& write_callback = 0, RegisterCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(index_pipe_addrmap, index_party_pgr_buffer_mem_word), 1, true, write_callback, read_callback, std::string("PartyPgrBufferMemWordMutable")+":"+boost::lexical_cast<std::string>(index_pipe_addrmap) + "," + boost::lexical_cast<std::string>(index_party_pgr_buffer_mem_word))
    {
    }
  PartyPgrBufferMemWordMutable(
      
  )
    : RegisterBlockIndirect(0, 0, 0, true, 0, 0, "PartyPgrBufferMemWordMutable")
    {
    }
public:




  uint8_t mem_word() { return mem_word_; }
  void mem_word(const uint8_t &v) { mem_word_=v; }

  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    if (read_callback_) read_callback_();
    *data0 = (static_cast<uint64_t>(mem_word_) & 0x1);
    *data1 = 0;
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    mem_word_ = (data0 & 0x1);
    if (write_callback_) write_callback_();
    return true;
  }

  void reset(
      
      ) {
    mem_word_ = 0;
    if (write_callback_) write_callback_();
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    r += indent_string + std::string("PartyPgrBufferMemWordMutable") + ":\n";
    r += indent_string + "  " + std::string("mem_word") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(mem_word_) ) + "\n";
    all_zeros &= (0 == mem_word_);
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
    r += indent_string + std::string("PartyPgrBufferMemWordMutable") + ":\n";
    r += indent_string + "  " + std::string("mem_word") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(mem_word_) ) + "\n";
    all_zeros &= (0 == mem_word_);
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  uint8_t mem_word_;
private:
  static uint64_t StartOffset(
      int index_pipe_addrmap, int index_party_pgr_buffer_mem_word
      ) {
    uint64_t offset=0;
    offset += 0x20000000000; // to get to pipes
    assert(index_pipe_addrmap < 4);
    offset += index_pipe_addrmap * 0x2000000000; // pipe_addrmap[]
    offset += 0x1c8003c000; // to get to party_pgr
    assert(index_party_pgr_buffer_mem_word < 1024);
    offset += index_party_pgr_buffer_mem_word * 0x1; // party_pgr_buffer_mem_word[]
    return offset;
  }

};








class PartyPgrBufferMemWordArray : public model_core::RegisterBlockIndirect<RegisterArrayCallback> {
public:
  PartyPgrBufferMemWordArray(
      int chipNumber, int index_pipe_addrmap, RegisterArrayCallback& write_callback = 0, RegisterArrayCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(index_pipe_addrmap), 1 * ArraySize(index_pipe_addrmap, -1), false, write_callback, read_callback, std::string("PartyPgrBufferMemWordArray")+":"+boost::lexical_cast<std::string>(index_pipe_addrmap)),
    array(RealArraySize(index_pipe_addrmap, -1)),
    size0_(ArraySize(index_pipe_addrmap, 0)),
    real_size0_(RealArraySize(index_pipe_addrmap, 0))
    {
    }
public:




  uint8_t &mem_word(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].mem_word();
  }
  bool calculate_index(
      uint64_t* offset, uint32_t* a0
      ) const {
    int i = (*offset)/1;
    (*offset) -= (i*1);
    uint32_t t = i;
    *a0 = t;
    bool in_bounds = true;
    in_bounds &= CheckArrayBounds(*a0,real_size0_,size0_,*offset,0,true);
    return in_bounds;
  }


  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    if (read_callback_) read_callback_(a0);
    array[a0].read(offset,data0,data1,T);
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    array[a0].write(offset,data0,data1,T);
    if (write_callback_) write_callback_(a0);
    return true;
  }

  void reset(
      
      ) {
    for (uint32_t i=0;i<array.size();++i) {
      array[i].reset();
      int t = i;
      int a0 = t;
      if (write_callback_) write_callback_(a0);
    }
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    all_zeros=false;
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return "OUT_OF_BOUNDS";
    std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
    if (! r_s.empty()) {
      r += indent_string + std::string("PartyPgrBufferMemWordArray") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
    }
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
    all_zeros=false;
    for (uint32_t a0=0;a0<real_size0_;++a0) {
      std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
      if (! r_s.empty()) {
        r += indent_string + std::string("PartyPgrBufferMemWordArray") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
      }
    }
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  std::vector<PartyPgrBufferMemWord> array;
  uint32_t size0_;
  uint32_t real_size0_;
private:
  static uint64_t StartOffset(
      int index_pipe_addrmap
      ) {
    uint64_t offset=0;
    offset += 0x20000000000; // to get to pipes
    assert(index_pipe_addrmap < 4);
    offset += index_pipe_addrmap * 0x2000000000; // pipe_addrmap[]
    offset += 0x1c8003c000; // to get to party_pgr
    return offset;
  }

  static int ArraySize(
      int index_pipe_addrmap, int dimension
      ) {
    switch (dimension) {
      case -1:
        return 1024;
        break;
      case 0:
        return 1024;
        break;
      default:
        assert(0);
        break;
    }
  }

  static int RealArraySize(
      int index_pipe_addrmap, int dimension
      ) {
    switch (dimension) {
      case -1:
        return 1024;
        break;
      case 0:
        return 1024;
        break;
      default:
        assert(0);
        break;
    }
  }

};








class PartyPgrBufferMemWordArrayMutable : public model_core::RegisterBlockIndirect<RegisterArrayCallback> {
public:
  PartyPgrBufferMemWordArrayMutable(
      int chipNumber, int index_pipe_addrmap, RegisterArrayCallback& write_callback = 0, RegisterArrayCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(index_pipe_addrmap), 1 * ArraySize(index_pipe_addrmap, -1), true, write_callback, read_callback, std::string("PartyPgrBufferMemWordArrayMutable")+":"+boost::lexical_cast<std::string>(index_pipe_addrmap)),
    array(RealArraySize(index_pipe_addrmap, -1)),
    size0_(ArraySize(index_pipe_addrmap, 0)),
    real_size0_(RealArraySize(index_pipe_addrmap, 0))
    {
    }
public:




  uint8_t mem_word(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].mem_word();
  }
  void mem_word(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].mem_word(v);
  }
  bool calculate_index(
      uint64_t* offset, uint32_t* a0
      ) const {
    int i = (*offset)/1;
    (*offset) -= (i*1);
    uint32_t t = i;
    *a0 = t;
    bool in_bounds = true;
    in_bounds &= CheckArrayBounds(*a0,real_size0_,size0_,*offset,0,true);
    return in_bounds;
  }


  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    if (read_callback_) read_callback_(a0);
    array[a0].read(offset,data0,data1,T);
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    array[a0].write(offset,data0,data1,T);
    if (write_callback_) write_callback_(a0);
    return true;
  }

  void reset(
      
      ) {
    for (uint32_t i=0;i<array.size();++i) {
      array[i].reset();
      int t = i;
      int a0 = t;
      if (write_callback_) write_callback_(a0);
    }
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    all_zeros=false;
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return "OUT_OF_BOUNDS";
    std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
    if (! r_s.empty()) {
      r += indent_string + std::string("PartyPgrBufferMemWordArrayMutable") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
    }
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
    all_zeros=false;
    for (uint32_t a0=0;a0<real_size0_;++a0) {
      std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
      if (! r_s.empty()) {
        r += indent_string + std::string("PartyPgrBufferMemWordArrayMutable") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
      }
    }
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  std::vector<PartyPgrBufferMemWordMutable> array;
  uint32_t size0_;
  uint32_t real_size0_;
private:
  static uint64_t StartOffset(
      int index_pipe_addrmap
      ) {
    uint64_t offset=0;
    offset += 0x20000000000; // to get to pipes
    assert(index_pipe_addrmap < 4);
    offset += index_pipe_addrmap * 0x2000000000; // pipe_addrmap[]
    offset += 0x1c8003c000; // to get to party_pgr
    return offset;
  }

  static int ArraySize(
      int index_pipe_addrmap, int dimension
      ) {
    switch (dimension) {
      case -1:
        return 1024;
        break;
      case 0:
        return 1024;
        break;
      default:
        assert(0);
        break;
    }
  }

  static int RealArraySize(
      int index_pipe_addrmap, int dimension
      ) {
    switch (dimension) {
      case -1:
        return 1024;
        break;
      case 0:
        return 1024;
        break;
      default:
        assert(0);
        break;
    }
  }

};








class PartyPgrMemRspec : public model_core::RegisterBlockIndirect<RegisterCallback> {
public:
  PartyPgrMemRspec(
      int chipNumber, int index_pipe_addrmap, RegisterCallback& write_callback = 0, RegisterCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(index_pipe_addrmap), 8192, false, write_callback, read_callback, std::string("PartyPgrMemRspec")+":"+boost::lexical_cast<std::string>(index_pipe_addrmap))
    {
    }
  PartyPgrMemRspec(
      
  )
    : RegisterBlockIndirect(0, 0, 0, false, 0, 0, "PartyPgrMemRspec")
    {
    }
public:







  PartyPgrBufferMemWord &buffer_mem_word(int j0) { return buffer_mem_word_[j0]; }

  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    if (offset < 0x400) {
      offset -= 0x0;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      buffer_mem_word_[ i0 ].read( offset, data0,data1,T );
    }
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    if (offset < 0x400) {
      offset -= 0x0;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      buffer_mem_word_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    return true;
  }

  void reset(
      
      ) {
    for (auto &f0 : buffer_mem_word_) {
      f0.reset();
    }
    if (write_callback_) write_callback_();
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    all_zeros=false;
    if (offset < 0x400) {
      offset -= 0x0;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += buffer_mem_word_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
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
    all_zeros=false;
    for (uint32_t a0=0;a0<1024;++a0) {
      r += buffer_mem_word_[a0].to_string(print_zeros,indent_string) ;
    }
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  std::array< PartyPgrBufferMemWord, 1024 > buffer_mem_word_;
private:
  static uint64_t StartOffset(
      int index_pipe_addrmap
      ) {
    uint64_t offset=0;
    offset += 0x20000000000; // to get to pipes
    assert(index_pipe_addrmap < 4);
    offset += index_pipe_addrmap * 0x2000000000; // pipe_addrmap[]
    offset += 0x1c8003c000; // to get to party_pgr
    return offset;
  }

};








class PartyPgrMemRspecMutable : public model_core::RegisterBlockIndirect<RegisterCallback> {
public:
  PartyPgrMemRspecMutable(
      int chipNumber, int index_pipe_addrmap, RegisterCallback& write_callback = 0, RegisterCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(index_pipe_addrmap), 8192, true, write_callback, read_callback, std::string("PartyPgrMemRspecMutable")+":"+boost::lexical_cast<std::string>(index_pipe_addrmap))
    {
    }
  PartyPgrMemRspecMutable(
      
  )
    : RegisterBlockIndirect(0, 0, 0, true, 0, 0, "PartyPgrMemRspecMutable")
    {
    }
public:







  PartyPgrBufferMemWordMutable &buffer_mem_word(int j0) { return buffer_mem_word_[j0]; }

  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    if (offset < 0x400) {
      offset -= 0x0;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      buffer_mem_word_[ i0 ].read( offset, data0,data1,T );
    }
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    if (offset < 0x400) {
      offset -= 0x0;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      buffer_mem_word_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    return true;
  }

  void reset(
      
      ) {
    for (auto &f0 : buffer_mem_word_) {
      f0.reset();
    }
    if (write_callback_) write_callback_();
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    all_zeros=false;
    if (offset < 0x400) {
      offset -= 0x0;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += buffer_mem_word_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
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
    all_zeros=false;
    for (uint32_t a0=0;a0<1024;++a0) {
      r += buffer_mem_word_[a0].to_string(print_zeros,indent_string) ;
    }
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  std::array< PartyPgrBufferMemWordMutable, 1024 > buffer_mem_word_;
private:
  static uint64_t StartOffset(
      int index_pipe_addrmap
      ) {
    uint64_t offset=0;
    offset += 0x20000000000; // to get to pipes
    assert(index_pipe_addrmap < 4);
    offset += index_pipe_addrmap * 0x2000000000; // pipe_addrmap[]
    offset += 0x1c8003c000; // to get to party_pgr
    return offset;
  }

};




class MauAddrmapDummyRegister : public model_core::RegisterBlockIndirect<RegisterCallback> {
public:
  MauAddrmapDummyRegister(
      int chipNumber, int index_pipe_addrmap, int index_mau_addrmap, RegisterCallback& write_callback = 0, RegisterCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(index_pipe_addrmap, index_mau_addrmap), 1, false, write_callback, read_callback, std::string("MauAddrmapDummyRegister")+":"+boost::lexical_cast<std::string>(index_pipe_addrmap) + "," + boost::lexical_cast<std::string>(index_mau_addrmap))
    {
    }
  MauAddrmapDummyRegister(
      
  )
    : RegisterBlockIndirect(0, 0, 0, false, 0, 0, "MauAddrmapDummyRegister")
    {
    }
public:
  uint32_t &dummy_register() { return dummy_register_; }

  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    if (read_callback_) read_callback_();
    *data0 = static_cast<uint64_t>(dummy_register_);
    *data1 = 0;
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    dummy_register_ = data0;
    if (write_callback_) write_callback_();
    return true;
  }

  void reset(
      
      ) {
    dummy_register_ = 0;
    if (write_callback_) write_callback_();
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    r += indent_string + std::string("MauAddrmapDummyRegister") + ":\n";
    r += indent_string + "  " + std::string("dummy_register") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(dummy_register_) ) + "\n";
    all_zeros &= (0 == dummy_register_);
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
    r += indent_string + std::string("MauAddrmapDummyRegister") + ":\n";
    r += indent_string + "  " + std::string("dummy_register") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(dummy_register_) ) + "\n";
    all_zeros &= (0 == dummy_register_);
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  uint32_t dummy_register_;
private:
  static uint64_t StartOffset(
      int index_pipe_addrmap, int index_mau_addrmap
      ) {
    uint64_t offset=0;
    offset += 0x20000000000; // to get to pipes
    assert(index_pipe_addrmap < 4);
    offset += index_pipe_addrmap * 0x2000000000; // pipe_addrmap[]
    assert(index_mau_addrmap < 12);
    offset += index_mau_addrmap * 0x200000000; // mau_addrmap[]
    return offset;
  }

};




class MauAddrmapDummyRegisterMutable : public model_core::RegisterBlockIndirect<RegisterCallback> {
public:
  MauAddrmapDummyRegisterMutable(
      int chipNumber, int index_pipe_addrmap, int index_mau_addrmap, RegisterCallback& write_callback = 0, RegisterCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(index_pipe_addrmap, index_mau_addrmap), 1, true, write_callback, read_callback, std::string("MauAddrmapDummyRegisterMutable")+":"+boost::lexical_cast<std::string>(index_pipe_addrmap) + "," + boost::lexical_cast<std::string>(index_mau_addrmap))
    {
    }
  MauAddrmapDummyRegisterMutable(
      
  )
    : RegisterBlockIndirect(0, 0, 0, true, 0, 0, "MauAddrmapDummyRegisterMutable")
    {
    }
public:
  uint32_t dummy_register() { return dummy_register_; }
  void dummy_register(const uint32_t &v) { dummy_register_=v; }

  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    if (read_callback_) read_callback_();
    *data0 = static_cast<uint64_t>(dummy_register_);
    *data1 = 0;
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    dummy_register_ = data0;
    if (write_callback_) write_callback_();
    return true;
  }

  void reset(
      
      ) {
    dummy_register_ = 0;
    if (write_callback_) write_callback_();
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    r += indent_string + std::string("MauAddrmapDummyRegisterMutable") + ":\n";
    r += indent_string + "  " + std::string("dummy_register") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(dummy_register_) ) + "\n";
    all_zeros &= (0 == dummy_register_);
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
    r += indent_string + std::string("MauAddrmapDummyRegisterMutable") + ":\n";
    r += indent_string + "  " + std::string("dummy_register") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(dummy_register_) ) + "\n";
    all_zeros &= (0 == dummy_register_);
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  uint32_t dummy_register_;
private:
  static uint64_t StartOffset(
      int index_pipe_addrmap, int index_mau_addrmap
      ) {
    uint64_t offset=0;
    offset += 0x20000000000; // to get to pipes
    assert(index_pipe_addrmap < 4);
    offset += index_pipe_addrmap * 0x2000000000; // pipe_addrmap[]
    assert(index_mau_addrmap < 12);
    offset += index_mau_addrmap * 0x200000000; // mau_addrmap[]
    return offset;
  }

};








class MauAddrmap : public model_core::RegisterBlockIndirect<RegisterCallback> {
public:
  MauAddrmap(
      int chipNumber, int index_pipe_addrmap, int index_mau_addrmap, RegisterCallback& write_callback = 0, RegisterCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(index_pipe_addrmap, index_mau_addrmap), 1, false, write_callback, read_callback, std::string("MauAddrmap")+":"+boost::lexical_cast<std::string>(index_pipe_addrmap) + "," + boost::lexical_cast<std::string>(index_mau_addrmap))
    {
    }
  MauAddrmap(
      
  )
    : RegisterBlockIndirect(0, 0, 0, false, 0, 0, "MauAddrmap")
    {
    }
public:
  MauAddrmapDummyRegister &dummy_register() { return dummy_register_; }

  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    if (offset < 0x1) {
      offset -= 0x0;
      if (read_callback_) read_callback_();
      dummy_register_.read( offset, data0,data1,T );
    }
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    if (offset < 0x1) {
      offset -= 0x0;
      dummy_register_.write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    return true;
  }

  void reset(
      
      ) {
    dummy_register_.reset();
    if (write_callback_) write_callback_();
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    all_zeros=false;
    if (offset < 0x1) {
      offset -= 0x0;
      r += dummy_register_.to_string(offset,print_zeros,indent_string) ;
    }
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
    all_zeros=false;
    r += dummy_register_.to_string(print_zeros,indent_string) ;
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  MauAddrmapDummyRegister dummy_register_;
private:
  static uint64_t StartOffset(
      int index_pipe_addrmap, int index_mau_addrmap
      ) {
    uint64_t offset=0;
    offset += 0x20000000000; // to get to pipes
    assert(index_pipe_addrmap < 4);
    offset += index_pipe_addrmap * 0x2000000000; // pipe_addrmap[]
    assert(index_mau_addrmap < 12);
    offset += index_mau_addrmap * 0x200000000; // mau_addrmap[]
    return offset;
  }

};








class MauAddrmapMutable : public model_core::RegisterBlockIndirect<RegisterCallback> {
public:
  MauAddrmapMutable(
      int chipNumber, int index_pipe_addrmap, int index_mau_addrmap, RegisterCallback& write_callback = 0, RegisterCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(index_pipe_addrmap, index_mau_addrmap), 1, true, write_callback, read_callback, std::string("MauAddrmapMutable")+":"+boost::lexical_cast<std::string>(index_pipe_addrmap) + "," + boost::lexical_cast<std::string>(index_mau_addrmap))
    {
    }
  MauAddrmapMutable(
      
  )
    : RegisterBlockIndirect(0, 0, 0, true, 0, 0, "MauAddrmapMutable")
    {
    }
public:
  MauAddrmapDummyRegisterMutable &dummy_register() { return dummy_register_; }

  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    if (offset < 0x1) {
      offset -= 0x0;
      if (read_callback_) read_callback_();
      dummy_register_.read( offset, data0,data1,T );
    }
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    if (offset < 0x1) {
      offset -= 0x0;
      dummy_register_.write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    return true;
  }

  void reset(
      
      ) {
    dummy_register_.reset();
    if (write_callback_) write_callback_();
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    all_zeros=false;
    if (offset < 0x1) {
      offset -= 0x0;
      r += dummy_register_.to_string(offset,print_zeros,indent_string) ;
    }
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
    all_zeros=false;
    r += dummy_register_.to_string(print_zeros,indent_string) ;
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  MauAddrmapDummyRegisterMutable dummy_register_;
private:
  static uint64_t StartOffset(
      int index_pipe_addrmap, int index_mau_addrmap
      ) {
    uint64_t offset=0;
    offset += 0x20000000000; // to get to pipes
    assert(index_pipe_addrmap < 4);
    offset += index_pipe_addrmap * 0x2000000000; // pipe_addrmap[]
    assert(index_mau_addrmap < 12);
    offset += index_mau_addrmap * 0x200000000; // mau_addrmap[]
    return offset;
  }

};








class MauAddrmapArray : public model_core::RegisterBlockIndirect<RegisterArrayCallback> {
public:
  MauAddrmapArray(
      int chipNumber, int index_pipe_addrmap, RegisterArrayCallback& write_callback = 0, RegisterArrayCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(index_pipe_addrmap), 8589934592 * ArraySize(index_pipe_addrmap, -1), false, write_callback, read_callback, std::string("MauAddrmapArray")+":"+boost::lexical_cast<std::string>(index_pipe_addrmap)),
    array(RealArraySize(index_pipe_addrmap, -1)),
    size0_(ArraySize(index_pipe_addrmap, 0)),
    real_size0_(RealArraySize(index_pipe_addrmap, 0))
    {
    }
public:
  MauAddrmapDummyRegister &dummy_register(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].dummy_register();
  }
  bool calculate_index(
      uint64_t* offset, uint32_t* a0
      ) const {
    int i = (*offset)/8589934592;
    (*offset) -= (i*8589934592);
    uint32_t t = i;
    *a0 = t;
    bool in_bounds = true;
    in_bounds &= CheckArrayBounds(*a0,real_size0_,size0_,*offset,0,true);
    return in_bounds;
  }


  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    if (read_callback_) read_callback_(a0);
    array[a0].read(offset,data0,data1,T);
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    array[a0].write(offset,data0,data1,T);
    if (write_callback_) write_callback_(a0);
    return true;
  }

  void reset(
      
      ) {
    for (uint32_t i=0;i<array.size();++i) {
      array[i].reset();
      int t = i;
      int a0 = t;
      if (write_callback_) write_callback_(a0);
    }
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    all_zeros=false;
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return "OUT_OF_BOUNDS";
    std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
    if (! r_s.empty()) {
      r += indent_string + std::string("MauAddrmapArray") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
    }
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
    all_zeros=false;
    for (uint32_t a0=0;a0<real_size0_;++a0) {
      std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
      if (! r_s.empty()) {
        r += indent_string + std::string("MauAddrmapArray") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
      }
    }
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  std::vector<MauAddrmap> array;
  uint32_t size0_;
  uint32_t real_size0_;
private:
  static uint64_t StartOffset(
      int index_pipe_addrmap
      ) {
    uint64_t offset=0;
    offset += 0x20000000000; // to get to pipes
    assert(index_pipe_addrmap < 4);
    offset += index_pipe_addrmap * 0x2000000000; // pipe_addrmap[]
    return offset;
  }

  static int ArraySize(
      int index_pipe_addrmap, int dimension
      ) {
    switch (dimension) {
      case -1:
        return 12;
        break;
      case 0:
        return 12;
        break;
      default:
        assert(0);
        break;
    }
  }

  static int RealArraySize(
      int index_pipe_addrmap, int dimension
      ) {
    switch (dimension) {
      case -1:
        return 12;
        break;
      case 0:
        return 12;
        break;
      default:
        assert(0);
        break;
    }
  }

};








class MauAddrmapArrayMutable : public model_core::RegisterBlockIndirect<RegisterArrayCallback> {
public:
  MauAddrmapArrayMutable(
      int chipNumber, int index_pipe_addrmap, RegisterArrayCallback& write_callback = 0, RegisterArrayCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(index_pipe_addrmap), 8589934592 * ArraySize(index_pipe_addrmap, -1), true, write_callback, read_callback, std::string("MauAddrmapArrayMutable")+":"+boost::lexical_cast<std::string>(index_pipe_addrmap)),
    array(RealArraySize(index_pipe_addrmap, -1)),
    size0_(ArraySize(index_pipe_addrmap, 0)),
    real_size0_(RealArraySize(index_pipe_addrmap, 0))
    {
    }
public:
  MauAddrmapDummyRegisterMutable &dummy_register(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].dummy_register();
  }
  bool calculate_index(
      uint64_t* offset, uint32_t* a0
      ) const {
    int i = (*offset)/8589934592;
    (*offset) -= (i*8589934592);
    uint32_t t = i;
    *a0 = t;
    bool in_bounds = true;
    in_bounds &= CheckArrayBounds(*a0,real_size0_,size0_,*offset,0,true);
    return in_bounds;
  }


  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    if (read_callback_) read_callback_(a0);
    array[a0].read(offset,data0,data1,T);
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    array[a0].write(offset,data0,data1,T);
    if (write_callback_) write_callback_(a0);
    return true;
  }

  void reset(
      
      ) {
    for (uint32_t i=0;i<array.size();++i) {
      array[i].reset();
      int t = i;
      int a0 = t;
      if (write_callback_) write_callback_(a0);
    }
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    all_zeros=false;
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return "OUT_OF_BOUNDS";
    std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
    if (! r_s.empty()) {
      r += indent_string + std::string("MauAddrmapArrayMutable") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
    }
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
    all_zeros=false;
    for (uint32_t a0=0;a0<real_size0_;++a0) {
      std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
      if (! r_s.empty()) {
        r += indent_string + std::string("MauAddrmapArrayMutable") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
      }
    }
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  std::vector<MauAddrmapMutable> array;
  uint32_t size0_;
  uint32_t real_size0_;
private:
  static uint64_t StartOffset(
      int index_pipe_addrmap
      ) {
    uint64_t offset=0;
    offset += 0x20000000000; // to get to pipes
    assert(index_pipe_addrmap < 4);
    offset += index_pipe_addrmap * 0x2000000000; // pipe_addrmap[]
    return offset;
  }

  static int ArraySize(
      int index_pipe_addrmap, int dimension
      ) {
    switch (dimension) {
      case -1:
        return 12;
        break;
      case 0:
        return 12;
        break;
      default:
        assert(0);
        break;
    }
  }

  static int RealArraySize(
      int index_pipe_addrmap, int dimension
      ) {
    switch (dimension) {
      case -1:
        return 12;
        break;
      case 0:
        return 12;
        break;
      default:
        assert(0);
        break;
    }
  }

};








class PipeAddrmap : public model_core::RegisterBlockIndirect<RegisterCallback> {
public:
  PipeAddrmap(
      int chipNumber, int index_pipe_addrmap, RegisterCallback& write_callback = 0, RegisterCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(index_pipe_addrmap), 122406977536, false, write_callback, read_callback, std::string("PipeAddrmap")+":"+boost::lexical_cast<std::string>(index_pipe_addrmap))
    {
    }
  PipeAddrmap(
      
  )
    : RegisterBlockIndirect(0, 0, 0, false, 0, 0, "PipeAddrmap")
    {
    }
public:







  MauAddrmap &mau(int j0) { return mau_[j0]; }



  PrsrMemMainRspec &i_prsr(int j0) { return i_prsr_[j0]; }





  PartyPgrMemRspec &party_pgr() { return party_pgr_; }



  PrsrMemMainRspec &e_prsr(int j0) { return e_prsr_[j0]; }

  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    if (offset < 0xc) {
      offset -= 0x0;
      int i0 = offset / 0x200000000;
      offset  -= i0 * 0x200000000;
      if (read_callback_) read_callback_();
      mau_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x1c80000000 && offset < 0x1c800072c0) {
      offset -= 0x1c80000000;
      int i0 = offset / 0x2000;
      offset  -= i0 * 0x2000;
      if (read_callback_) read_callback_();
      i_prsr_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x1c8003c000 && offset < 0x1c8003e000) {
      offset -= 0x1c8003c000;
      if (read_callback_) read_callback_();
      party_pgr_.read( offset, data0,data1,T );
    }
    else if (offset >= 0x1c80040000 && offset < 0x1c800472c0) {
      offset -= 0x1c80040000;
      int i0 = offset / 0x2000;
      offset  -= i0 * 0x2000;
      if (read_callback_) read_callback_();
      e_prsr_[ i0 ].read( offset, data0,data1,T );
    }
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    if (offset < 0xc) {
      offset -= 0x0;
      int i0 = offset / 0x200000000;
      offset  -= i0 * 0x200000000;
      mau_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x1c80000000 && offset < 0x1c800072c0) {
      offset -= 0x1c80000000;
      int i0 = offset / 0x2000;
      offset  -= i0 * 0x2000;
      i_prsr_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x1c8003c000 && offset < 0x1c8003e000) {
      offset -= 0x1c8003c000;
      party_pgr_.write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x1c80040000 && offset < 0x1c800472c0) {
      offset -= 0x1c80040000;
      int i0 = offset / 0x2000;
      offset  -= i0 * 0x2000;
      e_prsr_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    return true;
  }

  void reset(
      
      ) {
    for (auto &f0 : mau_) {
      f0.reset();
    }
    for (auto &f0 : i_prsr_) {
      f0.reset();
    }
    party_pgr_.reset();
    for (auto &f0 : e_prsr_) {
      f0.reset();
    }
    if (write_callback_) write_callback_();
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    all_zeros=false;
    if (offset < 0xc) {
      offset -= 0x0;
      int i0 = offset / 0x200000000;
      offset  -= i0 * 0x200000000;
      r += mau_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x1c80000000 && offset < 0x1c800072c0) {
      offset -= 0x1c80000000;
      int i0 = offset / 0x2000;
      offset  -= i0 * 0x2000;
      r += i_prsr_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x1c8003c000 && offset < 0x1c8003e000) {
      offset -= 0x1c8003c000;
      r += party_pgr_.to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x1c80040000 && offset < 0x1c800472c0) {
      offset -= 0x1c80040000;
      int i0 = offset / 0x2000;
      offset  -= i0 * 0x2000;
      r += e_prsr_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
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
    all_zeros=false;
    for (uint32_t a0=0;a0<12;++a0) {
      r += mau_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<18;++a0) {
      r += i_prsr_[a0].to_string(print_zeros,indent_string) ;
    }
    r += party_pgr_.to_string(print_zeros,indent_string) ;
    for (uint32_t a0=0;a0<18;++a0) {
      r += e_prsr_[a0].to_string(print_zeros,indent_string) ;
    }
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  std::array< MauAddrmap, 12 > mau_;
  std::array< PrsrMemMainRspec, 18 > i_prsr_;
  PartyPgrMemRspec party_pgr_;
  std::array< PrsrMemMainRspec, 18 > e_prsr_;
private:
  static uint64_t StartOffset(
      int index_pipe_addrmap
      ) {
    uint64_t offset=0;
    offset += 0x20000000000; // to get to pipes
    assert(index_pipe_addrmap < 4);
    offset += index_pipe_addrmap * 0x2000000000; // pipe_addrmap[]
    return offset;
  }

};








class PipeAddrmapMutable : public model_core::RegisterBlockIndirect<RegisterCallback> {
public:
  PipeAddrmapMutable(
      int chipNumber, int index_pipe_addrmap, RegisterCallback& write_callback = 0, RegisterCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(index_pipe_addrmap), 122406977536, true, write_callback, read_callback, std::string("PipeAddrmapMutable")+":"+boost::lexical_cast<std::string>(index_pipe_addrmap))
    {
    }
  PipeAddrmapMutable(
      
  )
    : RegisterBlockIndirect(0, 0, 0, true, 0, 0, "PipeAddrmapMutable")
    {
    }
public:







  MauAddrmapMutable &mau(int j0) { return mau_[j0]; }



  PrsrMemMainRspecMutable &i_prsr(int j0) { return i_prsr_[j0]; }





  PartyPgrMemRspecMutable &party_pgr() { return party_pgr_; }



  PrsrMemMainRspecMutable &e_prsr(int j0) { return e_prsr_[j0]; }

  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    if (offset < 0xc) {
      offset -= 0x0;
      int i0 = offset / 0x200000000;
      offset  -= i0 * 0x200000000;
      if (read_callback_) read_callback_();
      mau_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x1c80000000 && offset < 0x1c800072c0) {
      offset -= 0x1c80000000;
      int i0 = offset / 0x2000;
      offset  -= i0 * 0x2000;
      if (read_callback_) read_callback_();
      i_prsr_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x1c8003c000 && offset < 0x1c8003e000) {
      offset -= 0x1c8003c000;
      if (read_callback_) read_callback_();
      party_pgr_.read( offset, data0,data1,T );
    }
    else if (offset >= 0x1c80040000 && offset < 0x1c800472c0) {
      offset -= 0x1c80040000;
      int i0 = offset / 0x2000;
      offset  -= i0 * 0x2000;
      if (read_callback_) read_callback_();
      e_prsr_[ i0 ].read( offset, data0,data1,T );
    }
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    if (offset < 0xc) {
      offset -= 0x0;
      int i0 = offset / 0x200000000;
      offset  -= i0 * 0x200000000;
      mau_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x1c80000000 && offset < 0x1c800072c0) {
      offset -= 0x1c80000000;
      int i0 = offset / 0x2000;
      offset  -= i0 * 0x2000;
      i_prsr_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x1c8003c000 && offset < 0x1c8003e000) {
      offset -= 0x1c8003c000;
      party_pgr_.write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x1c80040000 && offset < 0x1c800472c0) {
      offset -= 0x1c80040000;
      int i0 = offset / 0x2000;
      offset  -= i0 * 0x2000;
      e_prsr_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    return true;
  }

  void reset(
      
      ) {
    for (auto &f0 : mau_) {
      f0.reset();
    }
    for (auto &f0 : i_prsr_) {
      f0.reset();
    }
    party_pgr_.reset();
    for (auto &f0 : e_prsr_) {
      f0.reset();
    }
    if (write_callback_) write_callback_();
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    all_zeros=false;
    if (offset < 0xc) {
      offset -= 0x0;
      int i0 = offset / 0x200000000;
      offset  -= i0 * 0x200000000;
      r += mau_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x1c80000000 && offset < 0x1c800072c0) {
      offset -= 0x1c80000000;
      int i0 = offset / 0x2000;
      offset  -= i0 * 0x2000;
      r += i_prsr_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x1c8003c000 && offset < 0x1c8003e000) {
      offset -= 0x1c8003c000;
      r += party_pgr_.to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x1c80040000 && offset < 0x1c800472c0) {
      offset -= 0x1c80040000;
      int i0 = offset / 0x2000;
      offset  -= i0 * 0x2000;
      r += e_prsr_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
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
    all_zeros=false;
    for (uint32_t a0=0;a0<12;++a0) {
      r += mau_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<18;++a0) {
      r += i_prsr_[a0].to_string(print_zeros,indent_string) ;
    }
    r += party_pgr_.to_string(print_zeros,indent_string) ;
    for (uint32_t a0=0;a0<18;++a0) {
      r += e_prsr_[a0].to_string(print_zeros,indent_string) ;
    }
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  std::array< MauAddrmapMutable, 12 > mau_;
  std::array< PrsrMemMainRspecMutable, 18 > i_prsr_;
  PartyPgrMemRspecMutable party_pgr_;
  std::array< PrsrMemMainRspecMutable, 18 > e_prsr_;
private:
  static uint64_t StartOffset(
      int index_pipe_addrmap
      ) {
    uint64_t offset=0;
    offset += 0x20000000000; // to get to pipes
    assert(index_pipe_addrmap < 4);
    offset += index_pipe_addrmap * 0x2000000000; // pipe_addrmap[]
    return offset;
  }

};








class PipeAddrmapArray : public model_core::RegisterBlockIndirect<RegisterArrayCallback> {
public:
  PipeAddrmapArray(
      int chipNumber, RegisterArrayCallback& write_callback = 0, RegisterArrayCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(), 137438953472 * ArraySize(-1), false, write_callback, read_callback, std::string("PipeAddrmapArray")),
    array(RealArraySize(-1)),
    size0_(ArraySize(0)),
    real_size0_(RealArraySize(0))
    {
    }
public:







  MauAddrmap &mau(uint32_t a0,int j0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].mau(j0);
  }



  PrsrMemMainRspec &i_prsr(uint32_t a0,int j0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].i_prsr(j0);
  }





  PartyPgrMemRspec &party_pgr(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].party_pgr();
  }



  PrsrMemMainRspec &e_prsr(uint32_t a0,int j0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].e_prsr(j0);
  }
  bool calculate_index(
      uint64_t* offset, uint32_t* a0
      ) const {
    int i = (*offset)/137438953472;
    (*offset) -= (i*137438953472);
    uint32_t t = i;
    *a0 = t;
    bool in_bounds = true;
    in_bounds &= CheckArrayBounds(*a0,real_size0_,size0_,*offset,0,true);
    return in_bounds;
  }


  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    if (read_callback_) read_callback_(a0);
    array[a0].read(offset,data0,data1,T);
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    array[a0].write(offset,data0,data1,T);
    if (write_callback_) write_callback_(a0);
    return true;
  }

  void reset(
      
      ) {
    for (uint32_t i=0;i<array.size();++i) {
      array[i].reset();
      int t = i;
      int a0 = t;
      if (write_callback_) write_callback_(a0);
    }
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    all_zeros=false;
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return "OUT_OF_BOUNDS";
    std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
    if (! r_s.empty()) {
      r += indent_string + std::string("PipeAddrmapArray") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
    }
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
    all_zeros=false;
    for (uint32_t a0=0;a0<real_size0_;++a0) {
      std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
      if (! r_s.empty()) {
        r += indent_string + std::string("PipeAddrmapArray") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
      }
    }
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  std::vector<PipeAddrmap> array;
  uint32_t size0_;
  uint32_t real_size0_;
private:
  static uint64_t StartOffset(
      
      ) {
    uint64_t offset=0;
    offset += 0x20000000000; // to get to pipes
    return offset;
  }

  static int ArraySize(
      int dimension
      ) {
    switch (dimension) {
      case -1:
        return 4;
        break;
      case 0:
        return 4;
        break;
      default:
        assert(0);
        break;
    }
  }

  static int RealArraySize(
      int dimension
      ) {
    switch (dimension) {
      case -1:
        return 4;
        break;
      case 0:
        return 4;
        break;
      default:
        assert(0);
        break;
    }
  }

};








class PipeAddrmapArrayMutable : public model_core::RegisterBlockIndirect<RegisterArrayCallback> {
public:
  PipeAddrmapArrayMutable(
      int chipNumber, RegisterArrayCallback& write_callback = 0, RegisterArrayCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(), 137438953472 * ArraySize(-1), true, write_callback, read_callback, std::string("PipeAddrmapArrayMutable")),
    array(RealArraySize(-1)),
    size0_(ArraySize(0)),
    real_size0_(RealArraySize(0))
    {
    }
public:







  MauAddrmapMutable &mau(uint32_t a0,int j0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].mau(j0);
  }



  PrsrMemMainRspecMutable &i_prsr(uint32_t a0,int j0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].i_prsr(j0);
  }





  PartyPgrMemRspecMutable &party_pgr(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].party_pgr();
  }



  PrsrMemMainRspecMutable &e_prsr(uint32_t a0,int j0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].e_prsr(j0);
  }
  bool calculate_index(
      uint64_t* offset, uint32_t* a0
      ) const {
    int i = (*offset)/137438953472;
    (*offset) -= (i*137438953472);
    uint32_t t = i;
    *a0 = t;
    bool in_bounds = true;
    in_bounds &= CheckArrayBounds(*a0,real_size0_,size0_,*offset,0,true);
    return in_bounds;
  }


  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    if (read_callback_) read_callback_(a0);
    array[a0].read(offset,data0,data1,T);
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    array[a0].write(offset,data0,data1,T);
    if (write_callback_) write_callback_(a0);
    return true;
  }

  void reset(
      
      ) {
    for (uint32_t i=0;i<array.size();++i) {
      array[i].reset();
      int t = i;
      int a0 = t;
      if (write_callback_) write_callback_(a0);
    }
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    all_zeros=false;
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return "OUT_OF_BOUNDS";
    std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
    if (! r_s.empty()) {
      r += indent_string + std::string("PipeAddrmapArrayMutable") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
    }
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
    all_zeros=false;
    for (uint32_t a0=0;a0<real_size0_;++a0) {
      std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
      if (! r_s.empty()) {
        r += indent_string + std::string("PipeAddrmapArrayMutable") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
      }
    }
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  std::vector<PipeAddrmapMutable> array;
  uint32_t size0_;
  uint32_t real_size0_;
private:
  static uint64_t StartOffset(
      
      ) {
    uint64_t offset=0;
    offset += 0x20000000000; // to get to pipes
    return offset;
  }

  static int ArraySize(
      int dimension
      ) {
    switch (dimension) {
      case -1:
        return 4;
        break;
      case 0:
        return 4;
        break;
      default:
        assert(0);
        break;
    }
  }

  static int RealArraySize(
      int dimension
      ) {
    switch (dimension) {
      case -1:
        return 4;
        break;
      case 0:
        return 4;
        break;
      default:
        assert(0);
        break;
    }
  }

};








class TmPrePmtMemWord : public model_core::RegisterBlockIndirect<RegisterCallback> {
public:
  enum TmPreMemRspecEnum {
    kPmt0MemWord0,
    kPmt0MemWord1,
    kPmt0MemWord2,
    kPmt0MemWord3,
    kPmt1MemWord0,
    kPmt1MemWord1,
    kPmt1MemWord2,
    kPmt1MemWord3
  };
public:
  TmPrePmtMemWord(
      int chipNumber, TmPreMemRspecEnum selector_tm_pre_mem_rspec, int index_tm_pre_pmt_mem_word, RegisterCallback& write_callback = 0, RegisterCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(selector_tm_pre_mem_rspec, index_tm_pre_pmt_mem_word), 1, false, write_callback, read_callback, std::string("TmPrePmtMemWord")+":"+boost::lexical_cast<std::string>(selector_tm_pre_mem_rspec) + "," + boost::lexical_cast<std::string>(index_tm_pre_pmt_mem_word))
    {
    }
  TmPrePmtMemWord(
      
  )
    : RegisterBlockIndirect(0, 0, 0, false, 0, 0, "TmPrePmtMemWord")
    {
    }
public:




  uint8_t &mem_word() { return mem_word_; }

  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    if (read_callback_) read_callback_();
    *data0 = (static_cast<uint64_t>(mem_word_) & 0x1);
    *data1 = 0;
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    mem_word_ = (data0 & 0x1);
    if (write_callback_) write_callback_();
    return true;
  }

  void reset(
      
      ) {
    mem_word_ = 0;
    if (write_callback_) write_callback_();
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    r += indent_string + std::string("TmPrePmtMemWord") + ":\n";
    r += indent_string + "  " + std::string("mem_word") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(mem_word_) ) + "\n";
    all_zeros &= (0 == mem_word_);
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
    r += indent_string + std::string("TmPrePmtMemWord") + ":\n";
    r += indent_string + "  " + std::string("mem_word") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(mem_word_) ) + "\n";
    all_zeros &= (0 == mem_word_);
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  uint8_t mem_word_;
private:
  static uint64_t StartOffset(
      TmPreMemRspecEnum selector_tm_pre_mem_rspec, int index_tm_pre_pmt_mem_word
      ) {
    uint64_t offset=0;
    offset += 0x6100000000; // to get to tm_pre
    switch (selector_tm_pre_mem_rspec) {
      case kPmt0MemWord0:
        offset += 0x80200000; // to get to pmt0_mem_word0
        assert(index_tm_pre_pmt_mem_word < 288);
        offset += index_tm_pre_pmt_mem_word * 0x1; // tm_pre_pmt_mem_word[]
        break;
      case kPmt0MemWord1:
        offset += 0x80240000; // to get to pmt0_mem_word1
        assert(index_tm_pre_pmt_mem_word < 288);
        offset += index_tm_pre_pmt_mem_word * 0x1; // tm_pre_pmt_mem_word[]
        break;
      case kPmt0MemWord2:
        offset += 0x80280000; // to get to pmt0_mem_word2
        assert(index_tm_pre_pmt_mem_word < 288);
        offset += index_tm_pre_pmt_mem_word * 0x1; // tm_pre_pmt_mem_word[]
        break;
      case kPmt0MemWord3:
        offset += 0x802c0000; // to get to pmt0_mem_word3
        assert(index_tm_pre_pmt_mem_word < 288);
        offset += index_tm_pre_pmt_mem_word * 0x1; // tm_pre_pmt_mem_word[]
        break;
      case kPmt1MemWord0:
        offset += 0x80300000; // to get to pmt1_mem_word0
        assert(index_tm_pre_pmt_mem_word < 288);
        offset += index_tm_pre_pmt_mem_word * 0x1; // tm_pre_pmt_mem_word[]
        break;
      case kPmt1MemWord1:
        offset += 0x80340000; // to get to pmt1_mem_word1
        assert(index_tm_pre_pmt_mem_word < 288);
        offset += index_tm_pre_pmt_mem_word * 0x1; // tm_pre_pmt_mem_word[]
        break;
      case kPmt1MemWord2:
        offset += 0x80380000; // to get to pmt1_mem_word2
        assert(index_tm_pre_pmt_mem_word < 288);
        offset += index_tm_pre_pmt_mem_word * 0x1; // tm_pre_pmt_mem_word[]
        break;
      case kPmt1MemWord3:
        offset += 0x803c0000; // to get to pmt1_mem_word3
        assert(index_tm_pre_pmt_mem_word < 288);
        offset += index_tm_pre_pmt_mem_word * 0x1; // tm_pre_pmt_mem_word[]
        break;
      default:
        assert(0);
        break;
    }
    return offset;
  }

};








class TmPrePmtMemWordMutable : public model_core::RegisterBlockIndirect<RegisterCallback> {
public:
  enum TmPreMemRspecEnum {
    kPmt0MemWord0,
    kPmt0MemWord1,
    kPmt0MemWord2,
    kPmt0MemWord3,
    kPmt1MemWord0,
    kPmt1MemWord1,
    kPmt1MemWord2,
    kPmt1MemWord3
  };
public:
  TmPrePmtMemWordMutable(
      int chipNumber, TmPreMemRspecEnum selector_tm_pre_mem_rspec, int index_tm_pre_pmt_mem_word, RegisterCallback& write_callback = 0, RegisterCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(selector_tm_pre_mem_rspec, index_tm_pre_pmt_mem_word), 1, true, write_callback, read_callback, std::string("TmPrePmtMemWordMutable")+":"+boost::lexical_cast<std::string>(selector_tm_pre_mem_rspec) + "," + boost::lexical_cast<std::string>(index_tm_pre_pmt_mem_word))
    {
    }
  TmPrePmtMemWordMutable(
      
  )
    : RegisterBlockIndirect(0, 0, 0, true, 0, 0, "TmPrePmtMemWordMutable")
    {
    }
public:




  uint8_t mem_word() { return mem_word_; }
  void mem_word(const uint8_t &v) { mem_word_=v; }

  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    if (read_callback_) read_callback_();
    *data0 = (static_cast<uint64_t>(mem_word_) & 0x1);
    *data1 = 0;
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    mem_word_ = (data0 & 0x1);
    if (write_callback_) write_callback_();
    return true;
  }

  void reset(
      
      ) {
    mem_word_ = 0;
    if (write_callback_) write_callback_();
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    r += indent_string + std::string("TmPrePmtMemWordMutable") + ":\n";
    r += indent_string + "  " + std::string("mem_word") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(mem_word_) ) + "\n";
    all_zeros &= (0 == mem_word_);
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
    r += indent_string + std::string("TmPrePmtMemWordMutable") + ":\n";
    r += indent_string + "  " + std::string("mem_word") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(mem_word_) ) + "\n";
    all_zeros &= (0 == mem_word_);
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  uint8_t mem_word_;
private:
  static uint64_t StartOffset(
      TmPreMemRspecEnum selector_tm_pre_mem_rspec, int index_tm_pre_pmt_mem_word
      ) {
    uint64_t offset=0;
    offset += 0x6100000000; // to get to tm_pre
    switch (selector_tm_pre_mem_rspec) {
      case kPmt0MemWord0:
        offset += 0x80200000; // to get to pmt0_mem_word0
        assert(index_tm_pre_pmt_mem_word < 288);
        offset += index_tm_pre_pmt_mem_word * 0x1; // tm_pre_pmt_mem_word[]
        break;
      case kPmt0MemWord1:
        offset += 0x80240000; // to get to pmt0_mem_word1
        assert(index_tm_pre_pmt_mem_word < 288);
        offset += index_tm_pre_pmt_mem_word * 0x1; // tm_pre_pmt_mem_word[]
        break;
      case kPmt0MemWord2:
        offset += 0x80280000; // to get to pmt0_mem_word2
        assert(index_tm_pre_pmt_mem_word < 288);
        offset += index_tm_pre_pmt_mem_word * 0x1; // tm_pre_pmt_mem_word[]
        break;
      case kPmt0MemWord3:
        offset += 0x802c0000; // to get to pmt0_mem_word3
        assert(index_tm_pre_pmt_mem_word < 288);
        offset += index_tm_pre_pmt_mem_word * 0x1; // tm_pre_pmt_mem_word[]
        break;
      case kPmt1MemWord0:
        offset += 0x80300000; // to get to pmt1_mem_word0
        assert(index_tm_pre_pmt_mem_word < 288);
        offset += index_tm_pre_pmt_mem_word * 0x1; // tm_pre_pmt_mem_word[]
        break;
      case kPmt1MemWord1:
        offset += 0x80340000; // to get to pmt1_mem_word1
        assert(index_tm_pre_pmt_mem_word < 288);
        offset += index_tm_pre_pmt_mem_word * 0x1; // tm_pre_pmt_mem_word[]
        break;
      case kPmt1MemWord2:
        offset += 0x80380000; // to get to pmt1_mem_word2
        assert(index_tm_pre_pmt_mem_word < 288);
        offset += index_tm_pre_pmt_mem_word * 0x1; // tm_pre_pmt_mem_word[]
        break;
      case kPmt1MemWord3:
        offset += 0x803c0000; // to get to pmt1_mem_word3
        assert(index_tm_pre_pmt_mem_word < 288);
        offset += index_tm_pre_pmt_mem_word * 0x1; // tm_pre_pmt_mem_word[]
        break;
      default:
        assert(0);
        break;
    }
    return offset;
  }

};








class TmPrePmtMemWordArray : public model_core::RegisterBlockIndirect<RegisterArrayCallback> {
public:
  enum TmPreMemRspecEnum {
    kPmt0MemWord0,
    kPmt0MemWord1,
    kPmt0MemWord2,
    kPmt0MemWord3,
    kPmt1MemWord0,
    kPmt1MemWord1,
    kPmt1MemWord2,
    kPmt1MemWord3
  };
public:
  TmPrePmtMemWordArray(
      int chipNumber, TmPreMemRspecEnum selector_tm_pre_mem_rspec, RegisterArrayCallback& write_callback = 0, RegisterArrayCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(selector_tm_pre_mem_rspec), 1 * ArraySize(selector_tm_pre_mem_rspec, -1), false, write_callback, read_callback, std::string("TmPrePmtMemWordArray")+":"+boost::lexical_cast<std::string>(selector_tm_pre_mem_rspec)),
    array(RealArraySize(selector_tm_pre_mem_rspec, -1)),
    size0_(ArraySize(selector_tm_pre_mem_rspec, 0)),
    real_size0_(RealArraySize(selector_tm_pre_mem_rspec, 0))
    {
    }
public:




  uint8_t &mem_word(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].mem_word();
  }
  bool calculate_index(
      uint64_t* offset, uint32_t* a0
      ) const {
    int i = (*offset)/1;
    (*offset) -= (i*1);
    uint32_t t = i;
    *a0 = t;
    bool in_bounds = true;
    in_bounds &= CheckArrayBounds(*a0,real_size0_,size0_,*offset,0,true);
    return in_bounds;
  }


  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    if (read_callback_) read_callback_(a0);
    array[a0].read(offset,data0,data1,T);
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    array[a0].write(offset,data0,data1,T);
    if (write_callback_) write_callback_(a0);
    return true;
  }

  void reset(
      
      ) {
    for (uint32_t i=0;i<array.size();++i) {
      array[i].reset();
      int t = i;
      int a0 = t;
      if (write_callback_) write_callback_(a0);
    }
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    all_zeros=false;
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return "OUT_OF_BOUNDS";
    std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
    if (! r_s.empty()) {
      r += indent_string + std::string("TmPrePmtMemWordArray") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
    }
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
    all_zeros=false;
    for (uint32_t a0=0;a0<real_size0_;++a0) {
      std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
      if (! r_s.empty()) {
        r += indent_string + std::string("TmPrePmtMemWordArray") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
      }
    }
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  std::vector<TmPrePmtMemWord> array;
  uint32_t size0_;
  uint32_t real_size0_;
private:
  static uint64_t StartOffset(
      TmPreMemRspecEnum selector_tm_pre_mem_rspec
      ) {
    uint64_t offset=0;
    offset += 0x6100000000; // to get to tm_pre
    switch (selector_tm_pre_mem_rspec) {
      case kPmt0MemWord0:
        offset += 0x80200000; // to get to pmt0_mem_word0
        break;
      case kPmt0MemWord1:
        offset += 0x80240000; // to get to pmt0_mem_word1
        break;
      case kPmt0MemWord2:
        offset += 0x80280000; // to get to pmt0_mem_word2
        break;
      case kPmt0MemWord3:
        offset += 0x802c0000; // to get to pmt0_mem_word3
        break;
      case kPmt1MemWord0:
        offset += 0x80300000; // to get to pmt1_mem_word0
        break;
      case kPmt1MemWord1:
        offset += 0x80340000; // to get to pmt1_mem_word1
        break;
      case kPmt1MemWord2:
        offset += 0x80380000; // to get to pmt1_mem_word2
        break;
      case kPmt1MemWord3:
        offset += 0x803c0000; // to get to pmt1_mem_word3
        break;
      default:
        assert(0);
        break;
    }
    return offset;
  }

  static int ArraySize(
      TmPreMemRspecEnum selector_tm_pre_mem_rspec, int dimension
      ) {
    switch (selector_tm_pre_mem_rspec) {
      case kPmt0MemWord0:
        switch (dimension) {
          case -1:
            return 288;
            break;
          case 0:
            return 288;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kPmt0MemWord1:
        switch (dimension) {
          case -1:
            return 288;
            break;
          case 0:
            return 288;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kPmt0MemWord2:
        switch (dimension) {
          case -1:
            return 288;
            break;
          case 0:
            return 288;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kPmt0MemWord3:
        switch (dimension) {
          case -1:
            return 288;
            break;
          case 0:
            return 288;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kPmt1MemWord0:
        switch (dimension) {
          case -1:
            return 288;
            break;
          case 0:
            return 288;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kPmt1MemWord1:
        switch (dimension) {
          case -1:
            return 288;
            break;
          case 0:
            return 288;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kPmt1MemWord2:
        switch (dimension) {
          case -1:
            return 288;
            break;
          case 0:
            return 288;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kPmt1MemWord3:
        switch (dimension) {
          case -1:
            return 288;
            break;
          case 0:
            return 288;
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
  }

  static int RealArraySize(
      TmPreMemRspecEnum selector_tm_pre_mem_rspec, int dimension
      ) {
    switch (selector_tm_pre_mem_rspec) {
      case kPmt0MemWord0:
        switch (dimension) {
          case -1:
            return 288;
            break;
          case 0:
            return 288;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kPmt0MemWord1:
        switch (dimension) {
          case -1:
            return 288;
            break;
          case 0:
            return 288;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kPmt0MemWord2:
        switch (dimension) {
          case -1:
            return 288;
            break;
          case 0:
            return 288;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kPmt0MemWord3:
        switch (dimension) {
          case -1:
            return 288;
            break;
          case 0:
            return 288;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kPmt1MemWord0:
        switch (dimension) {
          case -1:
            return 288;
            break;
          case 0:
            return 288;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kPmt1MemWord1:
        switch (dimension) {
          case -1:
            return 288;
            break;
          case 0:
            return 288;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kPmt1MemWord2:
        switch (dimension) {
          case -1:
            return 288;
            break;
          case 0:
            return 288;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kPmt1MemWord3:
        switch (dimension) {
          case -1:
            return 288;
            break;
          case 0:
            return 288;
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
  }

};








class TmPrePmtMemWordArrayMutable : public model_core::RegisterBlockIndirect<RegisterArrayCallback> {
public:
  enum TmPreMemRspecEnum {
    kPmt0MemWord0,
    kPmt0MemWord1,
    kPmt0MemWord2,
    kPmt0MemWord3,
    kPmt1MemWord0,
    kPmt1MemWord1,
    kPmt1MemWord2,
    kPmt1MemWord3
  };
public:
  TmPrePmtMemWordArrayMutable(
      int chipNumber, TmPreMemRspecEnum selector_tm_pre_mem_rspec, RegisterArrayCallback& write_callback = 0, RegisterArrayCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(selector_tm_pre_mem_rspec), 1 * ArraySize(selector_tm_pre_mem_rspec, -1), true, write_callback, read_callback, std::string("TmPrePmtMemWordArrayMutable")+":"+boost::lexical_cast<std::string>(selector_tm_pre_mem_rspec)),
    array(RealArraySize(selector_tm_pre_mem_rspec, -1)),
    size0_(ArraySize(selector_tm_pre_mem_rspec, 0)),
    real_size0_(RealArraySize(selector_tm_pre_mem_rspec, 0))
    {
    }
public:




  uint8_t mem_word(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].mem_word();
  }
  void mem_word(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].mem_word(v);
  }
  bool calculate_index(
      uint64_t* offset, uint32_t* a0
      ) const {
    int i = (*offset)/1;
    (*offset) -= (i*1);
    uint32_t t = i;
    *a0 = t;
    bool in_bounds = true;
    in_bounds &= CheckArrayBounds(*a0,real_size0_,size0_,*offset,0,true);
    return in_bounds;
  }


  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    if (read_callback_) read_callback_(a0);
    array[a0].read(offset,data0,data1,T);
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    array[a0].write(offset,data0,data1,T);
    if (write_callback_) write_callback_(a0);
    return true;
  }

  void reset(
      
      ) {
    for (uint32_t i=0;i<array.size();++i) {
      array[i].reset();
      int t = i;
      int a0 = t;
      if (write_callback_) write_callback_(a0);
    }
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    all_zeros=false;
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return "OUT_OF_BOUNDS";
    std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
    if (! r_s.empty()) {
      r += indent_string + std::string("TmPrePmtMemWordArrayMutable") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
    }
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
    all_zeros=false;
    for (uint32_t a0=0;a0<real_size0_;++a0) {
      std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
      if (! r_s.empty()) {
        r += indent_string + std::string("TmPrePmtMemWordArrayMutable") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
      }
    }
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  std::vector<TmPrePmtMemWordMutable> array;
  uint32_t size0_;
  uint32_t real_size0_;
private:
  static uint64_t StartOffset(
      TmPreMemRspecEnum selector_tm_pre_mem_rspec
      ) {
    uint64_t offset=0;
    offset += 0x6100000000; // to get to tm_pre
    switch (selector_tm_pre_mem_rspec) {
      case kPmt0MemWord0:
        offset += 0x80200000; // to get to pmt0_mem_word0
        break;
      case kPmt0MemWord1:
        offset += 0x80240000; // to get to pmt0_mem_word1
        break;
      case kPmt0MemWord2:
        offset += 0x80280000; // to get to pmt0_mem_word2
        break;
      case kPmt0MemWord3:
        offset += 0x802c0000; // to get to pmt0_mem_word3
        break;
      case kPmt1MemWord0:
        offset += 0x80300000; // to get to pmt1_mem_word0
        break;
      case kPmt1MemWord1:
        offset += 0x80340000; // to get to pmt1_mem_word1
        break;
      case kPmt1MemWord2:
        offset += 0x80380000; // to get to pmt1_mem_word2
        break;
      case kPmt1MemWord3:
        offset += 0x803c0000; // to get to pmt1_mem_word3
        break;
      default:
        assert(0);
        break;
    }
    return offset;
  }

  static int ArraySize(
      TmPreMemRspecEnum selector_tm_pre_mem_rspec, int dimension
      ) {
    switch (selector_tm_pre_mem_rspec) {
      case kPmt0MemWord0:
        switch (dimension) {
          case -1:
            return 288;
            break;
          case 0:
            return 288;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kPmt0MemWord1:
        switch (dimension) {
          case -1:
            return 288;
            break;
          case 0:
            return 288;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kPmt0MemWord2:
        switch (dimension) {
          case -1:
            return 288;
            break;
          case 0:
            return 288;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kPmt0MemWord3:
        switch (dimension) {
          case -1:
            return 288;
            break;
          case 0:
            return 288;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kPmt1MemWord0:
        switch (dimension) {
          case -1:
            return 288;
            break;
          case 0:
            return 288;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kPmt1MemWord1:
        switch (dimension) {
          case -1:
            return 288;
            break;
          case 0:
            return 288;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kPmt1MemWord2:
        switch (dimension) {
          case -1:
            return 288;
            break;
          case 0:
            return 288;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kPmt1MemWord3:
        switch (dimension) {
          case -1:
            return 288;
            break;
          case 0:
            return 288;
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
  }

  static int RealArraySize(
      TmPreMemRspecEnum selector_tm_pre_mem_rspec, int dimension
      ) {
    switch (selector_tm_pre_mem_rspec) {
      case kPmt0MemWord0:
        switch (dimension) {
          case -1:
            return 288;
            break;
          case 0:
            return 288;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kPmt0MemWord1:
        switch (dimension) {
          case -1:
            return 288;
            break;
          case 0:
            return 288;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kPmt0MemWord2:
        switch (dimension) {
          case -1:
            return 288;
            break;
          case 0:
            return 288;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kPmt0MemWord3:
        switch (dimension) {
          case -1:
            return 288;
            break;
          case 0:
            return 288;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kPmt1MemWord0:
        switch (dimension) {
          case -1:
            return 288;
            break;
          case 0:
            return 288;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kPmt1MemWord1:
        switch (dimension) {
          case -1:
            return 288;
            break;
          case 0:
            return 288;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kPmt1MemWord2:
        switch (dimension) {
          case -1:
            return 288;
            break;
          case 0:
            return 288;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kPmt1MemWord3:
        switch (dimension) {
          case -1:
            return 288;
            break;
          case 0:
            return 288;
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
  }

};








class TmPreLitBmMemWord : public model_core::RegisterBlockIndirect<RegisterCallback> {
public:
  enum TmPreMemRspecEnum {
    kLit0BmMemWord0,
    kLit0BmMemWord1,
    kLit0BmMemWord2,
    kLit0BmMemWord3,
    kLit1BmMemWord0,
    kLit1BmMemWord1,
    kLit1BmMemWord2,
    kLit1BmMemWord3
  };
public:
  TmPreLitBmMemWord(
      int chipNumber, TmPreMemRspecEnum selector_tm_pre_mem_rspec, int index_tm_pre_lit_bm_mem_word, RegisterCallback& write_callback = 0, RegisterCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(selector_tm_pre_mem_rspec, index_tm_pre_lit_bm_mem_word), 1, false, write_callback, read_callback, std::string("TmPreLitBmMemWord")+":"+boost::lexical_cast<std::string>(selector_tm_pre_mem_rspec) + "," + boost::lexical_cast<std::string>(index_tm_pre_lit_bm_mem_word))
    {
    }
  TmPreLitBmMemWord(
      
  )
    : RegisterBlockIndirect(0, 0, 0, false, 0, 0, "TmPreLitBmMemWord")
    {
    }
public:




  uint8_t &mem_word() { return mem_word_; }

  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    if (read_callback_) read_callback_();
    *data0 = (static_cast<uint64_t>(mem_word_) & 0x1);
    *data1 = 0;
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    mem_word_ = (data0 & 0x1);
    if (write_callback_) write_callback_();
    return true;
  }

  void reset(
      
      ) {
    mem_word_ = 0;
    if (write_callback_) write_callback_();
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    r += indent_string + std::string("TmPreLitBmMemWord") + ":\n";
    r += indent_string + "  " + std::string("mem_word") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(mem_word_) ) + "\n";
    all_zeros &= (0 == mem_word_);
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
    r += indent_string + std::string("TmPreLitBmMemWord") + ":\n";
    r += indent_string + "  " + std::string("mem_word") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(mem_word_) ) + "\n";
    all_zeros &= (0 == mem_word_);
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  uint8_t mem_word_;
private:
  static uint64_t StartOffset(
      TmPreMemRspecEnum selector_tm_pre_mem_rspec, int index_tm_pre_lit_bm_mem_word
      ) {
    uint64_t offset=0;
    offset += 0x6100000000; // to get to tm_pre
    switch (selector_tm_pre_mem_rspec) {
      case kLit0BmMemWord0:
        offset += 0x80000000; // to get to lit0_bm_mem_word0
        assert(index_tm_pre_lit_bm_mem_word < 256);
        offset += index_tm_pre_lit_bm_mem_word * 0x1; // tm_pre_lit_bm_mem_word[]
        break;
      case kLit0BmMemWord1:
        offset += 0x80040000; // to get to lit0_bm_mem_word1
        assert(index_tm_pre_lit_bm_mem_word < 256);
        offset += index_tm_pre_lit_bm_mem_word * 0x1; // tm_pre_lit_bm_mem_word[]
        break;
      case kLit0BmMemWord2:
        offset += 0x80080000; // to get to lit0_bm_mem_word2
        assert(index_tm_pre_lit_bm_mem_word < 256);
        offset += index_tm_pre_lit_bm_mem_word * 0x1; // tm_pre_lit_bm_mem_word[]
        break;
      case kLit0BmMemWord3:
        offset += 0x800c0000; // to get to lit0_bm_mem_word3
        assert(index_tm_pre_lit_bm_mem_word < 256);
        offset += index_tm_pre_lit_bm_mem_word * 0x1; // tm_pre_lit_bm_mem_word[]
        break;
      case kLit1BmMemWord0:
        offset += 0x80100000; // to get to lit1_bm_mem_word0
        assert(index_tm_pre_lit_bm_mem_word < 256);
        offset += index_tm_pre_lit_bm_mem_word * 0x1; // tm_pre_lit_bm_mem_word[]
        break;
      case kLit1BmMemWord1:
        offset += 0x80140000; // to get to lit1_bm_mem_word1
        assert(index_tm_pre_lit_bm_mem_word < 256);
        offset += index_tm_pre_lit_bm_mem_word * 0x1; // tm_pre_lit_bm_mem_word[]
        break;
      case kLit1BmMemWord2:
        offset += 0x80180000; // to get to lit1_bm_mem_word2
        assert(index_tm_pre_lit_bm_mem_word < 256);
        offset += index_tm_pre_lit_bm_mem_word * 0x1; // tm_pre_lit_bm_mem_word[]
        break;
      case kLit1BmMemWord3:
        offset += 0x801c0000; // to get to lit1_bm_mem_word3
        assert(index_tm_pre_lit_bm_mem_word < 256);
        offset += index_tm_pre_lit_bm_mem_word * 0x1; // tm_pre_lit_bm_mem_word[]
        break;
      default:
        assert(0);
        break;
    }
    return offset;
  }

};








class TmPreLitBmMemWordMutable : public model_core::RegisterBlockIndirect<RegisterCallback> {
public:
  enum TmPreMemRspecEnum {
    kLit0BmMemWord0,
    kLit0BmMemWord1,
    kLit0BmMemWord2,
    kLit0BmMemWord3,
    kLit1BmMemWord0,
    kLit1BmMemWord1,
    kLit1BmMemWord2,
    kLit1BmMemWord3
  };
public:
  TmPreLitBmMemWordMutable(
      int chipNumber, TmPreMemRspecEnum selector_tm_pre_mem_rspec, int index_tm_pre_lit_bm_mem_word, RegisterCallback& write_callback = 0, RegisterCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(selector_tm_pre_mem_rspec, index_tm_pre_lit_bm_mem_word), 1, true, write_callback, read_callback, std::string("TmPreLitBmMemWordMutable")+":"+boost::lexical_cast<std::string>(selector_tm_pre_mem_rspec) + "," + boost::lexical_cast<std::string>(index_tm_pre_lit_bm_mem_word))
    {
    }
  TmPreLitBmMemWordMutable(
      
  )
    : RegisterBlockIndirect(0, 0, 0, true, 0, 0, "TmPreLitBmMemWordMutable")
    {
    }
public:




  uint8_t mem_word() { return mem_word_; }
  void mem_word(const uint8_t &v) { mem_word_=v; }

  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    if (read_callback_) read_callback_();
    *data0 = (static_cast<uint64_t>(mem_word_) & 0x1);
    *data1 = 0;
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    mem_word_ = (data0 & 0x1);
    if (write_callback_) write_callback_();
    return true;
  }

  void reset(
      
      ) {
    mem_word_ = 0;
    if (write_callback_) write_callback_();
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    r += indent_string + std::string("TmPreLitBmMemWordMutable") + ":\n";
    r += indent_string + "  " + std::string("mem_word") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(mem_word_) ) + "\n";
    all_zeros &= (0 == mem_word_);
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
    r += indent_string + std::string("TmPreLitBmMemWordMutable") + ":\n";
    r += indent_string + "  " + std::string("mem_word") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(mem_word_) ) + "\n";
    all_zeros &= (0 == mem_word_);
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  uint8_t mem_word_;
private:
  static uint64_t StartOffset(
      TmPreMemRspecEnum selector_tm_pre_mem_rspec, int index_tm_pre_lit_bm_mem_word
      ) {
    uint64_t offset=0;
    offset += 0x6100000000; // to get to tm_pre
    switch (selector_tm_pre_mem_rspec) {
      case kLit0BmMemWord0:
        offset += 0x80000000; // to get to lit0_bm_mem_word0
        assert(index_tm_pre_lit_bm_mem_word < 256);
        offset += index_tm_pre_lit_bm_mem_word * 0x1; // tm_pre_lit_bm_mem_word[]
        break;
      case kLit0BmMemWord1:
        offset += 0x80040000; // to get to lit0_bm_mem_word1
        assert(index_tm_pre_lit_bm_mem_word < 256);
        offset += index_tm_pre_lit_bm_mem_word * 0x1; // tm_pre_lit_bm_mem_word[]
        break;
      case kLit0BmMemWord2:
        offset += 0x80080000; // to get to lit0_bm_mem_word2
        assert(index_tm_pre_lit_bm_mem_word < 256);
        offset += index_tm_pre_lit_bm_mem_word * 0x1; // tm_pre_lit_bm_mem_word[]
        break;
      case kLit0BmMemWord3:
        offset += 0x800c0000; // to get to lit0_bm_mem_word3
        assert(index_tm_pre_lit_bm_mem_word < 256);
        offset += index_tm_pre_lit_bm_mem_word * 0x1; // tm_pre_lit_bm_mem_word[]
        break;
      case kLit1BmMemWord0:
        offset += 0x80100000; // to get to lit1_bm_mem_word0
        assert(index_tm_pre_lit_bm_mem_word < 256);
        offset += index_tm_pre_lit_bm_mem_word * 0x1; // tm_pre_lit_bm_mem_word[]
        break;
      case kLit1BmMemWord1:
        offset += 0x80140000; // to get to lit1_bm_mem_word1
        assert(index_tm_pre_lit_bm_mem_word < 256);
        offset += index_tm_pre_lit_bm_mem_word * 0x1; // tm_pre_lit_bm_mem_word[]
        break;
      case kLit1BmMemWord2:
        offset += 0x80180000; // to get to lit1_bm_mem_word2
        assert(index_tm_pre_lit_bm_mem_word < 256);
        offset += index_tm_pre_lit_bm_mem_word * 0x1; // tm_pre_lit_bm_mem_word[]
        break;
      case kLit1BmMemWord3:
        offset += 0x801c0000; // to get to lit1_bm_mem_word3
        assert(index_tm_pre_lit_bm_mem_word < 256);
        offset += index_tm_pre_lit_bm_mem_word * 0x1; // tm_pre_lit_bm_mem_word[]
        break;
      default:
        assert(0);
        break;
    }
    return offset;
  }

};








class TmPreLitBmMemWordArray : public model_core::RegisterBlockIndirect<RegisterArrayCallback> {
public:
  enum TmPreMemRspecEnum {
    kLit0BmMemWord0,
    kLit0BmMemWord1,
    kLit0BmMemWord2,
    kLit0BmMemWord3,
    kLit1BmMemWord0,
    kLit1BmMemWord1,
    kLit1BmMemWord2,
    kLit1BmMemWord3
  };
public:
  TmPreLitBmMemWordArray(
      int chipNumber, TmPreMemRspecEnum selector_tm_pre_mem_rspec, RegisterArrayCallback& write_callback = 0, RegisterArrayCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(selector_tm_pre_mem_rspec), 1 * ArraySize(selector_tm_pre_mem_rspec, -1), false, write_callback, read_callback, std::string("TmPreLitBmMemWordArray")+":"+boost::lexical_cast<std::string>(selector_tm_pre_mem_rspec)),
    array(RealArraySize(selector_tm_pre_mem_rspec, -1)),
    size0_(ArraySize(selector_tm_pre_mem_rspec, 0)),
    real_size0_(RealArraySize(selector_tm_pre_mem_rspec, 0))
    {
    }
public:




  uint8_t &mem_word(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].mem_word();
  }
  bool calculate_index(
      uint64_t* offset, uint32_t* a0
      ) const {
    int i = (*offset)/1;
    (*offset) -= (i*1);
    uint32_t t = i;
    *a0 = t;
    bool in_bounds = true;
    in_bounds &= CheckArrayBounds(*a0,real_size0_,size0_,*offset,0,true);
    return in_bounds;
  }


  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    if (read_callback_) read_callback_(a0);
    array[a0].read(offset,data0,data1,T);
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    array[a0].write(offset,data0,data1,T);
    if (write_callback_) write_callback_(a0);
    return true;
  }

  void reset(
      
      ) {
    for (uint32_t i=0;i<array.size();++i) {
      array[i].reset();
      int t = i;
      int a0 = t;
      if (write_callback_) write_callback_(a0);
    }
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    all_zeros=false;
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return "OUT_OF_BOUNDS";
    std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
    if (! r_s.empty()) {
      r += indent_string + std::string("TmPreLitBmMemWordArray") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
    }
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
    all_zeros=false;
    for (uint32_t a0=0;a0<real_size0_;++a0) {
      std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
      if (! r_s.empty()) {
        r += indent_string + std::string("TmPreLitBmMemWordArray") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
      }
    }
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  std::vector<TmPreLitBmMemWord> array;
  uint32_t size0_;
  uint32_t real_size0_;
private:
  static uint64_t StartOffset(
      TmPreMemRspecEnum selector_tm_pre_mem_rspec
      ) {
    uint64_t offset=0;
    offset += 0x6100000000; // to get to tm_pre
    switch (selector_tm_pre_mem_rspec) {
      case kLit0BmMemWord0:
        offset += 0x80000000; // to get to lit0_bm_mem_word0
        break;
      case kLit0BmMemWord1:
        offset += 0x80040000; // to get to lit0_bm_mem_word1
        break;
      case kLit0BmMemWord2:
        offset += 0x80080000; // to get to lit0_bm_mem_word2
        break;
      case kLit0BmMemWord3:
        offset += 0x800c0000; // to get to lit0_bm_mem_word3
        break;
      case kLit1BmMemWord0:
        offset += 0x80100000; // to get to lit1_bm_mem_word0
        break;
      case kLit1BmMemWord1:
        offset += 0x80140000; // to get to lit1_bm_mem_word1
        break;
      case kLit1BmMemWord2:
        offset += 0x80180000; // to get to lit1_bm_mem_word2
        break;
      case kLit1BmMemWord3:
        offset += 0x801c0000; // to get to lit1_bm_mem_word3
        break;
      default:
        assert(0);
        break;
    }
    return offset;
  }

  static int ArraySize(
      TmPreMemRspecEnum selector_tm_pre_mem_rspec, int dimension
      ) {
    switch (selector_tm_pre_mem_rspec) {
      case kLit0BmMemWord0:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kLit0BmMemWord1:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kLit0BmMemWord2:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kLit0BmMemWord3:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kLit1BmMemWord0:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kLit1BmMemWord1:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kLit1BmMemWord2:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kLit1BmMemWord3:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
  }

  static int RealArraySize(
      TmPreMemRspecEnum selector_tm_pre_mem_rspec, int dimension
      ) {
    switch (selector_tm_pre_mem_rspec) {
      case kLit0BmMemWord0:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kLit0BmMemWord1:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kLit0BmMemWord2:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kLit0BmMemWord3:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kLit1BmMemWord0:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kLit1BmMemWord1:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kLit1BmMemWord2:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kLit1BmMemWord3:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
  }

};








class TmPreLitBmMemWordArrayMutable : public model_core::RegisterBlockIndirect<RegisterArrayCallback> {
public:
  enum TmPreMemRspecEnum {
    kLit0BmMemWord0,
    kLit0BmMemWord1,
    kLit0BmMemWord2,
    kLit0BmMemWord3,
    kLit1BmMemWord0,
    kLit1BmMemWord1,
    kLit1BmMemWord2,
    kLit1BmMemWord3
  };
public:
  TmPreLitBmMemWordArrayMutable(
      int chipNumber, TmPreMemRspecEnum selector_tm_pre_mem_rspec, RegisterArrayCallback& write_callback = 0, RegisterArrayCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(selector_tm_pre_mem_rspec), 1 * ArraySize(selector_tm_pre_mem_rspec, -1), true, write_callback, read_callback, std::string("TmPreLitBmMemWordArrayMutable")+":"+boost::lexical_cast<std::string>(selector_tm_pre_mem_rspec)),
    array(RealArraySize(selector_tm_pre_mem_rspec, -1)),
    size0_(ArraySize(selector_tm_pre_mem_rspec, 0)),
    real_size0_(RealArraySize(selector_tm_pre_mem_rspec, 0))
    {
    }
public:




  uint8_t mem_word(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].mem_word();
  }
  void mem_word(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].mem_word(v);
  }
  bool calculate_index(
      uint64_t* offset, uint32_t* a0
      ) const {
    int i = (*offset)/1;
    (*offset) -= (i*1);
    uint32_t t = i;
    *a0 = t;
    bool in_bounds = true;
    in_bounds &= CheckArrayBounds(*a0,real_size0_,size0_,*offset,0,true);
    return in_bounds;
  }


  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    if (read_callback_) read_callback_(a0);
    array[a0].read(offset,data0,data1,T);
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    array[a0].write(offset,data0,data1,T);
    if (write_callback_) write_callback_(a0);
    return true;
  }

  void reset(
      
      ) {
    for (uint32_t i=0;i<array.size();++i) {
      array[i].reset();
      int t = i;
      int a0 = t;
      if (write_callback_) write_callback_(a0);
    }
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    all_zeros=false;
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return "OUT_OF_BOUNDS";
    std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
    if (! r_s.empty()) {
      r += indent_string + std::string("TmPreLitBmMemWordArrayMutable") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
    }
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
    all_zeros=false;
    for (uint32_t a0=0;a0<real_size0_;++a0) {
      std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
      if (! r_s.empty()) {
        r += indent_string + std::string("TmPreLitBmMemWordArrayMutable") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
      }
    }
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  std::vector<TmPreLitBmMemWordMutable> array;
  uint32_t size0_;
  uint32_t real_size0_;
private:
  static uint64_t StartOffset(
      TmPreMemRspecEnum selector_tm_pre_mem_rspec
      ) {
    uint64_t offset=0;
    offset += 0x6100000000; // to get to tm_pre
    switch (selector_tm_pre_mem_rspec) {
      case kLit0BmMemWord0:
        offset += 0x80000000; // to get to lit0_bm_mem_word0
        break;
      case kLit0BmMemWord1:
        offset += 0x80040000; // to get to lit0_bm_mem_word1
        break;
      case kLit0BmMemWord2:
        offset += 0x80080000; // to get to lit0_bm_mem_word2
        break;
      case kLit0BmMemWord3:
        offset += 0x800c0000; // to get to lit0_bm_mem_word3
        break;
      case kLit1BmMemWord0:
        offset += 0x80100000; // to get to lit1_bm_mem_word0
        break;
      case kLit1BmMemWord1:
        offset += 0x80140000; // to get to lit1_bm_mem_word1
        break;
      case kLit1BmMemWord2:
        offset += 0x80180000; // to get to lit1_bm_mem_word2
        break;
      case kLit1BmMemWord3:
        offset += 0x801c0000; // to get to lit1_bm_mem_word3
        break;
      default:
        assert(0);
        break;
    }
    return offset;
  }

  static int ArraySize(
      TmPreMemRspecEnum selector_tm_pre_mem_rspec, int dimension
      ) {
    switch (selector_tm_pre_mem_rspec) {
      case kLit0BmMemWord0:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kLit0BmMemWord1:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kLit0BmMemWord2:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kLit0BmMemWord3:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kLit1BmMemWord0:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kLit1BmMemWord1:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kLit1BmMemWord2:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kLit1BmMemWord3:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
  }

  static int RealArraySize(
      TmPreMemRspecEnum selector_tm_pre_mem_rspec, int dimension
      ) {
    switch (selector_tm_pre_mem_rspec) {
      case kLit0BmMemWord0:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kLit0BmMemWord1:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kLit0BmMemWord2:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kLit0BmMemWord3:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kLit1BmMemWord0:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kLit1BmMemWord1:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kLit1BmMemWord2:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kLit1BmMemWord3:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
  }

};








class TmPreFifoMemWord : public model_core::RegisterBlockIndirect<RegisterCallback> {
public:
  TmPreFifoMemWord(
      int chipNumber, int index_tm_pre_fifo_mem_word, RegisterCallback& write_callback = 0, RegisterCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(index_tm_pre_fifo_mem_word), 1, false, write_callback, read_callback, std::string("TmPreFifoMemWord")+":"+boost::lexical_cast<std::string>(index_tm_pre_fifo_mem_word))
    {
    }
  TmPreFifoMemWord(
      
  )
    : RegisterBlockIndirect(0, 0, 0, false, 0, 0, "TmPreFifoMemWord")
    {
    }
public:




  uint8_t &mem_word() { return mem_word_; }

  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    if (read_callback_) read_callback_();
    *data0 = (static_cast<uint64_t>(mem_word_) & 0x1);
    *data1 = 0;
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    mem_word_ = (data0 & 0x1);
    if (write_callback_) write_callback_();
    return true;
  }

  void reset(
      
      ) {
    mem_word_ = 0;
    if (write_callback_) write_callback_();
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    r += indent_string + std::string("TmPreFifoMemWord") + ":\n";
    r += indent_string + "  " + std::string("mem_word") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(mem_word_) ) + "\n";
    all_zeros &= (0 == mem_word_);
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
    r += indent_string + std::string("TmPreFifoMemWord") + ":\n";
    r += indent_string + "  " + std::string("mem_word") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(mem_word_) ) + "\n";
    all_zeros &= (0 == mem_word_);
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  uint8_t mem_word_;
private:
  static uint64_t StartOffset(
      int index_tm_pre_fifo_mem_word
      ) {
    uint64_t offset=0;
    offset += 0x6100000000; // to get to tm_pre
    offset += 0x40400000; // to get to fifo_mem_word
    assert(index_tm_pre_fifo_mem_word < 4);
    offset += index_tm_pre_fifo_mem_word * 0x1; // tm_pre_fifo_mem_word[]
    return offset;
  }

};








class TmPreFifoMemWordMutable : public model_core::RegisterBlockIndirect<RegisterCallback> {
public:
  TmPreFifoMemWordMutable(
      int chipNumber, int index_tm_pre_fifo_mem_word, RegisterCallback& write_callback = 0, RegisterCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(index_tm_pre_fifo_mem_word), 1, true, write_callback, read_callback, std::string("TmPreFifoMemWordMutable")+":"+boost::lexical_cast<std::string>(index_tm_pre_fifo_mem_word))
    {
    }
  TmPreFifoMemWordMutable(
      
  )
    : RegisterBlockIndirect(0, 0, 0, true, 0, 0, "TmPreFifoMemWordMutable")
    {
    }
public:




  uint8_t mem_word() { return mem_word_; }
  void mem_word(const uint8_t &v) { mem_word_=v; }

  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    if (read_callback_) read_callback_();
    *data0 = (static_cast<uint64_t>(mem_word_) & 0x1);
    *data1 = 0;
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    mem_word_ = (data0 & 0x1);
    if (write_callback_) write_callback_();
    return true;
  }

  void reset(
      
      ) {
    mem_word_ = 0;
    if (write_callback_) write_callback_();
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    r += indent_string + std::string("TmPreFifoMemWordMutable") + ":\n";
    r += indent_string + "  " + std::string("mem_word") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(mem_word_) ) + "\n";
    all_zeros &= (0 == mem_word_);
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
    r += indent_string + std::string("TmPreFifoMemWordMutable") + ":\n";
    r += indent_string + "  " + std::string("mem_word") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(mem_word_) ) + "\n";
    all_zeros &= (0 == mem_word_);
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  uint8_t mem_word_;
private:
  static uint64_t StartOffset(
      int index_tm_pre_fifo_mem_word
      ) {
    uint64_t offset=0;
    offset += 0x6100000000; // to get to tm_pre
    offset += 0x40400000; // to get to fifo_mem_word
    assert(index_tm_pre_fifo_mem_word < 4);
    offset += index_tm_pre_fifo_mem_word * 0x1; // tm_pre_fifo_mem_word[]
    return offset;
  }

};








class TmPreFifoMemWordArray : public model_core::RegisterBlockIndirect<RegisterArrayCallback> {
public:
  TmPreFifoMemWordArray(
      int chipNumber, RegisterArrayCallback& write_callback = 0, RegisterArrayCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(), 1 * ArraySize(-1), false, write_callback, read_callback, std::string("TmPreFifoMemWordArray")),
    array(RealArraySize(-1)),
    size0_(ArraySize(0)),
    real_size0_(RealArraySize(0))
    {
    }
public:




  uint8_t &mem_word(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].mem_word();
  }
  bool calculate_index(
      uint64_t* offset, uint32_t* a0
      ) const {
    int i = (*offset)/1;
    (*offset) -= (i*1);
    uint32_t t = i;
    *a0 = t;
    bool in_bounds = true;
    in_bounds &= CheckArrayBounds(*a0,real_size0_,size0_,*offset,0,true);
    return in_bounds;
  }


  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    if (read_callback_) read_callback_(a0);
    array[a0].read(offset,data0,data1,T);
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    array[a0].write(offset,data0,data1,T);
    if (write_callback_) write_callback_(a0);
    return true;
  }

  void reset(
      
      ) {
    for (uint32_t i=0;i<array.size();++i) {
      array[i].reset();
      int t = i;
      int a0 = t;
      if (write_callback_) write_callback_(a0);
    }
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    all_zeros=false;
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return "OUT_OF_BOUNDS";
    std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
    if (! r_s.empty()) {
      r += indent_string + std::string("TmPreFifoMemWordArray") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
    }
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
    all_zeros=false;
    for (uint32_t a0=0;a0<real_size0_;++a0) {
      std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
      if (! r_s.empty()) {
        r += indent_string + std::string("TmPreFifoMemWordArray") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
      }
    }
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  std::vector<TmPreFifoMemWord> array;
  uint32_t size0_;
  uint32_t real_size0_;
private:
  static uint64_t StartOffset(
      
      ) {
    uint64_t offset=0;
    offset += 0x6100000000; // to get to tm_pre
    offset += 0x40400000; // to get to fifo_mem_word
    return offset;
  }

  static int ArraySize(
      int dimension
      ) {
    switch (dimension) {
      case -1:
        return 4;
        break;
      case 0:
        return 4;
        break;
      default:
        assert(0);
        break;
    }
  }

  static int RealArraySize(
      int dimension
      ) {
    switch (dimension) {
      case -1:
        return 4;
        break;
      case 0:
        return 4;
        break;
      default:
        assert(0);
        break;
    }
  }

};








class TmPreFifoMemWordArrayMutable : public model_core::RegisterBlockIndirect<RegisterArrayCallback> {
public:
  TmPreFifoMemWordArrayMutable(
      int chipNumber, RegisterArrayCallback& write_callback = 0, RegisterArrayCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(), 1 * ArraySize(-1), true, write_callback, read_callback, std::string("TmPreFifoMemWordArrayMutable")),
    array(RealArraySize(-1)),
    size0_(ArraySize(0)),
    real_size0_(RealArraySize(0))
    {
    }
public:




  uint8_t mem_word(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].mem_word();
  }
  void mem_word(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].mem_word(v);
  }
  bool calculate_index(
      uint64_t* offset, uint32_t* a0
      ) const {
    int i = (*offset)/1;
    (*offset) -= (i*1);
    uint32_t t = i;
    *a0 = t;
    bool in_bounds = true;
    in_bounds &= CheckArrayBounds(*a0,real_size0_,size0_,*offset,0,true);
    return in_bounds;
  }


  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    if (read_callback_) read_callback_(a0);
    array[a0].read(offset,data0,data1,T);
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    array[a0].write(offset,data0,data1,T);
    if (write_callback_) write_callback_(a0);
    return true;
  }

  void reset(
      
      ) {
    for (uint32_t i=0;i<array.size();++i) {
      array[i].reset();
      int t = i;
      int a0 = t;
      if (write_callback_) write_callback_(a0);
    }
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    all_zeros=false;
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return "OUT_OF_BOUNDS";
    std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
    if (! r_s.empty()) {
      r += indent_string + std::string("TmPreFifoMemWordArrayMutable") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
    }
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
    all_zeros=false;
    for (uint32_t a0=0;a0<real_size0_;++a0) {
      std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
      if (! r_s.empty()) {
        r += indent_string + std::string("TmPreFifoMemWordArrayMutable") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
      }
    }
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  std::vector<TmPreFifoMemWordMutable> array;
  uint32_t size0_;
  uint32_t real_size0_;
private:
  static uint64_t StartOffset(
      
      ) {
    uint64_t offset=0;
    offset += 0x6100000000; // to get to tm_pre
    offset += 0x40400000; // to get to fifo_mem_word
    return offset;
  }

  static int ArraySize(
      int dimension
      ) {
    switch (dimension) {
      case -1:
        return 4;
        break;
      case 0:
        return 4;
        break;
      default:
        assert(0);
        break;
    }
  }

  static int RealArraySize(
      int dimension
      ) {
    switch (dimension) {
      case -1:
        return 4;
        break;
      case 0:
        return 4;
        break;
      default:
        assert(0);
        break;
    }
  }

};








class TmPreLitNpMemWord : public model_core::RegisterBlockIndirect<RegisterCallback> {
public:
  enum TmPreMemRspecEnum {
    kLit0NpMemWord,
    kLit1NpMemWord
  };
public:
  TmPreLitNpMemWord(
      int chipNumber, TmPreMemRspecEnum selector_tm_pre_mem_rspec, int index_tm_pre_lit_np_mem_word, RegisterCallback& write_callback = 0, RegisterCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(selector_tm_pre_mem_rspec, index_tm_pre_lit_np_mem_word), 1, false, write_callback, read_callback, std::string("TmPreLitNpMemWord")+":"+boost::lexical_cast<std::string>(selector_tm_pre_mem_rspec) + "," + boost::lexical_cast<std::string>(index_tm_pre_lit_np_mem_word))
    {
    }
  TmPreLitNpMemWord(
      
  )
    : RegisterBlockIndirect(0, 0, 0, false, 0, 0, "TmPreLitNpMemWord")
    {
    }
public:




  uint8_t &mem_word() { return mem_word_; }

  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    if (read_callback_) read_callback_();
    *data0 = (static_cast<uint64_t>(mem_word_) & 0x1);
    *data1 = 0;
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    mem_word_ = (data0 & 0x1);
    if (write_callback_) write_callback_();
    return true;
  }

  void reset(
      
      ) {
    mem_word_ = 0;
    if (write_callback_) write_callback_();
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    r += indent_string + std::string("TmPreLitNpMemWord") + ":\n";
    r += indent_string + "  " + std::string("mem_word") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(mem_word_) ) + "\n";
    all_zeros &= (0 == mem_word_);
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
    r += indent_string + std::string("TmPreLitNpMemWord") + ":\n";
    r += indent_string + "  " + std::string("mem_word") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(mem_word_) ) + "\n";
    all_zeros &= (0 == mem_word_);
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  uint8_t mem_word_;
private:
  static uint64_t StartOffset(
      TmPreMemRspecEnum selector_tm_pre_mem_rspec, int index_tm_pre_lit_np_mem_word
      ) {
    uint64_t offset=0;
    offset += 0x6100000000; // to get to tm_pre
    switch (selector_tm_pre_mem_rspec) {
      case kLit0NpMemWord:
        offset += 0x40200000; // to get to lit0_np_mem_word
        assert(index_tm_pre_lit_np_mem_word < 256);
        offset += index_tm_pre_lit_np_mem_word * 0x1; // tm_pre_lit_np_mem_word[]
        break;
      case kLit1NpMemWord:
        offset += 0x40300000; // to get to lit1_np_mem_word
        assert(index_tm_pre_lit_np_mem_word < 256);
        offset += index_tm_pre_lit_np_mem_word * 0x1; // tm_pre_lit_np_mem_word[]
        break;
      default:
        assert(0);
        break;
    }
    return offset;
  }

};








class TmPreLitNpMemWordMutable : public model_core::RegisterBlockIndirect<RegisterCallback> {
public:
  enum TmPreMemRspecEnum {
    kLit0NpMemWord,
    kLit1NpMemWord
  };
public:
  TmPreLitNpMemWordMutable(
      int chipNumber, TmPreMemRspecEnum selector_tm_pre_mem_rspec, int index_tm_pre_lit_np_mem_word, RegisterCallback& write_callback = 0, RegisterCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(selector_tm_pre_mem_rspec, index_tm_pre_lit_np_mem_word), 1, true, write_callback, read_callback, std::string("TmPreLitNpMemWordMutable")+":"+boost::lexical_cast<std::string>(selector_tm_pre_mem_rspec) + "," + boost::lexical_cast<std::string>(index_tm_pre_lit_np_mem_word))
    {
    }
  TmPreLitNpMemWordMutable(
      
  )
    : RegisterBlockIndirect(0, 0, 0, true, 0, 0, "TmPreLitNpMemWordMutable")
    {
    }
public:




  uint8_t mem_word() { return mem_word_; }
  void mem_word(const uint8_t &v) { mem_word_=v; }

  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    if (read_callback_) read_callback_();
    *data0 = (static_cast<uint64_t>(mem_word_) & 0x1);
    *data1 = 0;
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    mem_word_ = (data0 & 0x1);
    if (write_callback_) write_callback_();
    return true;
  }

  void reset(
      
      ) {
    mem_word_ = 0;
    if (write_callback_) write_callback_();
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    r += indent_string + std::string("TmPreLitNpMemWordMutable") + ":\n";
    r += indent_string + "  " + std::string("mem_word") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(mem_word_) ) + "\n";
    all_zeros &= (0 == mem_word_);
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
    r += indent_string + std::string("TmPreLitNpMemWordMutable") + ":\n";
    r += indent_string + "  " + std::string("mem_word") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(mem_word_) ) + "\n";
    all_zeros &= (0 == mem_word_);
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  uint8_t mem_word_;
private:
  static uint64_t StartOffset(
      TmPreMemRspecEnum selector_tm_pre_mem_rspec, int index_tm_pre_lit_np_mem_word
      ) {
    uint64_t offset=0;
    offset += 0x6100000000; // to get to tm_pre
    switch (selector_tm_pre_mem_rspec) {
      case kLit0NpMemWord:
        offset += 0x40200000; // to get to lit0_np_mem_word
        assert(index_tm_pre_lit_np_mem_word < 256);
        offset += index_tm_pre_lit_np_mem_word * 0x1; // tm_pre_lit_np_mem_word[]
        break;
      case kLit1NpMemWord:
        offset += 0x40300000; // to get to lit1_np_mem_word
        assert(index_tm_pre_lit_np_mem_word < 256);
        offset += index_tm_pre_lit_np_mem_word * 0x1; // tm_pre_lit_np_mem_word[]
        break;
      default:
        assert(0);
        break;
    }
    return offset;
  }

};








class TmPreLitNpMemWordArray : public model_core::RegisterBlockIndirect<RegisterArrayCallback> {
public:
  enum TmPreMemRspecEnum {
    kLit0NpMemWord,
    kLit1NpMemWord
  };
public:
  TmPreLitNpMemWordArray(
      int chipNumber, TmPreMemRspecEnum selector_tm_pre_mem_rspec, RegisterArrayCallback& write_callback = 0, RegisterArrayCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(selector_tm_pre_mem_rspec), 1 * ArraySize(selector_tm_pre_mem_rspec, -1), false, write_callback, read_callback, std::string("TmPreLitNpMemWordArray")+":"+boost::lexical_cast<std::string>(selector_tm_pre_mem_rspec)),
    array(RealArraySize(selector_tm_pre_mem_rspec, -1)),
    size0_(ArraySize(selector_tm_pre_mem_rspec, 0)),
    real_size0_(RealArraySize(selector_tm_pre_mem_rspec, 0))
    {
    }
public:




  uint8_t &mem_word(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].mem_word();
  }
  bool calculate_index(
      uint64_t* offset, uint32_t* a0
      ) const {
    int i = (*offset)/1;
    (*offset) -= (i*1);
    uint32_t t = i;
    *a0 = t;
    bool in_bounds = true;
    in_bounds &= CheckArrayBounds(*a0,real_size0_,size0_,*offset,0,true);
    return in_bounds;
  }


  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    if (read_callback_) read_callback_(a0);
    array[a0].read(offset,data0,data1,T);
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    array[a0].write(offset,data0,data1,T);
    if (write_callback_) write_callback_(a0);
    return true;
  }

  void reset(
      
      ) {
    for (uint32_t i=0;i<array.size();++i) {
      array[i].reset();
      int t = i;
      int a0 = t;
      if (write_callback_) write_callback_(a0);
    }
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    all_zeros=false;
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return "OUT_OF_BOUNDS";
    std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
    if (! r_s.empty()) {
      r += indent_string + std::string("TmPreLitNpMemWordArray") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
    }
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
    all_zeros=false;
    for (uint32_t a0=0;a0<real_size0_;++a0) {
      std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
      if (! r_s.empty()) {
        r += indent_string + std::string("TmPreLitNpMemWordArray") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
      }
    }
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  std::vector<TmPreLitNpMemWord> array;
  uint32_t size0_;
  uint32_t real_size0_;
private:
  static uint64_t StartOffset(
      TmPreMemRspecEnum selector_tm_pre_mem_rspec
      ) {
    uint64_t offset=0;
    offset += 0x6100000000; // to get to tm_pre
    switch (selector_tm_pre_mem_rspec) {
      case kLit0NpMemWord:
        offset += 0x40200000; // to get to lit0_np_mem_word
        break;
      case kLit1NpMemWord:
        offset += 0x40300000; // to get to lit1_np_mem_word
        break;
      default:
        assert(0);
        break;
    }
    return offset;
  }

  static int ArraySize(
      TmPreMemRspecEnum selector_tm_pre_mem_rspec, int dimension
      ) {
    switch (selector_tm_pre_mem_rspec) {
      case kLit0NpMemWord:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kLit1NpMemWord:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
  }

  static int RealArraySize(
      TmPreMemRspecEnum selector_tm_pre_mem_rspec, int dimension
      ) {
    switch (selector_tm_pre_mem_rspec) {
      case kLit0NpMemWord:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kLit1NpMemWord:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
  }

};








class TmPreLitNpMemWordArrayMutable : public model_core::RegisterBlockIndirect<RegisterArrayCallback> {
public:
  enum TmPreMemRspecEnum {
    kLit0NpMemWord,
    kLit1NpMemWord
  };
public:
  TmPreLitNpMemWordArrayMutable(
      int chipNumber, TmPreMemRspecEnum selector_tm_pre_mem_rspec, RegisterArrayCallback& write_callback = 0, RegisterArrayCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(selector_tm_pre_mem_rspec), 1 * ArraySize(selector_tm_pre_mem_rspec, -1), true, write_callback, read_callback, std::string("TmPreLitNpMemWordArrayMutable")+":"+boost::lexical_cast<std::string>(selector_tm_pre_mem_rspec)),
    array(RealArraySize(selector_tm_pre_mem_rspec, -1)),
    size0_(ArraySize(selector_tm_pre_mem_rspec, 0)),
    real_size0_(RealArraySize(selector_tm_pre_mem_rspec, 0))
    {
    }
public:




  uint8_t mem_word(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].mem_word();
  }
  void mem_word(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].mem_word(v);
  }
  bool calculate_index(
      uint64_t* offset, uint32_t* a0
      ) const {
    int i = (*offset)/1;
    (*offset) -= (i*1);
    uint32_t t = i;
    *a0 = t;
    bool in_bounds = true;
    in_bounds &= CheckArrayBounds(*a0,real_size0_,size0_,*offset,0,true);
    return in_bounds;
  }


  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    if (read_callback_) read_callback_(a0);
    array[a0].read(offset,data0,data1,T);
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    array[a0].write(offset,data0,data1,T);
    if (write_callback_) write_callback_(a0);
    return true;
  }

  void reset(
      
      ) {
    for (uint32_t i=0;i<array.size();++i) {
      array[i].reset();
      int t = i;
      int a0 = t;
      if (write_callback_) write_callback_(a0);
    }
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    all_zeros=false;
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return "OUT_OF_BOUNDS";
    std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
    if (! r_s.empty()) {
      r += indent_string + std::string("TmPreLitNpMemWordArrayMutable") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
    }
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
    all_zeros=false;
    for (uint32_t a0=0;a0<real_size0_;++a0) {
      std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
      if (! r_s.empty()) {
        r += indent_string + std::string("TmPreLitNpMemWordArrayMutable") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
      }
    }
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  std::vector<TmPreLitNpMemWordMutable> array;
  uint32_t size0_;
  uint32_t real_size0_;
private:
  static uint64_t StartOffset(
      TmPreMemRspecEnum selector_tm_pre_mem_rspec
      ) {
    uint64_t offset=0;
    offset += 0x6100000000; // to get to tm_pre
    switch (selector_tm_pre_mem_rspec) {
      case kLit0NpMemWord:
        offset += 0x40200000; // to get to lit0_np_mem_word
        break;
      case kLit1NpMemWord:
        offset += 0x40300000; // to get to lit1_np_mem_word
        break;
      default:
        assert(0);
        break;
    }
    return offset;
  }

  static int ArraySize(
      TmPreMemRspecEnum selector_tm_pre_mem_rspec, int dimension
      ) {
    switch (selector_tm_pre_mem_rspec) {
      case kLit0NpMemWord:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kLit1NpMemWord:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
  }

  static int RealArraySize(
      TmPreMemRspecEnum selector_tm_pre_mem_rspec, int dimension
      ) {
    switch (selector_tm_pre_mem_rspec) {
      case kLit0NpMemWord:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kLit1NpMemWord:
        switch (dimension) {
          case -1:
            return 256;
            break;
          case 0:
            return 256;
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
  }

};








class TmPrePbtMemWord : public model_core::RegisterBlockIndirect<RegisterCallback> {
public:
  enum TmPreMemRspecEnum {
    kPbt0MemWord,
    kPbt1MemWord
  };
public:
  TmPrePbtMemWord(
      int chipNumber, TmPreMemRspecEnum selector_tm_pre_mem_rspec, int index_tm_pre_pbt_mem_word, RegisterCallback& write_callback = 0, RegisterCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(selector_tm_pre_mem_rspec, index_tm_pre_pbt_mem_word), 1, false, write_callback, read_callback, std::string("TmPrePbtMemWord")+":"+boost::lexical_cast<std::string>(selector_tm_pre_mem_rspec) + "," + boost::lexical_cast<std::string>(index_tm_pre_pbt_mem_word))
    {
    }
  TmPrePbtMemWord(
      
  )
    : RegisterBlockIndirect(0, 0, 0, false, 0, 0, "TmPrePbtMemWord")
    {
    }
public:




  uint8_t &mem_word() { return mem_word_; }

  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    if (read_callback_) read_callback_();
    *data0 = (static_cast<uint64_t>(mem_word_) & 0x1);
    *data1 = 0;
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    mem_word_ = (data0 & 0x1);
    if (write_callback_) write_callback_();
    return true;
  }

  void reset(
      
      ) {
    mem_word_ = 0;
    if (write_callback_) write_callback_();
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    r += indent_string + std::string("TmPrePbtMemWord") + ":\n";
    r += indent_string + "  " + std::string("mem_word") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(mem_word_) ) + "\n";
    all_zeros &= (0 == mem_word_);
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
    r += indent_string + std::string("TmPrePbtMemWord") + ":\n";
    r += indent_string + "  " + std::string("mem_word") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(mem_word_) ) + "\n";
    all_zeros &= (0 == mem_word_);
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  uint8_t mem_word_;
private:
  static uint64_t StartOffset(
      TmPreMemRspecEnum selector_tm_pre_mem_rspec, int index_tm_pre_pbt_mem_word
      ) {
    uint64_t offset=0;
    offset += 0x6100000000; // to get to tm_pre
    switch (selector_tm_pre_mem_rspec) {
      case kPbt0MemWord:
        offset += 0x40000000; // to get to pbt0_mem_word
        assert(index_tm_pre_pbt_mem_word < 288);
        offset += index_tm_pre_pbt_mem_word * 0x1; // tm_pre_pbt_mem_word[]
        break;
      case kPbt1MemWord:
        offset += 0x40100000; // to get to pbt1_mem_word
        assert(index_tm_pre_pbt_mem_word < 288);
        offset += index_tm_pre_pbt_mem_word * 0x1; // tm_pre_pbt_mem_word[]
        break;
      default:
        assert(0);
        break;
    }
    return offset;
  }

};








class TmPrePbtMemWordMutable : public model_core::RegisterBlockIndirect<RegisterCallback> {
public:
  enum TmPreMemRspecEnum {
    kPbt0MemWord,
    kPbt1MemWord
  };
public:
  TmPrePbtMemWordMutable(
      int chipNumber, TmPreMemRspecEnum selector_tm_pre_mem_rspec, int index_tm_pre_pbt_mem_word, RegisterCallback& write_callback = 0, RegisterCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(selector_tm_pre_mem_rspec, index_tm_pre_pbt_mem_word), 1, true, write_callback, read_callback, std::string("TmPrePbtMemWordMutable")+":"+boost::lexical_cast<std::string>(selector_tm_pre_mem_rspec) + "," + boost::lexical_cast<std::string>(index_tm_pre_pbt_mem_word))
    {
    }
  TmPrePbtMemWordMutable(
      
  )
    : RegisterBlockIndirect(0, 0, 0, true, 0, 0, "TmPrePbtMemWordMutable")
    {
    }
public:




  uint8_t mem_word() { return mem_word_; }
  void mem_word(const uint8_t &v) { mem_word_=v; }

  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    if (read_callback_) read_callback_();
    *data0 = (static_cast<uint64_t>(mem_word_) & 0x1);
    *data1 = 0;
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    mem_word_ = (data0 & 0x1);
    if (write_callback_) write_callback_();
    return true;
  }

  void reset(
      
      ) {
    mem_word_ = 0;
    if (write_callback_) write_callback_();
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    r += indent_string + std::string("TmPrePbtMemWordMutable") + ":\n";
    r += indent_string + "  " + std::string("mem_word") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(mem_word_) ) + "\n";
    all_zeros &= (0 == mem_word_);
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
    r += indent_string + std::string("TmPrePbtMemWordMutable") + ":\n";
    r += indent_string + "  " + std::string("mem_word") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(mem_word_) ) + "\n";
    all_zeros &= (0 == mem_word_);
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  uint8_t mem_word_;
private:
  static uint64_t StartOffset(
      TmPreMemRspecEnum selector_tm_pre_mem_rspec, int index_tm_pre_pbt_mem_word
      ) {
    uint64_t offset=0;
    offset += 0x6100000000; // to get to tm_pre
    switch (selector_tm_pre_mem_rspec) {
      case kPbt0MemWord:
        offset += 0x40000000; // to get to pbt0_mem_word
        assert(index_tm_pre_pbt_mem_word < 288);
        offset += index_tm_pre_pbt_mem_word * 0x1; // tm_pre_pbt_mem_word[]
        break;
      case kPbt1MemWord:
        offset += 0x40100000; // to get to pbt1_mem_word
        assert(index_tm_pre_pbt_mem_word < 288);
        offset += index_tm_pre_pbt_mem_word * 0x1; // tm_pre_pbt_mem_word[]
        break;
      default:
        assert(0);
        break;
    }
    return offset;
  }

};








class TmPrePbtMemWordArray : public model_core::RegisterBlockIndirect<RegisterArrayCallback> {
public:
  enum TmPreMemRspecEnum {
    kPbt0MemWord,
    kPbt1MemWord
  };
public:
  TmPrePbtMemWordArray(
      int chipNumber, TmPreMemRspecEnum selector_tm_pre_mem_rspec, RegisterArrayCallback& write_callback = 0, RegisterArrayCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(selector_tm_pre_mem_rspec), 1 * ArraySize(selector_tm_pre_mem_rspec, -1), false, write_callback, read_callback, std::string("TmPrePbtMemWordArray")+":"+boost::lexical_cast<std::string>(selector_tm_pre_mem_rspec)),
    array(RealArraySize(selector_tm_pre_mem_rspec, -1)),
    size0_(ArraySize(selector_tm_pre_mem_rspec, 0)),
    real_size0_(RealArraySize(selector_tm_pre_mem_rspec, 0))
    {
    }
public:




  uint8_t &mem_word(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].mem_word();
  }
  bool calculate_index(
      uint64_t* offset, uint32_t* a0
      ) const {
    int i = (*offset)/1;
    (*offset) -= (i*1);
    uint32_t t = i;
    *a0 = t;
    bool in_bounds = true;
    in_bounds &= CheckArrayBounds(*a0,real_size0_,size0_,*offset,0,true);
    return in_bounds;
  }


  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    if (read_callback_) read_callback_(a0);
    array[a0].read(offset,data0,data1,T);
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    array[a0].write(offset,data0,data1,T);
    if (write_callback_) write_callback_(a0);
    return true;
  }

  void reset(
      
      ) {
    for (uint32_t i=0;i<array.size();++i) {
      array[i].reset();
      int t = i;
      int a0 = t;
      if (write_callback_) write_callback_(a0);
    }
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    all_zeros=false;
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return "OUT_OF_BOUNDS";
    std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
    if (! r_s.empty()) {
      r += indent_string + std::string("TmPrePbtMemWordArray") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
    }
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
    all_zeros=false;
    for (uint32_t a0=0;a0<real_size0_;++a0) {
      std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
      if (! r_s.empty()) {
        r += indent_string + std::string("TmPrePbtMemWordArray") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
      }
    }
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  std::vector<TmPrePbtMemWord> array;
  uint32_t size0_;
  uint32_t real_size0_;
private:
  static uint64_t StartOffset(
      TmPreMemRspecEnum selector_tm_pre_mem_rspec
      ) {
    uint64_t offset=0;
    offset += 0x6100000000; // to get to tm_pre
    switch (selector_tm_pre_mem_rspec) {
      case kPbt0MemWord:
        offset += 0x40000000; // to get to pbt0_mem_word
        break;
      case kPbt1MemWord:
        offset += 0x40100000; // to get to pbt1_mem_word
        break;
      default:
        assert(0);
        break;
    }
    return offset;
  }

  static int ArraySize(
      TmPreMemRspecEnum selector_tm_pre_mem_rspec, int dimension
      ) {
    switch (selector_tm_pre_mem_rspec) {
      case kPbt0MemWord:
        switch (dimension) {
          case -1:
            return 288;
            break;
          case 0:
            return 288;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kPbt1MemWord:
        switch (dimension) {
          case -1:
            return 288;
            break;
          case 0:
            return 288;
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
  }

  static int RealArraySize(
      TmPreMemRspecEnum selector_tm_pre_mem_rspec, int dimension
      ) {
    switch (selector_tm_pre_mem_rspec) {
      case kPbt0MemWord:
        switch (dimension) {
          case -1:
            return 288;
            break;
          case 0:
            return 288;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kPbt1MemWord:
        switch (dimension) {
          case -1:
            return 288;
            break;
          case 0:
            return 288;
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
  }

};








class TmPrePbtMemWordArrayMutable : public model_core::RegisterBlockIndirect<RegisterArrayCallback> {
public:
  enum TmPreMemRspecEnum {
    kPbt0MemWord,
    kPbt1MemWord
  };
public:
  TmPrePbtMemWordArrayMutable(
      int chipNumber, TmPreMemRspecEnum selector_tm_pre_mem_rspec, RegisterArrayCallback& write_callback = 0, RegisterArrayCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(selector_tm_pre_mem_rspec), 1 * ArraySize(selector_tm_pre_mem_rspec, -1), true, write_callback, read_callback, std::string("TmPrePbtMemWordArrayMutable")+":"+boost::lexical_cast<std::string>(selector_tm_pre_mem_rspec)),
    array(RealArraySize(selector_tm_pre_mem_rspec, -1)),
    size0_(ArraySize(selector_tm_pre_mem_rspec, 0)),
    real_size0_(RealArraySize(selector_tm_pre_mem_rspec, 0))
    {
    }
public:




  uint8_t mem_word(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].mem_word();
  }
  void mem_word(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].mem_word(v);
  }
  bool calculate_index(
      uint64_t* offset, uint32_t* a0
      ) const {
    int i = (*offset)/1;
    (*offset) -= (i*1);
    uint32_t t = i;
    *a0 = t;
    bool in_bounds = true;
    in_bounds &= CheckArrayBounds(*a0,real_size0_,size0_,*offset,0,true);
    return in_bounds;
  }


  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    if (read_callback_) read_callback_(a0);
    array[a0].read(offset,data0,data1,T);
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    array[a0].write(offset,data0,data1,T);
    if (write_callback_) write_callback_(a0);
    return true;
  }

  void reset(
      
      ) {
    for (uint32_t i=0;i<array.size();++i) {
      array[i].reset();
      int t = i;
      int a0 = t;
      if (write_callback_) write_callback_(a0);
    }
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    all_zeros=false;
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return "OUT_OF_BOUNDS";
    std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
    if (! r_s.empty()) {
      r += indent_string + std::string("TmPrePbtMemWordArrayMutable") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
    }
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
    all_zeros=false;
    for (uint32_t a0=0;a0<real_size0_;++a0) {
      std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
      if (! r_s.empty()) {
        r += indent_string + std::string("TmPrePbtMemWordArrayMutable") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
      }
    }
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  std::vector<TmPrePbtMemWordMutable> array;
  uint32_t size0_;
  uint32_t real_size0_;
private:
  static uint64_t StartOffset(
      TmPreMemRspecEnum selector_tm_pre_mem_rspec
      ) {
    uint64_t offset=0;
    offset += 0x6100000000; // to get to tm_pre
    switch (selector_tm_pre_mem_rspec) {
      case kPbt0MemWord:
        offset += 0x40000000; // to get to pbt0_mem_word
        break;
      case kPbt1MemWord:
        offset += 0x40100000; // to get to pbt1_mem_word
        break;
      default:
        assert(0);
        break;
    }
    return offset;
  }

  static int ArraySize(
      TmPreMemRspecEnum selector_tm_pre_mem_rspec, int dimension
      ) {
    switch (selector_tm_pre_mem_rspec) {
      case kPbt0MemWord:
        switch (dimension) {
          case -1:
            return 288;
            break;
          case 0:
            return 288;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kPbt1MemWord:
        switch (dimension) {
          case -1:
            return 288;
            break;
          case 0:
            return 288;
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
  }

  static int RealArraySize(
      TmPreMemRspecEnum selector_tm_pre_mem_rspec, int dimension
      ) {
    switch (selector_tm_pre_mem_rspec) {
      case kPbt0MemWord:
        switch (dimension) {
          case -1:
            return 288;
            break;
          case 0:
            return 288;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kPbt1MemWord:
        switch (dimension) {
          case -1:
            return 288;
            break;
          case 0:
            return 288;
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
  }

};








class TmPreMitMemWord : public model_core::RegisterBlockIndirect<RegisterCallback> {
public:
  enum TmPreMemRspecEnum {
    kMit0MemWord,
    kMit1MemWord,
    kMit2MemWord,
    kMit3MemWord
  };
public:
  TmPreMitMemWord(
      int chipNumber, TmPreMemRspecEnum selector_tm_pre_mem_rspec, int index_tm_pre_mit_mem_word, RegisterCallback& write_callback = 0, RegisterCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(selector_tm_pre_mem_rspec, index_tm_pre_mit_mem_word), 1, false, write_callback, read_callback, std::string("TmPreMitMemWord")+":"+boost::lexical_cast<std::string>(selector_tm_pre_mem_rspec) + "," + boost::lexical_cast<std::string>(index_tm_pre_mit_mem_word))
    {
    }
  TmPreMitMemWord(
      
  )
    : RegisterBlockIndirect(0, 0, 0, false, 0, 0, "TmPreMitMemWord")
    {
    }
public:




  uint8_t &mem_word() { return mem_word_; }

  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    if (read_callback_) read_callback_();
    *data0 = (static_cast<uint64_t>(mem_word_) & 0x1);
    *data1 = 0;
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    mem_word_ = (data0 & 0x1);
    if (write_callback_) write_callback_();
    return true;
  }

  void reset(
      
      ) {
    mem_word_ = 0;
    if (write_callback_) write_callback_();
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    r += indent_string + std::string("TmPreMitMemWord") + ":\n";
    r += indent_string + "  " + std::string("mem_word") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(mem_word_) ) + "\n";
    all_zeros &= (0 == mem_word_);
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
    r += indent_string + std::string("TmPreMitMemWord") + ":\n";
    r += indent_string + "  " + std::string("mem_word") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(mem_word_) ) + "\n";
    all_zeros &= (0 == mem_word_);
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  uint8_t mem_word_;
private:
  static uint64_t StartOffset(
      TmPreMemRspecEnum selector_tm_pre_mem_rspec, int index_tm_pre_mit_mem_word
      ) {
    uint64_t offset=0;
    offset += 0x6100000000; // to get to tm_pre
    switch (selector_tm_pre_mem_rspec) {
      case kMit0MemWord:
        offset += 0x100000; // to get to mit0_mem_word
        assert(index_tm_pre_mit_mem_word < 16384);
        offset += index_tm_pre_mit_mem_word * 0x1; // tm_pre_mit_mem_word[]
        break;
      case kMit1MemWord:
        offset += 0x200000; // to get to mit1_mem_word
        assert(index_tm_pre_mit_mem_word < 16384);
        offset += index_tm_pre_mit_mem_word * 0x1; // tm_pre_mit_mem_word[]
        break;
      case kMit2MemWord:
        offset += 0x300000; // to get to mit2_mem_word
        assert(index_tm_pre_mit_mem_word < 16384);
        offset += index_tm_pre_mit_mem_word * 0x1; // tm_pre_mit_mem_word[]
        break;
      case kMit3MemWord:
        offset += 0x400000; // to get to mit3_mem_word
        assert(index_tm_pre_mit_mem_word < 16384);
        offset += index_tm_pre_mit_mem_word * 0x1; // tm_pre_mit_mem_word[]
        break;
      default:
        assert(0);
        break;
    }
    return offset;
  }

};








class TmPreMitMemWordMutable : public model_core::RegisterBlockIndirect<RegisterCallback> {
public:
  enum TmPreMemRspecEnum {
    kMit0MemWord,
    kMit1MemWord,
    kMit2MemWord,
    kMit3MemWord
  };
public:
  TmPreMitMemWordMutable(
      int chipNumber, TmPreMemRspecEnum selector_tm_pre_mem_rspec, int index_tm_pre_mit_mem_word, RegisterCallback& write_callback = 0, RegisterCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(selector_tm_pre_mem_rspec, index_tm_pre_mit_mem_word), 1, true, write_callback, read_callback, std::string("TmPreMitMemWordMutable")+":"+boost::lexical_cast<std::string>(selector_tm_pre_mem_rspec) + "," + boost::lexical_cast<std::string>(index_tm_pre_mit_mem_word))
    {
    }
  TmPreMitMemWordMutable(
      
  )
    : RegisterBlockIndirect(0, 0, 0, true, 0, 0, "TmPreMitMemWordMutable")
    {
    }
public:




  uint8_t mem_word() { return mem_word_; }
  void mem_word(const uint8_t &v) { mem_word_=v; }

  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    if (read_callback_) read_callback_();
    *data0 = (static_cast<uint64_t>(mem_word_) & 0x1);
    *data1 = 0;
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    mem_word_ = (data0 & 0x1);
    if (write_callback_) write_callback_();
    return true;
  }

  void reset(
      
      ) {
    mem_word_ = 0;
    if (write_callback_) write_callback_();
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    r += indent_string + std::string("TmPreMitMemWordMutable") + ":\n";
    r += indent_string + "  " + std::string("mem_word") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(mem_word_) ) + "\n";
    all_zeros &= (0 == mem_word_);
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
    r += indent_string + std::string("TmPreMitMemWordMutable") + ":\n";
    r += indent_string + "  " + std::string("mem_word") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(mem_word_) ) + "\n";
    all_zeros &= (0 == mem_word_);
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  uint8_t mem_word_;
private:
  static uint64_t StartOffset(
      TmPreMemRspecEnum selector_tm_pre_mem_rspec, int index_tm_pre_mit_mem_word
      ) {
    uint64_t offset=0;
    offset += 0x6100000000; // to get to tm_pre
    switch (selector_tm_pre_mem_rspec) {
      case kMit0MemWord:
        offset += 0x100000; // to get to mit0_mem_word
        assert(index_tm_pre_mit_mem_word < 16384);
        offset += index_tm_pre_mit_mem_word * 0x1; // tm_pre_mit_mem_word[]
        break;
      case kMit1MemWord:
        offset += 0x200000; // to get to mit1_mem_word
        assert(index_tm_pre_mit_mem_word < 16384);
        offset += index_tm_pre_mit_mem_word * 0x1; // tm_pre_mit_mem_word[]
        break;
      case kMit2MemWord:
        offset += 0x300000; // to get to mit2_mem_word
        assert(index_tm_pre_mit_mem_word < 16384);
        offset += index_tm_pre_mit_mem_word * 0x1; // tm_pre_mit_mem_word[]
        break;
      case kMit3MemWord:
        offset += 0x400000; // to get to mit3_mem_word
        assert(index_tm_pre_mit_mem_word < 16384);
        offset += index_tm_pre_mit_mem_word * 0x1; // tm_pre_mit_mem_word[]
        break;
      default:
        assert(0);
        break;
    }
    return offset;
  }

};








class TmPreMitMemWordArray : public model_core::RegisterBlockIndirect<RegisterArrayCallback> {
public:
  enum TmPreMemRspecEnum {
    kMit0MemWord,
    kMit1MemWord,
    kMit2MemWord,
    kMit3MemWord
  };
public:
  TmPreMitMemWordArray(
      int chipNumber, TmPreMemRspecEnum selector_tm_pre_mem_rspec, RegisterArrayCallback& write_callback = 0, RegisterArrayCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(selector_tm_pre_mem_rspec), 1 * ArraySize(selector_tm_pre_mem_rspec, -1), false, write_callback, read_callback, std::string("TmPreMitMemWordArray")+":"+boost::lexical_cast<std::string>(selector_tm_pre_mem_rspec)),
    array(RealArraySize(selector_tm_pre_mem_rspec, -1)),
    size0_(ArraySize(selector_tm_pre_mem_rspec, 0)),
    real_size0_(RealArraySize(selector_tm_pre_mem_rspec, 0))
    {
    }
public:




  uint8_t &mem_word(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].mem_word();
  }
  bool calculate_index(
      uint64_t* offset, uint32_t* a0
      ) const {
    int i = (*offset)/1;
    (*offset) -= (i*1);
    uint32_t t = i;
    *a0 = t;
    bool in_bounds = true;
    in_bounds &= CheckArrayBounds(*a0,real_size0_,size0_,*offset,0,true);
    return in_bounds;
  }


  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    if (read_callback_) read_callback_(a0);
    array[a0].read(offset,data0,data1,T);
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    array[a0].write(offset,data0,data1,T);
    if (write_callback_) write_callback_(a0);
    return true;
  }

  void reset(
      
      ) {
    for (uint32_t i=0;i<array.size();++i) {
      array[i].reset();
      int t = i;
      int a0 = t;
      if (write_callback_) write_callback_(a0);
    }
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    all_zeros=false;
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return "OUT_OF_BOUNDS";
    std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
    if (! r_s.empty()) {
      r += indent_string + std::string("TmPreMitMemWordArray") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
    }
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
    all_zeros=false;
    for (uint32_t a0=0;a0<real_size0_;++a0) {
      std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
      if (! r_s.empty()) {
        r += indent_string + std::string("TmPreMitMemWordArray") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
      }
    }
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  std::vector<TmPreMitMemWord> array;
  uint32_t size0_;
  uint32_t real_size0_;
private:
  static uint64_t StartOffset(
      TmPreMemRspecEnum selector_tm_pre_mem_rspec
      ) {
    uint64_t offset=0;
    offset += 0x6100000000; // to get to tm_pre
    switch (selector_tm_pre_mem_rspec) {
      case kMit0MemWord:
        offset += 0x100000; // to get to mit0_mem_word
        break;
      case kMit1MemWord:
        offset += 0x200000; // to get to mit1_mem_word
        break;
      case kMit2MemWord:
        offset += 0x300000; // to get to mit2_mem_word
        break;
      case kMit3MemWord:
        offset += 0x400000; // to get to mit3_mem_word
        break;
      default:
        assert(0);
        break;
    }
    return offset;
  }

  static int ArraySize(
      TmPreMemRspecEnum selector_tm_pre_mem_rspec, int dimension
      ) {
    switch (selector_tm_pre_mem_rspec) {
      case kMit0MemWord:
        switch (dimension) {
          case -1:
            return 16384;
            break;
          case 0:
            return 16384;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kMit1MemWord:
        switch (dimension) {
          case -1:
            return 16384;
            break;
          case 0:
            return 16384;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kMit2MemWord:
        switch (dimension) {
          case -1:
            return 16384;
            break;
          case 0:
            return 16384;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kMit3MemWord:
        switch (dimension) {
          case -1:
            return 16384;
            break;
          case 0:
            return 16384;
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
  }

  static int RealArraySize(
      TmPreMemRspecEnum selector_tm_pre_mem_rspec, int dimension
      ) {
    switch (selector_tm_pre_mem_rspec) {
      case kMit0MemWord:
        switch (dimension) {
          case -1:
            return 16384;
            break;
          case 0:
            return 16384;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kMit1MemWord:
        switch (dimension) {
          case -1:
            return 16384;
            break;
          case 0:
            return 16384;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kMit2MemWord:
        switch (dimension) {
          case -1:
            return 16384;
            break;
          case 0:
            return 16384;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kMit3MemWord:
        switch (dimension) {
          case -1:
            return 16384;
            break;
          case 0:
            return 16384;
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
  }

};








class TmPreMitMemWordArrayMutable : public model_core::RegisterBlockIndirect<RegisterArrayCallback> {
public:
  enum TmPreMemRspecEnum {
    kMit0MemWord,
    kMit1MemWord,
    kMit2MemWord,
    kMit3MemWord
  };
public:
  TmPreMitMemWordArrayMutable(
      int chipNumber, TmPreMemRspecEnum selector_tm_pre_mem_rspec, RegisterArrayCallback& write_callback = 0, RegisterArrayCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(selector_tm_pre_mem_rspec), 1 * ArraySize(selector_tm_pre_mem_rspec, -1), true, write_callback, read_callback, std::string("TmPreMitMemWordArrayMutable")+":"+boost::lexical_cast<std::string>(selector_tm_pre_mem_rspec)),
    array(RealArraySize(selector_tm_pre_mem_rspec, -1)),
    size0_(ArraySize(selector_tm_pre_mem_rspec, 0)),
    real_size0_(RealArraySize(selector_tm_pre_mem_rspec, 0))
    {
    }
public:




  uint8_t mem_word(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].mem_word();
  }
  void mem_word(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].mem_word(v);
  }
  bool calculate_index(
      uint64_t* offset, uint32_t* a0
      ) const {
    int i = (*offset)/1;
    (*offset) -= (i*1);
    uint32_t t = i;
    *a0 = t;
    bool in_bounds = true;
    in_bounds &= CheckArrayBounds(*a0,real_size0_,size0_,*offset,0,true);
    return in_bounds;
  }


  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    if (read_callback_) read_callback_(a0);
    array[a0].read(offset,data0,data1,T);
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    array[a0].write(offset,data0,data1,T);
    if (write_callback_) write_callback_(a0);
    return true;
  }

  void reset(
      
      ) {
    for (uint32_t i=0;i<array.size();++i) {
      array[i].reset();
      int t = i;
      int a0 = t;
      if (write_callback_) write_callback_(a0);
    }
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    all_zeros=false;
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return "OUT_OF_BOUNDS";
    std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
    if (! r_s.empty()) {
      r += indent_string + std::string("TmPreMitMemWordArrayMutable") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
    }
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
    all_zeros=false;
    for (uint32_t a0=0;a0<real_size0_;++a0) {
      std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
      if (! r_s.empty()) {
        r += indent_string + std::string("TmPreMitMemWordArrayMutable") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
      }
    }
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  std::vector<TmPreMitMemWordMutable> array;
  uint32_t size0_;
  uint32_t real_size0_;
private:
  static uint64_t StartOffset(
      TmPreMemRspecEnum selector_tm_pre_mem_rspec
      ) {
    uint64_t offset=0;
    offset += 0x6100000000; // to get to tm_pre
    switch (selector_tm_pre_mem_rspec) {
      case kMit0MemWord:
        offset += 0x100000; // to get to mit0_mem_word
        break;
      case kMit1MemWord:
        offset += 0x200000; // to get to mit1_mem_word
        break;
      case kMit2MemWord:
        offset += 0x300000; // to get to mit2_mem_word
        break;
      case kMit3MemWord:
        offset += 0x400000; // to get to mit3_mem_word
        break;
      default:
        assert(0);
        break;
    }
    return offset;
  }

  static int ArraySize(
      TmPreMemRspecEnum selector_tm_pre_mem_rspec, int dimension
      ) {
    switch (selector_tm_pre_mem_rspec) {
      case kMit0MemWord:
        switch (dimension) {
          case -1:
            return 16384;
            break;
          case 0:
            return 16384;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kMit1MemWord:
        switch (dimension) {
          case -1:
            return 16384;
            break;
          case 0:
            return 16384;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kMit2MemWord:
        switch (dimension) {
          case -1:
            return 16384;
            break;
          case 0:
            return 16384;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kMit3MemWord:
        switch (dimension) {
          case -1:
            return 16384;
            break;
          case 0:
            return 16384;
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
  }

  static int RealArraySize(
      TmPreMemRspecEnum selector_tm_pre_mem_rspec, int dimension
      ) {
    switch (selector_tm_pre_mem_rspec) {
      case kMit0MemWord:
        switch (dimension) {
          case -1:
            return 16384;
            break;
          case 0:
            return 16384;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kMit1MemWord:
        switch (dimension) {
          case -1:
            return 16384;
            break;
          case 0:
            return 16384;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kMit2MemWord:
        switch (dimension) {
          case -1:
            return 16384;
            break;
          case 0:
            return 16384;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case kMit3MemWord:
        switch (dimension) {
          case -1:
            return 16384;
            break;
          case 0:
            return 16384;
            break;
          default:
            assert(0);
            break;
        }
        break;
      default:
        assert(0);
        break;
    }
  }

};








class TmPreRdmMemWord : public model_core::RegisterBlockIndirect<RegisterCallback> {
public:
  TmPreRdmMemWord(
      int chipNumber, int index_tm_pre_rdm_mem_word, RegisterCallback& write_callback = 0, RegisterCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(index_tm_pre_rdm_mem_word), 1, false, write_callback, read_callback, std::string("TmPreRdmMemWord")+":"+boost::lexical_cast<std::string>(index_tm_pre_rdm_mem_word))
    {
    }
  TmPreRdmMemWord(
      
  )
    : RegisterBlockIndirect(0, 0, 0, false, 0, 0, "TmPreRdmMemWord")
    {
    }
public:




  uint8_t &mem_word() { return mem_word_; }

  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    if (read_callback_) read_callback_();
    *data0 = (static_cast<uint64_t>(mem_word_) & 0x1);
    *data1 = 0;
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    mem_word_ = (data0 & 0x1);
    if (write_callback_) write_callback_();
    return true;
  }

  void reset(
      
      ) {
    mem_word_ = 0;
    if (write_callback_) write_callback_();
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    r += indent_string + std::string("TmPreRdmMemWord") + ":\n";
    r += indent_string + "  " + std::string("mem_word") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(mem_word_) ) + "\n";
    all_zeros &= (0 == mem_word_);
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
    r += indent_string + std::string("TmPreRdmMemWord") + ":\n";
    r += indent_string + "  " + std::string("mem_word") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(mem_word_) ) + "\n";
    all_zeros &= (0 == mem_word_);
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  uint8_t mem_word_;
private:
  static uint64_t StartOffset(
      int index_tm_pre_rdm_mem_word
      ) {
    uint64_t offset=0;
    offset += 0x6100000000; // to get to tm_pre
    assert(index_tm_pre_rdm_mem_word < 417792);
    offset += index_tm_pre_rdm_mem_word * 0x1; // tm_pre_rdm_mem_word[]
    return offset;
  }

};








class TmPreRdmMemWordMutable : public model_core::RegisterBlockIndirect<RegisterCallback> {
public:
  TmPreRdmMemWordMutable(
      int chipNumber, int index_tm_pre_rdm_mem_word, RegisterCallback& write_callback = 0, RegisterCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(index_tm_pre_rdm_mem_word), 1, true, write_callback, read_callback, std::string("TmPreRdmMemWordMutable")+":"+boost::lexical_cast<std::string>(index_tm_pre_rdm_mem_word))
    {
    }
  TmPreRdmMemWordMutable(
      
  )
    : RegisterBlockIndirect(0, 0, 0, true, 0, 0, "TmPreRdmMemWordMutable")
    {
    }
public:




  uint8_t mem_word() { return mem_word_; }
  void mem_word(const uint8_t &v) { mem_word_=v; }

  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    if (read_callback_) read_callback_();
    *data0 = (static_cast<uint64_t>(mem_word_) & 0x1);
    *data1 = 0;
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    mem_word_ = (data0 & 0x1);
    if (write_callback_) write_callback_();
    return true;
  }

  void reset(
      
      ) {
    mem_word_ = 0;
    if (write_callback_) write_callback_();
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    r += indent_string + std::string("TmPreRdmMemWordMutable") + ":\n";
    r += indent_string + "  " + std::string("mem_word") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(mem_word_) ) + "\n";
    all_zeros &= (0 == mem_word_);
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
    r += indent_string + std::string("TmPreRdmMemWordMutable") + ":\n";
    r += indent_string + "  " + std::string("mem_word") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(mem_word_) ) + "\n";
    all_zeros &= (0 == mem_word_);
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  uint8_t mem_word_;
private:
  static uint64_t StartOffset(
      int index_tm_pre_rdm_mem_word
      ) {
    uint64_t offset=0;
    offset += 0x6100000000; // to get to tm_pre
    assert(index_tm_pre_rdm_mem_word < 417792);
    offset += index_tm_pre_rdm_mem_word * 0x1; // tm_pre_rdm_mem_word[]
    return offset;
  }

};








class TmPreRdmMemWordArray : public model_core::RegisterBlockIndirect<RegisterArrayCallback> {
public:
  TmPreRdmMemWordArray(
      int chipNumber, RegisterArrayCallback& write_callback = 0, RegisterArrayCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(), 1 * ArraySize(-1), false, write_callback, read_callback, std::string("TmPreRdmMemWordArray")),
    array(RealArraySize(-1)),
    size0_(ArraySize(0)),
    real_size0_(RealArraySize(0))
    {
    }
public:




  uint8_t &mem_word(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].mem_word();
  }
  bool calculate_index(
      uint64_t* offset, uint32_t* a0
      ) const {
    int i = (*offset)/1;
    (*offset) -= (i*1);
    uint32_t t = i;
    *a0 = t;
    bool in_bounds = true;
    in_bounds &= CheckArrayBounds(*a0,real_size0_,size0_,*offset,0,true);
    return in_bounds;
  }


  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    if (read_callback_) read_callback_(a0);
    array[a0].read(offset,data0,data1,T);
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    array[a0].write(offset,data0,data1,T);
    if (write_callback_) write_callback_(a0);
    return true;
  }

  void reset(
      
      ) {
    for (uint32_t i=0;i<array.size();++i) {
      array[i].reset();
      int t = i;
      int a0 = t;
      if (write_callback_) write_callback_(a0);
    }
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    all_zeros=false;
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return "OUT_OF_BOUNDS";
    std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
    if (! r_s.empty()) {
      r += indent_string + std::string("TmPreRdmMemWordArray") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
    }
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
    all_zeros=false;
    for (uint32_t a0=0;a0<real_size0_;++a0) {
      std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
      if (! r_s.empty()) {
        r += indent_string + std::string("TmPreRdmMemWordArray") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
      }
    }
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  std::vector<TmPreRdmMemWord> array;
  uint32_t size0_;
  uint32_t real_size0_;
private:
  static uint64_t StartOffset(
      
      ) {
    uint64_t offset=0;
    offset += 0x6100000000; // to get to tm_pre
    return offset;
  }

  static int ArraySize(
      int dimension
      ) {
    switch (dimension) {
      case -1:
        return 417792;
        break;
      case 0:
        return 417792;
        break;
      default:
        assert(0);
        break;
    }
  }

  static int RealArraySize(
      int dimension
      ) {
    switch (dimension) {
      case -1:
        return 417792;
        break;
      case 0:
        return 417792;
        break;
      default:
        assert(0);
        break;
    }
  }

};








class TmPreRdmMemWordArrayMutable : public model_core::RegisterBlockIndirect<RegisterArrayCallback> {
public:
  TmPreRdmMemWordArrayMutable(
      int chipNumber, RegisterArrayCallback& write_callback = 0, RegisterArrayCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(), 1 * ArraySize(-1), true, write_callback, read_callback, std::string("TmPreRdmMemWordArrayMutable")),
    array(RealArraySize(-1)),
    size0_(ArraySize(0)),
    real_size0_(RealArraySize(0))
    {
    }
public:




  uint8_t mem_word(uint32_t a0)
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    return array[a0].mem_word();
  }
  void mem_word(uint32_t a0,const uint8_t &v) 
  {
    CheckArrayBounds(a0,real_size0_,size0_,0,0,false);
    array[a0].mem_word(v);
  }
  bool calculate_index(
      uint64_t* offset, uint32_t* a0
      ) const {
    int i = (*offset)/1;
    (*offset) -= (i*1);
    uint32_t t = i;
    *a0 = t;
    bool in_bounds = true;
    in_bounds &= CheckArrayBounds(*a0,real_size0_,size0_,*offset,0,true);
    return in_bounds;
  }


  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    if (read_callback_) read_callback_(a0);
    array[a0].read(offset,data0,data1,T);
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return true;
    array[a0].write(offset,data0,data1,T);
    if (write_callback_) write_callback_(a0);
    return true;
  }

  void reset(
      
      ) {
    for (uint32_t i=0;i<array.size();++i) {
      array[i].reset();
      int t = i;
      int a0 = t;
      if (write_callback_) write_callback_(a0);
    }
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    all_zeros=false;
    uint32_t a0;
     if (!calculate_index(&offset,&a0)) return "OUT_OF_BOUNDS";
    std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
    if (! r_s.empty()) {
      r += indent_string + std::string("TmPreRdmMemWordArrayMutable") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
    }
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
    all_zeros=false;
    for (uint32_t a0=0;a0<real_size0_;++a0) {
      std::string r_s = array[a0].to_string(print_zeros,indent_string+"  ");
      if (! r_s.empty()) {
        r += indent_string + std::string("TmPreRdmMemWordArrayMutable") + "["+boost::lexical_cast<std::string>(a0)+"]"+ ":\n" + r_s ;
      }
    }
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  std::vector<TmPreRdmMemWordMutable> array;
  uint32_t size0_;
  uint32_t real_size0_;
private:
  static uint64_t StartOffset(
      
      ) {
    uint64_t offset=0;
    offset += 0x6100000000; // to get to tm_pre
    return offset;
  }

  static int ArraySize(
      int dimension
      ) {
    switch (dimension) {
      case -1:
        return 417792;
        break;
      case 0:
        return 417792;
        break;
      default:
        assert(0);
        break;
    }
  }

  static int RealArraySize(
      int dimension
      ) {
    switch (dimension) {
      case -1:
        return 417792;
        break;
      case 0:
        return 417792;
        break;
      default:
        assert(0);
        break;
    }
  }

};








class TmPreMemRspec : public model_core::RegisterBlockIndirect<RegisterCallback> {
public:
  TmPreMemRspec(
      int chipNumber, RegisterCallback& write_callback = 0, RegisterCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(), 4294967296, false, write_callback, read_callback, std::string("TmPreMemRspec"))
    {
    }
  TmPreMemRspec(
      
  )
    : RegisterBlockIndirect(0, 0, 0, false, 0, 0, "TmPreMemRspec")
    {
    }
public:







  TmPreRdmMemWord &rdm_mem_word(int j0) { return rdm_mem_word_[j0]; }







  TmPreMitMemWord &mit0_mem_word(int j0) { return mit0_mem_word_[j0]; }







  TmPreMitMemWord &mit1_mem_word(int j0) { return mit1_mem_word_[j0]; }







  TmPreMitMemWord &mit2_mem_word(int j0) { return mit2_mem_word_[j0]; }







  TmPreMitMemWord &mit3_mem_word(int j0) { return mit3_mem_word_[j0]; }







  TmPrePbtMemWord &pbt0_mem_word(int j0) { return pbt0_mem_word_[j0]; }







  TmPrePbtMemWord &pbt1_mem_word(int j0) { return pbt1_mem_word_[j0]; }







  TmPreLitNpMemWord &lit0_np_mem_word(int j0) { return lit0_np_mem_word_[j0]; }







  TmPreLitNpMemWord &lit1_np_mem_word(int j0) { return lit1_np_mem_word_[j0]; }







  TmPreFifoMemWord &fifo_mem_word(int j0) { return fifo_mem_word_[j0]; }







  TmPreLitBmMemWord &lit0_bm_mem_word0(int j0) { return lit0_bm_mem_word0_[j0]; }







  TmPreLitBmMemWord &lit0_bm_mem_word1(int j0) { return lit0_bm_mem_word1_[j0]; }







  TmPreLitBmMemWord &lit0_bm_mem_word2(int j0) { return lit0_bm_mem_word2_[j0]; }







  TmPreLitBmMemWord &lit0_bm_mem_word3(int j0) { return lit0_bm_mem_word3_[j0]; }







  TmPreLitBmMemWord &lit1_bm_mem_word0(int j0) { return lit1_bm_mem_word0_[j0]; }







  TmPreLitBmMemWord &lit1_bm_mem_word1(int j0) { return lit1_bm_mem_word1_[j0]; }







  TmPreLitBmMemWord &lit1_bm_mem_word2(int j0) { return lit1_bm_mem_word2_[j0]; }







  TmPreLitBmMemWord &lit1_bm_mem_word3(int j0) { return lit1_bm_mem_word3_[j0]; }







  TmPrePmtMemWord &pmt0_mem_word0(int j0) { return pmt0_mem_word0_[j0]; }







  TmPrePmtMemWord &pmt0_mem_word1(int j0) { return pmt0_mem_word1_[j0]; }







  TmPrePmtMemWord &pmt0_mem_word2(int j0) { return pmt0_mem_word2_[j0]; }







  TmPrePmtMemWord &pmt0_mem_word3(int j0) { return pmt0_mem_word3_[j0]; }







  TmPrePmtMemWord &pmt1_mem_word0(int j0) { return pmt1_mem_word0_[j0]; }







  TmPrePmtMemWord &pmt1_mem_word1(int j0) { return pmt1_mem_word1_[j0]; }







  TmPrePmtMemWord &pmt1_mem_word2(int j0) { return pmt1_mem_word2_[j0]; }







  TmPrePmtMemWord &pmt1_mem_word3(int j0) { return pmt1_mem_word3_[j0]; }

  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    if (offset < 0x66000) {
      offset -= 0x0;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      rdm_mem_word_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x100000 && offset < 0x104000) {
      offset -= 0x100000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      mit0_mem_word_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x200000 && offset < 0x204000) {
      offset -= 0x200000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      mit1_mem_word_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x300000 && offset < 0x304000) {
      offset -= 0x300000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      mit2_mem_word_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x400000 && offset < 0x404000) {
      offset -= 0x400000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      mit3_mem_word_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x40000000 && offset < 0x40000120) {
      offset -= 0x40000000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      pbt0_mem_word_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x40100000 && offset < 0x40100120) {
      offset -= 0x40100000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      pbt1_mem_word_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x40200000 && offset < 0x40200100) {
      offset -= 0x40200000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      lit0_np_mem_word_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x40300000 && offset < 0x40300100) {
      offset -= 0x40300000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      lit1_np_mem_word_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x40400000 && offset < 0x40400004) {
      offset -= 0x40400000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      fifo_mem_word_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x80000000 && offset < 0x80000100) {
      offset -= 0x80000000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      lit0_bm_mem_word0_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x80040000 && offset < 0x80040100) {
      offset -= 0x80040000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      lit0_bm_mem_word1_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x80080000 && offset < 0x80080100) {
      offset -= 0x80080000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      lit0_bm_mem_word2_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x800c0000 && offset < 0x800c0100) {
      offset -= 0x800c0000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      lit0_bm_mem_word3_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x80100000 && offset < 0x80100100) {
      offset -= 0x80100000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      lit1_bm_mem_word0_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x80140000 && offset < 0x80140100) {
      offset -= 0x80140000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      lit1_bm_mem_word1_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x80180000 && offset < 0x80180100) {
      offset -= 0x80180000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      lit1_bm_mem_word2_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x801c0000 && offset < 0x801c0100) {
      offset -= 0x801c0000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      lit1_bm_mem_word3_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x80200000 && offset < 0x80200120) {
      offset -= 0x80200000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      pmt0_mem_word0_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x80240000 && offset < 0x80240120) {
      offset -= 0x80240000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      pmt0_mem_word1_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x80280000 && offset < 0x80280120) {
      offset -= 0x80280000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      pmt0_mem_word2_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x802c0000 && offset < 0x802c0120) {
      offset -= 0x802c0000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      pmt0_mem_word3_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x80300000 && offset < 0x80300120) {
      offset -= 0x80300000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      pmt1_mem_word0_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x80340000 && offset < 0x80340120) {
      offset -= 0x80340000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      pmt1_mem_word1_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x80380000 && offset < 0x80380120) {
      offset -= 0x80380000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      pmt1_mem_word2_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x803c0000 && offset < 0x803c0120) {
      offset -= 0x803c0000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      pmt1_mem_word3_[ i0 ].read( offset, data0,data1,T );
    }
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    if (offset < 0x66000) {
      offset -= 0x0;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      rdm_mem_word_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x100000 && offset < 0x104000) {
      offset -= 0x100000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      mit0_mem_word_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x200000 && offset < 0x204000) {
      offset -= 0x200000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      mit1_mem_word_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x300000 && offset < 0x304000) {
      offset -= 0x300000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      mit2_mem_word_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x400000 && offset < 0x404000) {
      offset -= 0x400000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      mit3_mem_word_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x40000000 && offset < 0x40000120) {
      offset -= 0x40000000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      pbt0_mem_word_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x40100000 && offset < 0x40100120) {
      offset -= 0x40100000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      pbt1_mem_word_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x40200000 && offset < 0x40200100) {
      offset -= 0x40200000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      lit0_np_mem_word_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x40300000 && offset < 0x40300100) {
      offset -= 0x40300000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      lit1_np_mem_word_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x40400000 && offset < 0x40400004) {
      offset -= 0x40400000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      fifo_mem_word_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x80000000 && offset < 0x80000100) {
      offset -= 0x80000000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      lit0_bm_mem_word0_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x80040000 && offset < 0x80040100) {
      offset -= 0x80040000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      lit0_bm_mem_word1_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x80080000 && offset < 0x80080100) {
      offset -= 0x80080000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      lit0_bm_mem_word2_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x800c0000 && offset < 0x800c0100) {
      offset -= 0x800c0000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      lit0_bm_mem_word3_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x80100000 && offset < 0x80100100) {
      offset -= 0x80100000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      lit1_bm_mem_word0_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x80140000 && offset < 0x80140100) {
      offset -= 0x80140000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      lit1_bm_mem_word1_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x80180000 && offset < 0x80180100) {
      offset -= 0x80180000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      lit1_bm_mem_word2_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x801c0000 && offset < 0x801c0100) {
      offset -= 0x801c0000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      lit1_bm_mem_word3_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x80200000 && offset < 0x80200120) {
      offset -= 0x80200000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      pmt0_mem_word0_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x80240000 && offset < 0x80240120) {
      offset -= 0x80240000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      pmt0_mem_word1_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x80280000 && offset < 0x80280120) {
      offset -= 0x80280000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      pmt0_mem_word2_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x802c0000 && offset < 0x802c0120) {
      offset -= 0x802c0000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      pmt0_mem_word3_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x80300000 && offset < 0x80300120) {
      offset -= 0x80300000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      pmt1_mem_word0_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x80340000 && offset < 0x80340120) {
      offset -= 0x80340000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      pmt1_mem_word1_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x80380000 && offset < 0x80380120) {
      offset -= 0x80380000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      pmt1_mem_word2_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x803c0000 && offset < 0x803c0120) {
      offset -= 0x803c0000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      pmt1_mem_word3_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    return true;
  }

  void reset(
      
      ) {
    for (auto &f0 : rdm_mem_word_) {
      f0.reset();
    }
    for (auto &f0 : mit0_mem_word_) {
      f0.reset();
    }
    for (auto &f0 : mit1_mem_word_) {
      f0.reset();
    }
    for (auto &f0 : mit2_mem_word_) {
      f0.reset();
    }
    for (auto &f0 : mit3_mem_word_) {
      f0.reset();
    }
    for (auto &f0 : pbt0_mem_word_) {
      f0.reset();
    }
    for (auto &f0 : pbt1_mem_word_) {
      f0.reset();
    }
    for (auto &f0 : lit0_np_mem_word_) {
      f0.reset();
    }
    for (auto &f0 : lit1_np_mem_word_) {
      f0.reset();
    }
    for (auto &f0 : fifo_mem_word_) {
      f0.reset();
    }
    for (auto &f0 : lit0_bm_mem_word0_) {
      f0.reset();
    }
    for (auto &f0 : lit0_bm_mem_word1_) {
      f0.reset();
    }
    for (auto &f0 : lit0_bm_mem_word2_) {
      f0.reset();
    }
    for (auto &f0 : lit0_bm_mem_word3_) {
      f0.reset();
    }
    for (auto &f0 : lit1_bm_mem_word0_) {
      f0.reset();
    }
    for (auto &f0 : lit1_bm_mem_word1_) {
      f0.reset();
    }
    for (auto &f0 : lit1_bm_mem_word2_) {
      f0.reset();
    }
    for (auto &f0 : lit1_bm_mem_word3_) {
      f0.reset();
    }
    for (auto &f0 : pmt0_mem_word0_) {
      f0.reset();
    }
    for (auto &f0 : pmt0_mem_word1_) {
      f0.reset();
    }
    for (auto &f0 : pmt0_mem_word2_) {
      f0.reset();
    }
    for (auto &f0 : pmt0_mem_word3_) {
      f0.reset();
    }
    for (auto &f0 : pmt1_mem_word0_) {
      f0.reset();
    }
    for (auto &f0 : pmt1_mem_word1_) {
      f0.reset();
    }
    for (auto &f0 : pmt1_mem_word2_) {
      f0.reset();
    }
    for (auto &f0 : pmt1_mem_word3_) {
      f0.reset();
    }
    if (write_callback_) write_callback_();
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    all_zeros=false;
    if (offset < 0x66000) {
      offset -= 0x0;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += rdm_mem_word_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x100000 && offset < 0x104000) {
      offset -= 0x100000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += mit0_mem_word_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x200000 && offset < 0x204000) {
      offset -= 0x200000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += mit1_mem_word_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x300000 && offset < 0x304000) {
      offset -= 0x300000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += mit2_mem_word_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x400000 && offset < 0x404000) {
      offset -= 0x400000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += mit3_mem_word_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x40000000 && offset < 0x40000120) {
      offset -= 0x40000000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += pbt0_mem_word_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x40100000 && offset < 0x40100120) {
      offset -= 0x40100000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += pbt1_mem_word_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x40200000 && offset < 0x40200100) {
      offset -= 0x40200000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += lit0_np_mem_word_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x40300000 && offset < 0x40300100) {
      offset -= 0x40300000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += lit1_np_mem_word_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x40400000 && offset < 0x40400004) {
      offset -= 0x40400000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += fifo_mem_word_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x80000000 && offset < 0x80000100) {
      offset -= 0x80000000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += lit0_bm_mem_word0_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x80040000 && offset < 0x80040100) {
      offset -= 0x80040000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += lit0_bm_mem_word1_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x80080000 && offset < 0x80080100) {
      offset -= 0x80080000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += lit0_bm_mem_word2_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x800c0000 && offset < 0x800c0100) {
      offset -= 0x800c0000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += lit0_bm_mem_word3_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x80100000 && offset < 0x80100100) {
      offset -= 0x80100000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += lit1_bm_mem_word0_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x80140000 && offset < 0x80140100) {
      offset -= 0x80140000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += lit1_bm_mem_word1_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x80180000 && offset < 0x80180100) {
      offset -= 0x80180000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += lit1_bm_mem_word2_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x801c0000 && offset < 0x801c0100) {
      offset -= 0x801c0000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += lit1_bm_mem_word3_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x80200000 && offset < 0x80200120) {
      offset -= 0x80200000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += pmt0_mem_word0_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x80240000 && offset < 0x80240120) {
      offset -= 0x80240000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += pmt0_mem_word1_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x80280000 && offset < 0x80280120) {
      offset -= 0x80280000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += pmt0_mem_word2_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x802c0000 && offset < 0x802c0120) {
      offset -= 0x802c0000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += pmt0_mem_word3_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x80300000 && offset < 0x80300120) {
      offset -= 0x80300000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += pmt1_mem_word0_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x80340000 && offset < 0x80340120) {
      offset -= 0x80340000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += pmt1_mem_word1_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x80380000 && offset < 0x80380120) {
      offset -= 0x80380000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += pmt1_mem_word2_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x803c0000 && offset < 0x803c0120) {
      offset -= 0x803c0000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += pmt1_mem_word3_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
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
    all_zeros=false;
    for (uint32_t a0=0;a0<417792;++a0) {
      r += rdm_mem_word_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<16384;++a0) {
      r += mit0_mem_word_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<16384;++a0) {
      r += mit1_mem_word_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<16384;++a0) {
      r += mit2_mem_word_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<16384;++a0) {
      r += mit3_mem_word_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<288;++a0) {
      r += pbt0_mem_word_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<288;++a0) {
      r += pbt1_mem_word_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<256;++a0) {
      r += lit0_np_mem_word_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<256;++a0) {
      r += lit1_np_mem_word_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<4;++a0) {
      r += fifo_mem_word_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<256;++a0) {
      r += lit0_bm_mem_word0_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<256;++a0) {
      r += lit0_bm_mem_word1_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<256;++a0) {
      r += lit0_bm_mem_word2_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<256;++a0) {
      r += lit0_bm_mem_word3_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<256;++a0) {
      r += lit1_bm_mem_word0_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<256;++a0) {
      r += lit1_bm_mem_word1_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<256;++a0) {
      r += lit1_bm_mem_word2_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<256;++a0) {
      r += lit1_bm_mem_word3_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<288;++a0) {
      r += pmt0_mem_word0_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<288;++a0) {
      r += pmt0_mem_word1_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<288;++a0) {
      r += pmt0_mem_word2_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<288;++a0) {
      r += pmt0_mem_word3_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<288;++a0) {
      r += pmt1_mem_word0_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<288;++a0) {
      r += pmt1_mem_word1_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<288;++a0) {
      r += pmt1_mem_word2_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<288;++a0) {
      r += pmt1_mem_word3_[a0].to_string(print_zeros,indent_string) ;
    }
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  std::array< TmPreRdmMemWord, 417792 > rdm_mem_word_;
  std::array< TmPreMitMemWord, 16384 > mit0_mem_word_;
  std::array< TmPreMitMemWord, 16384 > mit1_mem_word_;
  std::array< TmPreMitMemWord, 16384 > mit2_mem_word_;
  std::array< TmPreMitMemWord, 16384 > mit3_mem_word_;
  std::array< TmPrePbtMemWord, 288 > pbt0_mem_word_;
  std::array< TmPrePbtMemWord, 288 > pbt1_mem_word_;
  std::array< TmPreLitNpMemWord, 256 > lit0_np_mem_word_;
  std::array< TmPreLitNpMemWord, 256 > lit1_np_mem_word_;
  std::array< TmPreFifoMemWord, 4 > fifo_mem_word_;
  std::array< TmPreLitBmMemWord, 256 > lit0_bm_mem_word0_;
  std::array< TmPreLitBmMemWord, 256 > lit0_bm_mem_word1_;
  std::array< TmPreLitBmMemWord, 256 > lit0_bm_mem_word2_;
  std::array< TmPreLitBmMemWord, 256 > lit0_bm_mem_word3_;
  std::array< TmPreLitBmMemWord, 256 > lit1_bm_mem_word0_;
  std::array< TmPreLitBmMemWord, 256 > lit1_bm_mem_word1_;
  std::array< TmPreLitBmMemWord, 256 > lit1_bm_mem_word2_;
  std::array< TmPreLitBmMemWord, 256 > lit1_bm_mem_word3_;
  std::array< TmPrePmtMemWord, 288 > pmt0_mem_word0_;
  std::array< TmPrePmtMemWord, 288 > pmt0_mem_word1_;
  std::array< TmPrePmtMemWord, 288 > pmt0_mem_word2_;
  std::array< TmPrePmtMemWord, 288 > pmt0_mem_word3_;
  std::array< TmPrePmtMemWord, 288 > pmt1_mem_word0_;
  std::array< TmPrePmtMemWord, 288 > pmt1_mem_word1_;
  std::array< TmPrePmtMemWord, 288 > pmt1_mem_word2_;
  std::array< TmPrePmtMemWord, 288 > pmt1_mem_word3_;
private:
  static uint64_t StartOffset(
      
      ) {
    uint64_t offset=0;
    offset += 0x6100000000; // to get to tm_pre
    return offset;
  }

};








class TmPreMemRspecMutable : public model_core::RegisterBlockIndirect<RegisterCallback> {
public:
  TmPreMemRspecMutable(
      int chipNumber, RegisterCallback& write_callback = 0, RegisterCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(), 4294967296, true, write_callback, read_callback, std::string("TmPreMemRspecMutable"))
    {
    }
  TmPreMemRspecMutable(
      
  )
    : RegisterBlockIndirect(0, 0, 0, true, 0, 0, "TmPreMemRspecMutable")
    {
    }
public:







  TmPreRdmMemWordMutable &rdm_mem_word(int j0) { return rdm_mem_word_[j0]; }







  TmPreMitMemWordMutable &mit0_mem_word(int j0) { return mit0_mem_word_[j0]; }







  TmPreMitMemWordMutable &mit1_mem_word(int j0) { return mit1_mem_word_[j0]; }







  TmPreMitMemWordMutable &mit2_mem_word(int j0) { return mit2_mem_word_[j0]; }







  TmPreMitMemWordMutable &mit3_mem_word(int j0) { return mit3_mem_word_[j0]; }







  TmPrePbtMemWordMutable &pbt0_mem_word(int j0) { return pbt0_mem_word_[j0]; }







  TmPrePbtMemWordMutable &pbt1_mem_word(int j0) { return pbt1_mem_word_[j0]; }







  TmPreLitNpMemWordMutable &lit0_np_mem_word(int j0) { return lit0_np_mem_word_[j0]; }







  TmPreLitNpMemWordMutable &lit1_np_mem_word(int j0) { return lit1_np_mem_word_[j0]; }







  TmPreFifoMemWordMutable &fifo_mem_word(int j0) { return fifo_mem_word_[j0]; }







  TmPreLitBmMemWordMutable &lit0_bm_mem_word0(int j0) { return lit0_bm_mem_word0_[j0]; }







  TmPreLitBmMemWordMutable &lit0_bm_mem_word1(int j0) { return lit0_bm_mem_word1_[j0]; }







  TmPreLitBmMemWordMutable &lit0_bm_mem_word2(int j0) { return lit0_bm_mem_word2_[j0]; }







  TmPreLitBmMemWordMutable &lit0_bm_mem_word3(int j0) { return lit0_bm_mem_word3_[j0]; }







  TmPreLitBmMemWordMutable &lit1_bm_mem_word0(int j0) { return lit1_bm_mem_word0_[j0]; }







  TmPreLitBmMemWordMutable &lit1_bm_mem_word1(int j0) { return lit1_bm_mem_word1_[j0]; }







  TmPreLitBmMemWordMutable &lit1_bm_mem_word2(int j0) { return lit1_bm_mem_word2_[j0]; }







  TmPreLitBmMemWordMutable &lit1_bm_mem_word3(int j0) { return lit1_bm_mem_word3_[j0]; }







  TmPrePmtMemWordMutable &pmt0_mem_word0(int j0) { return pmt0_mem_word0_[j0]; }







  TmPrePmtMemWordMutable &pmt0_mem_word1(int j0) { return pmt0_mem_word1_[j0]; }







  TmPrePmtMemWordMutable &pmt0_mem_word2(int j0) { return pmt0_mem_word2_[j0]; }







  TmPrePmtMemWordMutable &pmt0_mem_word3(int j0) { return pmt0_mem_word3_[j0]; }







  TmPrePmtMemWordMutable &pmt1_mem_word0(int j0) { return pmt1_mem_word0_[j0]; }







  TmPrePmtMemWordMutable &pmt1_mem_word1(int j0) { return pmt1_mem_word1_[j0]; }







  TmPrePmtMemWordMutable &pmt1_mem_word2(int j0) { return pmt1_mem_word2_[j0]; }







  TmPrePmtMemWordMutable &pmt1_mem_word3(int j0) { return pmt1_mem_word3_[j0]; }

  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    if (offset < 0x66000) {
      offset -= 0x0;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      rdm_mem_word_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x100000 && offset < 0x104000) {
      offset -= 0x100000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      mit0_mem_word_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x200000 && offset < 0x204000) {
      offset -= 0x200000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      mit1_mem_word_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x300000 && offset < 0x304000) {
      offset -= 0x300000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      mit2_mem_word_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x400000 && offset < 0x404000) {
      offset -= 0x400000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      mit3_mem_word_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x40000000 && offset < 0x40000120) {
      offset -= 0x40000000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      pbt0_mem_word_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x40100000 && offset < 0x40100120) {
      offset -= 0x40100000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      pbt1_mem_word_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x40200000 && offset < 0x40200100) {
      offset -= 0x40200000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      lit0_np_mem_word_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x40300000 && offset < 0x40300100) {
      offset -= 0x40300000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      lit1_np_mem_word_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x40400000 && offset < 0x40400004) {
      offset -= 0x40400000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      fifo_mem_word_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x80000000 && offset < 0x80000100) {
      offset -= 0x80000000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      lit0_bm_mem_word0_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x80040000 && offset < 0x80040100) {
      offset -= 0x80040000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      lit0_bm_mem_word1_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x80080000 && offset < 0x80080100) {
      offset -= 0x80080000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      lit0_bm_mem_word2_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x800c0000 && offset < 0x800c0100) {
      offset -= 0x800c0000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      lit0_bm_mem_word3_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x80100000 && offset < 0x80100100) {
      offset -= 0x80100000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      lit1_bm_mem_word0_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x80140000 && offset < 0x80140100) {
      offset -= 0x80140000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      lit1_bm_mem_word1_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x80180000 && offset < 0x80180100) {
      offset -= 0x80180000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      lit1_bm_mem_word2_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x801c0000 && offset < 0x801c0100) {
      offset -= 0x801c0000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      lit1_bm_mem_word3_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x80200000 && offset < 0x80200120) {
      offset -= 0x80200000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      pmt0_mem_word0_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x80240000 && offset < 0x80240120) {
      offset -= 0x80240000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      pmt0_mem_word1_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x80280000 && offset < 0x80280120) {
      offset -= 0x80280000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      pmt0_mem_word2_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x802c0000 && offset < 0x802c0120) {
      offset -= 0x802c0000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      pmt0_mem_word3_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x80300000 && offset < 0x80300120) {
      offset -= 0x80300000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      pmt1_mem_word0_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x80340000 && offset < 0x80340120) {
      offset -= 0x80340000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      pmt1_mem_word1_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x80380000 && offset < 0x80380120) {
      offset -= 0x80380000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      pmt1_mem_word2_[ i0 ].read( offset, data0,data1,T );
    }
    else if (offset >= 0x803c0000 && offset < 0x803c0120) {
      offset -= 0x803c0000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      if (read_callback_) read_callback_();
      pmt1_mem_word3_[ i0 ].read( offset, data0,data1,T );
    }
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    if (offset < 0x66000) {
      offset -= 0x0;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      rdm_mem_word_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x100000 && offset < 0x104000) {
      offset -= 0x100000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      mit0_mem_word_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x200000 && offset < 0x204000) {
      offset -= 0x200000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      mit1_mem_word_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x300000 && offset < 0x304000) {
      offset -= 0x300000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      mit2_mem_word_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x400000 && offset < 0x404000) {
      offset -= 0x400000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      mit3_mem_word_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x40000000 && offset < 0x40000120) {
      offset -= 0x40000000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      pbt0_mem_word_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x40100000 && offset < 0x40100120) {
      offset -= 0x40100000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      pbt1_mem_word_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x40200000 && offset < 0x40200100) {
      offset -= 0x40200000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      lit0_np_mem_word_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x40300000 && offset < 0x40300100) {
      offset -= 0x40300000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      lit1_np_mem_word_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x40400000 && offset < 0x40400004) {
      offset -= 0x40400000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      fifo_mem_word_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x80000000 && offset < 0x80000100) {
      offset -= 0x80000000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      lit0_bm_mem_word0_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x80040000 && offset < 0x80040100) {
      offset -= 0x80040000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      lit0_bm_mem_word1_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x80080000 && offset < 0x80080100) {
      offset -= 0x80080000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      lit0_bm_mem_word2_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x800c0000 && offset < 0x800c0100) {
      offset -= 0x800c0000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      lit0_bm_mem_word3_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x80100000 && offset < 0x80100100) {
      offset -= 0x80100000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      lit1_bm_mem_word0_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x80140000 && offset < 0x80140100) {
      offset -= 0x80140000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      lit1_bm_mem_word1_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x80180000 && offset < 0x80180100) {
      offset -= 0x80180000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      lit1_bm_mem_word2_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x801c0000 && offset < 0x801c0100) {
      offset -= 0x801c0000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      lit1_bm_mem_word3_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x80200000 && offset < 0x80200120) {
      offset -= 0x80200000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      pmt0_mem_word0_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x80240000 && offset < 0x80240120) {
      offset -= 0x80240000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      pmt0_mem_word1_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x80280000 && offset < 0x80280120) {
      offset -= 0x80280000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      pmt0_mem_word2_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x802c0000 && offset < 0x802c0120) {
      offset -= 0x802c0000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      pmt0_mem_word3_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x80300000 && offset < 0x80300120) {
      offset -= 0x80300000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      pmt1_mem_word0_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x80340000 && offset < 0x80340120) {
      offset -= 0x80340000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      pmt1_mem_word1_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x80380000 && offset < 0x80380120) {
      offset -= 0x80380000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      pmt1_mem_word2_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x803c0000 && offset < 0x803c0120) {
      offset -= 0x803c0000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      pmt1_mem_word3_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    return true;
  }

  void reset(
      
      ) {
    for (auto &f0 : rdm_mem_word_) {
      f0.reset();
    }
    for (auto &f0 : mit0_mem_word_) {
      f0.reset();
    }
    for (auto &f0 : mit1_mem_word_) {
      f0.reset();
    }
    for (auto &f0 : mit2_mem_word_) {
      f0.reset();
    }
    for (auto &f0 : mit3_mem_word_) {
      f0.reset();
    }
    for (auto &f0 : pbt0_mem_word_) {
      f0.reset();
    }
    for (auto &f0 : pbt1_mem_word_) {
      f0.reset();
    }
    for (auto &f0 : lit0_np_mem_word_) {
      f0.reset();
    }
    for (auto &f0 : lit1_np_mem_word_) {
      f0.reset();
    }
    for (auto &f0 : fifo_mem_word_) {
      f0.reset();
    }
    for (auto &f0 : lit0_bm_mem_word0_) {
      f0.reset();
    }
    for (auto &f0 : lit0_bm_mem_word1_) {
      f0.reset();
    }
    for (auto &f0 : lit0_bm_mem_word2_) {
      f0.reset();
    }
    for (auto &f0 : lit0_bm_mem_word3_) {
      f0.reset();
    }
    for (auto &f0 : lit1_bm_mem_word0_) {
      f0.reset();
    }
    for (auto &f0 : lit1_bm_mem_word1_) {
      f0.reset();
    }
    for (auto &f0 : lit1_bm_mem_word2_) {
      f0.reset();
    }
    for (auto &f0 : lit1_bm_mem_word3_) {
      f0.reset();
    }
    for (auto &f0 : pmt0_mem_word0_) {
      f0.reset();
    }
    for (auto &f0 : pmt0_mem_word1_) {
      f0.reset();
    }
    for (auto &f0 : pmt0_mem_word2_) {
      f0.reset();
    }
    for (auto &f0 : pmt0_mem_word3_) {
      f0.reset();
    }
    for (auto &f0 : pmt1_mem_word0_) {
      f0.reset();
    }
    for (auto &f0 : pmt1_mem_word1_) {
      f0.reset();
    }
    for (auto &f0 : pmt1_mem_word2_) {
      f0.reset();
    }
    for (auto &f0 : pmt1_mem_word3_) {
      f0.reset();
    }
    if (write_callback_) write_callback_();
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    all_zeros=false;
    if (offset < 0x66000) {
      offset -= 0x0;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += rdm_mem_word_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x100000 && offset < 0x104000) {
      offset -= 0x100000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += mit0_mem_word_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x200000 && offset < 0x204000) {
      offset -= 0x200000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += mit1_mem_word_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x300000 && offset < 0x304000) {
      offset -= 0x300000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += mit2_mem_word_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x400000 && offset < 0x404000) {
      offset -= 0x400000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += mit3_mem_word_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x40000000 && offset < 0x40000120) {
      offset -= 0x40000000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += pbt0_mem_word_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x40100000 && offset < 0x40100120) {
      offset -= 0x40100000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += pbt1_mem_word_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x40200000 && offset < 0x40200100) {
      offset -= 0x40200000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += lit0_np_mem_word_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x40300000 && offset < 0x40300100) {
      offset -= 0x40300000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += lit1_np_mem_word_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x40400000 && offset < 0x40400004) {
      offset -= 0x40400000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += fifo_mem_word_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x80000000 && offset < 0x80000100) {
      offset -= 0x80000000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += lit0_bm_mem_word0_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x80040000 && offset < 0x80040100) {
      offset -= 0x80040000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += lit0_bm_mem_word1_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x80080000 && offset < 0x80080100) {
      offset -= 0x80080000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += lit0_bm_mem_word2_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x800c0000 && offset < 0x800c0100) {
      offset -= 0x800c0000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += lit0_bm_mem_word3_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x80100000 && offset < 0x80100100) {
      offset -= 0x80100000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += lit1_bm_mem_word0_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x80140000 && offset < 0x80140100) {
      offset -= 0x80140000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += lit1_bm_mem_word1_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x80180000 && offset < 0x80180100) {
      offset -= 0x80180000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += lit1_bm_mem_word2_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x801c0000 && offset < 0x801c0100) {
      offset -= 0x801c0000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += lit1_bm_mem_word3_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x80200000 && offset < 0x80200120) {
      offset -= 0x80200000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += pmt0_mem_word0_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x80240000 && offset < 0x80240120) {
      offset -= 0x80240000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += pmt0_mem_word1_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x80280000 && offset < 0x80280120) {
      offset -= 0x80280000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += pmt0_mem_word2_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x802c0000 && offset < 0x802c0120) {
      offset -= 0x802c0000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += pmt0_mem_word3_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x80300000 && offset < 0x80300120) {
      offset -= 0x80300000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += pmt1_mem_word0_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x80340000 && offset < 0x80340120) {
      offset -= 0x80340000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += pmt1_mem_word1_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x80380000 && offset < 0x80380120) {
      offset -= 0x80380000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += pmt1_mem_word2_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x803c0000 && offset < 0x803c0120) {
      offset -= 0x803c0000;
      int i0 = offset / 0x1;
      offset  -= i0 * 0x1;
      r += pmt1_mem_word3_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
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
    all_zeros=false;
    for (uint32_t a0=0;a0<417792;++a0) {
      r += rdm_mem_word_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<16384;++a0) {
      r += mit0_mem_word_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<16384;++a0) {
      r += mit1_mem_word_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<16384;++a0) {
      r += mit2_mem_word_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<16384;++a0) {
      r += mit3_mem_word_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<288;++a0) {
      r += pbt0_mem_word_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<288;++a0) {
      r += pbt1_mem_word_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<256;++a0) {
      r += lit0_np_mem_word_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<256;++a0) {
      r += lit1_np_mem_word_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<4;++a0) {
      r += fifo_mem_word_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<256;++a0) {
      r += lit0_bm_mem_word0_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<256;++a0) {
      r += lit0_bm_mem_word1_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<256;++a0) {
      r += lit0_bm_mem_word2_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<256;++a0) {
      r += lit0_bm_mem_word3_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<256;++a0) {
      r += lit1_bm_mem_word0_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<256;++a0) {
      r += lit1_bm_mem_word1_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<256;++a0) {
      r += lit1_bm_mem_word2_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<256;++a0) {
      r += lit1_bm_mem_word3_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<288;++a0) {
      r += pmt0_mem_word0_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<288;++a0) {
      r += pmt0_mem_word1_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<288;++a0) {
      r += pmt0_mem_word2_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<288;++a0) {
      r += pmt0_mem_word3_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<288;++a0) {
      r += pmt1_mem_word0_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<288;++a0) {
      r += pmt1_mem_word1_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<288;++a0) {
      r += pmt1_mem_word2_[a0].to_string(print_zeros,indent_string) ;
    }
    for (uint32_t a0=0;a0<288;++a0) {
      r += pmt1_mem_word3_[a0].to_string(print_zeros,indent_string) ;
    }
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  std::array< TmPreRdmMemWordMutable, 417792 > rdm_mem_word_;
  std::array< TmPreMitMemWordMutable, 16384 > mit0_mem_word_;
  std::array< TmPreMitMemWordMutable, 16384 > mit1_mem_word_;
  std::array< TmPreMitMemWordMutable, 16384 > mit2_mem_word_;
  std::array< TmPreMitMemWordMutable, 16384 > mit3_mem_word_;
  std::array< TmPrePbtMemWordMutable, 288 > pbt0_mem_word_;
  std::array< TmPrePbtMemWordMutable, 288 > pbt1_mem_word_;
  std::array< TmPreLitNpMemWordMutable, 256 > lit0_np_mem_word_;
  std::array< TmPreLitNpMemWordMutable, 256 > lit1_np_mem_word_;
  std::array< TmPreFifoMemWordMutable, 4 > fifo_mem_word_;
  std::array< TmPreLitBmMemWordMutable, 256 > lit0_bm_mem_word0_;
  std::array< TmPreLitBmMemWordMutable, 256 > lit0_bm_mem_word1_;
  std::array< TmPreLitBmMemWordMutable, 256 > lit0_bm_mem_word2_;
  std::array< TmPreLitBmMemWordMutable, 256 > lit0_bm_mem_word3_;
  std::array< TmPreLitBmMemWordMutable, 256 > lit1_bm_mem_word0_;
  std::array< TmPreLitBmMemWordMutable, 256 > lit1_bm_mem_word1_;
  std::array< TmPreLitBmMemWordMutable, 256 > lit1_bm_mem_word2_;
  std::array< TmPreLitBmMemWordMutable, 256 > lit1_bm_mem_word3_;
  std::array< TmPrePmtMemWordMutable, 288 > pmt0_mem_word0_;
  std::array< TmPrePmtMemWordMutable, 288 > pmt0_mem_word1_;
  std::array< TmPrePmtMemWordMutable, 288 > pmt0_mem_word2_;
  std::array< TmPrePmtMemWordMutable, 288 > pmt0_mem_word3_;
  std::array< TmPrePmtMemWordMutable, 288 > pmt1_mem_word0_;
  std::array< TmPrePmtMemWordMutable, 288 > pmt1_mem_word1_;
  std::array< TmPrePmtMemWordMutable, 288 > pmt1_mem_word2_;
  std::array< TmPrePmtMemWordMutable, 288 > pmt1_mem_word3_;
private:
  static uint64_t StartOffset(
      
      ) {
    uint64_t offset=0;
    offset += 0x6100000000; // to get to tm_pre
    return offset;
  }

};








class Chip : public model_core::RegisterBlockIndirect<RegisterCallback> {
public:
  Chip(
      int chipNumber, RegisterCallback& write_callback = 0, RegisterCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(), 2748779069440, false, write_callback, read_callback, std::string("Chip"))
    {
    }
  Chip(
      
  )
    : RegisterBlockIndirect(0, 0, 0, false, 0, 0, "Chip")
    {
    }
public:





  TmPreMemRspec &tm_pre() { return tm_pre_; }







  PipeAddrmap &pipes(int j0) { return pipes_[j0]; }

  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    if (offset >= 0x6100000000 && offset < 0x6200000000) {
      offset -= 0x6100000000;
      if (read_callback_) read_callback_();
      tm_pre_.read( offset, data0,data1,T );
    }
    else if (offset >= 0x20000000000 && offset < 0x27200190000) {
      offset -= 0x20000000000;
      int i0 = offset / 0x2000000000;
      offset  -= i0 * 0x2000000000;
      if (read_callback_) read_callback_();
      pipes_[ i0 ].read( offset, data0,data1,T );
    }
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    if (offset >= 0x6100000000 && offset < 0x6200000000) {
      offset -= 0x6100000000;
      tm_pre_.write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x20000000000 && offset < 0x27200190000) {
      offset -= 0x20000000000;
      int i0 = offset / 0x2000000000;
      offset  -= i0 * 0x2000000000;
      pipes_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    return true;
  }

  void reset(
      
      ) {
    tm_pre_.reset();
    for (auto &f0 : pipes_) {
      f0.reset();
    }
    if (write_callback_) write_callback_();
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    all_zeros=false;
    if (offset >= 0x6100000000 && offset < 0x6200000000) {
      offset -= 0x6100000000;
      r += tm_pre_.to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x20000000000 && offset < 0x27200190000) {
      offset -= 0x20000000000;
      int i0 = offset / 0x2000000000;
      offset  -= i0 * 0x2000000000;
      r += pipes_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
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
    all_zeros=false;
    r += tm_pre_.to_string(print_zeros,indent_string) ;
    for (uint32_t a0=0;a0<4;++a0) {
      r += pipes_[a0].to_string(print_zeros,indent_string) ;
    }
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  TmPreMemRspec tm_pre_;
  std::array< PipeAddrmap, 4 > pipes_;
private:
  static uint64_t StartOffset(
      
      ) {
    uint64_t offset=0;
    return offset;
  }

};








class ChipMutable : public model_core::RegisterBlockIndirect<RegisterCallback> {
public:
  ChipMutable(
      int chipNumber, RegisterCallback& write_callback = 0, RegisterCallback& read_callback = 0
  )
    : RegisterBlockIndirect(chipNumber, StartOffset(), 2748779069440, true, write_callback, read_callback, std::string("ChipMutable"))
    {
    }
  ChipMutable(
      
  )
    : RegisterBlockIndirect(0, 0, 0, true, 0, 0, "ChipMutable")
    {
    }
public:





  TmPreMemRspecMutable &tm_pre() { return tm_pre_; }







  PipeAddrmapMutable &pipes(int j0) { return pipes_[j0]; }

  bool read(
      uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T
      ) const {
    if (offset >= 0x6100000000 && offset < 0x6200000000) {
      offset -= 0x6100000000;
      if (read_callback_) read_callback_();
      tm_pre_.read( offset, data0,data1,T );
    }
    else if (offset >= 0x20000000000 && offset < 0x27200190000) {
      offset -= 0x20000000000;
      int i0 = offset / 0x2000000000;
      offset  -= i0 * 0x2000000000;
      if (read_callback_) read_callback_();
      pipes_[ i0 ].read( offset, data0,data1,T );
    }
    return true;
  }


  bool write(
      uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T
      ) {
    if (offset >= 0x6100000000 && offset < 0x6200000000) {
      offset -= 0x6100000000;
      tm_pre_.write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    else if (offset >= 0x20000000000 && offset < 0x27200190000) {
      offset -= 0x20000000000;
      int i0 = offset / 0x2000000000;
      offset  -= i0 * 0x2000000000;
      pipes_[ i0 ].write( offset, data0,data1,T );
      if (write_callback_) write_callback_();
    }
    return true;
  }

  void reset(
      
      ) {
    tm_pre_.reset();
    for (auto &f0 : pipes_) {
      f0.reset();
    }
    if (write_callback_) write_callback_();
  }

  std::string to_string(
      uint64_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    all_zeros=false;
    if (offset >= 0x6100000000 && offset < 0x6200000000) {
      offset -= 0x6100000000;
      r += tm_pre_.to_string(offset,print_zeros,indent_string) ;
    }
    else if (offset >= 0x20000000000 && offset < 0x27200190000) {
      offset -= 0x20000000000;
      int i0 = offset / 0x2000000000;
      offset  -= i0 * 0x2000000000;
      r += pipes_[ i0 ].to_string(offset,print_zeros,indent_string) ;
    }
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
    all_zeros=false;
    r += tm_pre_.to_string(print_zeros,indent_string) ;
    for (uint32_t a0=0;a0<4;++a0) {
      r += pipes_[a0].to_string(print_zeros,indent_string) ;
    }
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  TmPreMemRspecMutable tm_pre_;
  std::array< PipeAddrmapMutable, 4 > pipes_;
private:
  static uint64_t StartOffset(
      
      ) {
    uint64_t offset=0;
    return offset;
  }

};
  }; // namespace memory_classes
}; // namespace tofino
#endif // #ifndef ___TOFINO_REGISTER_INCLUDES_MODEL_MEM_H__
