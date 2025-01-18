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

#ifndef _UTESTS_REGISTER_MAPPER_
#define _UTESTS_REGISTER_MAPPER_

#include <cinttypes>
#include <string>
#include <vector>
#include <unordered_map>
#include <boost/optional/optional.hpp>
#include <boost/fusion/container.hpp>
#include <boost/mpl/int.hpp>

typedef boost::fusion::vector2< std::string,boost::optional<std::vector<int> > > PathElement;

typedef void* RegPtr;

struct RegisterMapper;
typedef RegisterMapper* RegisterMapperPtr;

struct MapElement {
  RegPtr      reg_ptr_;
  RegisterMapperPtr   next_;
  std::vector<int> array_dims_;
  size_t      size_;
};

struct RegisterMapper {
  
  RegisterMapper( std::unordered_map<std::string,MapElement> m ) : map_(m) {}
  ~RegisterMapper() {
    for (auto it = map_.begin(); it!=map_.end(); ++it)  {
      delete it->second.next_;
    }
    
    map_.clear();
  }
  std::unordered_map<std::string,MapElement> map_;

  RegPtr map(std::vector<PathElement> s, int index=0) {
    intptr_t array_offset = 0;
    std::string name{ boost::fusion::at<boost::mpl::int_<0>>(s[index]) };
    MapElement& this_el = map_[ name ];
    int size = this_el.size_;
    auto path_element_array = boost::fusion::at<boost::mpl::int_<1>>( s[index] );
    for (int i= this_el.array_dims_.size()-1; i >= 0; --i) {
      assert( path_element_array );
      //printf("%d %d %d\n",i,(*path_element_array)[i],this_el.array_dims_[i]);
      array_offset += size * (*path_element_array)[i];
      size *= this_el.array_dims_[i];
    }
    if ( this_el.next_ ) {
      return reinterpret_cast<void*> ( array_offset +
                                       reinterpret_cast<intptr_t>(this_el.reg_ptr_) +
                                       reinterpret_cast<intptr_t>(this_el.next_->map(s,index+1)) );
    }
    else {
      return reinterpret_cast<void*> ( array_offset +
                                       reinterpret_cast<intptr_t>(this_el.reg_ptr_) );
    }
  }
};

#endif // _UTESTS_REGISTER_MAPPER_
