// SPDX-FileCopyrightText: 2021 Intel Corporation
// Copyright (C) 2021 Intel Corporation.
//
// SPDX-License-Identifier: Apache-2.0

#include <exception>
#include <fstream>
#include <iostream>
#include <memory>
#include <regex>

#include <tdi/common/tdi_info.hpp>
#include <tdi/common/tdi_json_parser/tdi_cjson.hpp>
#include <tdi/common/tdi_json_parser/tdi_table_info.hpp>
#include <tdi/common/tdi_utils.hpp>

namespace tdi {

bool Annotation::operator<(const Annotation &other) const {
  return (this->full_name_ < other.full_name_);
}
bool Annotation::operator==(const Annotation &other) const {
  return (this->full_name_ == other.full_name_);
}
bool Annotation::operator==(const std::string &other_str) const {
  return (this->full_name_ == other_str);
}
tdi_status_t Annotation::fullNameGet(std::string *full_name) const {
  *full_name = full_name_;
  return TDI_SUCCESS;
}

std::vector<tdi_id_t> TableInfo::keyFieldIdListGet() const {
  std::vector<tdi_id_t> id_vec;
  for (const auto &kv : table_key_map_) {
    id_vec.push_back(kv.first);
  }
  std::sort(id_vec.begin(), id_vec.end());
  return id_vec;
}

const KeyFieldInfo *TableInfo::keyFieldGet(const std::string &name) const {
  if (name_key_map_.find(name) == name_key_map_.end()) {
    LOG_WARN("%s:%d %s Field \"%s\" not found in key field list",
              __func__,
              __LINE__,
              nameGet().c_str(),
              name.c_str());
    return nullptr;
  }
  return name_key_map_.at(name);
}

const KeyFieldInfo *TableInfo::keyFieldGet(const tdi_id_t &field_id) const {
  if (table_key_map_.find(field_id) == table_key_map_.end()) {
    LOG_WARN("%s:%d %s Field \"%d\" not found in key field list",
              __func__,
              __LINE__,
              nameGet().c_str(),
              field_id);
    return nullptr;
  }
  return table_key_map_.at(field_id).get();
}

std::vector<tdi_id_t> TableInfo::dataFieldIdListGet(
    const tdi_id_t &action_id) const {
  std::vector<tdi_id_t> id_vec;
  if (action_id) {
    if (table_action_map_.find(action_id) == table_action_map_.end()) {
      LOG_WARN("%s:%d %s Action Id %d Not Found",
                __func__,
                __LINE__,
                nameGet().c_str(),
                action_id);
    } else {
      for (const auto &kv : table_action_map_.at(action_id)->data_fields_) {
        id_vec.push_back(kv.first);
      }
    }
  }
  // Also include common data fields
  for (const auto &kv : table_data_map_) {
    id_vec.push_back(kv.first);
  }
  std::sort(id_vec.begin(), id_vec.end());
  return id_vec;
}

std::vector<tdi_id_t> TableInfo::dataFieldIdListGet() const {
  return this->dataFieldIdListGet(0);
}

tdi_id_t TableInfo::dataFieldIdGet(const std::string &name) const {
  return dataFieldIdGet(name, 0);
}

tdi_id_t TableInfo::dataFieldIdGet(const std::string &name,
                                   const tdi_id_t &action_id) const {
  const auto dataField = dataFieldGet(name, action_id);
  if (dataField == nullptr) {
    return 0;
  }
  return dataField->idGet();
}

const DataFieldInfo *TableInfo::dataFieldGet(const std::string &name,
                                             const tdi_id_t &action_id) const {
  if (action_id &&
      table_action_map_.find(action_id) != table_action_map_.end()) {
    auto action_info = table_action_map_.at(action_id).get();
    if (action_info->data_fields_names_.find(name) !=
        action_info->data_fields_names_.end()) {
      return action_info->data_fields_names_.at(name);
    }
  }
  if (name_data_map_.find(name) != name_data_map_.end()) {
    return name_data_map_.at(name);
  }
  LOG_WARN("%s:%d %s Field \"%s\" not found in data field list",
            __func__,
            __LINE__,
            nameGet().c_str(),
            name.c_str());
  return nullptr;
}

const DataFieldInfo *TableInfo::dataFieldGet(const std::string &name) const {
  return dataFieldGet(name, 0);
}

const DataFieldInfo *TableInfo::dataFieldGet(const tdi_id_t &field_id,
                                             const tdi_id_t &action_id) const {
  if (action_id &&
      table_action_map_.find(action_id) != table_action_map_.end()) {
    auto action_info = table_action_map_.at(action_id).get();
    if (action_info->data_fields_.find(field_id) !=
        action_info->data_fields_.end()) {
      return action_info->data_fields_.at(field_id).get();
    }
  }
  if (table_data_map_.find(field_id) != table_data_map_.end()) {
    return table_data_map_.at(field_id).get();
  }
  LOG_WARN("%s:%d %s Field \"%d\" not found in data field list",
            __func__,
            __LINE__,
            nameGet().c_str(),
            field_id);
  return nullptr;
}

const DataFieldInfo *TableInfo::dataFieldGet(const tdi_id_t &field_id) const {
  return dataFieldGet(field_id, 0);
}

const ActionInfo *TableInfo::actionGet(const std::string &name) const {
  if (name_action_map_.find(name) != name_action_map_.end()) {
    return name_action_map_.at(name);
  }
  LOG_WARN("%s:%d %s Action  \"%s\" not found",
            __func__,
            __LINE__,
            nameGet().c_str(),
            name.c_str());
  return nullptr;
}

const ActionInfo *TableInfo::actionGet(const tdi_id_t &action_id) const {
  if (table_action_map_.find(action_id) != table_action_map_.end()) {
    return table_action_map_.at(action_id).get();
  }
  LOG_WARN("%s:%d %s Action  \"%d\" not found",
            __func__,
            __LINE__,
            nameGet().c_str(),
            action_id);
  return nullptr;
}

std::vector<tdi_id_t> TableInfo::actionIdListGet() const {
  std::vector<tdi_id_t> id_vec;
  for (const auto &kv : table_action_map_) {
    id_vec.push_back(kv.first);
  }
  std::sort(id_vec.begin(), id_vec.end());
  return id_vec;
}

#if 0
tdi_status_t Table::getDataField(const tdi_id_t &field_id,
                                 const TableDataField **field) const {
  return this->getDataField(field_id, 0, field);
}

tdi_status_t Table::getDataField(const tdi_id_t &field_id,
                                 const tdi_id_t &action_id,
                                 const TableDataField **field) const {
  std::vector<tdi_id_t> empty;
  return this->getDataField(field_id, action_id, empty, field);
}

tdi_status_t Table::getDataField(const tdi_id_t &field_id,
                                 const tdi_id_t &action_id,
                                 const std::vector<tdi_id_t> &container_id_vec,
                                 const TableDataField **field) const {
  *field = nullptr;
  if (action_info_list.find(action_id) != action_info_list.end()) {
    *field =
        this->getDataFieldHelper(field_id,
                                 container_id_vec,
                                 0,
                                 action_info_list.at(action_id)->data_fields);
    if (*field) return TDI_SUCCESS;
  }
  // We need to search the common data too
  *field = this->getDataFieldHelper(
      field_id, container_id_vec, 0, common_data_fields);
  if (*field) return TDI_SUCCESS;
  // Logging only warning so as to not overwhelm logs. Users supposed to check
  // error code if something wrong
  LOG_WARN("%s:%d Data field ID %d actionID %d not found",
           __func__,
           __LINE__,
           field_id,
           action_id);
  return TDI_OBJECT_NOT_FOUND;
}

const TableDataField *Table::getDataFieldHelper(
    const tdi_id_t &field_id,
    const std::vector<tdi_id_t> &container_id_vec,
    const uint32_t depth,
    const std::map<tdi_id_t, std::unique_ptr<TableDataField>> &field_map)
    const {
  if (field_map.find(field_id) != field_map.end()) {
    return field_map.at(field_id).get();
  }
  // iterate all fields and recursively search for the data field
  // if this field is a container field and it exists in the contaienr
  // set provided by caller.
  // If container ID vector is empty, then just try and find first
  // instance
  for (const auto &p : field_map) {
    if (((container_id_vec.size() >= depth + 1 &&
          container_id_vec[depth] == p.first) ||
         container_id_vec.empty()) &&
        p.second.get()->isContainerValid()) {
      auto field = this->getDataFieldHelper(field_id,
                                            container_id_vec,
                                            depth + 1,
                                            p.second.get()->containerMapGet());
      if (field) return field;
    }
  }
  return nullptr;
}

tdi_status_t Table::getDataField(const std::string &field_name,
                                 const tdi_id_t &action_id,
                                 const std::vector<tdi_id_t> &container_id_vec,
                                 const TableDataField **field) const {
  *field = nullptr;
  if (action_info_list.find(action_id) != action_info_list.end()) {
    *field = this->getDataFieldHelper(
        field_name,
        container_id_vec,
        0,
        action_info_list.at(action_id)->data_fields_names);
    if (*field) return TDI_SUCCESS;
  }
  // We need to search the common data too
  *field = this->getDataFieldHelper(
      field_name, container_id_vec, 0, common_data_fields_names);
  if (*field) return TDI_SUCCESS;
  // Logging only warning so as to not overwhelm logs. Users supposed to check
  // error code if something wrong
  // TODO change this to WARN
  LOG_TRACE("%s:%d Data field name %s actionID %d not found",
            __func__,
            __LINE__,
            field_name.c_str(),
            action_id);
  return TDI_OBJECT_NOT_FOUND;
}

const TableDataField *Table::getDataFieldHelper(
    const std::string &field_name,
    const std::vector<tdi_id_t> &container_id_vec,
    const uint32_t depth,
    const std::map<std::string, TableDataField *> &field_map) const {
  if (field_map.find(field_name) != field_map.end()) {
    return field_map.at(field_name);
  }
  // iterate all fields and recursively search for the data field
  // if this field is a container field and it exists in the contaienr
  // vector provided by caller.
  // If container ID vector is empty, then just try and find first
  // instance
  for (const auto &p : field_map) {
    if (((container_id_vec.size() >= depth + 1 &&
          container_id_vec[depth] == p.second->idGet()) ||
         container_id_vec.empty()) &&
        p.second->isContainerValid()) {
      auto field = this->getDataFieldHelper(field_name,
                                            container_id_vec,
                                            depth + 1,
                                            p.second->containerNameMapGet());
      if (field) return field;
    }
  }
  return nullptr;
}

#endif

}  // namespace tdi
