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

#include <common/rmt-assert.h>
#include <model_core/register_block.h>
#include <model_core/model.h>
//#include <rmt-log.h>

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;

namespace model_core {
  
RegisterBlockBase::RegisterBlockBase(int chip, int offset, int size, bool isMutable, std::string name) :
    name_(name)
{
  ::GLOBAL_MODEL->Subscribe(this, chip,offset,size,isMutable,name);
}

RegisterBlockIndirectBase::RegisterBlockIndirectBase(int chip, uint64_t offset, int64_t size,
                                                     bool isMutable,std::string name):
    name_(name)
{
  ::GLOBAL_MODEL->IndirectSubscribe(this, chip,offset,size,isMutable,name);
}

bool RegisterBlockBase::CheckArrayBoundsInternal(uint32_t index,uint32_t real_size,uint32_t size,uint64_t offset,
                                                 uint32_t which, const std::string& name,bool warn_on_real)
{
  if ( index < real_size ) {
    return true;
  }
  else if ( index < size ) {
    if (warn_on_real) {
      // TODO: these functions are too low level to use
      //  the logging functions, but must be something
      //  better than printf?
      //printf( "Access to non-existent part of array range %d of %s: index=%d, used size=%d\n",
      //        which,name.c_str(),index,real_size);
    }
    else {
      printf( "Access to non-existent part of array range %d of %s: index=%d, used size=%d\n",
                 which,name.c_str(),index,real_size);
      RMT_ASSERT(0);
    }
  }
  else {
    printf( "Access past end of array range %d of %s: index=%d, size=%d",which,name.c_str(),
               index,size);
    RMT_ASSERT(0);
  }
  return false;
}


}
