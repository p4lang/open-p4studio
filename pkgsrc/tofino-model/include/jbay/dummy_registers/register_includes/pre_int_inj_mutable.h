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

// Generated using:
//    ./tools/make_dummy_register.py --chip_name=jbay include/tofino/register_includes/pre_int_inj_mutable.h

#ifndef __REGISTER_INCLUDES_PRE_INT_INJ_MUTABLE_H__
#define __REGISTER_INCLUDES_PRE_INT_INJ_MUTABLE_H__


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

class PreIntInjMutable : public model_core::DummyRegisterBlock<RegisterCallback> {
public:
  PreIntInjMutable(
      int chipNumber, int index_tm_pre_pipe_rspec, RegisterCallback& write_callback = 0, RegisterCallback& read_callback = 0
  )
    : DummyRegisterBlock()
    {
    }
  PreIntInjMutable(
      
  )
    : DummyRegisterBlock()
    {
    }
public:





  uint16_t err_inj_reserved() { assert(0); return 0; }
  void err_inj_reserved(const uint16_t &v) { assert(0); }





  uint8_t fifo_mbe_inj() { assert(0); return 0; }
  void fifo_mbe_inj(const uint8_t &v) { assert(0); }





  uint8_t mit_mbe_inj() { assert(0); return 0; }
  void mit_mbe_inj(const uint8_t &v) { assert(0); }





  uint8_t lit0_bm_mbe_inj() { assert(0); return 0; }
  void lit0_bm_mbe_inj(const uint8_t &v) { assert(0); }





  uint8_t lit1_bm_mbe_inj() { assert(0); return 0; }
  void lit1_bm_mbe_inj(const uint8_t &v) { assert(0); }





  uint8_t lit0_np_mbe_inj() { assert(0); return 0; }
  void lit0_np_mbe_inj(const uint8_t &v) { assert(0); }





  uint8_t lit1_np_mbe_inj() { assert(0); return 0; }
  void lit1_np_mbe_inj(const uint8_t &v) { assert(0); }





  uint8_t pmt0_mbe_inj() { assert(0); return 0; }
  void pmt0_mbe_inj(const uint8_t &v) { assert(0); }





  uint8_t pmt1_mbe_inj() { assert(0); return 0; }
  void pmt1_mbe_inj(const uint8_t &v) { assert(0); }





  uint8_t rdm_mbe_inj() { assert(0); return 0; }
  void rdm_mbe_inj(const uint8_t &v) { assert(0); }





  uint8_t fifo_sbe_inj() { assert(0); return 0; }
  void fifo_sbe_inj(const uint8_t &v) { assert(0); }





  uint8_t mit_sbe_inj() { assert(0); return 0; }
  void mit_sbe_inj(const uint8_t &v) { assert(0); }





  uint8_t lit0_bm_sbe_inj() { assert(0); return 0; }
  void lit0_bm_sbe_inj(const uint8_t &v) { assert(0); }





  uint8_t lit1_bm_sbe_inj() { assert(0); return 0; }
  void lit1_bm_sbe_inj(const uint8_t &v) { assert(0); }





  uint8_t lit0_np_sbe_inj() { assert(0); return 0; }
  void lit0_np_sbe_inj(const uint8_t &v) { assert(0); }





  uint8_t lit1_np_sbe_inj() { assert(0); return 0; }
  void lit1_np_sbe_inj(const uint8_t &v) { assert(0); }





  uint8_t pmt0_sbe_inj() { assert(0); return 0; }
  void pmt0_sbe_inj(const uint8_t &v) { assert(0); }





  uint8_t pmt1_sbe_inj() { assert(0); return 0; }
  void pmt1_sbe_inj(const uint8_t &v) { assert(0); }





  uint8_t rdm_sbe_inj() { assert(0); return 0; }
  void rdm_sbe_inj(const uint8_t &v) { assert(0); }

