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

#ifndef _SHARED_MAC_H_
#define _SHARED_MAC_H_

#include <cstdint>
#include <cinttypes>

#include <common/rmt-assert.h>
#include <mac-channel.h>


namespace MODEL_CHIP_NAMESPACE {

// Define types for possible Mac subclasses
// Note, we obfuscate the MAC vendor name somewhat
struct MacType {
  static constexpr int kNone   = 0;
  static constexpr int kVmacC3 = 1; // ComiraUmac3
  static constexpr int kVmacC4 = 2; // ComiraUmac4
  static constexpr int kVmacT1 = 3; // TambaMac
};


class Packet;

class Mac {
 public:
  static constexpr int kMacChannelMax = RmtDefs::kMacChannelMax;

  Mac(int n_chans) : chans_(), n_chans_(n_chans) {
    RMT_ASSERT((n_chans > 0) && (n_chans <= kMacChannelMax));
  }
  virtual ~Mac() { reset(); }


  // COUNTER handling - not much else implemented at the moment

  bool is_chan_valid(int chan) {
    return ((chan >= 0) && (chan < n_chans_));
  }
  // Read counter value if MacChannel obj exists (else return 0)
  uint64_t read_counter(bool rx, int chan, int i, bool clear) {
    MacChannel *ch = lookup_chan(chan);
    return (ch != nullptr) ?ch->read_counter(rx, i, clear) :UINT64_C(0);
  }
  uint64_t read_counter_range(bool rx, int chan, int a, int z, bool clear) {
    MacChannel *ch = lookup_chan(chan);
    return (ch != nullptr) ?ch->read_counter_range(rx, a, z, clear) :UINT64_C(0);
  }
  uint64_t read_counter_mask(bool rx, int chan,
                             int base1, uint32_t mask1, int base2, uint32_t mask2,
                             bool clear, bool subtract) {
    MacChannel *ch = lookup_chan(chan);
    if (ch == nullptr) return UINT64_C(0);
    return ch->read_counter_mask(rx, base1, mask1, base2, mask2, clear, subtract);
  }
  // Increment ALL applicable counters for Gress/Chan based on len/pri/flags
  // Create MacChannel obj if it doesn't yet exist
  void increment_counters(bool rx, int chan, int len, uint32_t pri, uint32_t flags) {
    if (is_chan_valid(chan)) get_chan(chan)->increment_counters(rx, len, pri, flags);
  }
  void increment_counters(bool rx, int chan, Packet *pkt, uint32_t flags) {
    if (is_chan_valid(chan)) get_chan(chan)->increment_counters(rx, pkt, flags);
  }
  // Clear all counters for Gress/Chan - no create
  void clear_counters(bool rx, int chan) {
    MacChannel *ch = lookup_chan(chan);
    if (ch != nullptr) ch->clear_counters(rx);
  }
  // Some convenience funcs
  void clear_counters(int chan) {
    clear_counters(true, chan); clear_counters(false, chan);
  }
  void increment_rx_counters(int chan, Packet *pkt, uint32_t flags) {
    increment_counters(true, chan, pkt, flags);
  }
  void increment_tx_counters(int chan, Packet *pkt, uint32_t flags) {
    increment_counters(false, chan, pkt, flags);
  }


  // Subclasses should implement these counter funcs themselves
  virtual uint64_t mac_counter_read(int chan, int cntr_index, bool clear) {
    // Default impl - linear offsets - RX counters then TX counters
    if (cntr_index < 0) return UINT64_C(0xFFFFFFFFFFFFFFFF);
    if (cntr_index < MacCounters::kTotCounters) // Assume RX
      return read_counter(true, chan, cntr_index, clear);
    cntr_index -= MacCounters::kTotCounters;
    if (cntr_index < MacCounters::kTotCounters) // Assume TX
      return read_counter(false, chan, cntr_index, clear);
    return UINT64_C(0xFFFFFFFFFFFFFFFF);
  }
  virtual uint32_t mac_counter_base_addr() const { return 0xFFFFFFFFu; }
  virtual uint32_t mac_stride()            const { return 0xFFFFFFFFu; }
  virtual int      mac_block()             const { return -1; }
  virtual int      mac_type()              const { return MacType::kNone; }


 private:
  void reset() {
    for (int i = 0; i < kMacChannelMax; i++) {
      if (chans_[i] != nullptr) delete chans_[i];
      chans_[i] = nullptr;
    }
  }
  MacChannel *lookup_chan(int chan) {
    return (is_chan_valid(chan)) ?chans_[chan % n_chans_] :nullptr;
  }
  MacChannel *get_chan(int chan) {
    if (!is_chan_valid(chan)) return nullptr;
    // Create MacChannel on demand
    MacChannel *ch = lookup_chan(chan);
    if (ch != nullptr) return ch;
    ch = new MacChannel();
    chans_[chan] = ch;
    return ch;
  }


 private:
  std::array< MacChannel*, kMacChannelMax >  chans_;
  int                                        n_chans_;

}; // Class MacCounters


}

#endif // _SHARED_MAC_H_
