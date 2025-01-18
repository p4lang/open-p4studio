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

#include <p2s.h>
#include <port.h>
#include <register_utils.h>
#include <rmt-object-manager.h>

namespace MODEL_CHIP_NAMESPACE {

P2s::P2s(RmtObjectManager *om, int pipeIndex) :
  PipeObject(om, pipeIndex),
  ctr_sample_(pipe_adapter(ctr_sample_, chip_index(), pipeIndex, [this]() { sample(); })),
  ctr_time_(pipe_adapter(ctr_time_, chip_index(), pipeIndex)),
  pkt_ctr_(pipe_adapter(pkt_ctr_, chip_index(), pipeIndex)),
  byte_ctr_(pipe_adapter(byte_ctr_, chip_index(), pipeIndex)),
  // shadow counter constructors take a lambda that will be called for each
  // channel index when the shadow counter sample() method is called; the
  // lambda sets the corresponding register value.
  shadow_pkt_ctr_(
      model_core::ShadowCounterArray<RmtDefs::kPortsPerPipe>(
          36, [this](uint32_t chanIndex, uint64_t value) {
            pkt_ctr_.cnt(chanIndex, value);
          })),
  shadow_byte_ctr_(
      model_core::ShadowCounterArray<RmtDefs::kPortsPerPipe>(
          48, [this](uint32_t chanIndex, uint64_t value) {
            byte_ctr_.cnt(chanIndex, value);
          })) {
  reset();
}

void P2s::reset() {
  ctr_sample_.reset();
  ctr_time_.reset();
  pkt_ctr_.reset();
  byte_ctr_.reset();
}

void P2s::sample() {
  if (1u == ctr_sample_.sample()) {
    shadow_pkt_ctr_.sample();
    shadow_byte_ctr_.sample();
    ctr_time_.time(get_object_manager()->time_get_cycles());
    ctr_sample_.sample(0u);
  }
}

void P2s::increment_pkt_ctr(int portIndex, uint64_t amount) {
  int chanIndex = get_local_port_index(portIndex);
  shadow_pkt_ctr_.increment(chanIndex, amount);
}

void P2s::increment_byte_ctr(int portIndex, uint64_t amount) {
  int chanIndex = get_local_port_index(portIndex);
  shadow_byte_ctr_.increment(chanIndex, amount);
}

}
