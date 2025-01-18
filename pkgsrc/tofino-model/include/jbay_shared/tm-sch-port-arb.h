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

#ifndef __JBAY_SHARED_TM_SCH_PORT_ARB__
#define __JBAY_SHARED_TM_SCH_PORT_ARB__
#include <tm-object.h>
#include <tm-defines.h>

namespace MODEL_CHIP_NAMESPACE {

class TmSchPortArb : TmObject {
 public:
  // Constructor
  TmSchPortArb(RmtObjectManager *om, uint8_t pipe_index);
  void set_port_mode(uint8_t port_id, uint8_t cfg);
  void set_port_rdy(uint8_t port_id, bool rdy);
  uint8_t do_port_selection();

 private:
  // Winner of last cycle
  uint8_t prev_winner;

  // Data Structures
  uint8_t port_mode[TmDefs::kNumPortPerPipe];
  bool port_rdy[TmDefs::kNumPortPerPipe];

  // Variables
  uint8_t port_ptr[TmDefs::kNumMacPerPipe];
  uint8_t mac_ptr;

  // Functions
  uint8_t process_mac(uint8_t mac_id);
  uint8_t get_port_mask(uint8_t port_mode);

};

}
#endif
