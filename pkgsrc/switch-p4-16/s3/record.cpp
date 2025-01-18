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

#include "s3/record.h"

#include <mutex>  // NOLINT(build/c++11)
#include <set>
#include <vector>
#include <memory>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <thread>  // NOLINT(build/c++11)

#include "s3/switch_store.h"
#include "./store.h"

namespace smi {
namespace record {
using ::smi::logging::switch_log;
std::ofstream out;
std::mutex out_mtx;
bool recording_on = false;
char record_filename[64];

// This thread-local variable controls whether a record will be prepended
// with '#' or not. During the replay, records starting with '#' are not
// replayed. For main-thread, this variable is turned to true during init begin
// and changed back to false after initi completes. For sub-threads, it
// remains false throughout and so commenting is not added.
// During internal API calls, the comment mode is set to true to prevent replay
// of internal objects and then turned back to false at the end of API call.
thread_local bool record_comment_mode = false;

void record_comment_mode_set(bool on) { record_comment_mode = on; }

bool record_comment_mode_get(void) { return record_comment_mode; }

void record_file_init(std::string record_file) {
  recording_on = true;
  strncpy(record_filename, record_file.c_str(), sizeof(record_filename) - 1);
  record_filename[sizeof(record_filename) - 1] = 0;
  out.open(record_filename, std::ofstream::trunc);
}

void record_file_clean(void) {
  if (!recording_on) return;

  recording_on = false;
  out.close();
}

void record_file_write_line(std::string str) {
  std::lock_guard<std::mutex> guard(out_mtx);
  out << str << std::endl;
}

std::string get_ts() {
  char buffer[64];
  timespec ts;
  timespec_get(&ts, TIME_UTC);
  size_t size =
      strftime(buffer, 32, "%Y-%m-%d.%T.", std::localtime(&ts.tv_sec));
  snprintf(&buffer[size], sizeof(buffer) - size, "%06ld", ts.tv_nsec);
  return std::string(buffer);
}

// TODO(AB): combine/move to utils
std::vector<std::string> split(const std::string &s, char delimiter) {
  std::vector<std::string> tokens;
  std::string token;
  std::istringstream tokenStream(s);
  while (std::getline(tokenStream, token, delimiter)) {
    tokens.push_back(token);
  }
  return tokens;
}

void record_add_create(const switch_object_type_t object_type,
                       const std::set<attr_w> &attrs,
                       switch_object_id_t object_id,
                       switch_status_t status) {
  if (!recording_on) return;

  ModelInfo *model_info = switch_store::switch_model_info_get();
  const ObjectInfo *object_info = model_info->get_object_info(object_type);
  if (object_info->get_object_class() == OBJECT_CLASS_AUTO) return;
  std::string object_name = object_info->get_object_name();

  std::stringstream ss;
  if (record_comment_mode_get()) ss << "#";
  ss << get_ts() << ":" << std::this_thread::get_id();
  ss << "|c|" << status << "|" << object_name << ":" << object_id << "|0";
  for (auto &attr : attrs) {
    ss << "|" << attr;
  }
  record_file_write_line(ss.str());
}

void record_add_set(switch_object_id_t object_id,
                    const attr_w &attr,
                    switch_status_t status) {
  if (!recording_on) return;

  ModelInfo *model_info = switch_store::switch_model_info_get();
  switch_object_type_t object_type = switch_store::object_type_query(object_id);
  const ObjectInfo *object_info = model_info->get_object_info(object_type);
  if (object_info->get_object_class() == OBJECT_CLASS_AUTO) return;
  std::string object_name = object_info->get_object_name();

  std::stringstream ss;
  if (record_comment_mode_get()) ss << "#";
  ss << get_ts() << ":" << std::this_thread::get_id();
  ss << "|s|" << status << "|" << object_name << ":" << object_id << "|0|"
     << attr;
  record_file_write_line(ss.str());
}

void record_add_get(switch_object_id_t object_id,
                    const switch_attr_id_t attr_id,
                    const attr_w &attr,
                    switch_status_t status) {
  if (!recording_on) return;

  ModelInfo *model_info = switch_store::switch_model_info_get();
  switch_object_type_t object_type = switch_store::object_type_query(object_id);
  const ObjectInfo *object_info = model_info->get_object_info(object_type);
  if (object_info->get_object_class() == OBJECT_CLASS_AUTO) return;
  std::string object_name = object_info->get_object_name();

  std::stringstream ss;
  if (record_comment_mode_get()) ss << "#";
  ss << get_ts() << ":" << std::this_thread::get_id();
  ss << "|g|" << status << "|" << object_name << ":" << object_id << "|"
     << attr_id << "|" << attr;
  record_file_write_line(ss.str());
}

void record_add_remove(switch_object_id_t object_id, switch_status_t status) {
  if (!recording_on) return;

  ModelInfo *model_info = switch_store::switch_model_info_get();
  switch_object_type_t object_type = switch_store::object_type_query(object_id);
  const ObjectInfo *object_info = model_info->get_object_info(object_type);
  if (object_info->get_object_class() == OBJECT_CLASS_AUTO) return;
  std::string object_name = object_info->get_object_name();

  std::stringstream ss;
  if (record_comment_mode_get()) ss << "#";
  ss << get_ts() << ":" << std::this_thread::get_id();
  ss << "|r|" << status << "|" << object_name << ":" << object_id << "|0|";
  record_file_write_line(ss.str());
}

void record_add_notify(std::string notif) {
  if (!recording_on) return;

  std::stringstream ss;
  if (record_comment_mode_get()) ss << "#";
  ss << get_ts() << ":" << std::this_thread::get_id();
  ss << "|n|0|" << notif;
  record_file_write_line(ss.str());
}

void parse_attr_value(switch_attr_type_t attr_type,
                      std::string attr_value,
                      switch_attribute_value_t &attr_t) {
  attr_t.type = attr_type;

  switch (attr_type) {
    case SWITCH_TYPE_BOOL:
      std::istringstream(attr_value) >> std::boolalpha >> attr_t.booldata >>
          std::noboolalpha;
      break;
    case SWITCH_TYPE_UINT8:
      attr_t.u8 =
          static_cast<uint8_t>(std::stoul(attr_value.c_str(), nullptr, 0));
      break;
    case SWITCH_TYPE_UINT16:
      attr_t.u16 =
          static_cast<uint16_t>(std::stoul(attr_value.c_str(), nullptr, 0));
      break;
    case SWITCH_TYPE_UINT32:
      attr_t.u32 =
          static_cast<uint32_t>(std::stoul(attr_value.c_str(), nullptr, 0));
      break;
    case SWITCH_TYPE_UINT64:
      attr_t.u64 = std::strtoul(attr_value.c_str(), nullptr, 0);
      break;
    case SWITCH_TYPE_INT64:
      attr_t.s64 = std::strtol(attr_value.c_str(), nullptr, 0);
      break;
    case SWITCH_TYPE_STRING:
      snprintf(attr_t.text.text,
               SWITCH_MAX_STRING_LEN,
               "%s",
               attr_value.substr(1, attr_value.length() - 2).c_str());
      break;
    case SWITCH_TYPE_MAC:
      attr_util::parse_mac(attr_value, attr_t.mac);
      break;
    case SWITCH_TYPE_IP_ADDRESS:
      attr_util::parse_ip_address(attr_value, attr_t.ipaddr);
      break;
    case SWITCH_TYPE_IP_PREFIX:
      attr_util::parse_ip_prefix(attr_value, attr_t.ipprefix);
      break;
    case SWITCH_TYPE_OBJECT_ID:
      attr_t.oid.data = std::strtoul(attr_value.c_str(), NULL, 0);
      break;
    case SWITCH_TYPE_RANGE: {
      std::vector<std::string> toks = split(attr_value, '-');
      attr_t.range.min =
          static_cast<uint32_t>(std::stoul(toks[0].c_str(), nullptr, 0));
      attr_t.range.max =
          static_cast<uint32_t>(std::stoul(toks[1].c_str(), nullptr, 0));
    } break;

    case SWITCH_TYPE_ENUM: {
      auto start = attr_value.find_first_of("(");
      auto str_enum = attr_value.substr(start + 1);
      str_enum.pop_back();
      attr_t.enumdata.enumdata = std::strtoul(str_enum.c_str(), nullptr, 0);
    } break;

    default:
      switch_log(SWITCH_API_LEVEL_ERROR,
                 SWITCH_OT_NONE,
                 "{}.{}: Unsupported attr type: {}",
                 __func__,
                 __LINE__,
                 attr_type);
      break;
  }
}

void parse_attr_value_list(switch_attr_type_t attr_type,
                           std::string attr_value,
                           switch_attribute_value_t &attr_t) {
  // empty list
  if ((attr_value[0] == '[') && (attr_value[1] == ']')) {
    switch_attr_list_t list_t;
    list_t.list_type = attr_type;
    list_t.count = 0;
    list_t.list = nullptr;
    attr_t.type = SWITCH_TYPE_LIST;
    attr_t.list = list_t;
    return;
  }

  attr_value.erase(attr_value.begin());     // get rid of '['
  attr_value[attr_value.size() - 1] = ',';  // replace ']'

  std::vector<std::string> tokens = split(attr_value, ',');

  switch_attr_list_t list_t;
  list_t.list_type = attr_type;
  list_t.count = tokens.size();
  list_t.list = new switch_attribute_value_t[tokens.size()];

  for (unsigned i = 0; i < tokens.size(); i++) {
    switch_attribute_value_t attr_val_t;
    parse_attr_value(attr_type, tokens[i], attr_val_t);
    list_t.list[i] = attr_val_t;
  }
  attr_t.type = SWITCH_TYPE_LIST;
  attr_t.list = list_t;
}

// parse one pair 'attr=value' and insert to attrs set
void parse_attr_value_pair(const ObjectInfo *object_info,
                           std::string attr_value_pair,
                           std::set<attr_w> &attrs) {
  auto equal = attr_value_pair.find_first_of("=");
  auto attr_name = attr_value_pair.substr(0, equal);
  auto attr_value = attr_value_pair.substr(equal + 1);
  switch_attribute_t attr_t;
  switch_status_t status;

  const AttributeMetadata *attr_md =
      object_info->get_attr_metadata_from_name(attr_name);
  if (!attr_md) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "Failed to get attr_md for {}",
               attr_name);
    return;
  }
  attr_t.id = attr_md->attr_id;
  switch (attr_md->type) {
    case SWITCH_TYPE_LIST: {
      auto value_md = attr_md->get_value_metadata();
      parse_attr_value_list(value_md->type, attr_value, attr_t.value);
    } break;
    default:
      parse_attr_value(attr_md->type, attr_value, attr_t.value);
      break;
  }
  attr_w attr(0);
  status = attr.attr_import(attr_t);
  if (status != SWITCH_STATUS_SUCCESS) {
    switch_log(SWITCH_API_LEVEL_ERROR, SWITCH_OT_NONE, "attr_import failed");
    return;
  }

