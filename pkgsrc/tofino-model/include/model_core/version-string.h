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

#ifndef _MODEL_CORE_VERSION_STRING_H
#define _MODEL_CORE_VERSION_STRING_H

#include <string>

namespace model_core {

class VersionString {
 public:
  int major_, minor_, revision_;

  VersionString(const std::string version_str);
  bool operator>(const VersionString &other);
  bool operator==(const VersionString &other);
  bool operator!=(const VersionString &other);
  bool operator<(const VersionString &other);
  std::string str();

 private:
  std::string version_str_;
  void parse(std::string version_str);
};

}
#endif //_MODEL_CORE_VERSION_STRING_H
