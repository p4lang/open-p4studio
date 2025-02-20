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

#ifndef _SHARED_MAC_CHANNEL_H_
#define _SHARED_MAC_CHANNEL_H_

#include <mac-counters.h>

namespace MODEL_CHIP_NAMESPACE {

class Packet;

class MacChannel {
 public:
  MacChannel() : rx_counters_(), tx_counters_() { }
  virtual ~MacChannel() { }


  // COUNTER handling - not much else implemented at the moment

  uint64_t read_counter(bool rx, int i, bool clear) {
    return get_cntrs(rx).read_counter(i, clear);
  }
  uint64_t read_counter_range(bool rx, int a, int z, bool clear) {
    return get_cntrs(rx).read_counter_range(a, z, clear);
  }
  uint64_t read_counter_mask(bool rx, int base, uint32_t mask, bool clear) {
    return get_cntrs(rx).read_counter_mask(base, mask, clear);
  }
  uint64_t read_counter_mask(bool rx,
                             int base1, uint32_t mask1, int base2, uint32_t mask2,
                             bool clear, bool subtract) {
    return get_cntrs(rx).read_counter_mask(base1, mask1, base2, mask2, clear, subtract);
  }
  void clear_counters(bool rx) {
    get_cntrs(rx).clear_all();
  }
  void increment_counters(bool rx, int len, uint32_t pri, uint32_t flags) {
    get_cntrs(rx).increment_counters(len, pri, flags);
  }
  void increment_counters(bool rx, Packet *pkt, uint32_t flags) {
    get_cntrs(rx).increment_counters(pkt, flags);
  }

 private:
  MacCounters& get_cntrs(bool rx) { return (rx) ?rx_counters_ :tx_counters_; }

 private:
  MacCounters  rx_counters_;
  MacCounters  tx_counters_;

}; // Class MacChannel

}

#endif // _SHARED_MAC_CHANNEL_H_