  attrs.insert(attr);
}

// timestamp:tid|oper|status|objecttype:objectid|attr_id|attr1=value1|attr2=value2|...
// attr_id is valid only for get
// attr1, attr2, etc. not valid for delete
void parse_line(std::string &line,
                char &oper,
                switch_object_type_t &object_type,
                switch_object_id_t &object_id,
                switch_attr_id_t &attr_id,
                std::set<attr_w> &attrs) {
  std::vector<std::string> tokens;
  std::stringstream ss(line);
  std::string tmp;

  while (std::getline(ss, tmp, '|')) tokens.push_back(tmp);

  // ignore notification
  oper = tokens[1][0];
  if (oper == 'n') return;

  auto status = tokens[2];
  auto start = tokens[3].find_first_of(":");
  auto object_name = tokens[3].substr(0, start);
  auto str_object_id = tokens[3].substr(start + 1);
  auto str_attr_id = tokens[4];

  attr_id = static_cast<uint16_t>(std::stoul(str_attr_id.c_str(), nullptr, 0));

  ModelInfo *model_info = switch_store::switch_model_info_get();
  const ObjectInfo *object_info =
      model_info->get_object_info_from_name(object_name);
  if (object_info == NULL) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "Failed to get object info for {}",
               object_name);
    return;
  }
  object_type = object_info->object_type;
  object_id.data = std::strtoul(str_object_id.c_str(), NULL, 0);

  // Parse the attribute/value pairs and insert to attrs set
  for (size_t i = 5; i < tokens.size(); i++) {
    const std::string &attr_value_pair = tokens[i];
    // parse one pair 'attr=value' and insert to attrs set
    // value could be a base type or list
    parse_attr_value_pair(object_info, attr_value_pair, attrs);
  }
}

