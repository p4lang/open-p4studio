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

#include <utests/test_util.h>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

#include "pktgen_util.h"


namespace MODEL_CHIP_TEST_NAMESPACE {

using BV128 = MODEL_CHIP_NAMESPACE::BitVector<128>;
using BV72  = MODEL_CHIP_NAMESPACE::BitVector< 72>;
using PORT  = MODEL_CHIP_NAMESPACE::Port;
using PKT   = MODEL_CHIP_NAMESPACE::Packet;

PktgenRandApp::PktgenRandApp(TestUtil *tu, int pipe, int app, uint64_t seed,
                             const struct PktgenRandAppConfig &cfg, uint8_t pgen_chans)
    : tu_(tu), pipe_(pipe), app_(app), seed_(seed), seed2_(seed), cfg_(cfg),
      pgen_chans_(pgen_chans)
{
  setup_configuration();
}
PktgenRandApp::~PktgenRandApp() {
  disable_configuration();
}

bool PktgenRandApp::pgen_chan_enabled() {
  return (((pgen_chans() >> chan()) & 1) == 1);
}
bool PktgenRandApp::ebuf_linked(int ebuf) {
  assert((ebuf >= TestUtil::kPgenOutPortEbuf0) &&
         (ebuf <= TestUtil::kPgenOutPortEbuf3));
  return (recirc_out_ebuf_ == ebuf);
}
bool PktgenRandApp::dprsr_triggered() {
  bool dprsr_app = (type_ == TestUtil::kPgenAppTypeDprsr);
  return enabled() && pgen_chan_enabled() && dprsr_app;
}
bool PktgenRandApp::recirc_triggered() {
  bool recirc_app = (type_ == TestUtil::kPgenAppTypeRecirc);
  return enabled() && pgen_chan_enabled() && recirc_app;
}
bool PktgenRandApp::recirc_triggered(int out_ebuf) {
  return recirc_triggered() && ebuf_linked(out_ebuf);
}
// deparser is not really packet triggered, it is
//  metadata triggered, but it's easier to mostly
//  treat it as packet triggered
bool PktgenRandApp::pkt_triggered() {
  return dprsr_triggered() || recirc_triggered();
}
bool PktgenRandApp::timeout_triggered() {
  bool tmo_app = ((type_ == TestUtil::kPgenAppTypeTimer) ||
                  (type_ == TestUtil::kPgenAppTypePeriodic));
  return enabled() && pgen_chan_enabled() && tmo_app;
}
bool PktgenRandApp::fifo_full() {
  return (n_triggers_pending() >= kAppFifoSize);
}
bool PktgenRandApp::key_matches(const BV128 &key) {
  if (!pkt_triggered()) return false;
  bool match = key.masked_equals(recirc_val_bv_, recirc_mask_bv_);
  //printf("PktgenRandApp::key_matches Pipe=%d App=%d Key=0x%016" PRIx64 "%016" PRIx64 " "
  //       "val=0x%016" PRIx64 "%016" PRIx64 " "
  //       "mask=0x%016" PRIx64 "%016" PRIx64 " %s\n",
  //       pipe_, app_, key.get_word(64), key.get_word(0),
  //       recirc_val_bv_.get_word(64), recirc_val_bv_.get_word(0),
  //       recirc_mask_bv_.get_word(64), recirc_mask_bv_.get_word(0),
  //       match?"MATCH!!!":"MISS");
  return match;
}
bool PktgenRandApp::make_key_from_pkt(PKT *p, BV128 *bv) {
  assert((p != nullptr) && (bv != nullptr));
  uint8_t  buf[16] = { 0 };
  uint64_t data[2] = { UINT64_C(0) };
  if (p->get_buf(buf, 0, 16) != 16) return false;
  int pos[2] = { model_common::Util::fill_val(&data[0], 8, buf, 16, 0),
                 model_common::Util::fill_val(&data[1], 8, buf, 16, 8) };
  if ((pos[0] != 8) || (pos[1] != 16)) return false;
  bv->set_word(data[0], 64); // first byte goes into bits 127:120
  bv->set_word(data[1],  0);
  return true;
}
uint16_t PktgenRandApp::make_pkt_hdr(uint8_t *hdrbuf,
                                     uint16_t min_hdrlen, uint16_t max_hdrlen,
                                     uint16_t pktlen) {
  assert(hdrbuf != nullptr);
  assert((min_hdrlen >= 4) && (min_hdrlen <= max_hdrlen) && (max_hdrlen <= pktlen));
  uint16_t pay_addr = payload_addr();
  bool from_pkt = ((flags_ & TestUtil::kPgenAppFlagAddrSizeFromPkt) != 0);
  if (from_pkt) assert((pay_addr <= 12) && (min_hdrlen >= pay_addr+3));
  seed2_ = tu_->mmix_rand64(seed2_);
  uint16_t hdrlen = tu_->xrandrange(seed2_, 9999, min_hdrlen, max_hdrlen);
  for (int i = 0; i < max_hdrlen; i++) hdrbuf[i] = (uint8_t)0;
  for (int i = 0; i < hdrlen; i++) hdrbuf[i] = tu_->xrand8(seed2_,10000+i);
  if (!from_pkt) return hdrlen;
  int offs[14] = { 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
  uint16_t addr = tu_->xrandrange(seed2_, 12101,  0,  1020);
  uint16_t size = tu_->xrandrange(seed2_, 12102, 64, (1024-addr)*16);
  uint32_t size_addr = ( ((addr & 0x03FFF) << 0) | ((size & 0x3FFF) << 10) );
  int off0 = offs[pay_addr];
  hdrbuf[off0+0] = static_cast<uint8_t>( (size_addr >>  0) & 0xFF );
  hdrbuf[off0+1] = static_cast<uint8_t>( (size_addr >>  8) & 0xFF );
  hdrbuf[off0+2] = static_cast<uint8_t>( (size_addr >> 16) & 0xFF );
  return hdrlen;
}
bool PktgenRandApp::pkt_matches(PKT *p) {
  BV128 bv(UINT64_C(0));
  if (!make_key_from_pkt(p, &bv)) return false;
  return key_matches(bv);
}
uint64_t PktgenRandApp::n_timeouts_expected() {
  if (!timeout_triggered() || !pgen_chan_enabled()) return UINT64_C(0);
  uint64_t t_now = UINT64_C(0);
  model_timer::ModelTimerGetTime(t_now);
  assert(t_now >= t_setup_);
  uint64_t t_delta = t_now - t_setup_;
  uint64_t n_act_timeouts = n_triggers_act();
  uint64_t n_exp_timeouts = UINT64_C(0);
  if (type_ == TestUtil::kPgenAppTypeTimer)
    n_exp_timeouts = (t_delta >= timer_cycles_) ?UINT64_C(1) :UINT64_C(0);
  else
    n_exp_timeouts = t_delta / timer_cycles_;
  if (n_exp_timeouts == n_act_timeouts) return n_exp_timeouts;

  // Often see expected=actual+-1 with a further periodic call imminent
  // so apply a small fudge factor so we treat this scenario as ok
  uint64_t t_delta2 = t_delta;
  uint64_t t_fudge = UINT64_C(5);
  if (n_act_timeouts < n_exp_timeouts) {
    if (t_delta2 > t_fudge) t_delta2 -= t_fudge; else t_delta2 = UINT64_C(0);
  } else {
    assert(n_act_timeouts > n_exp_timeouts);
    t_delta2 += t_fudge;
  }
  // Recalculate n_exp_timeouts with fudge applied
  if (type_ == TestUtil::kPgenAppTypeTimer)
    n_exp_timeouts = (t_delta2 >= timer_cycles_) ?UINT64_C(1) :UINT64_C(0);
  else
    n_exp_timeouts = t_delta2 / timer_cycles_;
  if (n_exp_timeouts == n_act_timeouts) return n_exp_timeouts;


  // Still mismatching so print out debug to let us figure out better fudge factor
  printf("(%d,%d) Timeouts(exp=%" PRId64 ",act=%" PRId64 ") TimerCycles=%d  "
	 "(Tnow=%" PRId64 ",Tsetup=%" PRId64 ")  Tdelta=%" PRId64 "--fudged-->%" PRId64 "\n",
	 pipe_, app_, n_exp_timeouts, n_act_timeouts, timer_cycles_,
	 t_now, t_setup_, t_delta, t_delta2);
  return n_exp_timeouts;
}
bool PktgenRandApp::portdown_triggered(int port) {
  if ((port < 0) || (port >= kNumPortsPerPipe)) return false;
  if (! enabled() ) return false;
  if (! pgen_chan_enabled() ) return false;
  if (type_ != TestUtil::kPgenAppTypeLinkdown) return false;
  // check if this port down already triggered
  if ( tu_->pktgen_cmn_portdown_disable_get(pipe_,port) ) return false;
  int mask = ((flags_ & TestUtil::kPgenAppFlagUsePortDownMask1) != 0) ?1 :0;
  BV72 bv = tu_->pktgen_cmn_portdown_ctrl_get(pipe_, mask);
  return bv.bit_set(port);
}
int PktgenRandApp::header_len() { // in bytes
  bool no_key = ((flags_ & TestUtil::kPgenAppFlagNoKey) != 0);
  switch (type_) {
    case TestUtil::kPgenAppTypeTimer:    return 6;
    case TestUtil::kPgenAppTypePeriodic: return 6;
    case TestUtil::kPgenAppTypeLinkdown: return 6;
    case TestUtil::kPgenAppTypeRecirc:   return no_key ?6 :22;
    case TestUtil::kPgenAppTypeDprsr:    return no_key ?6 :22;
    case TestUtil::kPgenAppTypePfc:      return 22;
    default: assert(0);
  }
}

bool PktgenRandApp::pick_enabled() {
  return (tu_->xrand8(seed_, 2001, 3) != 0); // enable 7 in 8
}
uint8_t PktgenRandApp::pick_type() {
  assert((cfg_.maskTypes & TestUtil::kPgenAppTypeMask) != 0);
  if (__builtin_popcount(static_cast<unsigned int>(cfg_.maskTypes)) > 1) {
    for (int i = 0; i <= 100; i++) {
      uint8_t type = tu_->xrandrange(seed_, 1900+i,
                                     TestUtil::kPgenAppTypeMin, TestUtil::kPgenAppTypeMax);
      if (((cfg_.maskTypes >> type) & 1) == 1) return type;
    }
  }
  return __builtin_ffs(static_cast<int>(cfg_.maskTypes)) - 1;
}
uint8_t PktgenRandApp::pick_flags() {
  return (tu_->xrand8(seed_, 2002) & cfg_.maskFlags) | cfg_.setFlags;
}
uint8_t PktgenRandApp::pick_chan()  {
  int chan = tu_->xrandrange(seed_, 2003,
                             TestUtil::kPgenAppChanMin, TestUtil::kPgenAppChanMax);
  // XXX: WIP: use EVEN chans only
  if (MODEL_CHIP_NAMESPACE::RmtObject::is_chip1()) chan = (chan / 2) * 2;
  return static_cast<uint8_t>(chan);
}
uint8_t PktgenRandApp::pick_prio() {
  // TODO: allow arb priority
  //return tu_->xrand8(seed_, 2004, 2);
  return 0;
}
uint16_t PktgenRandApp::pick_payload_addr(bool from_pkt) {
  return tu_->xrandrange(seed_, 2101, 0, (from_pkt) ?12 :1020);
}
uint16_t PktgenRandApp::pick_payload_size(uint16_t addr) {
  return tu_->xrandrange(seed_, 2102, 64, (1024-addr)*16);
}
uint8_t PktgenRandApp::pick_ing_port() {
  int chan = tu_->xrandrange(seed_, 2201, 0, 71);
  // XXX: WIP: use EVEN chans only
  if (MODEL_CHIP_NAMESPACE::RmtObject::is_chip1()) chan = (chan / 2) * 2;
  return static_cast<uint8_t>(chan);
}
bool    PktgenRandApp::pick_ing_inc()          { return tu_->xrandbool(seed_,  2202);        }
uint8_t PktgenRandApp::pick_ing_wrap_port(uint8_t ing_port) {
  int chan = tu_->xrandrange(seed_, 2203, ing_port, 71);
  // XXX: WIP: use EVEN chans only
  if (MODEL_CHIP_NAMESPACE::RmtObject::is_chip1()) chan = (chan / 2) * 2;
  return static_cast<uint8_t>(chan);
}
uint8_t PktgenRandApp::pick_ing_port_pipe_id() { return tu_->xrandrange(seed_, 2204, 0, 3);  }

uint8_t PktgenRandApp::pick_recirc_out_ebuf()  { return tu_->xrandrange(seed_, 2301, 0, 3);  }
BV128   PktgenRandApp::pick_recirc_val() {
  BV128 bv(UINT64_C(0));
  bv.set_word(tu_->xrand64(seed_, 2302),  0);
  bv.set_word(tu_->xrand64(seed_, 2303), 64);
  return bv;
}
BV128  PktgenRandApp::pick_recirc_mask(bool addrsize_from_pkt, uint16_t addr) {
  assert((cfg_.maskBits >= 0) && (cfg_.maskBits <= 64));
  if (addrsize_from_pkt) assert(addr <= 12);
  int offs[14] = { 104, 96, 88, 80, 72, 64, 56, 48, 40, 32, 24, 16, 8, 0 };
  int avoidLo = (addrsize_from_pkt) ?offs[addr]+0  : 999;
  int avoidHi = (addrsize_from_pkt) ?offs[addr]+23 :-999;
  BV128 bv(UINT64_C(0));
  // Avoid certain 24b range of 128b header if getting addr/size from pkt
  // (allows us to put anything we like in there and not affect match)
  int bitcnt = 0;
  for (int i = 0; ((i <= 100) && (bitcnt < cfg_.maskBits)); i++) {
    int bitpos = tu_->xrandrange(seed_, 2400+i, 0, 127);
    if ((bitpos >= avoidLo) && (bitpos <= avoidHi)) {
      // Bit chosen in prohibited range - try again
    } else {
      bv.set_bit(bitpos);
      bitcnt++;
    }
  }
  return bv;
}

uint16_t PktgenRandApp::pick_num_batches(bool limit_batches) {
  assert ((cfg_.maxBatches > 0) && (cfg_.maxBatchesPackets >= cfg_.maxBatches));
  return tu_->xrandrange(seed_, 2601, 0, (limit_batches) ?0 :cfg_.maxBatches-1);
}
uint16_t PktgenRandApp::pick_num_pkts(bool limit_pkts, uint16_t num_batches) {
  assert ((cfg_.maxPackets > 0) && (cfg_.maxBatchesPackets >= cfg_.maxPackets));
  if (num_batches == 0) num_batches = 1;
  uint16_t max_packets = cfg_.maxBatchesPackets / num_batches;
  if (cfg_.maxPackets < max_packets) max_packets = cfg_.maxPackets;
  return tu_->xrandrange(seed_, 2602, 0, (limit_pkts) ?0 :max_packets-1);
}

uint32_t PktgenRandApp::pick_ipg_min() {
  assert(cfg_.minPeriod < 0x7FFFFFFF);
  assert(cfg_.minPeriod <= cfg_.maxPeriod);
  return tu_->xrandrange(seed_, 2603, cfg_.minPeriod, cfg_.maxPeriod);
}
uint32_t PktgenRandApp::pick_ipg_max(uint32_t ipg_min) {
  assert(cfg_.maxPeriod <= 0x7FFFFFFF);
  assert(cfg_.minPeriod <= cfg_.maxPeriod);
  uint32_t abs_max = ipg_min + TestUtil::kPgenAppIpgJitterMax;
  uint32_t max = (cfg_.maxPeriod > abs_max) ?abs_max :cfg_.maxPeriod;
  return tu_->xrandrange(seed_, 2604, ipg_min, max);
}
uint32_t PktgenRandApp::pick_ibg_min(uint32_t ipg_max, uint16_t num_packets) {
  assert(cfg_.minPeriod < 0x7FFFFFFF);
  assert(cfg_.minPeriod <= cfg_.maxPeriod);
  return tu_->xrandrange(seed_, 2605, cfg_.minPeriod, cfg_.maxPeriod);
}
uint32_t PktgenRandApp::pick_ibg_max(uint32_t ibg_min, uint16_t num_packets) {
  assert(cfg_.maxPeriod <= 0x7FFFFFFF);
  assert(cfg_.minPeriod <= cfg_.maxPeriod);
  uint32_t abs_max = ibg_min + TestUtil::kPgenAppIbgJitterMax;
  uint32_t cfg_max = cfg_.maxPeriod;
  uint32_t max = (cfg_max > abs_max) ?abs_max :cfg_max;
  return tu_->xrandrange(seed_, 2606, ibg_min, max);
}
uint32_t PktgenRandApp::pick_timer_cycles(uint32_t ipg_max, uint16_t num_packets,
                                          uint32_t ibg_max, uint16_t num_batches) {
  assert(cfg_.minPeriod < 0x7FFFFFFF);
  assert(cfg_.minPeriod <= cfg_.maxPeriod);
  uint32_t delay = (ipg_max * (num_packets + 1)) + (ibg_max * (num_batches + 1));
  uint32_t tc_min = delay * 1;
  uint32_t tc_max = delay * 2;
  return tu_->xrandrange(seed_, 2701, tc_min, tc_max);
}

BV72 PktgenRandApp::pick_portdown_mask(int which) {
  const bool rsvd0 = MODEL_CHIP_NAMESPACE::RmtObject::is_chip1();
  int inc = rsvd0 ?2: 1; // XXX: WIP: only set even ports
  BV72 bv(UINT64_C(0));
  for (int i = 0; i < 72; i += inc) {
    // set about 90% of the bits (even bits in case WIP)
    if ( 0 != tu_->xrandrange(seed_, 2800 + which + i,  0, 9) ) {
      bv.set_bit( i );
    }
  }
  return bv;
}

void PktgenRandApp::calculate_scaling(uint32_t max, uint32_t limit,
                                      uint32_t *scaled_max, uint32_t *scale) {
  assert((scaled_max != NULL) && (scale != NULL));
  // this was setting scale=1 *scale *= 2 each time,
  // but I think this is a shift, so should start at 0 and be incremented
  *scaled_max = max; *scale = 0;
  while (*scaled_max > limit) { *scaled_max /= 2; *scale += 1; }
}

void PktgenRandApp::reset_dynamic_state() {
  n_triggers_exp_ = UINT64_C(0);
  n_triggers_act_ = UINT64_C(0);
  t_setup_ = UINT64_C(0);
  t_teardown_ = UINT64_C(0);
  t_last_trigger_ = UINT64_C(0);
  t_last_batch_trigger_ = UINT64_C(0);
  t_last_pkt_trigger_ = UINT64_C(0);
  last_port_ = 0;
  last_batch_num_ = 0;
  last_pkt_num_ = 0;
  expected_port_downs_.fill_all_zeros();
}
void PktgenRandApp::reset_configuration_state() {
  // Set key vars to sane values so we don't get spurious asserts on programming
  enabled_ = false;
  type_ = flags_ = chan_ = prio_ = recirc_out_ebuf_ = 0;
  payload_addr_ = 0; payload_size_ = 64;
  ing_port_ = ing_wrap_port_ = ing_port_pipe_id_ = 0;
  ing_inc_ = false;
  recirc_val_bv_.fill_all_zeros(); recirc_mask_bv_.fill_all_zeros();
  num_batches_ = num_pkts_ = 1;
  ipg_min_ = ipg_max_ = ibg_min_ = ibg_max_ = timer_cycles_ = 0;
  ipg_scaled_max_ = ipg_scale_ = ibg_scaled_max_ = ibg_scale_ = 0;
  portdown_mask0_.fill_all_zeros(); portdown_mask1_.fill_all_zeros();
}

void PktgenRandApp::pick_configuration_state() {
  enabled_ = (cfg_.maskTypes == 0) ?false :pick_enabled();
  if (!enabled_) return;

  type_  = pick_type();
  flags_ = pick_flags();
  chan_  = pick_chan();
  prio_  = pick_prio();

  bool from_pkt = ((flags_ & TestUtil::kPgenAppFlagAddrSizeFromPkt) != 0);
  if ( ! dprsr_triggered() ) {
    payload_addr_ = pick_payload_addr(from_pkt);
    payload_size_ = pick_payload_size(payload_addr_);
  }
  else {
    RMT_ASSERT( !from_pkt );
    // Deparser triggered, this info has to match the metadata passed to maybe_trigger
    //  in test_jbay_pktgen.cpp
    //  it would be better if these were randomised here and retrieved there
    payload_addr_ = 17; // TODO - randomize
    payload_size_ = 67; // TODO - randomize
  }

  ing_port_ = pick_ing_port();
  ing_inc_  = pick_ing_inc();
  ing_wrap_port_ = pick_ing_wrap_port(ing_port_);
  ing_port_pipe_id_ = pick_ing_port_pipe_id();

  recirc_out_ebuf_ = pick_recirc_out_ebuf();
  recirc_val_bv_ = pick_recirc_val();
  recirc_mask_bv_ = pick_recirc_mask(from_pkt, payload_addr_);

  bool limit_batches = ((flags_ & TestUtil::kPgenAppFlagLimitPktsBatches) != 0);
  num_batches_ = pick_num_batches(limit_batches);
  bool limit_pkts = ((flags_ & TestUtil::kPgenAppFlagLimitPktsPackets) != 0);
  num_pkts_ = pick_num_pkts(limit_pkts, num_batches_);
  ipg_min_ = pick_ipg_min();
  ipg_max_ = pick_ipg_max(ipg_min_);
  ibg_min_ = pick_ibg_min(ipg_max_, num_pkts_);
  ibg_max_ = pick_ibg_max(ibg_min_, num_pkts_);
  timer_cycles_ = pick_timer_cycles(ipg_max_, num_pkts_, ibg_max_, num_batches_);
  portdown_mask0_ = pick_portdown_mask(0);
  portdown_mask1_ = pick_portdown_mask(1);
  // make masks non-overlapping - if both set, clear one randomly
  for (int i=0;i<72;++i) {
    if ( portdown_mask1_.bit_set(i) && portdown_mask0_.bit_set(i) ) {
      if ( 0 != tu_->xrandrange(seed_, 2822 + i,  0, 1) ) {
        portdown_mask0_.clear_bit(i);
      }
      else {
        portdown_mask1_.clear_bit(i);
      }
    }
  }

  uint32_t scaled_max, scale;
  // added - min as I think the scaled max is relative to min
  calculate_scaling(ibg_max_-ibg_min_, 255, &scaled_max, &scale);
  ibg_scaled_max_ = static_cast<uint8_t>(scaled_max);
  ibg_scale_ = static_cast<uint8_t>(scale);
  // changed this from a * to a shift and added back min
  ibg_max_ = (ibg_scaled_max_ << ibg_scale_) + ibg_min_; // Recalc now scaled
  calculate_scaling(ipg_max_-ipg_min_, 255, &scaled_max, &scale);
  ipg_scaled_max_ = static_cast<uint8_t>(scaled_max);
  ipg_scale_ = static_cast<uint8_t>(scale);
  ipg_max_ = (ipg_scaled_max_ << ipg_scale_) + ipg_min_; // Recalc now scaled
}

void PktgenRandApp::program_reset() {
  BV128 zero128(UINT64_C(0));
  BV72  zero72(UINT64_C(0));
  //printf("PktgenRandApp:reset APP:%d_%d:\n", pipe_, app_);
  tu_->pktgen_app_ctrl_set(pipe_, app_, false, 0, 0, 0, 0);
  tu_->pktgen_app_payload_ctrl_set(pipe_, app_, 0, 64, false);
  tu_->pktgen_app_ingr_port_ctrl_set(pipe_, app_, 0, false, 0, 0);
  tu_->pktgen_app_recirc_set(pipe_, app_, 0, zero128, zero128);
  tu_->pktgen_app_event_set(pipe_, app_, 1, 1, 0, 0, 0, 0, 0, 0, 0);
  // this doesn't do anything as the register is write one to clear
  //tu_->pktgen_cmn_portdown_ctrl_set(pipe_, zero72, zero72, zero72);
  BV72 portdown_disable{};
  portdown_disable.fill_all_ones(); // register is write 1 to clear
  tu_->pktgen_cmn_portdown_disable_set(pipe_, portdown_disable);
}
void PktgenRandApp::program_configuration() {
  bool from_pkt = ((flags_ & TestUtil::kPgenAppFlagAddrSizeFromPkt) != 0);
  //printf("PktgenRandApp:config Start APP:%d_%d:\n", pipe_, app_);
  tu_->pktgen_app_event_set(pipe_, app_,
                            num_batches_, num_pkts_,
                            ibg_min_, ibg_scaled_max_, ibg_scale_,
                            ipg_min_, ipg_scaled_max_, ipg_scale_,
                            timer_cycles_);
  tu_->pktgen_app_recirc_set(pipe_, app_,
                             recirc_out_ebuf_, recirc_val_bv_, recirc_mask_bv_);
  tu_->pktgen_app_ingr_port_ctrl_set(pipe_, app_,
                                     ing_port_, ing_inc_, ing_wrap_port_, ing_port_pipe_id_);
  tu_->pktgen_app_payload_ctrl_set(pipe_, app_,
                                   payload_addr_, payload_size_, from_pkt);
  // Need to configure register fields *before* enabling
  if (enabled_)
    tu_->pktgen_app_ctrl_set(pipe_, app_,
                             false, type_, chan_, prio_, flags_);
  //printf("PktgenRandApp:start APP:%d_%d: Chan=%d\n", pipe_, app_, chan_);
  BV72 portdown_disable{};
  portdown_disable.fill_all_ones(); // register is write 1 to clear
  tu_->pktgen_cmn_portdown_ctrl_set(pipe_,
                                    portdown_mask0_, portdown_mask1_);
  tu_->pktgen_cmn_portdown_disable_set(pipe_, portdown_disable);
  //printf("PktgenRandApp:config End APP:%d_%d: Chan=%d\n", pipe_, app_, chan_);

    tu_->pktgen_app_ctrl_set(pipe_, app_,
                           enabled_, type_, chan_, prio_, flags_);

}

void PktgenRandApp::setup_configuration() {
  program_reset();
  reset_dynamic_state();
  reset_configuration_state();
  pick_configuration_state();
  program_configuration();
  model_timer::ModelTimerGetTime(t_setup_);
}
void PktgenRandApp::disable_configuration() {
  model_timer::ModelTimerGetTime(t_teardown_);
  program_reset();
  // Dynamic and Configuration state preserved till next setup
}



PktgenRandAppMgr::PktgenRandAppMgr(TestUtil *tu, int pipe, uint64_t seed,
                                   struct PktgenRandAppConfig *cfg)
    : tu_(tu), pipe_(pipe), seed_(seed), cfg_(cfg),
      tbc_chans_(0), ethcpu_chans_(0), ebuf_chans_(0), recirc_chans_(0), pktgen_chans_(0) {

  // Seed ports, bufs and apps based on CTOR seed - stash these
  uint64_t tmp_seed = seed_;
  port_seed_ = tmp_seed; tmp_seed = tu_->mmix_rand64(tmp_seed);

  // Note 2x 64b words for each PktBuf/Meta1Buf entry
  for (int i = 0; i < kPktBufWords*2; i++) {
    pktbuf_vals_[ i ] = tmp_seed; tmp_seed = tu_->mmix_rand64(tmp_seed);
  }
  for (int i = 0; i < kMeta1BufWords*2; i++) {
    meta1buf_vals_[ i ] = tmp_seed; tmp_seed = tu_->mmix_rand64(tmp_seed);
  }
  for (int app = 0; app < kNumAppsPerPipe; app++) {
    app_seed_[app] = tmp_seed; tmp_seed = tu_->mmix_rand64(tmp_seed);
  }

  for (int i = 0; i <= TestUtil::kPgenAppTypeMax; i++) num_each_type_[i] = 0;
  for (int app = 0; app < kNumAppsPerPipe; app++) apps_[app] = NULL;;
  last_app_ = 0xFF;
}
PktgenRandAppMgr::~PktgenRandAppMgr() {
  teardown_apps();
  teardown_ports();
  teardown_bufs();
}

uint8_t PktgenRandAppMgr::pick_type(int app, uint8_t mask) {
  assert((mask & TestUtil::kPgenAppTypeMask) != 0);
  if (__builtin_popcount(static_cast<unsigned int>(mask)) > 1) {
    for (int i = 0; i <= 100; i++) {
      uint8_t type = tu_->xrandrange(app_seed_[app], 1800+i,
                                     TestUtil::kPgenAppTypeMin, TestUtil::kPgenAppTypeMax);
      if (((mask >> type) & 1) == 1) return type;
    }
  }
  return __builtin_ffs(static_cast<int>(mask)) - 1;
}
void PktgenRandAppMgr::setup_apps() {
  for (int i = 0; i <= TestUtil::kPgenAppTypeMax; i++) num_each_type_[i] = 0;
  // For each app
  for (int app = 0; app < kNumAppsPerPipe; app++) {
    uint8_t mask = 0;
    // Figure out what types are still available
    for (int i = 0; i <= TestUtil::kPgenAppTypeMax; i++) {
      if (num_each_type_[i] < cfg_->numEachType[i]) mask |= 1<<i;
    }
    cfg_->maskTypes = 0; cfg_->maskFlags = 0; cfg_->setFlags = 0;
    if (mask != 0) {
      // Pick an available type
      uint8_t type = pick_type(app, mask);
      bool first = (num_each_type_[type] == 0);
      ++num_each_type_[type];
      // Modify cfg_-> for this type (use type specific flags)
      cfg_->maskTypes = 1<<type;
      cfg_->maskFlags = cfg_->flagsMaskEachType[type];
      cfg_->setFlags  = cfg_->flagsSetEachType[type];
      if (first) cfg_->setFlags |= cfg_->flagsSetFirstAppOfType[type];
    }
    // Allocate app
    apps_[app] = new PktgenRandApp(tu_, pipe_, app, app_seed_[app],
                                   *cfg_, pktgen_chans_);
  }
}
void PktgenRandAppMgr::disable_apps() {
  for (int app = 0; app < kNumAppsPerPipe; app++) {
    if (apps_[app] != nullptr) apps_[app]->disable_configuration();
  }
}
void PktgenRandAppMgr::teardown_apps() {
  for (int app = 0; app < kNumAppsPerPipe; app++) {
    if (apps_[app] != nullptr) delete apps_[app];
    apps_[app] = NULL;
  }
}
void PktgenRandAppMgr::dump_apps() {
  const char *types[6] = { "Timer", "Periodic", "Linkdown",
                           "Recirc", "Dprsr", "PFC" };
  printf("Pipe[%d] Chans[TBC=0x%02x EthCPU=0x%02x Ebuf=0x%02x]\n",
         pipe_, tbc_chans_, ethcpu_chans_, ebuf_chans_);
  for (int app = 0; app < kNumAppsPerPipe; app++) {
    PktgenRandApp *papp = apps_[app];
    bool    en = ((papp != nullptr) && (papp->enabled()));
    printf("\t APP(%d,%d) \t ", pipe_, app);
    if (en) {
      printf("Type=%s Flags=0x%02x Chan=%d Addr/Size=%d/%d "
             "Port=%d Period=%d #Batches=%d #Pkts=%d  %s\n",
             types[papp->type()], papp->flags(), papp->chan(),
             papp->payload_addr(), papp->payload_size(),
             papp->ing_port(), papp->timer_cycles(),
             papp->num_batches(), papp->num_pkts(),
             papp->pgen_chan_enabled() ?"" :"Chan *NOT* PktGen enabled!");

    } else {
      printf("DISABLED\n");
    }
  }
}
void PktgenRandAppMgr::setup_bufs() {
  // Note here data0 is MSWord, data1 is LSWord !!!!!
  uint64_t data0, data1;
  // pktgen_mem_pkt_set will byteswap internally
  for (int entry = 0; entry < kPktBufWords; entry++) {
    data0 = pktbuf_vals_[ (entry * 2) + 0 ];
    data1 = pktbuf_vals_[ (entry * 2) + 1 ];
    tu_->pktgen_mem_pkt_set(pipe_, entry, data0, data1);
  }
  // pktgen_mem_meta_set may byteswap internally (JBay)
  // or may unswap data0/data1 (WIP)
  for (int entry = 0; entry < kMeta1BufWords; entry++) {
    data0 = meta1buf_vals_[ (entry * 2) + 0 ];
    data1 = meta1buf_vals_[ (entry * 2) + 1 ];
    tu_->pktgen_mem_meta1_set(pipe_, entry, data0, data1);
  }
  // but ultimately data0 will come off wire before data1
  // and should match what we have in entry0/1 of these arrays
}
void PktgenRandAppMgr::teardown_bufs() {
  for (int entry = 0; entry < kPktBufWords; entry++) {
    tu_->pktgen_mem_pkt_set(pipe_, entry, UINT64_C(0), UINT64_C(0));
  }
  for (int entry = 0; entry < kMeta1BufWords; entry++) {
    tu_->pktgen_mem_meta1_set(pipe_, entry, UINT64_C(0), UINT64_C(0));
  }
}
void PktgenRandAppMgr::setup_ports(uint8_t flags) {
  const bool rsvd0 = MODEL_CHIP_NAMESPACE::RmtObject::is_chip1();
  // Clear Tbc/EthCpu flags if not pipe 0
  if (pipe_ != 0)
    flags &= ~(TestUtil::kPgenOutPortFlagsTbc|TestUtil::kPgenOutPortFlagsEthCpu);

  tbc_chans_ = ethcpu_chans_ = ebuf_chans_ = 0;

  if ((flags & TestUtil::kPgenOutPortFlagsTbc) != 0) {
    if (tu_->xrandbool(port_seed_, 1001)) {
      tbc_chans_ = 0x01; // Always chan0
    }
  }
  uint8_t ethcpu_mode = 0;
  if ((flags & TestUtil::kPgenOutPortFlagsEthCpu) != 0) {
    if (tu_->xrandbool(port_seed_, 1002)) {
      // Pick random chan_mode and determine chans from that
      // XXX: WIP: Allow only modes that use EVEN EthCPU channels
      ethcpu_mode = tu_->xrandrange(port_seed_, 1003, 0, rsvd0?1:4);
      switch (ethcpu_mode) {
        case 0: ethcpu_chans_ = 0x04; break; // Eth0      ==Chan2
        case 1: ethcpu_chans_ = 0x14; break; // Eth0/2    ==Chan2/4
        case 2: ethcpu_chans_ = 0x34; break; // Eth0/2/3  ==Chan2/4/5
        case 3: ethcpu_chans_ = 0x1C; break; // Eth0/1/2  ==Chan2/3/4
        case 4: ethcpu_chans_ = 0x3C; break; // Eth0/1/2/3==Chan2/3/4/5
        default: assert(0);
      }
    }
  }
  if ((flags & TestUtil::kPgenOutPortFlagsEbuf) != 0) {
    uint8_t poss_ebuf_chan0 = 0x55; // Binary[7:0]=01010101
    // Active tbc/ethcpu chans prevent ebuf_chans being used
    if ((   tbc_chans_ & 0x01) != 0) poss_ebuf_chan0 &= ~0x01;
    if ((flags & TestUtil::kPgenOutPortFlagsAllowReuse) != 0) {
      // Here we allow unused ethcpu chans to be reused by ebuf
      if ((ethcpu_chans_ & 0x04) != 0) poss_ebuf_chan0 &= ~0x04;
      if ((ethcpu_chans_ & 0x10) != 0) poss_ebuf_chan0 &= ~0x10;
    } else {
      // Here we do NOT allow unused ethcpu chans to be reused by ebuf
      if ((ethcpu_chans_ & 0x14) != 0) poss_ebuf_chan0 &= ~0x14;
    }
    // Select a subset of possible ebuf_chan0 channels
    // Or together a couple of random uint8s to improve chances of selecting a chan
    uint8_t mask0 = tu_->xrand8(port_seed_, 1004) | tu_->xrand8(port_seed_, 1005);
    uint8_t ebuf_chan0 = ( poss_ebuf_chan0 & mask0 );
    // And then of these select ones to also have chan1 active too
    uint8_t mask1 = tu_->xrand8(port_seed_, 1006) | tu_->xrand8(port_seed_, 1007);
    uint8_t ebuf_chan1 = ( ebuf_chan0 & mask1 ) << 1;
    // XXX: WIP: ebuf chan1 not allowed - only EVEN channel 0 ok
    if (rsvd0) ebuf_chan1 = 0;
    ebuf_chans_ = ebuf_chan0|ebuf_chan1;
  }

  // Now we can program H/W - note only program tbc/ethcpu on Pipe0
  if (pipe_ == 0) {
    bool    tbc_en = (tbc_chans_ != 0);
    uint8_t tbc_chan_en = (tbc_en) ?1 :0;
    tu_->pktgen_cmn_output_port_ctrl_set(pipe_, TestUtil::kPgenOutPortTbc,
                                         tbc_en, tbc_chan_en, 0);
  }
  if (pipe_ == 0) {
    // Note need to shift right 2 to get ethcpu chan_en[4]
    bool    ethcpu_en = (ethcpu_chans_ != 0);
    uint8_t ethcpu_chan_en = (ethcpu_en) ?((ethcpu_chans_ >> 2) & 0xF) :0;
    tu_->pktgen_cmn_output_port_ctrl_set(pipe_, TestUtil::kPgenOutPortEthCpu,
                                         ethcpu_en, ethcpu_chan_en, ethcpu_mode);
  }
  for (int i = 0; i < 4; i++) {
    int ii = i+i; // Shift 0,2,4,6
    bool    ebuf_en = (((ebuf_chans_ >> ii) & 1) == 1);
    uint8_t ebuf_chan_en   =  (ebuf_en) ?((ebuf_chans_ >> ii) & 0x3) :0;
    uint8_t ebuf_chan_mode = ((ebuf_en) && (ebuf_chan_en == 0x3)) ?1 :0;
    tu_->pktgen_cmn_output_port_ctrl_set(pipe_, TestUtil::kPgenOutPortEbuf0+i,
                                         ebuf_en, ebuf_chan_en, ebuf_chan_mode);
  }
  // Finally enable ALL ebuf output channels to use IBP input channels

  recirc_chans_ = pktgen_chans_ = ebuf_chans_;
  tu_->pktgen_cmn_input_port_ctrl_set(pipe_, recirc_chans_, pktgen_chans_);
  for (int app = 0; app < 16; app++) {
    //printf("PktgenRandAppMgr:SETUP_PORTS:%d_%d: Recirc=0x%x PGen=0x%x\n",
    //       pipe_, app, recirc_chans_, pktgen_chans_);
  }
}
void PktgenRandAppMgr::teardown_ports() {
  if (pipe_ == 0) {
    tu_->pktgen_cmn_output_port_ctrl_set(pipe_, TestUtil::kPgenOutPortTbc, false, 0, 0);
    tu_->pktgen_cmn_output_port_ctrl_set(pipe_, TestUtil::kPgenOutPortEthCpu, false, 0, 0);
  }
  for (int i = 0; i < 4; i++) {
    tu_->pktgen_cmn_output_port_ctrl_set(pipe_, TestUtil::kPgenOutPortEbuf0+i, false, 0, 0);
  }
  tu_->pktgen_cmn_input_port_ctrl_set(pipe_, 0, 0);
}

bool PktgenRandAppMgr::chan_for_pktgen(uint8_t chan) {
  assert(chan < 8);
  return (((pktgen_chans_ >> chan) & 1) == 1);
}
bool PktgenRandAppMgr::port_for_pktgen(uint16_t port) {
  assert( pipe_ == PORT::get_pipe_num(port) );
  int mac = PORT::get_mac_num(port);
  // XXX: WIP: still want chan num in 0-7 so use get_chan_num, NOT get_mac_chan (which is 0-3 on WIP)
  int chan = PORT::get_chan_num(port);
  return ((mac == 0) && (chan_for_pktgen(chan)));
}
bool PktgenRandAppMgr::chan_for_recirc(uint8_t chan) {
  assert(chan < 8);
  return (((recirc_chans_ >> chan) & 1) == 1);
}
bool PktgenRandAppMgr::port_for_recirc(uint16_t port) {
  assert( pipe_ == PORT::get_pipe_num(port) );
  int mac = PORT::get_mac_num(port);
  // XXX: WIP: still want chan num in 0-7 so use get_chan_num
  int chan = PORT::get_chan_num(port);
  return ((mac == 0) && (chan_for_recirc(chan)));
}
int PktgenRandAppMgr::port_get_ebuf(uint16_t port) {
  // Ebuf0=Chans0,1 Ebuf1=Chans2,3 etc
  // XXX: WIP: still want chan num in 0-7 so use get_chan_num
  int ebuf = PORT::get_chan_num(port) / 2;
  return (port_for_pktgen(port)) ?ebuf :-1;
}

int PktgenRandAppMgr::check_app_generated_pkt(PKT *p, PktInfo *pi, bool *periodic) {
  assert(p != nullptr);
  const bool jbay = MODEL_CHIP_NAMESPACE::RmtObject::is_jbayXX();
  // Only checking *generated* packets here - NOT recirc packets
  constexpr int bufsz = 16+16+6+16; // 54

  // Extract key fields from packet header
  uint8_t buf[bufsz] = { 0 };
  int pktlen = p->len();
  int gotlen = p->get_buf(buf, 0, bufsz);
  // Must have Meta(32B) PGenHdr(6B)
  //  JBay: Zero(8B) Meta0(8B) Meta1(16B)
  //    WIP:          Meta0(8B) Meta1(24B)
  if (gotlen < 32+6) return -1;
  uint64_t hdr[4] = { UINT64_C(0) };
  uint64_t pgen = UINT64_C(0);
  int f[5] = { model_common::Util::fill_val(&hdr[0],   8, buf,bufsz,  0),
               model_common::Util::fill_val(&hdr[1],   8, buf,bufsz,  8),
               model_common::Util::fill_val(&hdr[2],   8, buf,bufsz, 16),
               model_common::Util::fill_val(&hdr[3],   8, buf,bufsz, 24),
               model_common::Util::fill_val(&pgen,     8, buf,bufsz, 32) };
  // Handle different metadata layout - extra 8B *leading* zeros from PktGen on JBay
  // NB. IPB will transform these into *trailing* zeros for some reason on JBay!
  uint64_t meta0    =   (jbay) ?hdr[1]:hdr[0];
  uint64_t meta1[3] = { (jbay) ?hdr[2]:hdr[1], (jbay) ?hdr[3]:hdr[2],
                        (jbay) ?UINT64_C(0):hdr[3] };
  if ((f[0] != 0 +8) || (f[1] != 8 +8) || (f[2] != 16 +8) || (f[3] != 24 +8) || (f[4] < 32 +6))
    return -2;
  uint16_t port     = static_cast<uint16_t>( (meta0 >> 48) & UINT64_C(         0x1FF) );
  uint64_t ts       =                      ( (meta0 >>  0) & UINT64_C(0xFFFFFFFFFFFF) );
  uint8_t  app_pipe = static_cast<uint8_t> ( (pgen  >> 60) & UINT64_C(           0x3) );
  uint8_t  app_id   = static_cast<uint8_t> ( (pgen  >> 56) & UINT64_C(           0xF) );
  uint16_t batch_id = static_cast<uint16_t>( (pgen  >> 32) & UINT64_C(        0xFFFF) );
  uint16_t pkt_id   = static_cast<uint16_t>( (pgen  >> 16) & UINT64_C(        0xFFFF) );
  // I think that the port can be in any pipe
  if (PORT::get_pipe_local_port_index(port) >= kNumPortsPerPipe)
    return -3;
  if (app_id >= kNumAppsPerPipe)
    return -4;

  // Check app is valid and enabled
  PktgenRandApp *app = get_app(app_id);
  if (app == nullptr)
    return -5;
  *periodic = (app->type() == TestUtil::kPgenAppTypePeriodic);
  //printf("APP_RX_start (%d,%d): p=%d ts=%" PRId64 " "
  //       "#triggers[exp=%" PRId64 ",act=%" PRId64 "]\n",
  //       pi->src_pipe(), app_id, pipe_, ts,
  //       app->n_triggers_exp(), app->n_triggers_act());

  if (!app->enabled())
    return -6;
  // Check app is using a channel enabled for pktgen
  if (!app->pgen_chan_enabled())
    return -7;
  if (app->ing_port_pipe_id() != app_pipe)
    return -8;

  // Figure out how much PGEN header to expect (NOT including Meta0/Meta1)
  int hdrlen = app->header_len();
  assert(hdrlen <= 6+16);
  if (gotlen < 16+16+hdrlen)
    return -10;

  // Now fill in the key/PFC info if available
  uint64_t key_pfc[2] = { UINT64_C(0) };
  bool got_key_pfc = false;
  if (hdrlen == 6+16) {
    int f2[2] = { model_common::Util::fill_val(&key_pfc[0], 8, buf,bufsz, 16+16+6),
                  model_common::Util::fill_val(&key_pfc[1], 8, buf,bufsz, 16+16+6+8) };
    if ((f2[0] != (16+16+6) +8) || (f2[1] != (16+16+6+8) +8))
      return -11;
    got_key_pfc = true;
  }

  // Check batch_id pkt_id in range
  bool is_portdown = app->type() == TestUtil::kPgenAppTypeLinkdown;
  bool first_pkt = ((is_portdown || (batch_id == 0)) && (pkt_id == 0));
  bool new_batch = false;
  // the batch id field is different for port down apps
  if ((!is_portdown && (batch_id > app->num_batches())) || (pkt_id > app->num_pkts()))
    return -20;

  switch( app->type()) {
    case TestUtil::kPgenAppTypePfc:
      // PFC app always has batch/pkt 0 0
      if ((batch_id != 0) || (pkt_id != 0))
        return -21;
      break;
    case TestUtil::kPgenAppTypeLinkdown:
      // Linkdown app uses batch_id to contain
      // real pipe and real port that went down
      {
        int port_down = PORT::get_pipe_local_port_index(batch_id);
        // after the first packet we clear expected_port_down as it is
        //  used to prevent port downs while one is still pending
        if ( first_pkt && (! app->get_expected_port_down( port_down )) )
          return -19;
        if ( PORT::get_pipe_num(batch_id) != pipe_)
          return -22;
        app->clear_expected_port_down( port_down );
      }
      break;
    default:
      // For other apps check batch_id pkt_id what we expect
      // If !first_pkt fish out last_batch_num last_pkt_num
      // If same batch check pkt_id == last_pkt_num+1
      // Else check batch_id == last_batch_num+1 and pkt_id == 0
      if (!first_pkt) {
        uint16_t last_batch_id = app->last_batch_num();
        uint16_t last_pkt_id = app->last_pkt_num();
        if (batch_id == last_batch_id) {
          if (pkt_id != last_pkt_id+1)
            return -23;
        } else {
          if (batch_id != last_batch_id+1)
            return -24;
          if (pkt_id != 0)
            return -25;
          new_batch = true;
        }
      }
      break;
  }
  // Write back updated state for next packet check
  app->set_last_batch_num(batch_id);
  app->set_last_pkt_num(pkt_id);


  // Check TS in acceptable [min,max] range
  if (!first_pkt) {
    uint64_t min_jit = static_cast<uint64_t>( new_batch ?app->ibg_min() :app->ipg_min() );
    uint64_t max_jit = static_cast<uint64_t>( new_batch ?app->ibg_max() :app->ipg_max() );
    uint64_t T_last = app->t_last_pkt_trigger();

    // Fake out T_last if first_pkt
    if (first_pkt) T_last = (ts > min_jit) ?ts-min_jit :UINT64_C(0);
    // DONE: fixed the timer change (from -3 back to -1)
    bool err = ((ts < (T_last + min_jit -1)) || (ts > (T_last + max_jit + 1)));
    if (err) printf("%s(%d,%d): RxTS=%" PRId64 " %s in expected %s min/max range "
                    "[%" PRId64 "+%" PRId64 "-1,%" PRId64 "+%" PRId64 "] "
                    "(LAST %s TS=%" PRId64 ") %s %s\n",
                    err?"TS_ERR":"TS_OK ", pi->src_pipe(), app_id, ts,
                    err?"NOT":"IS", new_batch?"ibg":"ipg",
                    T_last, min_jit, T_last, max_jit, new_batch?"batch":"pkt", T_last,
                    first_pkt?"!!!!!!!!!! FAKED":"", err?"!!!!!!!!!!":"");
    if (err) printf("%s(%d,%d):  %sBatch%d [%d %d,%d (%d,%d)]  %sPkt%d [%d %d,%d (%d,%d)]\n",
                    err?"TS_ERR":"TS_OK ", pi->src_pipe(), app_id,
                    new_batch?"New":"", batch_id, app->num_batches(), app->ibg_min(), app->ibg_max(),
                    app->ibg_scaled_max(), app->ibg_scale(),
                    first_pkt?"First":"", pkt_id, app->num_pkts(), app->ipg_min(), app->ipg_max(),
                    app->ipg_scaled_max(), app->ipg_scale());
    if (err) pi->print("\tTS_ERR_PKTINFO"); // else pi->print("\tTS_OK_PKTINFO");
    if (err)
      return new_batch ?-30 :-31;
  }
  // Write back updated state for next packet check
  if (first_pkt) {
    //printf("TS_OK(%d,%d): FirstT=%" PRId64 " "
    //       "%sBatch%d [%d %d,%d (%d,%d)]  %sPkt%d [%d %d,%d (%d,%d)]\n",
    //       pi->src_pipe(), app_id, ts,
    //       new_batch?"New":"", batch_id, app->num_batches(), app->ibg_min(), app->ibg_max(),
    //       app->ibg_scaled_max(), app->ibg_scale(),
    //       first_pkt?"First":"", pkt_id, app->num_pkts(), app->ipg_min(), app->ipg_max(),
    //       app->ipg_scaled_max(), app->ipg_scale());
  }
  if (first_pkt || new_batch) {
    //printf("TS_OK (%d,%d): Setting t_last_batch_ts = %" PRId64 "\n",
    //       pi->src_pipe(), app_id, ts);
    app->set_t_last_batch_trigger(ts);
  }
  app->set_t_last_pkt_trigger(ts);
  //printf("TS_OK (%d,%d): Setting t_last_pkt_ts = %" PRId64 "\n",
  //       pi->src_pipe(), app_id, ts);
  app->set_t_last_trigger(ts);


  // Check port is what we expect
  // - if first_pkt (CC: or first in batch) should be ing_port
  // - otherwise if ing_inc should be last_port+1 (+2 if WIP)(and maybe wrap)
  int pipe_and_port = PORT::make_port_index(app->ing_port_pipe_id(), app->ing_port());
  if (first_pkt || new_batch) {
    // I think this includes the pipe
    if (port != pipe_and_port) {
      printf("PORT_ERR(%d,%d): RxPort=0x%x(%d,%d) AppPort=0x%x "
             "(IngPort=%d,IngInc=%c,IngWrapPort=%d,IngPortPipeId=%d) !!!!!!!!!!\n",
             pi->src_pipe(), app_id, port, PORT::get_pipe_num(port), PORT::get_pipe_local_port_index(port),
             pipe_and_port, app->ing_port(), app->ing_inc()?'T':'F',
             app->ing_wrap_port(), app->ing_port_pipe_id());
      pi->print("\tPORT_ERR_PKTINFO");
      return -40;
    }
  } else {
    if (app->ing_inc()) {
      // XXX: WIP: port incremented by 2 since no odd ports used on WIP
      const uint16_t inc_val = MODEL_CHIP_NAMESPACE::RmtObject::is_chip1() ?2 :1;
      uint16_t last_port = app->last_port();
      uint16_t exp_port = last_port + inc_val;
      // I think that the it wraps back to the ing_port after going beyond ing_wrap_port
      if (exp_port == (app->ing_wrap_port() + inc_val)) exp_port = app->ing_port();
      pipe_and_port = PORT::make_port_index(app->ing_port_pipe_id(), exp_port);
      if (port != pipe_and_port)
        return -41;
    } else {
      if (port != pipe_and_port)
        return -42;
    }
  }
  // Write back updated state for next packet check
  app->set_last_port( PORT::get_pipe_local_port_index(port) );



  // Now that we've verified port we can check meta1
  // I think the index is the port part with the pipe masked off
  int meta1_index = PORT::get_pipe_local_port_index(port) * 2;
  // Here index0 is MSWord, index1 is LSWord
  uint64_t exp_meta1[2] = { meta1buf_vals_[ meta1_index + 0 ], meta1buf_vals_[ meta1_index + 1 ] };
  if ((meta1[0] != exp_meta1[0]) || (meta1[1] != exp_meta1[1]))
    return -50;


  // If our app is pkt_triggered and we've got a key, check it matches
  BV128 key_pfc_bv(UINT64_C(0));
  if (app->pkt_triggered() && got_key_pfc && !app->dprsr_triggered()) {
    key_pfc_bv.set_word(key_pfc[1],  0);
    key_pfc_bv.set_word(key_pfc[0], 64); // First 64b in pkt are MSBs
    if (!app->key_matches(key_pfc_bv))
      return -60;
  }

  // Finally check payload as expected
  // First of all need to figure out what chunk of packet buffer we used
  uint16_t addr = app->payload_addr();
  uint16_t size = app->payload_size();
  assert(  ((addr*16) + size) <= (kPktBufWords*16)  );
  bool addr_size_ok = true;
  bool check_payload = true;
  bool from_pkt = ((app->flags() & TestUtil::kPgenAppFlagAddrSizeFromPkt) != 0);
  if (from_pkt) {
    if (got_key_pfc) {
      // Size/Addr from trigger pkt - values should be visible in key (if we have one)
      if (addr > 12)
        return -70;
      int offs[14] = { 104, 96, 88, 80, 72, 64, 56, 48, 40, 32, 24, 16, 8, 0 };
      uint32_t size_addr = static_cast<uint32_t>( key_pfc_bv.get_word(offs[addr], 24) );
      addr = static_cast<uint16_t>( (size_addr >>  0) & 0x03FF );
      size = static_cast<uint16_t>( (size_addr >> 10) & 0x3FFF );
      // Given we randomly generated packet data it's possible that
      // an addr_size extracted from a packet could be invalid
      addr_size_ok = ( ((addr*16) + size) <= (kPktBufWords*16) );
    } else {
      // No key so can't work out addr/size - just don't check payload in this case
      check_payload = false;
    }
  }

  // Now we can check payload
  if (check_payload) {
    // First of all check packet is length we expect (including 4 bytes added FCS/CRC)
    if (16+16+hdrlen+size+4 != pktlen) {
      printf("LEN_ERR(%d,%d): ActualPktLen=%d ExpectedPktLen=%d(16+16+%d+%d) "
             "FromPkt=%c !!!!!!!!!!\n",
             pi->src_pipe(), app_id, pktlen, 16+16+hdrlen+size,hdrlen,size,
             from_pkt?'T':'F');
      pi->print("\tLEN_ERR_PKTINFO");
      return (addr_size_ok) ?-80 :-81; // Padding?
    }

    int size8 = (size/8)*8;
    if (size8 > 0) {
      // Now check (up to) the first 256B of the payload
      uint8_t pay_buf[256] = { 0 };
      int pay_buflen = (size8 < 256) ?size8 :256;
      if (p->get_buf(pay_buf, 16+16+hdrlen, pay_buflen) != pay_buflen)
        return -82;
      for (int entry = 0; entry < pay_buflen/8; entry++) {
        uint64_t exp_data = pktbuf_vals_[ (addr*2) + entry ];
        uint64_t act_data = 0u;
        int pos = entry*8;
        pos = model_common::Util::fill_val(&act_data, 8, pay_buf, pay_buflen, pos);
        if (pos != ((entry*8)+8) )
          return -83;
        if (exp_data != act_data) {
          // See if we can find actual data we received somewhere else in pktbuf!
          int fnd_at = -1;
          for (int i = 0; i < kPktBufWords*2; i++) {
            if (pktbuf_vals_[ i ] == act_data) {
              fnd_at = i;
              break;
            }
          }
          printf("DAT_ERR(%d,%d): ActualData[%d]=0x%016" PRIx64 " "
                 "ExpData[%d]=0x%016" PRIx64 " "
                 "(Addr=%d,Entry=%d,FromPkt=%c,AddrSizeOK=%c,FoundInBuf{%d,%s}) "
                 "!!!!!!!!!!\n", pi->src_pipe(), app_id,
                 entry, act_data, (addr*2)+entry, exp_data,
                 addr, entry, from_pkt?'T':'F', addr_size_ok?'T':'F',
                 fnd_at/2,((fnd_at%2)==0)?"data0":"data1");
          pi->print("\tDAT_ERR_PKTINFO");
          return (addr_size_ok) ?-84 :-85;
        }
      }
    }
  }

  // If we get here with !addr_size_ok return an error
  // This could happen if PGEN impl pads with zeros say
  if (!addr_size_ok) {
    return -86;
  }


  // Write back updated state for next packet check
  if (first_pkt) app->inc_n_triggers_act();
  // if ( app->dprsr_triggered() ) {
  //   printf("APP_RX_end (%d,%d): p=%d ts=%" PRId64 " "
  //          "#triggers[exp=%" PRId64 ",act=%" PRId64 "]  first=%c\n",
  //          pi->src_pipe(), app_id, pipe_, ts,
  //          app->n_triggers_exp(), app->n_triggers_act(),
  //          (first_pkt)?'T':'F');
  // }
  // TODO: check app interleaving is as configured
  set_last_app(app_id);

  //printf("APP_RX_end (%d,%d): p=%d ts=%" PRId64 " "
  //       "#triggers[exp=%" PRId64 ",act=%" PRId64 "]  first=%c\n",
  //       pi->src_pipe(), app_id, pipe_, ts,
  //       app->n_triggers_exp(), app->n_triggers_act(),
  //       (first_pkt)?'T':'F');
  return 0;
}





}
