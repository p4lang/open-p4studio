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

#ifndef _TOFINOXX_JBAY_AFC_METADATA_
#define _TOFINOXX_JBAY_AFC_METADATA_

#include <afc-metadata-common.h>

namespace MODEL_CHIP_NAMESPACE {

class AFCMetadata : public AFCMetadataCommon {
 public:
  AFCMetadata() : AFCMetadataCommon() { }
  AFCMetadata(uint32_t phv32)
      : AFCMetadataCommon( ((phv32 >> 31) & ((0x1u <<  1) - 1)),    // qfc
                           ((phv32 >> 22) & ((0x1u <<  9) - 1)),    // portId
                           ((phv32 >> 15) & ((0x1u <<  7) - 1)),    // qid
                           ((phv32 >>  0) & ((0x1u << 15) - 1)) ) { // credit
  }
  AFCMetadata(uint32_t phv32, const uint32_t die_id) : AFCMetadata(phv32) {
    setDieId(die_id);
  }
  ~AFCMetadata() { }
};

}

#endif //_TOFINOXX_JBAY_AFC_METADATA_
