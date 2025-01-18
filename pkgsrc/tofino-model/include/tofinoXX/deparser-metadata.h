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

#ifndef _TOFINOXX_DEPARSER_METADATA_
#define _TOFINOXX_DEPARSER_METADATA_

#include <bitvector.h>
#include <rmt-defs.h>

namespace MODEL_CHIP_NAMESPACE {

  template<int DATA_WIDTH>
  struct DeparserMetadata {
    DeparserMetadata() : valid(false) {};
    ~DeparserMetadata() {};
    bool valid;
    BitVector<DATA_WIDTH> data{};
  };

  struct LearnQuantumType: public DeparserMetadata<RmtDefs::kLearnQuantumWidth> {
    LearnQuantumType() {};
    ~LearnQuantumType() {};
    int length=0;
  };

  template<int DATA_WIDTH>
  struct DeparserMetadataInfo {
    uint8_t phv_idx_;
    bool valid_;
    bool default_valid_;
    DeparserMetadata<DATA_WIDTH> default_value_;
    uint8_t shift_;
    bool shift_valid_;

    DeparserMetadataInfo(const uint8_t &phv_idx, bool valid, bool default_valid,
                         DeparserMetadata<DATA_WIDTH> default_value)
      : phv_idx_(phv_idx), valid_(valid), default_valid_(default_valid),
        default_value_(default_value), shift_(0), shift_valid_(false)
    {}

    void set_shift(const uint8_t &shift) {
      shift_ = shift;
      shift_valid_ = true;
    }

    ~DeparserMetadataInfo() {}
  };

  template<typename T, int METADATA_WIDTH>
  DeparserMetadataInfo<METADATA_WIDTH> get_metadata_info(T &register_block) {
    uint8_t phv = register_block.phv();
    bool valid = register_block.valid();
    const auto default_value = register_block.dflt_value();
    DeparserMetadata<METADATA_WIDTH> default_value_metadata{};
    for (int i = 0; i < METADATA_WIDTH; ++i) {
      bool value = ((default_value & (1 << i)) != 0) ? true : false;
      default_value_metadata.data.set_bit(value, i);
    }

    return DeparserMetadataInfo<METADATA_WIDTH>(phv, valid, true, default_value_metadata);
  }

  template<typename T, int METADATA_WIDTH>
  DeparserMetadataInfo<METADATA_WIDTH> get_metadata_info_with_default_valid(T &register_block) {
    auto metadata_info = get_metadata_info<T, METADATA_WIDTH>(register_block);
    metadata_info.default_valid_ = register_block.dflt_vld();

    return metadata_info;
  }

}

#endif // _TOFINOXX_DEPARSER_METADATA_
