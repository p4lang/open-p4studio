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

#ifndef _MODEL_CORE_P4_TARGET_CONF_H
#define _MODEL_CORE_P4_TARGET_CONF_H

#include <sstream>
#include "json-loader.h"

 class P4TargetConf : public model_core::JSONLoader{
 public:
  static constexpr int kMaxConfFileSize = 1000000;  // max chars to read from conf file
  P4TargetConf(const char* file_name) :
    JSONLoader((nullptr == file_name) ? "" : file_name,
               "p4 target config file",
               kMaxConfFileSize) { }
  ~P4TargetConf() { }

  const rapidjson::Value& FindMember(
      const rapidjson::Value& obj,
      const char* key,
      std::stringstream *err) const {
    if (!obj.IsObject()) {
      *err << "Tried to find member in non-object.";
      throw std::invalid_argument(err->str());
    }
    rapidjson::Value::ConstMemberIterator itr = obj.FindMember(key);
    if (itr == obj.MemberEnd()) {
      *err << "Could not find key '" << key << "' in JSON object.";
      throw std::invalid_argument(err->str());
    }
    const rapidjson::Value& val = itr->value; // This to stop Klocwork whingeing
    return val;
  }

  /**
   * Get path to context.json file from the parsed JSON config file.
   * @param chip_idx Chip index
   * @param pipe_idx Pipe index
   * @return path to context.json for the chip/pipe combination, or an empty
   *     string if the config file was not loaded or the chip/pipe combination
   *     is not found in the config file.
   */
  std::string GetContext(const int chip_idx, const int pipe_idx) const {
    std::stringstream err;
    std::string result;
    if (!loaded_) return result;
    try {
      // assume first json schema
      auto& p4_devices = FindMember(*document_, "p4_devices", &err);
      err << "Trying p4_devices: ";
      for (auto& device : p4_devices.GetArray()) {
        auto& instance = FindMember(device, "device-id", &err);
        if (instance == chip_idx) {
          auto& p4_programs = FindMember(device, "p4_programs", &err);
          for (auto& program : p4_programs.GetArray()) {
            auto& p4_pipelines = FindMember(program, "p4_pipelines", &err);
            for (auto& pipeline : p4_pipelines.GetArray()) {
              std::stringstream ignored_err;
              try {
                auto& pipe_scope = FindMember(pipeline, "pipe_scope", &ignored_err);
                for (auto& pipe : pipe_scope.GetArray()) {
                  if (pipe == pipe_idx) {
                    return FindMember(pipeline, "context", &ignored_err).GetString();
                  }
                }
              } catch (std::invalid_argument&) {
                // XXX: if the pipe_scope element is missing we assume
                // the context file is default for all pipe indexes, but don't
                // return this until we've checked all other pipeline arrays
                // for an explicit match to the given pipe_idx
                result = FindMember(pipeline, "context", &err).GetString();
              }
            }
          }
        }
      }
    } catch (std::invalid_argument&) {
      // fall back to other json schema
      err << " Trying p4_program_list: ";
      auto& p4_program_list = FindMember(*document_, "p4_program_list", &err);
      for (auto& program : p4_program_list.GetArray()) {
        auto& instance = FindMember(program, "instance", &err);
        if (instance == chip_idx) {
          return FindMember(program, "table-config", &err).GetString();
        }
      }
    }
    return result;
  }
};

#endif  /* _MODEL_CORE_P4_TARGET_CONF_H */
