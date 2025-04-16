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

#include <stdexcept>
#include <model_core/version-string.h>

namespace model_core {

  VersionString::VersionString(const std::string version_str) :
      version_str_(version_str)
  {
    parse(version_str_);
  }

  bool VersionString::operator>(const VersionString &other) {
    if (major_ == other.major_) {
      if (minor_ == other.minor_) {
        return revision_ > other.revision_;
      }
      return minor_ > other.minor_;
    }
    return major_ > other.major_;
  }

  bool VersionString::operator==(const VersionString &other) {
    return (major_ == other.major_) &&
           (minor_ == other.minor_) &&
           (revision_ == other.revision_);
  }

  bool VersionString::operator!=(const VersionString &other) {
    return !(*this == other);
  }

  bool VersionString::operator<(const VersionString &other) {
    return (*this != other) && !(*this > other);
  }

  void VersionString::parse(std::string version_str) {
    int num = sscanf(version_str.c_str(), "%d.%d.%d", &major_, &minor_, &revision_);
    if (num != 3) {
      throw std::invalid_argument("Invalid version string");
    }
  }

  std::string VersionString::str() { return version_str_;}
}
