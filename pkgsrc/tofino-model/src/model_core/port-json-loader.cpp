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

#include "model_core/port-json-loader.h"
#include <iostream>
#include "rapidjson/stringbuffer.h"

namespace model_core {

PortJSONLoader::PortJSONLoader(const std::string& filename,
                               const std::string& description)
    : JSONLoader(filename,
                 description,
                 kMaxPortFileCharacters_,
                 check_loaded_doc) {
  if (!IsLoaded()) {
    throw std::invalid_argument(
        "Failed to load port to veth i/f mapping file: " + filename);
  }
  parseIF();
  parseVeth();
  if (veth_map_.empty() && if_map_.empty()) {
    throw std::invalid_argument(
        "JSON file: " + filename + " contained no IF or Veth data \n");
  }
}

PortJSONLoader::~PortJSONLoader() {}

void PortJSONLoader::parseVeth() {
  if (document_->HasMember("PortToVeth")) {
    auto& array = FindMemberArray(*document_, "PortToVeth");
    //Loop through array
    for (rapidjson::Value::ConstValueIterator itr = array.Begin();
         itr != array.End(); ++itr) {
      //For each object consisting of the 3 values
      const rapidjson::Value& object = *itr;
      //Find the corresponding member of the object and get the int val
      int device_port = FindMember(object, "device_port").GetInt();
      int veth1 = FindMember(object, "veth1").GetInt();
      int veth2 = FindMember(object, "veth2").GetInt();
      veth_map_.emplace_back(device_port, veth1, veth2);
    }
  }
}

void PortJSONLoader::parseIF() {
  if (document_->HasMember("PortToIf")) {
    //Find the array value in that doc
    auto& array = FindMemberArray(*document_, "PortToIf");
    //Loop through array
    for (rapidjson::Value::ConstValueIterator itr = array.Begin();
         itr != array.End(); ++itr) {
      //For each object consisting of the 2 values
      const rapidjson::Value& object = *itr;
      //Find the corresponding member of the object and get the int val
      int device_port = FindMember(object, "device_port").GetInt();
      std::string if_name = FindMember(object, "if").GetString();
      if_map_.emplace_back(device_port, if_name);
    }
  }
}

std::vector<VethMapping>& PortJSONLoader::getVethMap() {
  return veth_map_;
}

std::vector<IFMapping>& PortJSONLoader::getIFMap() {
  return if_map_;
}

const bool PortJSONLoader::check_loaded_doc(std::unique_ptr<rapidjson::Document>& document) {
  if (!document->IsObject()) {
    throw std::invalid_argument("Not a JSON object.");
  }
  return true;
}

}
