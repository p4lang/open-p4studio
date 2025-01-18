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

#ifndef _JBAYXX_P2S_H_
#define _JBAYXX_P2S_H_

#include <model_core/shadow_counter.h>
#include <pipe-object.h>
#include <rmt-defs.h>
#include <register_includes/p2s_reg_pkt_ctr_array_mutable.h>
#include <register_includes/p2s_reg_byte_ctr_array_mutable.h>
#include <register_includes/p2s_reg_ctr_sample_mutable.h>
#include <register_includes/p2s_reg_ctr_time_mutable.h>

namespace MODEL_CHIP_NAMESPACE {

class P2s : public PipeObject {
 public:
  P2s(RmtObjectManager *om, int pipeIndex);
  void increment_pkt_ctr(int portIndex, uint64_t amount=1);
  void increment_byte_ctr(int portIndex, uint64_t amount=1);

 private:
  void reset();
  void sample();

  register_classes::P2sRegCtrSampleMutable    ctr_sample_;
  register_classes::P2sRegCtrTimeMutable      ctr_time_;
  register_classes::P2sRegPktCtrArrayMutable  pkt_ctr_;
  register_classes::P2sRegByteCtrArrayMutable byte_ctr_;

  model_core::ShadowCounterArray<RmtDefs::kPortsPerPipe> shadow_pkt_ctr_;
  model_core::ShadowCounterArray<RmtDefs::kPortsPerPipe> shadow_byte_ctr_;
};

}

#endif //_JBAYXX_P2S_H_