  bool read(
      uint32_t offset, uint32_t* data
      ) const {
    if (read_callback_) read_callback_();
    *data = (err_inj_reserved_ & 0x3fff);
    *data |= ((fifo_mbe_inj_ & 0x1) << 14);
    *data |= ((mit_mbe_inj_ & 0x1) << 15);
    *data |= ((lit0_bm_mbe_inj_ & 0x1) << 16);
    *data |= ((lit1_bm_mbe_inj_ & 0x1) << 17);
    *data |= ((lit0_np_mbe_inj_ & 0x1) << 18);
    *data |= ((lit1_np_mbe_inj_ & 0x1) << 19);
    *data |= ((pmt0_mbe_inj_ & 0x1) << 20);
    *data |= ((pmt1_mbe_inj_ & 0x1) << 21);
    *data |= ((rdm_mbe_inj_ & 0x1) << 22);
    *data |= ((fifo_sbe_inj_ & 0x1) << 23);
    *data |= ((mit_sbe_inj_ & 0x1) << 24);
    *data |= ((lit0_bm_sbe_inj_ & 0x1) << 25);
    *data |= ((lit1_bm_sbe_inj_ & 0x1) << 26);
    *data |= ((lit0_np_sbe_inj_ & 0x1) << 27);
    *data |= ((lit1_np_sbe_inj_ & 0x1) << 28);
    *data |= ((pmt0_sbe_inj_ & 0x1) << 29);
    *data |= ((pmt1_sbe_inj_ & 0x1) << 30);
    *data |= ((rdm_sbe_inj_ & 0x1) << 31);
    return true;
  }


  bool write(
      uint32_t offset, uint32_t data
      ) {
    err_inj_reserved_ = (data & 0x3fff);
    fifo_mbe_inj_ = ((data >> 14) & 0x1);
    mit_mbe_inj_ = ((data >> 15) & 0x1);
    lit0_bm_mbe_inj_ = ((data >> 16) & 0x1);
    lit1_bm_mbe_inj_ = ((data >> 17) & 0x1);
    lit0_np_mbe_inj_ = ((data >> 18) & 0x1);
    lit1_np_mbe_inj_ = ((data >> 19) & 0x1);
    pmt0_mbe_inj_ = ((data >> 20) & 0x1);
    pmt1_mbe_inj_ = ((data >> 21) & 0x1);
    rdm_mbe_inj_ = ((data >> 22) & 0x1);
    fifo_sbe_inj_ = ((data >> 23) & 0x1);
    mit_sbe_inj_ = ((data >> 24) & 0x1);
    lit0_bm_sbe_inj_ = ((data >> 25) & 0x1);
    lit1_bm_sbe_inj_ = ((data >> 26) & 0x1);
    lit0_np_sbe_inj_ = ((data >> 27) & 0x1);
    lit1_np_sbe_inj_ = ((data >> 28) & 0x1);
    pmt0_sbe_inj_ = ((data >> 29) & 0x1);
    pmt1_sbe_inj_ = ((data >> 30) & 0x1);
    rdm_sbe_inj_ = ((data >> 31) & 0x1);
    if (write_callback_) write_callback_();
    return true;
  }

  void reset(
      
      ) {
    err_inj_reserved_ = 0x0;
    fifo_mbe_inj_ = 0x0;
    mit_mbe_inj_ = 0x0;
    lit0_bm_mbe_inj_ = 0x0;
    lit1_bm_mbe_inj_ = 0x0;
    lit0_np_mbe_inj_ = 0x0;
    lit1_np_mbe_inj_ = 0x0;
    pmt0_mbe_inj_ = 0x0;
    pmt1_mbe_inj_ = 0x0;
    rdm_mbe_inj_ = 0x0;
    fifo_sbe_inj_ = 0x0;
    mit_sbe_inj_ = 0x0;
    lit0_bm_sbe_inj_ = 0x0;
    lit1_bm_sbe_inj_ = 0x0;
    lit0_np_sbe_inj_ = 0x0;
    lit1_np_sbe_inj_ = 0x0;
    pmt0_sbe_inj_ = 0x0;
    pmt1_sbe_inj_ = 0x0;
    rdm_sbe_inj_ = 0x0;
    if (write_callback_) write_callback_();
  }

