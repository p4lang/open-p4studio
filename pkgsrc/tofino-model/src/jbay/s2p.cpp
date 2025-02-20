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

#include <s2p.h>
#include <port.h>
#include <packet.h>
#include <register_utils.h>
#include <rmt-object-manager.h>

namespace MODEL_CHIP_NAMESPACE {


S2p::S2p(RmtObjectManager *om, int pipeIndex) :
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
          })),
  pipe_map_(pipe_adapter(pipe_map_, chip_index(), pipeIndex)),
  // copy2cpu instantiated but not attached to any logic
  copy2cpu_(pipe_adapter(copy2cpu_, chip_index(), pipeIndex)) {

  reset();
}

void S2p::reset() {
  ctr_sample_.reset();
  ctr_time_.reset();
  pkt_ctr_.reset();
  byte_ctr_.reset();
  pipe_map_.reset();
  copy2cpu_.reset();
}

void S2p::sample() {
  if (1u == ctr_sample_.sample()) {
    shadow_pkt_ctr_.sample();
    shadow_byte_ctr_.sample();
    ctr_time_.time(get_object_manager()->time_get_cycles());
    ctr_sample_.sample(0u);
  }
}

void S2p::increment_pkt_ctr(int portIndex, uint64_t amount) {
  int chanIndex = get_local_port_index(portIndex);
  shadow_pkt_ctr_.increment(chanIndex, amount);
}

void S2p::increment_byte_ctr(int portIndex, uint64_t amount) {
  int chanIndex = get_local_port_index(portIndex);
  shadow_byte_ctr_.increment(chanIndex, amount);
}

uint8_t S2p::map_pipe(uint8_t inpipe) {
  uint8_t outpipe = pipe_map_.pipe(inpipe & RmtDefs::kPipeMask) & RmtDefs::kPipeMask;
  return outpipe;
}
uint16_t S2p::map_pipe_vector(uint16_t inpipevec) {
  uint16_t outpipevec = 0;
  for (int p = 0; p < RmtDefs::kPipesMax; p++) {
    if (((inpipevec >> p) & 1) == 1) outpipevec |= (1 << map_pipe(p));
  }
  return outpipevec;
}
uint16_t S2p::map_port(uint16_t inport) {
  int inpipe = Port::get_pipe_num(inport);
  int outpipe = map_pipe(inpipe);
  uint16_t outport = Port::swap_pipe(inport, outpipe);
  return outport;
}

void S2p::map_logical_to_physical(Packet *pkt, bool mirrored) {
  // XXX: implement S2P logical->physical pipe mapping
  I2QueueingMetadata *i2q = pkt->i2qing_metadata();
  if (is_jbayA0()) {
    // XXX: duplicate JBayA0 bug - also map pipe in physical ingress port
    i2q->set_physical_ingress_port( map_port(i2q->physical_ingress_port()) );
  }
  // RTL maps pipe in egress_uc_port unconditionally, regardless of valid
  i2q->set_egress_unicast_port_only( map_port(i2q->egress_uc_port_regardless()) );

  uint16_t egress_pipe_vector = i2q->multicast_pipe_vector();
  if (is_jbayB0() && mirrored && i2q->cpu_needs_copy()) {
    // Augment egress_pipe_vec with value from S2P CSR if:
    // a) JBayB0 b) mirrored pkt c) copy-to-cpu bit set in i2q meta
    egress_pipe_vector |= static_cast<uint16_t>(copy2cpu_.pipe_vec());
  }
  // Then map logical to physical
  i2q->set_multicast_pipe_vector( map_pipe_vector(egress_pipe_vector) );

  // XXX: sanity check no pipe->TM mapping required
  static_assert( (RmtDefs::kPipeTmLocalPortShift == 0), "Unexpected Pipe->TM port map needed");
}


}
