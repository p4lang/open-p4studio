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

#ifndef MODEL_CORE_PORT_JSON_LOADER_H
#define MODEL_CORE_PORT_JSON_LOADER_H

#include <vector>
#include "json-loader.h"

namespace model_core {

class VethMapping {
 public:
  VethMapping(int device_port_, int veth1_, int veth2_) :
      device_port_(device_port_),
      veth1_(veth1_),
      veth2_(veth2_) {}
  ~VethMapping() {}

  int getDevicePort() const {
    return device_port_;
  }
  int getVeth1() const {
    return veth1_;
  }
  int getVeth2() const {
    return veth2_;
  }

 private:
  int device_port_;
  int veth1_;
  int veth2_;
};

class IFMapping {
 public:
  IFMapping(int device_port_, std::string if_name_) :
      device_port_(device_port_),
      if_name_(if_name_) {}
  ~IFMapping() {}

  int getDevicePort() const {
    return device_port_;
  }
  std::string getIFName() const {
    return if_name_;
  }

 private:
  int device_port_;
  std::string if_name_;
};

class PortJSONLoader : public model_core::JSONLoader {
 public:
  PortJSONLoader(const std::string& filename, const std::string& description);
  virtual ~PortJSONLoader();

  std::vector<VethMapping>& getVethMap();
  std::vector<IFMapping>& getIFMap();
  static const bool check_loaded_doc(std::unique_ptr<rapidjson::Document>& document);

 private:
  void parseVeth();
  void parseIF();

  std::vector<VethMapping> veth_map_;
  std::vector<IFMapping> if_map_;
  static constexpr int kMaxPortFileCharacters_ = 10000000;
};

}
#endif //MODEL_CORE_PORT_JSON_LOADER_H_
