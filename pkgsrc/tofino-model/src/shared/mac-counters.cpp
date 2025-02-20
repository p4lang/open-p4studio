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

#include <string>
#include <rmt-log.h>
#include <rmt-object-manager.h>
#include <mac-counters.h>
#include <packet.h>


namespace MODEL_CHIP_NAMESPACE {

void MacCounters::reset() {
  for (int i = 0; i < kTotCounters; i++) counters_[i] = UINT64_C(0);
}
uint64_t MacCounters::do_get(int cntr) const {
  RMT_ASSERT((cntr >= 0) && (cntr < kTotCounters));
  //printf("counters_[%d] is %" PRId64 "\n", cntr, counters_[cntr]);
  return counters_[cntr];
}
void MacCounters::do_set(int cntr, uint64_t val) {
  RMT_ASSERT((cntr >= 0) && (cntr < kTotCounters));
  //printf("counters_[%d] = %" PRId64 "\n", cntr,val);
  counters_[cntr] = val;
}
void MacCounters::do_zero(int cntr) {
  RMT_ASSERT((cntr >= 0) && (cntr < kTotCounters));
  do_set(cntr, UINT64_C(0));
}
void MacCounters::do_inc(int cntr, uint64_t val) {
  // NO check for wrap - would take 1000s of years at even 100x RefModel PPS
  RMT_ASSERT((cntr >= 0) && (cntr < kTotCounters));
  //printf("counters_[%d] += %" PRId64 "\n", cntr,val);
  counters_[cntr] += val;
}
uint64_t MacCounters::read_counter_mask_nolock(int base, uint32_t mask, bool clear) {
  uint64_t v = UINT64_C(0);
  int bot = __builtin_ffsl(mask) - 1;
  if ((base >= 0) && (base < kTotCounters) && (bot >= 0)) {
    RMT_ASSERT(mask != 0u);
    int top = __builtin_clzl(1u) - __builtin_clzl(mask);
    for (int i = bot; i <= top; i++) {
      if (((mask >> i) & 1) == 1) {
        int j = base + i;
        if (j < kTotCounters) {
          v += do_get(j);
          if (clear) do_zero(j);
        }
      }
    }
  }
  return v;
}


uint64_t MacCounters::read_counter(int i, bool clear) {
  if ((i < 0) || (i >= kTotCounters)) return UINT64_C(0);
  spinlock_.lock();
  uint64_t v = do_get(i);
  if (clear) do_zero(i);
  spinlock_.unlock();
  return v;
}
uint64_t MacCounters::read_counter_range(int a, int z, bool clear) {
  if ( (a < 0) || (a >= kTotCounters) ||
       (z < 0) || (z >= kTotCounters) || (a > z)) return UINT64_C(0);
  spinlock_.lock();
  uint64_t v = UINT64_C(0);
  for (int i = a; i <= z; i++) {
    v += do_get(i);
    if (clear) do_zero(i);
  }
  spinlock_.unlock();
  return v;
}
uint64_t MacCounters::read_counter_mask(int base, uint32_t mask, bool clear) {
 spinlock_.lock();
 uint64_t v = read_counter_mask_nolock(base, mask, clear);
 spinlock_.unlock();
 return v;
}
uint64_t MacCounters::read_counter_mask(int base1, uint32_t mask1, int base2, uint32_t mask2,
                                        bool clear, bool subtract) {
 spinlock_.lock();
 uint64_t v1 = read_counter_mask_nolock(base1, mask1, clear);
 uint64_t v2 = read_counter_mask_nolock(base2, mask2, clear);
 spinlock_.unlock();
 return (subtract) ?( (v1>v2) ?(v1-v2) :UINT64_C(0) ) :(v1+v2);
}
void MacCounters::clear_all() {
  (void)read_counter_range(0, kTotCounters, true);
}


uint32_t MacCounters::infer_flags(int len, uint32_t pri, uint32_t flags) {
  bool got_addrtype  = ((flags & Cntr::kAddrtypeMask) != 0u);
  bool got_payload   = ((flags & Cntr::kPayloadMask) != 0u);
  bool got_error     = ((flags & Cntr::kErrorMask) != 0u);
  bool got_crc_error = ((flags & Cntr::kCrcErrorMask) != 0u);
  if (!got_addrtype)                  flags |= 1u<<Cntr::Unicast;
  if (!got_payload && (pri > 0))      flags |= 1u<<Cntr::Vlan; // Assume VLAN if pri>0
  // We track size 64 and size range [1519..1535] *twice* hence 3u below.
  // This allows use to safely clear range pairs [64],[65..127]
  //  and [1024..1518],[1519..2047] independently
  if ( len <  64)                     flags |= 1u<<Cntr::Undersized;
  if ( len == 64)                     flags |= 3u<<Cntr::SizeEq64;
  if ((len >= 1519) && (len <= 1535)) flags |= 3u<<Cntr::Size15xx_A;
  if ( len >  9104)                   flags |= 1u<<Cntr::Oversized;
  if (got_crc_error)                  flags |= 1u<<Cntr::CrcError;
  if ( got_error)                     flags |= 1u<<Cntr::Error;
  if (!got_error)                     flags |= 1u<<Cntr::OK;
  flags |= 1u<<Cntr::All;
  return flags;
}
uint32_t MacCounters::flags_from_packet(Packet *pkt, uint32_t *pri) {
  // Examine header of pkt and determine flags/pri accordingly
  RMT_ASSERT(pkt != nullptr);
  if (pri != nullptr) *pri = 0;

  // 26B buffer should be enough to accommodate
  // all hdrs we need to examine
  constexpr int sz = 26;
  uint8_t buf[sz];
  int len = pkt->len();
  int got = pkt->get_buf(buf, 0, sz);
  RMT_ASSERT((got <= len) && (got <= sz));
  uint32_t flags = 0u;
  uint64_t da = 0u, sa = 0u, ll = 0u;
  uint16_t tp = 0u, vl = 0u, op = 0u, pr = 0u;
  int pos = 0;

  // Get DA so we can infer addrtype
  if (len < 6) return flags | (1u<<Cntr::Undersized);
  pos = model_common::Util::fill_val(&da,6, buf,sz, pos);
  if (da == UINT64_C(0xFFFFFFFFFFFF)) {
    flags |= 1u<<Cntr::Broadcast;
  } else if ((da & UINT64_C(0x010000000000)) != UINT64_C(0)) {
    flags |= 1u<<Cntr::Multicast;
  } else {
    flags |= 1u<<Cntr::Unicast;
  }

  // Get SA - not needed for anything at moment
  if (len < 12) return flags | (1u<<Cntr::Undersized);
  pos = model_common::Util::fill_val(&sa,6, buf,sz, pos);

  // Get Ethertype/Len
  if (len < 14) return flags | (1u<<Cntr::Undersized);
  pos = model_common::Util::fill_val(&tp,2, buf,sz, pos);

  if (tp >= 0x800) {
    // Normal Ether Encapsulation
    RMT_ASSERT(pos == 14); // Sanity check
  } else {
    // IEEE 802.2/802.3 LLC Encapsulation
    // Strip 802.2 LLC (3B), Zeros (3B) then get tp (2B) again
    if (len < 20) return flags | (1u<<Cntr::Undersized);
    pos = model_common::Util::fill_val(&ll,6, buf,sz, pos);
    if (len < 22) return flags | (1u<<Cntr::Undersized);
    pos = model_common::Util::fill_val(&tp,2, buf,sz, pos);
    RMT_ASSERT(pos == 22); // Sanity check
  }

  if ((tp == 0x8100) || (tp == 0x9100)) {
    // VLAN TPID - get VLAN TCI (2B), then TCI[15:13] = PCP aka pri
    if (len < pos+2) return flags | (1u<<Cntr::Undersized);
    (void)model_common::Util::fill_val(&vl,2, buf,sz, pos);
    flags |= 1u<<Cntr::Vlan;
    if (pri != nullptr) *pri = (vl >> 13) & 0x7;

  } else if ((tp == 0x8808) && (da == UINT64_C(0x0180C2000001))) {
    // PAUSE or PFC
    if (len < pos+2) return flags | (1u<<Cntr::Undersized);
    pos = model_common::Util::fill_val(&op,2, buf,sz, pos);
    if (op == 0x0101) {
      if (len < pos+2) return flags | (1u<<Cntr::Undersized);
      (void)model_common::Util::fill_val(&pr,2, buf,sz, pos);
      flags |= 1u<<Cntr::PriPause;
      pr &= 0xFF; // Switch on pri[8] if pri[7:0] all zero
      if (pri != nullptr) *pri = ((pr==0) ?0x100 :0) | (pr);
    } else if (op == 0x0001) {
      flags |= 1u<<Cntr::Pause;
    }
  }

  if ( len <  64)                     flags |= 1u<<Cntr::Undersized;
  if ( len == 64)                     flags |= 3u<<Cntr::SizeEq64;
  if ((len >= 1519) && (len <= 1535)) flags |= 3u<<Cntr::Size15xx_A;
  if ( len >  9104)                   flags |= 1u<<Cntr::Oversized;
  return flags;
}

// Increment ALL applicable counters based on len/pri/flags
void MacCounters::increment_counters(int len, uint32_t pri, uint32_t flags) {
  flags = infer_flags(len, pri, flags);
  int frame_top1 = (flags == 0u) ?-1 :__builtin_clz(1u) - __builtin_clz(flags);
  spinlock_.lock();

  // Count frames and octets
  for (int b = 0; b <= frame_top1; b++) {
    if (((flags >> b) & 1) == 1) {
      int f_index = get_frame_index(b);
      if (f_index >= 0) do_inc(f_index, UINT64_C(1));
      int o_index = get_octet_index(b);
      if (o_index >= 0) do_inc(o_index, static_cast<uint64_t>(len));
    }
  }

  // Now count PFC frames or ordinary PRI frames
  if (((flags >> Cntr::PriPause) & 1) == 1) {
    // Here we treat pri as a mask of possible priorities
    for (int b = 0; b < kNumPriPauseCounters; b++) {
      if (((pri >> b) & 1) == 1) {
        int pp_index = get_pripause_index(b);
        if (pp_index >= 0) do_inc(pp_index, UINT64_C(1));
      }
    }
  } else {
    // Here we treat pri as a *single* value not a mask
    int p_index = get_pri_index(pri);
    if (p_index >= 0) do_inc(p_index, UINT64_C(1));
  }

  // Finally map packet length into 1 or 2 buckets
  // incrementing frame count
  int b64_index = get_bucket64_index(len);
  if (b64_index >= 0) do_inc(b64_index, UINT64_C(1));
  int b512_index = get_bucket512_index(len);
  if (b512_index >= 0) do_inc(b512_index, UINT64_C(1));

  spinlock_.unlock();
}

// Augment flags from Packet data - then call function above
void MacCounters::increment_counters(Packet *pkt, uint32_t flags) {
  uint32_t err_flags = flags & Cntr::kErrorMask;     // Just keep error flags
  uint32_t pri = 0u;                                 // Work out pri from pkt
  uint32_t pkt_flags = flags_from_packet(pkt, &pri); // Work out other flags from pkt
  increment_counters(pkt->len(), pri, pkt_flags|err_flags);
}




}
