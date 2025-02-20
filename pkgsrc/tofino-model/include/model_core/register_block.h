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

#ifndef _MODEL_CORE_REGISTER_BLOCK_H_
#define _MODEL_CORE_REGISTER_BLOCK_H_

#include <memory>
#include <string>
#include <cassert>
#include <model_core/register_callback.h>
#include <common/disallow-copy-and-assign.h>

namespace model_core {

class RegisterBlockBase {
public:
  RegisterBlockBase(int chip, int offset, int size, bool isMutable, std::string name);
  virtual ~RegisterBlockBase() {}

  virtual bool read(uint32_t offset,uint32_t* data) const =0;
  virtual bool write(uint32_t offset, uint32_t data) =0;
  virtual std::string to_string(bool print_zeros = false, std::string indent_string = "") const =0;
  virtual std::string to_string(uint32_t offset,bool print_zeros = false, std::string indent_string = "") const =0;

  // warn_on_real will just warn if the index is over the real_size but still under the size.
  //   This is for non-power-of-2 multidimensional arrays where the real may be smaller than
  //   the size (which has to be a power of 2), and where writes might occur to the
  //   non-implemented pieces because, for instance, DMA is being used.
  bool CheckArrayBounds(uint32_t index,uint32_t real_size,uint32_t size,uint32_t offset,uint32_t which,
                        bool warn_on_real) const
  {
    return CheckArrayBoundsInternal(index,real_size,size,offset,which,name_,warn_on_real);
  }

  // can be used by the indirect block too
  static bool CheckArrayBoundsInternal(uint32_t index,uint32_t real_size,uint32_t size,uint64_t offset,
                                       uint32_t which, const std::string& name, bool warn_on_real);
private:
  std::string name_;
};

template <typename CB_TYPE>
class RegisterBlock : public RegisterBlockBase {
public:
  RegisterBlock(int chip, uint32_t offset, int size,bool isMutable,
                CB_TYPE write_callback, CB_TYPE read_callback,std::string name)
    : RegisterBlockBase(chip,offset,size,isMutable,name),
      write_callback_(write_callback),
      read_callback_(read_callback)
  {
  }
  virtual ~RegisterBlock() {}


  // The register adapters depend on the registers being constructed in place
  //  using NRVO (named return value optimization) so they are never moved,
  //  because they subscribe with the chip interface using their address and
  //  there is currently no code to de-subscribe and re-subscribe with a
  //  different address.
  // But in order for the register adapter code to be legal there must be
  //  a move constructor! So we must make sure that the move constructor
  //  is never used - ie the move is always optimized out using NRVO.
  // This was only a problem in one strange case where a register was
  //  being created using new and only in gcc5 (gcc4 and gcc6 were ok)
  //
  RegisterBlock(RegisterBlock<CB_TYPE>&&) // move constructor
      : RegisterBlockBase(0,0,0,false,"")
  {
    assert(0);  // don't want this running!
  }

protected:
  CB_TYPE write_callback_;
  CB_TYPE read_callback_;

 private:
  DISALLOW_COPY_AND_ASSIGN(RegisterBlock<CB_TYPE>);
};


class RegisterBlockIndirectBase {
public:
  RegisterBlockIndirectBase(int chip, uint64_t offset, int64_t size,
                            bool isMutable,std::string name);
  virtual ~RegisterBlockIndirectBase() {}

  virtual bool read(uint64_t offset,uint64_t* data0,uint64_t* data1,uint64_t T) const =0;
  virtual bool write(uint64_t offset,uint64_t data0,uint64_t data1,uint64_t T) =0;
  virtual std::string to_string(bool print_zeros = false, std::string indent_string = "") const =0;
  virtual std::string to_string(uint64_t offset,bool print_zeros = false, std::string indent_string = "") const =0;

  bool CheckArrayBounds(uint32_t index,uint32_t real_size,uint32_t size,uint64_t offset,uint32_t which,
                        bool warn_on_real) const
  {
    return RegisterBlockBase::CheckArrayBoundsInternal(index,real_size,size,offset,which,name_,warn_on_real);
  }
private:
  std::string name_;
};

template <typename CB_TYPE>
class RegisterBlockIndirect : public RegisterBlockIndirectBase {
public:
  RegisterBlockIndirect(int chip, uint64_t offset, int64_t size, bool isMutable,
                        CB_TYPE write_callback, CB_TYPE read_callback,std::string name)
      : RegisterBlockIndirectBase(chip,offset,size,isMutable,name),
      write_callback_(write_callback),
      read_callback_(read_callback)
  {
  }
  virtual ~RegisterBlockIndirect() {}

protected:
  CB_TYPE write_callback_;
  CB_TYPE read_callback_;
};


// this is used for dummy registers which are useful when a register
//  has been deleted in a new chip and we still want the code to link
template <typename CB_TYPE>
class DummyRegisterBlock {
public:
  DummyRegisterBlock(CB_TYPE write_callback = 0, CB_TYPE read_callback = 0) :
      write_callback_(write_callback),
      read_callback_(read_callback)
  {
  }
  virtual ~DummyRegisterBlock() {}

  bool CheckArrayBounds(uint32_t ,uint32_t ,uint32_t ,uint32_t ,uint32_t , bool ) const {
    return false;
  }

protected:
  CB_TYPE write_callback_;
  CB_TYPE read_callback_;
};

}

#endif // _MODEL_CORE_REGISTER_BLOCK_H_
