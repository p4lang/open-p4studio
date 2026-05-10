// SPDX-FileCopyrightText: 2021 Intel Corporation
// Copyright (C) 2021 Intel Corporation.
//
// SPDX-License-Identifier: Apache-2.0

#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <regex>
#include <string>
#include <vector>

#include <tdi/common/tdi_info.hpp>
#include <tdi/common/tdi_utils.hpp>

namespace tdi {

namespace {

std::vector<std::string> splitString(const std::string &s,
                                     const std::string &delimiter) {
  size_t pos = 0;
  size_t start_pos = 0;
  std::vector<std::string> token_list;
  while ((pos = s.find(delimiter, start_pos)) != std::string::npos) {
    token_list.push_back(s.substr(start_pos, pos - start_pos));
    start_pos = pos + delimiter.length();
  }
  token_list.push_back(s.substr(start_pos, pos - start_pos));
  return token_list;
}

/*
 * @brief Generate unique names
 * Split the name using . as delimiter. Then create partially qualified
 * names.
 * tokens(pipe0.SwitchIngress.forward) = set(pipe0, SwitchIngress, forward)
 * full_name_list = [pipe0.SwitchIngress.forward, SwitchIngress.forward,
 * forward]
 */

std::set<std::string> generateUniqueNames(const std::string &obj_name) {
  auto tokens = splitString(obj_name, ".");
  std::string last_token = "";
  std::set<std::string> full_name_list;
  for (auto it = tokens.rbegin(); it != tokens.rend(); ++it) {
    auto token = *it;
    if (last_token == "") {
      full_name_list.insert(token);
      last_token = token;
    } else {
      full_name_list.insert(token + "." + last_token);
      last_token = token + "." + last_token;
    }
  }
  return full_name_list;
}

/* @brief This function converts a nameMap to a fullNameMap. A fullNameMap is a
 *mapping of
 * all possible names of a table entity to the table object's raw pointer.
 *
 * pipe0.SI.forward = <forward_table_1>
 * SI.forward       = <forward_table_1>
 *
 * pipe0.SE.forward = <forward_table_2>
 * SE.forward       = <forward_table_2>
 *
 * pipe0.SI.fib = <fib_table>
 * SI.fib       = <fib_table>
 * fib          = <fib_table>
 */
template <typename T>
void populateFullNameMap(
    const std::map<std::string, std::unique_ptr<T>> &nameMap,
    std::map<std::string, const T *> *fullNameMap) {
  std::set<std::string> names_to_remove;
  // We need to trim the possible names down since all are not possible.
  // Loop over all the tables
  for (const auto &name_pair : nameMap) {
    // Generate all possible names for all the tables. The below map will
    // contain
    // mapping of
    // a table name to all the possible table names.
    const auto possible_name_list = generateUniqueNames(name_pair.first);
    // Loop over all the possible names. If they are not present in the map,
    // then add them to the map. Else, just mark this name in a set kept to
    // remove these later
    for (const auto &prospective_name : possible_name_list) {
      if ((*fullNameMap).find(prospective_name) != (*fullNameMap).end()) {
        names_to_remove.insert(prospective_name);
      } else {
        (*fullNameMap)[prospective_name] = name_pair.second.get();
      }
    }
  }

  // Remove the marked names from the map as well.
  for (const auto &name : names_to_remove) {
    (*fullNameMap).erase(name);
  }
}

}  // anonymous namespace

std::unique_ptr<const TdiInfo> TdiInfo::makeTdiInfo(
    const std::string &p4_name,
    std::unique_ptr<TdiInfoParser> tdi_info_parser,
    const tdi::TableFactory *factory) {
  try {
    std::unique_ptr<const TdiInfo> tdi_info(
        new TdiInfo(p4_name, std::move(tdi_info_parser), factory));
    return tdi_info;
  } catch (...) {
    LOG_ERROR("%s:%d Failed to create TdiInfo", __func__, __LINE__);
    return nullptr;
  }
}

TdiInfo::TdiInfo(const std::string &p4_name,
                 std::unique_ptr<TdiInfoParser> tdi_info_parser,
                 const tdi::TableFactory *factory)
    : p4_name_(p4_name), tdi_info_parser_(std::move(tdi_info_parser)) {
  // Go over all table_info and learn_info in the parser object and
  // create Table and Learn objects for them
  for (const auto &kv : tdi_info_parser_->tableInfoMapGet()) {
    if (tableMap.find(kv.first) != tableMap.end()) {
      LOG_WARN("%s:%d Table:%s Already exists. Not adding another",
                __func__,
                __LINE__,
                kv.first.c_str());
    } else {
      auto table = factory->makeTable(this, kv.second.get());
      if (!table) {
        LOG_WARN("%s:%d Unable to create Table:%s",
                  __func__,
                  __LINE__,
                  kv.first.c_str());
        continue;
      }
      if (tableIdMap.find(table->tableInfoGet()->idGet()) != tableIdMap.end()) {
        LOG_WARN("%s:%d Table:%s ID %d Already exists. Not adding again",
                  __func__,
                  __LINE__,
                  kv.first.c_str(),
                  table->tableInfoGet()->idGet());
      } else {
        tableIdMap[table->tableInfoGet()->idGet()] = table.get();
      }
      tableMap[kv.first] = std::move(table);
    }
  }
  populateFullNameMap<tdi::Table>(tableMap, &fullTableMap);

  // Creating Learn
  for (const auto &kv : tdi_info_parser_->learnInfoMapGet()) {
    if (learnMap.find(kv.first) != learnMap.end()) {
      LOG_ERROR("%s:%d Learn Table:%s Already exists",
                __func__,
                __LINE__,
                kv.first.c_str());
    } else {
      auto learn = std::unique_ptr<Learn>(new Learn(kv.second.get()));
      if (!learn) {
        LOG_WARN("%s:%d Unable to create Learn:%s",
                  __func__,
                  __LINE__,
                  kv.first.c_str());
        continue;
      }

      if (learnIdMap.find(learn->learnInfoGet()->idGet()) != learnIdMap.end()) {
        LOG_WARN("%s:%d Learn :%s ID %d Already exists. Not adding again",
                  __func__,
                  __LINE__,
                  kv.first.c_str(),
                  learn->learnInfoGet()->idGet());
      } else {
        learnIdMap[learn->learnInfoGet()->idGet()] = learn.get();
      }
      learnMap[kv.first] = std::move(learn);
    }
  }
  populateFullNameMap<tdi::Learn>(learnMap, &fullLearnMap);
}

tdi_status_t TdiInfo::tablesGet(
    std::vector<const Table *> *table_vec_ret) const {
  if (table_vec_ret == nullptr) {
    LOG_ERROR("%s:%d nullptr arg passed", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  for (auto const &item : tableMap) {
    table_vec_ret->push_back(item.second.get());
  }
  return TDI_SUCCESS;
}

tdi_status_t TdiInfo::tableFromNameGet(const std::string &name,
                                       const Table **table_ret) const {
  if (invalid_table_names.find(name) != invalid_table_names.end()) {
    LOG_ERROR("%s:%d Table \"%s\" was optimized out",
              __func__,
              __LINE__,
              name.c_str());
    return TDI_INVALID_ARG;
  }
  auto it = this->fullTableMap.find(name);
  if (it == this->fullTableMap.end()) {
    LOG_ERROR("%s:%d Table \"%s\" not found", __func__, __LINE__, name.c_str());
    return TDI_OBJECT_NOT_FOUND;
  } else {
    *table_ret = it->second;
    return TDI_SUCCESS;
  }
}

tdi_status_t TdiInfo::tableFromIdGet(const tdi_id_t &id,
                                     const Table **table_ret) const {
  auto it = tableIdMap.find(id);
  if (it == tableIdMap.end()) {
    LOG_ERROR("%s:%d Table_id \"%d\" not found", __func__, __LINE__, id);
    return TDI_OBJECT_NOT_FOUND;
  } else {
    *table_ret = it->second;
  }
  return TDI_SUCCESS;
}

tdi_status_t TdiInfo::learnsGet(
    std::vector<const Learn *> *learn_vec_ret) const {
  if (learn_vec_ret == nullptr) {
    LOG_ERROR("%s:%d nullptr arg passed", __func__, __LINE__);
    return TDI_INVALID_ARG;
  }
  for (auto &item : learnMap) {
    learn_vec_ret->push_back(item.second.get());
  }
  return TDI_SUCCESS;
}

tdi_status_t TdiInfo::learnFromNameGet(std::string name,
                                       const Learn **learn_ret) const {
  if (this->fullLearnMap.find(name) == this->fullLearnMap.end()) {
    LOG_ERROR(
        "%s:%d Learn Obj \"%s\" not found", __func__, __LINE__, name.c_str());
    return TDI_OBJECT_NOT_FOUND;
  }
  *learn_ret = this->fullLearnMap.at(name);
  return TDI_SUCCESS;
}

tdi_status_t TdiInfo::learnFromIdGet(tdi_id_t id,
                                     const Learn **learn_ret) const {
  auto it = learnIdMap.find(id);
  if (it == learnIdMap.end()) {
    LOG_ERROR("%s:%d Learn_id \"%d\" not found", __func__, __LINE__, id);
    return TDI_OBJECT_NOT_FOUND;
  } else {
    *learn_ret = it->second;
  }
  return TDI_SUCCESS;
}

const std::map<std::string, std::unique_ptr<tdi::Table>> &TdiInfo::tableMapGet()
    const {
  return tableMap;
}

const std::map<std::string, std::unique_ptr<tdi::Learn>> &TdiInfo::learnMapGet()
    const {
  return learnMap;
}

}  // namespace tdi
