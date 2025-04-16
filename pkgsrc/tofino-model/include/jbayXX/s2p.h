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

#ifndef _JBAYXX_S2P_
#define _JBAYXX_S2P_

#include <model_core/shadow_counter.h>
#include <pipe-object.h>
#include <rmt-defs.h>
#include <register_includes/s2p_reg_pkt_ctr_array_mutable.h>
#include <register_includes/s2p_reg_byte_ctr_array_mutable.h>
#include <register_includes/s2p_reg_ctr_sample_mutable.h>
#include <register_includes/s2p_reg_ctr_time_mutable.h>
#include <register_includes/s2p_reg_pipe_map.h>
#include <register_includes/s2p_reg_copy2cpu.h>

namespace MODEL_CHIP_NAMESPACE {

class Packet;

/**
 * Models a pipe's s2p function.
 */
class S2p : public PipeObject {
 public:
  S2p(RmtObjectManager *om, int pipeIndex);
  void increment_pkt_ctr(int portIndex, uint64_t amount=1);
  void increment_byte_ctr(int portIndex, uint64_t amount=1);
  void map_logical_to_physical(Packet *pkt, bool mirrored);

 private:
  void reset();
  void sample();
  uint8_t map_pipe(uint8_t inpipe);
  uint16_t map_pipe_vector(uint16_t inpipevec);
  uint16_t map_port(uint16_t inport);

  register_classes::S2pRegCtrSampleMutable    ctr_sample_;
  register_classes::S2pRegCtrTimeMutable      ctr_time_;
  register_classes::S2pRegPktCtrArrayMutable  pkt_ctr_;
  register_classes::S2pRegByteCtrArrayMutable byte_ctr_;
  model_core::ShadowCounterArray<RmtDefs::kPortsPerPipe> shadow_pkt_ctr_;
  model_core::ShadowCounterArray<RmtDefs::kPortsPerPipe> shadow_byte_ctr_;
  register_classes::S2pRegPipeMap             pipe_map_;
  register_classes::S2pRegCopy2cpu            copy2cpu_;

};

}

#endif //_JBAYXX_S2P_
