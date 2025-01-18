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

#ifndef _SHARED_PHV_FACTORY_
#define _SHARED_PHV_FACTORY_

#include <common/rmt-assert.h>
#include <string>
#include <cstdint>
#include <rmt-object.h>
#include <phv.h>


namespace MODEL_CHIP_NAMESPACE {

  class PhvFactory : private RmtObject {

 public:
    static bool kPhvInitTimeRandOnAlloc; // Defined in rmt-config.cpp
    static bool kPhvInitAllValid;

    PhvFactory(RmtObjectManager *om);
    ~PhvFactory();

    inline bool multi_write_disabled(int i) const {
      return multi_write_disabled_[i];
    }
    inline CacheId& phv_cache_id() {
      phv_cache_id_.SetNewId(); // Always hand out new id
      return phv_cache_id_;
    }

    inline void set_multi_write_disabled(int i, bool tf) {
      multi_write_disabled_[i] = tf;
    }

    Phv *phv_create();
    void phv_delete(Phv *phv);


 private:
    CacheId                                        phv_cache_id_;
    std::array<bool,Phv::kWordsMaxExtended>        multi_write_disabled_ = { };
  };

}
#endif // _SHARED_PHV_FACTORY_
