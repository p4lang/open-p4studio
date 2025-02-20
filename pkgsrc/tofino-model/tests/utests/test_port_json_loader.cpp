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

#include <cstdio>
#include "filesystem_helper.h"
#include "gtest.h"
#include "model_core/port-json-loader.h"

namespace model_core_tests {

TEST(TestPortJSONLoader, ReadJSONVeth) {
  std::string conf_file_name = get_resource_file_path("port-json-loader-veth-config.json");
  model_core::PortJSONLoader loader(conf_file_name, "Veth Test File");
  std::vector<model_core::VethMapping> veth_map = loader.getVethMap();
  ASSERT_EQ(veth_map.size(), 17u);

  ASSERT_EQ(veth_map[0].getDevicePort(), 0);
  ASSERT_EQ(veth_map[0].getVeth1(), 0);
  ASSERT_EQ(veth_map[0].getVeth2(), 1);

  ASSERT_EQ(veth_map[3].getVeth2(), 7);
}

TEST(TestPortJSONLoader, ReadJSONIF) {
  std::string conf_file_name = get_resource_file_path("port-json-loader-if-config.json");
  model_core::PortJSONLoader loader(conf_file_name, "IF Test File");
  std::vector<model_core::IFMapping> if_map = loader.getIFMap();
  ASSERT_EQ(if_map.size(), 17u);

  ASSERT_EQ(if_map[0].getDevicePort(), 0);
  ASSERT_EQ(if_map[0].getIFName(), "one");

  ASSERT_EQ(if_map[4].getDevicePort(), 4);
  ASSERT_EQ(if_map[4].getIFName(), "eight");
}

TEST(TestPortJSONLoader, ReadJSONBoth) {
  std::string
      conf_file_name = get_resource_file_path("port-json-loader-veth-if-config.json");
  model_core::PortJSONLoader
      loader(conf_file_name, "Test file with both veth and if");

  std::vector<model_core::VethMapping>& veth_map = loader.getVethMap();
  std::vector<model_core::IFMapping>& if_map = loader.getIFMap();

  ASSERT_EQ(if_map.size(), 2u);
  ASSERT_EQ(veth_map.size(), 2u);

  ASSERT_EQ(veth_map[0].getDevicePort(), 2);
  ASSERT_EQ(veth_map[0].getVeth1(), 0);
  ASSERT_EQ(veth_map[0].getVeth2(), 1);
  ASSERT_EQ(veth_map[1].getDevicePort(), 3);
  ASSERT_EQ(veth_map[1].getVeth1(), 2);
  ASSERT_EQ(veth_map[1].getVeth2(), 3);

  ASSERT_EQ(if_map[0].getDevicePort(), 0);
  ASSERT_EQ(if_map[0].getIFName(), "veth4");
  ASSERT_EQ(if_map[1].getDevicePort(), 1);
  ASSERT_EQ(if_map[1].getIFName(), "veth6");
}

TEST(TestPortJSONLoader, ReadJSONBad) {
  std::string conf_file_name = get_resource_file_path("port-json-loader-invalid-config.json");

  ASSERT_THROW(new model_core::PortJSONLoader(conf_file_name,
                                              "Invalid Test File"),
               std::invalid_argument);
}

}
