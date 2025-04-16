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

#include <iostream>
#include <tm-sch-port-arb.h>
#include <cstring>

namespace MODEL_CHIP_NAMESPACE {

TmSchPortArb::TmSchPortArb(RmtObjectManager *om, uint8_t pipe_index) : TmObject(om, pipe_index) {
  // No Winner
  prev_winner = 0xff;

  // Zero out all arrays
  memset(port_mode, 0, sizeof(port_mode));
  memset(port_rdy, 0, TmDefs::kNumPortPerPipe*sizeof(bool));
  memset(port_ptr, 0, sizeof(port_ptr));
  mac_ptr = 0;
}

void TmSchPortArb::set_port_mode(uint8_t port_id, uint8_t mode) {
  port_mode[port_id] = mode;
  RMT_ASSERT(port_mode[port_id] <= 7);
}

void TmSchPortArb::set_port_rdy(uint8_t port_id, bool rdy) {
  port_rdy[port_id] = rdy;
}

uint8_t TmSchPortArb::get_port_mask(uint8_t port_mode) {
  switch(port_mode) {
  case 7:
    // 400G
    return(0xf8);
    break;
  case 6:
    // 200G
    return(0xfc);
    break;
  case 5:
    // 100G
    return(0xfe);
    break;
  default:
    // 50G and lower
    return(0xff);
  }
}

uint8_t TmSchPortArb::process_mac(uint8_t mac_id) {
  RMT_ASSERT(mac_id < TmDefs::kNumMacPerPipe);
  uint8_t temp_winner = 0xff;
  uint8_t port_pointer = port_ptr[mac_id];
  uint8_t base_port_idx = (mac_id * TmDefs::kNumPortPerMac);
  for (int num_port = 0; num_port < TmDefs::kNumPortPerMac; num_port++) {
    // Calculate port idx in pipe
    uint8_t port_idx = base_port_idx + port_pointer;
    RMT_ASSERT(port_idx < TmDefs::kNumPortPerPipe);
    // Find a port in MAC which is ready
    if (port_rdy[port_idx]) {
      temp_winner = port_idx & get_port_mask(port_mode[port_idx]);

      // Same port cannot win in back to back clocks
      if (temp_winner == prev_winner) {
        // Move pointer to next port
        port_pointer = (port_pointer + 1) % TmDefs::kNumPortPerMac;
        temp_winner = 0xff;
        continue;
      }
    }
  }
  return temp_winner;
}

uint8_t TmSchPortArb::do_port_selection() {
  uint8_t final_winner = 0xff;

  for(int num_mac = 0; num_mac < TmDefs::kNumMacPerPipe; num_mac++) {

    // Check if current MAC has a winner
    final_winner = process_mac(mac_ptr);

    // Move pointer to next mac
    mac_ptr = ((mac_ptr + 1) % TmDefs::kNumMacPerPipe);

    // Check if winner found
    if (final_winner != 0xff)
      break;
  }

  // Set prev winner for next clk
  prev_winner = final_winner;

  return(final_winner);
}

}