  std::string to_string(
      uint32_t offset, bool print_zeros = false, std::string indent_string = ""
      ) const {
    std::string r("");
    bool all_zeros=true;
    r += indent_string + std::string("PreIntInjMutable") + ":\n";
    r += indent_string + "  " + std::string("err_inj_reserved") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(err_inj_reserved_) ) + "\n";
    all_zeros &= (0 == err_inj_reserved_);
    r += indent_string + "  " + std::string("fifo_mbe_inj") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(fifo_mbe_inj_) ) + "\n";
    all_zeros &= (0 == fifo_mbe_inj_);
    r += indent_string + "  " + std::string("mit_mbe_inj") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(mit_mbe_inj_) ) + "\n";
    all_zeros &= (0 == mit_mbe_inj_);
    r += indent_string + "  " + std::string("lit0_bm_mbe_inj") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(lit0_bm_mbe_inj_) ) + "\n";
    all_zeros &= (0 == lit0_bm_mbe_inj_);
    r += indent_string + "  " + std::string("lit1_bm_mbe_inj") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(lit1_bm_mbe_inj_) ) + "\n";
    all_zeros &= (0 == lit1_bm_mbe_inj_);
    r += indent_string + "  " + std::string("lit0_np_mbe_inj") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(lit0_np_mbe_inj_) ) + "\n";
    all_zeros &= (0 == lit0_np_mbe_inj_);
    r += indent_string + "  " + std::string("lit1_np_mbe_inj") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(lit1_np_mbe_inj_) ) + "\n";
    all_zeros &= (0 == lit1_np_mbe_inj_);
    r += indent_string + "  " + std::string("pmt0_mbe_inj") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(pmt0_mbe_inj_) ) + "\n";
    all_zeros &= (0 == pmt0_mbe_inj_);
    r += indent_string + "  " + std::string("pmt1_mbe_inj") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(pmt1_mbe_inj_) ) + "\n";
    all_zeros &= (0 == pmt1_mbe_inj_);
    r += indent_string + "  " + std::string("rdm_mbe_inj") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(rdm_mbe_inj_) ) + "\n";
    all_zeros &= (0 == rdm_mbe_inj_);
    r += indent_string + "  " + std::string("fifo_sbe_inj") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(fifo_sbe_inj_) ) + "\n";
    all_zeros &= (0 == fifo_sbe_inj_);
    r += indent_string + "  " + std::string("mit_sbe_inj") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(mit_sbe_inj_) ) + "\n";
    all_zeros &= (0 == mit_sbe_inj_);
    r += indent_string + "  " + std::string("lit0_bm_sbe_inj") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(lit0_bm_sbe_inj_) ) + "\n";
    all_zeros &= (0 == lit0_bm_sbe_inj_);
    r += indent_string + "  " + std::string("lit1_bm_sbe_inj") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(lit1_bm_sbe_inj_) ) + "\n";
    all_zeros &= (0 == lit1_bm_sbe_inj_);
    r += indent_string + "  " + std::string("lit0_np_sbe_inj") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(lit0_np_sbe_inj_) ) + "\n";
    all_zeros &= (0 == lit0_np_sbe_inj_);
    r += indent_string + "  " + std::string("lit1_np_sbe_inj") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(lit1_np_sbe_inj_) ) + "\n";
    all_zeros &= (0 == lit1_np_sbe_inj_);
    r += indent_string + "  " + std::string("pmt0_sbe_inj") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(pmt0_sbe_inj_) ) + "\n";
    all_zeros &= (0 == pmt0_sbe_inj_);
    r += indent_string + "  " + std::string("pmt1_sbe_inj") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(pmt1_sbe_inj_) ) + "\n";
    all_zeros &= (0 == pmt1_sbe_inj_);
    r += indent_string + "  " + std::string("rdm_sbe_inj") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(rdm_sbe_inj_) ) + "\n";
    all_zeros &= (0 == rdm_sbe_inj_);
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
    r += indent_string + std::string("PreIntInjMutable") + ":\n";
    r += indent_string + "  " + std::string("err_inj_reserved") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(err_inj_reserved_) ) + "\n";
    all_zeros &= (0 == err_inj_reserved_);
    r += indent_string + "  " + std::string("fifo_mbe_inj") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(fifo_mbe_inj_) ) + "\n";
    all_zeros &= (0 == fifo_mbe_inj_);
    r += indent_string + "  " + std::string("mit_mbe_inj") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(mit_mbe_inj_) ) + "\n";
    all_zeros &= (0 == mit_mbe_inj_);
    r += indent_string + "  " + std::string("lit0_bm_mbe_inj") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(lit0_bm_mbe_inj_) ) + "\n";
    all_zeros &= (0 == lit0_bm_mbe_inj_);
    r += indent_string + "  " + std::string("lit1_bm_mbe_inj") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(lit1_bm_mbe_inj_) ) + "\n";
    all_zeros &= (0 == lit1_bm_mbe_inj_);
    r += indent_string + "  " + std::string("lit0_np_mbe_inj") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(lit0_np_mbe_inj_) ) + "\n";
    all_zeros &= (0 == lit0_np_mbe_inj_);
    r += indent_string + "  " + std::string("lit1_np_mbe_inj") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(lit1_np_mbe_inj_) ) + "\n";
    all_zeros &= (0 == lit1_np_mbe_inj_);
    r += indent_string + "  " + std::string("pmt0_mbe_inj") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(pmt0_mbe_inj_) ) + "\n";
    all_zeros &= (0 == pmt0_mbe_inj_);
    r += indent_string + "  " + std::string("pmt1_mbe_inj") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(pmt1_mbe_inj_) ) + "\n";
    all_zeros &= (0 == pmt1_mbe_inj_);
    r += indent_string + "  " + std::string("rdm_mbe_inj") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(rdm_mbe_inj_) ) + "\n";
    all_zeros &= (0 == rdm_mbe_inj_);
    r += indent_string + "  " + std::string("fifo_sbe_inj") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(fifo_sbe_inj_) ) + "\n";
    all_zeros &= (0 == fifo_sbe_inj_);
    r += indent_string + "  " + std::string("mit_sbe_inj") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(mit_sbe_inj_) ) + "\n";
    all_zeros &= (0 == mit_sbe_inj_);
    r += indent_string + "  " + std::string("lit0_bm_sbe_inj") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(lit0_bm_sbe_inj_) ) + "\n";
    all_zeros &= (0 == lit0_bm_sbe_inj_);
    r += indent_string + "  " + std::string("lit1_bm_sbe_inj") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(lit1_bm_sbe_inj_) ) + "\n";
    all_zeros &= (0 == lit1_bm_sbe_inj_);
    r += indent_string + "  " + std::string("lit0_np_sbe_inj") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(lit0_np_sbe_inj_) ) + "\n";
    all_zeros &= (0 == lit0_np_sbe_inj_);
    r += indent_string + "  " + std::string("lit1_np_sbe_inj") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(lit1_np_sbe_inj_) ) + "\n";
    all_zeros &= (0 == lit1_np_sbe_inj_);
    r += indent_string + "  " + std::string("pmt0_sbe_inj") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(pmt0_sbe_inj_) ) + "\n";
    all_zeros &= (0 == pmt0_sbe_inj_);
    r += indent_string + "  " + std::string("pmt1_sbe_inj") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(pmt1_sbe_inj_) ) + "\n";
    all_zeros &= (0 == pmt1_sbe_inj_);
    r += indent_string + "  " + std::string("rdm_sbe_inj") +  ": 0x" + boost::str( boost::format("%x") % static_cast<uint>(rdm_sbe_inj_) ) + "\n";
    all_zeros &= (0 == rdm_sbe_inj_);
    if (all_zeros && !print_zeros) {
      return("");
    }
    else {
      return r;
    }
  }

