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

#ifndef _SHARED_AFC_METADATA_COMMON_
#define _SHARED_AFC_METADATA_COMMON_

#include <cstdint>

namespace MODEL_CHIP_NAMESPACE {

class AFCMetadataCommon {
 public:
  AFCMetadataCommon() : adv_qfc_(0), port_id_(0), qid_(0), credit_(0) { }
  AFCMetadataCommon(uint16_t adv_qfc, uint16_t port_id, uint16_t qid, uint16_t credit)
      : adv_qfc_(adv_qfc), port_id_(port_id), qid_(qid), credit_(credit) {
  }
  ~AFCMetadataCommon() { }

  const uint16_t &getPortId() const;
  const uint16_t &getQid() const;
  const uint16_t &getCredit() const;
  const uint16_t &getAdvQfc() const;

  void setDieId(const uint32_t die_id);
  void setPortId(uint16_t port_id);
  void setQid(uint16_t qid);
  void setCredit(uint16_t credit);
  void setAdvQfc(uint16_t adv_qfc);

  const uint64_t asUint64Val() const;

 private:
  uint16_t adv_qfc_;
  uint16_t port_id_;
  uint16_t qid_;
  uint16_t credit_;
};

}

#endif //_SHARED_AFC_METADATA_COMMON_
