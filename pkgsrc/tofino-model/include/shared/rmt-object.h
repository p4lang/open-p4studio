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

#ifndef _SHARED_RMT_OBJECT_
#define _SHARED_RMT_OBJECT_

#include <iostream>
#include <string>


namespace MODEL_CHIP_NAMESPACE {

class RmtObjectManager;

class RmtObject {

 public:
    RmtObject(RmtObjectManager *om);
    virtual ~RmtObject();
    RmtObject(const RmtObject& other) = delete;  // XXX

    RmtObjectManager *get_object_manager() const;

    int chip_index() const;

    static constexpr uint8_t chip_type() {
      return RmtDefs::kChipType;
    }
    static constexpr bool is_tofinoA0() {
      return (chip_type() == model_core::ChipType::kTofinoA0);
    }
    static constexpr bool is_tofino() {
      return is_tofinoA0();
    }
    static constexpr bool is_tofinoB0() {
      return (chip_type() == model_core::ChipType::kTofinoB0);
    }
    static constexpr bool is_tofinoXX() {
      return is_tofinoA0() || is_tofinoB0();
    }
    static constexpr bool is_jbayA0() {
      return (chip_type() == model_core::ChipType::kJbayA0);
    };
    static constexpr bool is_jbay() {
      return is_jbayA0();
    };
    static constexpr bool is_jbayB0() {
      return (chip_type() == model_core::ChipType::kJbayB0);
    };
    static constexpr bool is_jbayXX() {
      return is_jbay() || is_jbayB0();
    }
    static constexpr bool is_chip1() {
      return (chip_type() == model_core::ChipType::kRsvd0);
    };
    static constexpr bool is_chip1XX() {
      return is_chip1();
    };
    static constexpr bool is_chip2() {
      return (chip_type() == model_core::ChipType::kRsvd3);
    };
    static constexpr bool is_chip2XX() {
      return is_chip2();
    };
    static constexpr bool is_chip2_or_later() {
      return is_chip2XX();
    };
    static constexpr bool is_chip1_or_later() {
      return is_chip1XX() || is_chip2_or_later();
    };
    static constexpr bool is_jbay_or_later() {
      return is_jbayXX() || is_chip1_or_later();
    };
    static constexpr bool is_tofino_or_later() {
      return is_tofinoXX() || is_jbay_or_later();
    };

 private:
    RmtObject& operator=(const RmtObject&){ return *this; } // XXX
    RmtObjectManager *om_ = NULL;
  };

// sanity check
static_assert(RmtObject::is_tofino_or_later(), "Unknown chip type");

}

#endif // _SHARED_RMT_OBJECT_
