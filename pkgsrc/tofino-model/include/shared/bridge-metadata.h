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

#ifndef _SHARED_BRIDGE_METADATA_
#define _SHARED_BRIDGE_METADATA_

#include <rmt-defs.h>

namespace MODEL_CHIP_NAMESPACE {

// Metadata passed from ingress pipe to egress pipe
class BridgeMetadata {
  static constexpr unsigned kBrMetaBytes = RmtDefs::kBridgeMetadataBytes;
  static constexpr unsigned kBrMetaSize = (kBrMetaBytes < 8) ?8 :kBrMetaBytes;

 public:
  uint8_t get_byte(unsigned n) const {
    return (n < kBrMetaBytes) ?data_.at(n) :0;
  }
  void set_byte(unsigned n, uint8_t v) {
    if (n < kBrMetaBytes) data_.at(n) = v;
  }
  void reset() {
    for (unsigned i = 0; i < kBrMetaSize; ++i) {
      data_.at(i) = 0;
    }
  }
  void copy_from(const BridgeMetadata s) {
    for (unsigned i = 0; i < kBrMetaSize; ++i) {
      data_.at(i) = s.data_.at(i);
    }
  }

 private:
  std::array< uint8_t, kBrMetaSize >  data_;
};

}
#endif // _SHARED_BRIDGE_METADATA_