void record_file_replay(std::string replay_file) {
  std::ifstream infile(replay_file);
  std::string line;
  size_t count = 0;
  bool recording_status = recording_on;

  recording_on = false;
  if (!infile.is_open()) {
    switch_log(SWITCH_API_LEVEL_ERROR,
               SWITCH_OT_NONE,
               "failed to open file {}",
               replay_file);
  }

  while (std::getline(infile, line)) {
    char op;
    switch_object_type_t object_type;
    switch_object_id_t object_id;
    switch_attr_id_t attr_id;
    std::set<attr_w> attrs;

    if (line[0] == '#') continue;
    parse_line(line, op, object_type, object_id, attr_id, attrs);
    count++;

    switch_object_id_t object_handle;
    switch (op) {
      case 'c': {
        auto status =
            switch_store::object_create(object_type, attrs, object_handle);
        if (status != SWITCH_STATUS_SUCCESS) {
          // switch_log(SWITCH_API_LEVEL_ERROR,
          //            SWITCH_OT_NONE,
          //            "Create failed for {} {} status {}",
          //            object_type,
          //            attrs,
          //            status);
        }
      } break;
      case 's': {
        auto status = switch_store::attribute_set(object_id, *attrs.begin());
        if (status != SWITCH_STATUS_SUCCESS) {
          // switch_log(SWITCH_API_LEVEL_ERROR,
          //            SWITCH_OT_NONE,
          //            "Set failed for {} {} {}",
          //            object_id,
          //            attrs,
          //            status);
        }
      } break;
        break;
      case 'r': {
        auto status = switch_store::object_delete(object_id);
        if (status != SWITCH_STATUS_SUCCESS) {
          switch_log(SWITCH_API_LEVEL_ERROR,
                     SWITCH_OT_NONE,
                     "Remove failed for {} status {}",
                     object_id,
                     status);
        }
      } break;
      case 'g': {
        attr_w attr(0);
        auto status = switch_store::attribute_get(object_id, attr_id, attr);
        if (status != SWITCH_STATUS_SUCCESS) {
          // switch_log(SWITCH_API_LEVEL_ERROR,
          //            SWITCH_OT_NONE,
          //            "Get failed for {} {} {} status {}",
          //            object_id,
          //            attr_id,
          //            attr,
          //            status);
        }
      } break;
      case 'n':
        break;
      default:
        switch_log(SWITCH_API_LEVEL_ERROR, SWITCH_OT_NONE, "Unknown op {}", op);
        break;
    }
  }
  switch_log(SWITCH_API_LEVEL_INFO, SWITCH_OT_NONE, "Replayed {} lines", count);
  infile.close();
  recording_on = recording_status;
}

} /* namespace record */
} /* namespace smi */
