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

#include <rmt-event-emitter.h>
#include <rmt-object-manager.h>

namespace MODEL_CHIP_NAMESPACE {

RmtEventEmitter::RmtEventEmitter(RmtObjectManager *om, const int rmt_type)
    : om_(om), rmt_type_(rmt_type) {}

RmtEventEmitter::~RmtEventEmitter() {}

void RmtEventEmitter::log_phv(const Phv& phv) {
  om_->log_phv(rmt_type_, phv);
}

void RmtEventEmitter::log_packet(const Packet& packet) {
  om_->log_packet(rmt_type_, packet);
}

}  // namespace MODEL_CHIP_NAMESPACE
