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

#include <afc-metadata.h>

namespace MODEL_CHIP_NAMESPACE {

  const uint16_t &AFCMetadataCommon::getPortId() const {
    return port_id_;
  }
  const uint16_t &AFCMetadataCommon::getQid() const {
    return qid_;
  }
  const uint16_t &AFCMetadataCommon::getCredit() const {
    return credit_;
  }
  const uint16_t &AFCMetadataCommon::getAdvQfc() const {
    return adv_qfc_;
  }

  void AFCMetadataCommon::setDieId(const uint32_t die_id) {
    port_id_ &= 0x1FFu;
    port_id_ |= (die_id << 9u);
  }
  void AFCMetadataCommon::setPortId(uint16_t port_id) {
    port_id_ = port_id;
  }
  void AFCMetadataCommon::setQid(uint16_t qid) {
    qid_ = qid;
  }
  void AFCMetadataCommon::setCredit(uint16_t credit) {
    credit_ = credit;
  }
  void AFCMetadataCommon::setAdvQfc(uint16_t adv_qfc) {
    adv_qfc_ = adv_qfc;
  }

  const uint64_t AFCMetadataCommon::asUint64Val() const {
    // for model only, return a 64 bit representation of metadata
    uint64_t val = (static_cast<uint64_t>(adv_qfc_) << 48u) |
                   (static_cast<uint64_t>(port_id_) << 32u) |
                   (static_cast<uint64_t>(qid_)     << 16u) |
                   (static_cast<uint64_t>(credit_)  << 0);
    return val;
  }
}