private:
  uint16_t err_inj_reserved_;
  uint8_t fifo_mbe_inj_;
  uint8_t mit_mbe_inj_;
  uint8_t lit0_bm_mbe_inj_;
  uint8_t lit1_bm_mbe_inj_;
  uint8_t lit0_np_mbe_inj_;
  uint8_t lit1_np_mbe_inj_;
  uint8_t pmt0_mbe_inj_;
  uint8_t pmt1_mbe_inj_;
  uint8_t rdm_mbe_inj_;
  uint8_t fifo_sbe_inj_;
  uint8_t mit_sbe_inj_;
  uint8_t lit0_bm_sbe_inj_;
  uint8_t lit1_bm_sbe_inj_;
  uint8_t lit0_np_sbe_inj_;
  uint8_t lit1_np_sbe_inj_;
  uint8_t pmt0_sbe_inj_;
  uint8_t pmt1_sbe_inj_;
  uint8_t rdm_sbe_inj_;
private:
  static int StartOffset(
      int index_tm_pre_pipe_rspec
      ) {
    int offset=0;
    offset += 0x400000; // to get to tm_top
    offset += 0x200000; // to get to tm_pre_top
    assert(index_tm_pre_pipe_rspec < 4);
    offset += index_tm_pre_pipe_rspec * 0x8000; // tm_pre_pipe_rspec[]
    offset += 0x5c; // to get to int_inj
    return offset;
  }

};









  }; // namespace register_classes
}; // namespace jbay

#endif // __REGISTER_INCLUDES_PRE_INT_INJ_MUTABLE_H__
