// SPDX-FileCopyrightText: 2021 Intel Corporation
// Copyright (C) 2021 Intel Corporation.
//
// SPDX-License-Identifier: Apache-2.0

#include <exception>
#include <fstream>
#include <iostream>
#include <memory>
#include <regex>

#include <tdi/common/tdi_json_parser/tdi_learn_info.hpp>
#include <tdi/common/tdi_json_parser/tdi_cjson.hpp>
#include <tdi/common/tdi_utils.hpp>

namespace tdi {

std::vector<tdi_id_t> LearnInfo::dataFieldIdListGet() const {
  std::vector<tdi_id_t> id_vec;
  for (const auto &kv : learn_field_map_) {
    id_vec.push_back(kv.first);
  }
  std::sort(id_vec.begin(), id_vec.end());
  return id_vec;
}

tdi_id_t LearnInfo::dataFieldIdGet(const std::string &name) const {
  auto found = std::find_if(
      learn_field_map_.begin(),
      learn_field_map_.end(),
      [&name](const std::pair<const tdi_id_t, std::unique_ptr<DataFieldInfo>>
                  &map_item) { return (name == map_item.second->nameGet()); });
  if (found != learn_field_map_.end()) {
    return (*found).second->idGet();
  }

  LOG_ERROR("%s:%d %s Field \"%s\" not found in data field list",
            __func__,
            __LINE__,
            nameGet().c_str(),
            name.c_str());
  return 0;
}

const DataFieldInfo *LearnInfo::dataFieldGet(const tdi_id_t &field_id) const {
  if (learn_field_map_.find(field_id) == learn_field_map_.end()) {
    LOG_ERROR("%s:%d %s Field \"%d\" not found in data field list",
              __func__,
              __LINE__,
              nameGet().c_str(),
              field_id);
    return nullptr;
  }
  return learn_field_map_.at(field_id).get();
}

}  // namespace tdi
