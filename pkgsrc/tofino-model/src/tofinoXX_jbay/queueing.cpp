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

#include <packet.h>
#include <queueing.h>

namespace MODEL_CHIP_NAMESPACE {

  // Logic below to stub handling of multi-chip packages
  void Queueing::enqueue_read(Packet *pkt, int port) {
    RMT_ASSERT(false && "No Read die on Tofino/JBay");
  }
  void Queueing::enqueue_die(Packet *pkt) {
    RMT_ASSERT(pkt != nullptr);
    RMT_ASSERT(pkt->i2qing_metadata()->tm_vec() == 0);
    enqueue(pkt); // Just call vanilla enqueue
  }

}
