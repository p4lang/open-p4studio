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

#ifndef _SHARED_RMT_EVENT_EMITTER_
#define _SHARED_RMT_EVENT_EMITTER_

namespace MODEL_CHIP_NAMESPACE {

class RmtObjectManager;
class Phv;
class Packet;

class RmtEventEmitter {
 public:
  RmtEventEmitter(RmtObjectManager *om, const int rmt_type);
  virtual ~RmtEventEmitter();

  void log_phv(const Phv& phv);
  void log_packet(const Packet& packet);

 private:
  RmtObjectManager *om_ = nullptr;
  const int rmt_type_;
};

}

#endif  // _SHARED_RMT_EVENT_EMITTER_
