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

#ifndef _TOFINOXX_DEPARSER_CHIP_REG_
#define _TOFINOXX_DEPARSER_CHIP_REG_

// Deparser registers that are specific to tofinoXX
#include <register_includes/dprsr_ipipe_remap_r.h>
#include <register_includes/dprsr_epipe_remap_r.h>

namespace MODEL_CHIP_NAMESPACE {

class DeparserChipReg {

 public:
  DeparserChipReg(int chip, int pipe);
  virtual ~DeparserChipReg();
  void Reset();

  uint8_t get_ipipe_remap() {
    return ipipe_remap_.map();
  }

  uint8_t get_epipe_remap() {
    return epipe_remap_.map();
  }


 private:
  register_classes::DprsrIpipeRemapR            ipipe_remap_;
  register_classes::DprsrEpipeRemapR            epipe_remap_;
};
}

#endif // _TOFINOXX_DEPARSER_CHIP_REG_
