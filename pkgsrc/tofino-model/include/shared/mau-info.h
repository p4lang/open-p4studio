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

#ifndef _SHARED_MAU_INFO_
#define _SHARED_MAU_INFO_

#include <unordered_map>
#include <mau-info-defs.h>


namespace MODEL_CHIP_NAMESPACE {

  class RmtObjectManager;

  class MauInfo {

    static int                                        static_init();
    static std::vector<const char*>                  *static_string_vec_;
    static const std::unordered_map<std::string,int>  static_string_map_;

 public:
    MauInfo(RmtObjectManager *om, int pipeIndex, int mauIndex, Mau *mau)
        : mau_(mau), size_(static_init()), info_(size_)  {
    }
    virtual ~MauInfo()  {
      info_.clear();
    }


    inline void write(int index, uint32_t val) {
      RMT_ASSERT((index >= 0) && (index < size_));
      info_[index] = val;
    }
    inline uint32_t read(int index) {
      RMT_ASSERT(index >= 0);
      return (index < size_) ?info_[index] :0u;
    }
    inline void incr(int index, int inc=1) {
      write(index, read(index) + inc);
    }
    inline void reset(int index) {
      write(index, 0u);
    }

    // Array version read
    inline void read(uint32_t *array, int array_size, bool rst=false) {
      for (int i = 0; i < std::min(array_size, size_); i++) {
        array[i] = read(i);
        if (rst) reset(i);
      }
    }
    // Array version read but returning array of names too
    inline void read(uint32_t *array, int array_size, const char **name_array,
                     bool rst=false) {
      for (int i = 0; i < std::min(array_size, size_); i++) {
        if (array != NULL)      array[i] = read(i);
        if (name_array != NULL) name_array[i] = lookup_index(i);
        if (rst) reset(i);
      }
    }

    // Lookup index to get name
    inline const char* lookup_index(int index) {
      RMT_ASSERT(static_string_vec_ != NULL);
      RMT_ASSERT(index >= 0);
      return (index < size_) ?static_string_vec_->at(index) :NULL;
    }
    // Lookup name to get index
    int lookup_name(const char *name);

    // String versions read/write
    void write_name(const char *name, uint32_t val);
    // Allow reset on read
    uint32_t read_name(const char *name, bool rst);

    // Dump func for debug
    void dump();


 private:
    Mau                       *mau_;
    int                        size_;
    std::vector< uint32_t >    info_;

  }; // MauInfo

}

#endif // _SHARED_MAU_INFO_
