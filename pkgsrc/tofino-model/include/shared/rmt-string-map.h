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

#ifndef _SHARED_RMT_STRING_MAP_
#define _SHARED_RMT_STRING_MAP_

#include <cstdint>
#include <functional>
#include <rmt-log.h>
#include <unordered_map>
#include <vector>

namespace MODEL_CHIP_NAMESPACE {

  class RmtObjectManager;

  class RmtStringMap : public DefaultLogger {

 public:

    RmtStringMap(RmtObjectManager* om);
    ~RmtStringMap();

    // Lookup/Destroy/Initialise string->int map
    int  lookup(const char *key, const char *value);
    std::vector<std::string> get_all_keys();
    void clear();
    void init();


 private:
    RmtObjectManager* om_;
    std::unordered_map<std::string, std::function<int(const std::string&, const std::string&) > >         string_map_;

    static int set_bool(bool& b, std::string value);

    static bool parse_bool_value(std::string value);

    static int set_uint64(uint64_t& v64, std::string value);

  };
}

#endif // _SHARED_RMT_STRING_MAP_
