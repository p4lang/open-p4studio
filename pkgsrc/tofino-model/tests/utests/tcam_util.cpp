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
#include <mau.h>

#include <mau-dependencies.h>
#include <mau-predication.h>

#include "tcam_util.h"
#include "tcam_row_vh_util.h"


namespace MODEL_CHIP_TEST_NAMESPACE {

using BV512 = MODEL_CHIP_NAMESPACE::BitVector<512>;

TcamWrap::TcamWrap(TestUtil *tu, int pipe, int stage, int col, int row)
    : tu_(tu), pipe_(pipe), stage_(stage), col_(col), row_(row), debug_(0) {
  create();
}
TcamWrap::~TcamWrap() { }

void TcamWrap::create() {
  reset();
}

int16_t TcamWrap::combine_pri(int16_t hi, int16_t lo, int16_t inc) {
  return (hi >= 0) ?hi + inc :lo;
}
uint16_t TcamWrap::combine_bmp(uint16_t hi, uint16_t lo) {
  return
      (static_cast<uint16_t>(model_common::Util::adjacent_or(static_cast<uint64_t>(hi))) << 8) |
      (static_cast<uint16_t>(model_common::Util::adjacent_or(static_cast<uint64_t>(lo))) << 0) ;
}
void TcamWrap::reset_phys_results() {
  for (int i = 0; i < TcamConsts::kNumPhysResults; i++) res_[i].reset();
}
void TcamWrap::calc_phys_results(const BV512 &hits) {
  // Get raw results - calculate l0 results
  for (int i = 0; i < TcamConsts::kNumPhysResults; i++) {
    uint64_t raw = hits.get_word(i * 64, 64);
    res_[i].raw_ = raw;
    res_[i].bmp_l0_ = model_common::Util::sel_quarter(raw);
    res_[i].pri_l0_ = -1;
    if (raw != UINT64_C(0)) // Find highest bit
      res_[i].pri_l0_ =  __builtin_clzll(UINT64_C(1)) - __builtin_clzll(raw);
  }

  // Copy l0 -> l1, then maybe combine adjacent pairs - first PRI...
  for (int i = 0; i < TcamConsts::kNumPhysResults; i++) res_[i].pri_l1_ = res_[i].pri_l0_;
  if (res_[0].width_ > 1) res_[0].pri_l1_ = combine_pri(res_[1].pri_l0_, res_[0].pri_l0_, 64);
  if (res_[2].width_ > 1) res_[2].pri_l1_ = combine_pri(res_[3].pri_l0_, res_[2].pri_l0_, 64);
  if (res_[4].width_ > 1) res_[4].pri_l1_ = combine_pri(res_[5].pri_l0_, res_[4].pri_l0_, 64);
  if (res_[6].width_ > 1) res_[6].pri_l1_ = combine_pri(res_[7].pri_l0_, res_[6].pri_l0_, 64);
  // ...then BMP
  for (int i = 0; i < TcamConsts::kNumPhysResults; i++) res_[i].bmp_l1_ = res_[i].bmp_l0_;
  if (res_[0].width_ > 1) res_[0].bmp_l1_ = combine_bmp(res_[1].bmp_l0_, res_[0].bmp_l0_);
  if (res_[2].width_ > 1) res_[2].bmp_l1_ = combine_bmp(res_[3].bmp_l0_, res_[2].bmp_l0_);
  if (res_[4].width_ > 1) res_[4].bmp_l1_ = combine_bmp(res_[5].bmp_l0_, res_[4].bmp_l0_);
  if (res_[6].width_ > 1) res_[6].bmp_l1_ = combine_bmp(res_[7].bmp_l0_, res_[6].bmp_l0_);

  // Copy l1 -> l2, then maybe combine further pairs - first PRI...
  for (int i = 0; i < TcamConsts::kNumPhysResults; i++) res_[i].pri_l2_ = res_[i].pri_l1_;
  if (res_[0].width_ > 2) res_[0].pri_l2_ = combine_pri(res_[2].pri_l1_, res_[0].pri_l1_, 128);
  if (res_[4].width_ > 2) res_[4].pri_l2_ = combine_pri(res_[6].pri_l1_, res_[4].pri_l1_, 128);
  // ...then BMP
  for (int i = 0; i < TcamConsts::kNumPhysResults; i++) res_[i].bmp_l2_ = res_[i].bmp_l1_;
  if (res_[0].width_ > 2) res_[0].bmp_l2_ = combine_bmp(res_[2].bmp_l1_, res_[0].bmp_l1_);
  if (res_[4].width_ > 2) res_[4].bmp_l2_ = combine_bmp(res_[6].bmp_l1_, res_[4].bmp_l1_);

  // Copy l2 -> l3, then maybe combine further pairs - first PRI...
  for (int i = 0; i < TcamConsts::kNumPhysResults; i++) res_[i].pri_l3_ = res_[i].pri_l2_;
  if (res_[0].width_ > 4) res_[0].pri_l3_ = combine_pri(res_[4].pri_l2_, res_[0].pri_l2_, 256);
  // ...then BMP
  for (int i = 0; i < TcamConsts::kNumPhysResults; i++) res_[i].bmp_l3_ = res_[i].bmp_l2_;
  if (res_[0].width_ > 4) res_[0].bmp_l3_ = combine_bmp(res_[4].bmp_l2_, res_[0].bmp_l2_);

  // Now finalize results (we shift a width specific amount for pri results)
  for (int i = 0; i < TcamConsts::kNumPhysResults; i++) {
    if (res_[i].is_bitmap_) {
      res_[i].hit_ = (res_[i].bmp_l3_ != 0);
      res_[i].fin_ = res_[i].bmp_l3_;
    } else {
      res_[i].hit_ = false;
      if (res_[i].pri_l3_ >= 0) {
        res_[i].hit_ = true;
        switch (res_[i].width_) {
          case 8: res_[i].fin_ = (vpn() << 9) | res_[i].pri_l3_; break;
          case 4: res_[i].fin_ = (vpn() << 8) | res_[i].pri_l3_; break;
          case 2: res_[i].fin_ = (vpn() << 7) | res_[i].pri_l3_; break;
          case 1: res_[i].fin_ = (vpn() << 6) | res_[i].pri_l3_; break;
        }
      }
    }
    // Apply tcam_match_adr_shift 4 and mask in 0x7FFFF before output
    uint64_t fin_shifted = static_cast<uint64_t>(res_[i].fin_) << TcamConsts::kMatchAddrShift;
    res_[i].fin_addr_ = PhvMauData::tcam_match_addr( fin_shifted );
  }
  for (int p = 0; p < TcamConsts::kNumPhysResults; p++) {
    int ltc = physres_to_ltcam_[p];
    if ( (ltc != 0xFF) && (debug_ >= 1) && ((res_[p].hit_) || (debug_ >= 3)) ) {
      printf("TCAM[%d,%d] RESULT[%d] = %s Width=%d LTCAM=%d",
             row_, col_, p, (res_[p].is_bitmap_)?"BMP":"PRI", res_[p].width_, ltc);
      int w = res_[p].width_;
      if (res_[p].is_bitmap_) {
        printf("\n  BMP_L0:"); for (int i = 0; i < w; i++) printf(" 0x%04x", res_[p+i].bmp_l0_);
        printf("\n  BMP_L1:"); for (int i = 0; i < w; i++) printf(" 0x%04x", res_[p+i].bmp_l1_);
        printf("\n  BMP_L2:"); for (int i = 0; i < w; i++) printf(" 0x%04x", res_[p+i].bmp_l2_);
        printf("\n  BMP_L3:"); for (int i = 0; i < w; i++) printf(" 0x%04x", res_[p+i].bmp_l3_);
      } else {
        printf("\n  PRI_L0:"); for (int i = 0; i < w; i++) printf(" %6d", res_[p+i].pri_l0_);
        printf("\n  PRI_L1:"); for (int i = 0; i < w; i++) printf(" %6d", res_[p+i].pri_l1_);
        printf("\n  PRI_L2:"); for (int i = 0; i < w; i++) printf(" %6d", res_[p+i].pri_l2_);
        printf("\n  PRI_L3:"); for (int i = 0; i < w; i++) printf(" %6d", res_[p+i].pri_l3_);
      }
      printf("\n");
    }
  }
}
void TcamWrap::maybe_set_abit(int physres) {
  RMT_ASSERT((physres >= 0) && (physres < TcamConsts::kNumPhysResults));
  // Only if Tofino/JBay, hit and pri (must be anyway), set abit
  if ( ! (RmtObject::is_tofinoXX() || RmtObject::is_jbayXX()) ) return;
  if ( ! res_[physres].hit_) return;
  int hitindex = res_[physres].pri_l3_;
  RMT_ASSERT((hitindex >= 0) && (!res_[physres].is_bitmap_));
  res_[physres].abit_ = do_calc_abit( tu_->xrand64(value_seed_, hitindex) );
}

int TcamWrap::do_set_priority(uint32_t flags) {
  RMT_ASSERT(TcamConsts::kNumRows < 256);
  // Also configurably ignore if kStrictRowPrio flag set
  int prio = (col_ << 8) + (row_ << 0);
  if ((flags & TcamCtl::kStrictRowPrio) != 0) {
    // Configured to do StrictRowPrio 11,10,9,8,7,6,5,4,3,2,1,0
    return prio;
  } else if (RmtObject::is_tofinoXX() || RmtObject::is_jbayXX()) {
    // Otherwise Tofino/JBay always do StrictRowPrio 11,10,9,8,7,6,5,4,3,2,1,0
    return prio;
  } else if (RmtObject::is_chip1()) {
    // But WIP slightly different 11,10,9,8,7,6,0,1,2,3,4,5
    int rowprio = (row_ <= TcamConsts::kMidRowLower) ?TcamConsts::kMidRowLower - row_ :row_;
    prio = (col_ << 8) + (rowprio << 0);
    return prio;
  } else {
    RMT_ASSERT(0 && "What chip are we?");
  }
}

void TcamWrap::reset() {
  priority_ = 0;
  array_seed_ = UINT64_C(0);
  seed_ = UINT64_C(0);
  cnf_seed_ = UINT64_C(0);
  value_seed_ = UINT64_C(0);
  mask_seed_ = UINT64_C(0);
  ltcams_input_ = 0;
  ltcams_ = 0;
  ltcam_ = 0xFF;
  logical_table_ = 0xFF;
  vpn_ = 0xFF;
  flags_ = 0u;
  info_ = UINT64_C(0);
  last_n_matches_ = 0;
  last_word_in_ = UINT64_C(0);
  last_hits_in_.fill_all_zeros();
  last_hits_mask_.fill_all_zeros();
  last_matches_.fill_all_zeros();
  last_matches_post_mrd_.fill_all_zeros();
  last_matches_combine_.fill_all_zeros();
  last_hits_out_.fill_all_zeros();
  reset_phys_results();
  for (int ltc = 0; ltc < TcamConsts::kNumLogTcams; ltc++) ltcam_to_physres_[ltc] = 0xFF;
  for (int p = 0; p < TcamConsts::kNumPhysResults; p++) physres_to_ltcam_[p] = 0xFF;
}
void TcamWrap::set_debug(int debug) {
  debug_ = debug;
}
void TcamWrap::set_seed(uint64_t array_seed, uint64_t seed) {
  reset();
  array_seed_ = seed;
  seed_ = seed;
  // Generate some more seeds
  cnf_seed_   = tu_->mmix_rand64( (seed * UINT64_C(111029)) + UINT64_C(111031) );
  value_seed_ = tu_->mmix_rand64( (seed * UINT64_C(222041)) + UINT64_C(222053) );
  mask_seed_  = tu_->mmix_rand64( (seed * UINT64_C(333029)) + UINT64_C(333031) );
}


int TcamWrap::do_get_mask_bits() {
  // todo? bits masked per-entry *should* be dependent on MRD too
  int bits = TcamConsts::kEntryBitsSet; // Defaults to 8
  if (wide()) {  // But may shrink to 4 or 2
    int chain_len = __builtin_popcount(row_mask_);
    // Ocassionally (chain_len 9) *increase* bits that must match
    // (we want to test wide *misses* too)
    if      (chain_len == 9) bits = TcamConsts::kEntryBitsSet*2;
    else if (chain_len >= 6) bits = TcamConsts::kEntryBitsSet/4;
    else if (chain_len >= 3) bits = TcamConsts::kEntryBitsSet/2;
  }
  RMT_ASSERT(bits > 0);
  return bits;
}
uint64_t TcamWrap::do_calc_mask(uint64_t seed, int n_bits, uint64_t mask0) {
  // Calculate a random mask (with nbits set) from seed
  uint64_t bitmask = UINT64_C(0);
  for (int i = 0; i < n_bits; i++) {
    uint64_t bitmask_bit = tu_->xrandrange(seed, i, 1, TcamConsts::kWidth-1);
    bitmask |= (UINT64_C(1) << bitmask_bit);
  }
  return bitmask & mask0;
}
uint64_t TcamWrap::do_calc_value(uint64_t seed, uint64_t mask) {
  return seed & mask;
}
uint64_t TcamWrap::do_calc_w0(uint64_t value, uint64_t mask) {
  return (~value & mask) | ~mask;
}
uint64_t TcamWrap::do_calc_w1(uint64_t value, uint64_t mask) {
  return ( value & mask) | ~mask;
}
uint8_t TcamWrap::do_calc_abit(uint64_t value) {
  if (RmtObject::is_chip1() || ((flags_ & TcamCtl::kNoAbit) != 0)) return 0;
  return ((__builtin_popcountll(value) % 5) == 0); // 1 in 5
}
uint64_t TcamWrap::do_get_s0(uint64_t search) {
  return ~search;
}
uint64_t TcamWrap::do_get_s1(uint64_t search) {
  return search;
}
void TcamWrap::do_calc_w0_w1(int i, uint64_t *w0, uint64_t *w1, uint8_t *abit,
                             int debug, const char *str, uint64_t w64) {
  uint64_t rawval  = tu_->xrand64(value_seed_, i);
  uint64_t rawmask = tu_->xrand64(mask_seed_, i);
  int      mask_bits = do_get_mask_bits();
  uint64_t mask = do_calc_mask(rawmask, mask_bits, TcamConsts::kMask);
  uint64_t val  = do_calc_value(rawval, mask);
  *w0 = do_calc_w0(val, mask);
  *w1 = do_calc_w1(val, mask);
  *abit = do_calc_abit(rawval);
  if (debug >= 3) printf("TCAM[%d,%d](%3d=%d:%2d) "
                         "{RawV/M=0x%016" PRIx64 ",0x%016" PRIx64 " "
                         "V/M=0x%016" PRIx64 ",0x%016" PRIx64 " "
                         "W0=0x%016" PRIx64 ",W1=0x%016" PRIx64 ",ABIT=%d} "
                         "%s(0x%" PRIx64 ")\n",
                         row_, col_, i, i/64, i%64, rawval, rawmask, val, mask,
                         *w0, *w1, *abit, (str==nullptr)?"":str, w64);
}
uint64_t TcamWrap::do_compare(uint64_t w0, uint64_t w1, uint64_t s0, uint64_t s1) {
  return (((~w0) & s0 ) | ((~w1) & s1 ));
}
bool TcamWrap::do_match(uint64_t w0, uint64_t w1, uint64_t s0, uint64_t s1, int debug) {
  bool match = (do_compare(w0,w1, s0,s1) == UINT64_C(0));
  if (debug >= 9) {
    printf("TCAM[%d,%d] "
           "W0=0x%016" PRIx64 ",W1=0x%016" PRIx64 " "
           "S0=0x%016" PRIx64 ",S1=0x%016" PRIx64 "\n",
           row_, col_, w0, w1, s0, s1);
    printf("TCAM[%d,%d] %s[63..0]=", row_, col_, (match)?"  HIT":" MISS");
    for (int bit = 63; bit >= 0; bit--) {
      uint64_t msk = UINT64_C(1) << bit;
      uint64_t hit = (do_compare(w0 & msk, w1 & msk, s0 & msk, s1 & msk) == UINT64_C(0));
      if (hit) printf("X"); else printf(".");
    }
    printf("  {X=>hit}\n");
  }
  return match;

}



void TcamWrap::configure(uint16_t row_mask, uint32_t flags, uint8_t ltcams, uint64_t info) {
  priority_ = do_set_priority(flags); // Not simply row/col dependent as can be affected by flags
  row_mask_ = row_mask;
  flags_ = flags;
  ltcams_input_ = ltcams;
  ltcams_ = ltcams;
  if (ltcams == 0) {
    ltcam_ = 0xFF;
  } else {
    ltcam_ = __builtin_clz(1) - __builtin_clz(ltcams);
    if (RmtObject::is_tofinoXX() || RmtObject::is_jbayXX()) {
      // Tofino/JBay only support 1 LTCAM per TCAM. Choose highest numbered
      ltcams_ = 1 << ltcam_;
    }
  }
  info_ = info;
  // Derive logical_table/vpn deterministically from input ltcam(s)
  vpn_ = ltcams_ & 0x3F;   // Track lowest 6 LTCAMs in VPN
  if (RmtObject::is_chip1()) {
    // In WIP tcam_mode.tcam_logical_table changed to be a bitmask
    // of all the LTCAMs used by the TCAM - honour this here
    logical_table_ = ltcams_; // ALL LTCAMs appear in logical table
  } else {
    logical_table_ = ltcam_;  // Track highest LTCAM as logical table
  }
}


int TcamWrap::pick_result(uint8_t avail, uint8_t usable, uint8_t *still_avail) {
  if ((flags_ & TcamCtl::kUseResult0) != 0) {
    // Can be constrained to always use result0
    *still_avail = avail & ~1;
    return 0;
  }
  uint8_t poss = avail & usable;
  // Should always be some usable, available results!
  RMT_ASSERT(poss != 0);
  // Find number possible results we could use
  int n = __builtin_popcount(poss);
  // Choose result in [1..n] based on *top-level* seed & ltcams_ this TCAM
  int pick = tu_->xrandrange(array_seed_, static_cast<uint64_t>(ltcams_), 1, n);
  // Go thru possibilites (positions with bit set) till we find the pick'th
  for (int i = 0; i < 8; i++) {
    if (((poss >> i) & 1) == 1) pick--;
    if (pick == 0) {
      *still_avail = avail & ~(1<<i);
      return i;
    }
  }
  RMT_ASSERT(0 && "Unable to pick_result");
}
int TcamWrap::pick_width(uint8_t free_shifted, uint8_t max_width) {
  uint8_t width = max_width; // Always try max possible width
  while (width != 0) {
    // Mask in width and see if we just find ourself.
    // If so we can extend width to that amount
    if ( (free_shifted & ((1<<width)-1)) == 0x1 ) return width;
    width /= 2;
  }
  RMT_ASSERT(0 && "Unable to pick_width");
}
bool TcamWrap::pick_pri(int physres) {
  if ((flags_ & TcamCtl::kNoBitmap) != 0) return true; // Always Pri if NoBitmap set
  uint64_t seed_i = static_cast<uint64_t>(ltcams_) * static_cast<uint64_t>(physres);
  return tu_->xrandbool(array_seed_, seed_i);
}
void TcamWrap::recurse_setting_width_type(int result, int width, bool pri) {
  RMT_ASSERT((result >= 0) && (result < TcamConsts::kNumPhysResults));
  int halfwidth = width / 2;
  if (halfwidth > 0) {
    recurse_setting_width_type(result,             halfwidth, pri);
    recurse_setting_width_type(result + halfwidth, halfwidth, pri);
  }
  res_[result].width_ = width;
  res_[result].is_bitmap_ = !pri;
  return;
}


#if MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd0)
void TcamWrap::pick_result_config() {
  for (int p = 0; p < TcamConsts::kNumPhysResults; p++) physres_to_ltcam_[p] = 0xFF;

  // Pick physical results to use for each LTCAM - some constraints!
  // (for derivation these values see tcam_map_oxbar_ctl CSR doc)
  //
  uint8_t usable[8] = { 0x01, 0x03, 0x05, 0x0D, 0x11, 0x31, 0x51, 0xD1 };
  uint8_t avail = 0xFF; // All results 0-7 initially available
  for (int ltc = 0; ltc < TcamConsts::kNumLogTcams; ltc++) {
    ltcam_to_physres_[ltc] = 0xFF; // Indicates LTCAM unused this TCAM
    if (((ltcams_ >> ltc) & 1) == 1) {
      int physres = pick_result(avail, usable[ltc], &avail);
      ltcam_to_physres_[ltc] = physres;
      physres_to_ltcam_[physres] = ltc;
    }
  }
  // Now we've picked physical results to use for our LTCAMs
  // figure out what widths we should use for each. Each result
  // can potentially use other still available phys results
  // subject to further constraints:
  //  Result[0]    can use widths 8,4,2,1
  //  Result[4]     "   "  widths   4,2,1
  //  Result[2/6]   "   "  widths     2,1
  //  Result[1/3/5/7] only width        1
  //
  uint8_t still_free = ~avail; // Invert to simplify width pick
  uint8_t max_widths[8] = { 8, 1, 2, 1, 4, 1, 2, 1 };
  for (int p = 0; p < TcamConsts::kNumPhysResults; p++) res_[p].width_ = 0;
  for (int ltc = 0; ltc < TcamConsts::kNumLogTcams; ltc++) {
    int physres = ltcam_to_physres_[ltc];
    if (physres != 0xFF) {
      int width = pick_width(still_free >> physres, max_widths[physres]);
      // Set widths/pri on results we're combining
      recurse_setting_width_type(physres, width, pick_pri(physres));
    }
  }
  if (debug_ >= 3) {
    for (int p = 0; p < TcamConsts::kNumPhysResults; p++) {
      int ltc = physres_to_ltcam_[p];
      if (ltc != 0xFF) {
        printf("TCAM[%d,%d] RESULT[%d] = %s Width=%d LTCAM=%d\n",
               row_, col_, p, (res_[p].is_bitmap_)?"BMP":"PRI", res_[p].width_, ltc);
      }
    }
  }
}
void TcamWrap::install_config_chip() {
  pick_result_config();

  auto& mau_base = RegisterUtils::ref_mau(pipe_,stage_);
  auto& tcam_regs = mau_base.tcams.col[col_];
  auto a_tcam_map_oxbar_ctl = &tcam_regs.tcam_map_oxbar_ctl[row_];
  auto a_tcam_result_ctl = &tcam_regs.tcam_result_ctl[row_];
  uint32_t v_tcam_map_oxbar_ctl = 0u;
  uint32_t v_tcam_result_ctl = 0u;

  // First configurate tcam_map_oxbar_ctl
  int      physres_to_sel0[8] = { 0, -1, -1, -1, -1, -1, -1, -1  };
  uint32_t enable0 = (ltcam_to_physres_[0] != 0xFF) ?1u :0u;
  uint32_t select0 = (enable0 == 1u) ?physres_to_sel0[ ltcam_to_physres_[0] ] :0;
  RMT_ASSERT(select0 >= 0);
  setp_tcam_map_oxbar_ctl_tcam_map_oxbar_ctl_0_enable(&v_tcam_map_oxbar_ctl, enable0);
  setp_tcam_map_oxbar_ctl_tcam_map_oxbar_ctl_0_select(&v_tcam_map_oxbar_ctl, select0);
  int      physres_to_sel1[8] = { 0,  1, -1, -1, -1, -1, -1, -1  };
  uint32_t enable1 = (ltcam_to_physres_[1] != 0xFF) ?1u :0u;
  uint32_t select1 = (enable1 == 1u) ?physres_to_sel1[ ltcam_to_physres_[1] ] :0;
  RMT_ASSERT(select1 >= 0);
  setp_tcam_map_oxbar_ctl_tcam_map_oxbar_ctl_1_enable(&v_tcam_map_oxbar_ctl, enable1);
  setp_tcam_map_oxbar_ctl_tcam_map_oxbar_ctl_1_select(&v_tcam_map_oxbar_ctl, select1);
  int      physres_to_sel2[8] = { 0, -1,  1, -1, -1, -1, -1, -1  };
  uint32_t enable2 = (ltcam_to_physres_[2] != 0xFF) ?1u :0u;
  uint32_t select2 = (enable2 == 1u) ?physres_to_sel2[ ltcam_to_physres_[2] ] :0;
  RMT_ASSERT(select2 >= 0);
  setp_tcam_map_oxbar_ctl_tcam_map_oxbar_ctl_2_enable(&v_tcam_map_oxbar_ctl, enable2);
  setp_tcam_map_oxbar_ctl_tcam_map_oxbar_ctl_2_select(&v_tcam_map_oxbar_ctl, select2);
  int      physres_to_sel3[8] = { 0, -1,  1,  2, -1, -1, -1, -1  };
  uint32_t enable3 = (ltcam_to_physres_[3] != 0xFF) ?1u :0u;
  uint32_t select3 = (enable3 == 1u) ?physres_to_sel3[ ltcam_to_physres_[3] ] :0;
  RMT_ASSERT(select3 >= 0);
  setp_tcam_map_oxbar_ctl_tcam_map_oxbar_ctl_3_enable(&v_tcam_map_oxbar_ctl, enable3);
  setp_tcam_map_oxbar_ctl_tcam_map_oxbar_ctl_3_select(&v_tcam_map_oxbar_ctl, select3);
  int      physres_to_sel4[8] = { 0, -1, -1, -1,  1, -1, -1, -1  };
  uint32_t enable4 = (ltcam_to_physres_[4] != 0xFF) ?1u :0u;
  uint32_t select4 = (enable4 == 1u) ?physres_to_sel4[ ltcam_to_physres_[4] ] :0;
  RMT_ASSERT(select4 >= 0);
  setp_tcam_map_oxbar_ctl_tcam_map_oxbar_ctl_4_enable(&v_tcam_map_oxbar_ctl, enable4);
  setp_tcam_map_oxbar_ctl_tcam_map_oxbar_ctl_4_select(&v_tcam_map_oxbar_ctl, select4);
  int      physres_to_sel5[8] = { 0, -1, -1, -1,  1,  2, -1, -1  };
  uint32_t enable5 = (ltcam_to_physres_[5] != 0xFF) ?1u :0u;
  uint32_t select5 = (enable5 == 1u) ?physres_to_sel5[ ltcam_to_physres_[5] ] :0;
  RMT_ASSERT(select5 >= 0);
  setp_tcam_map_oxbar_ctl_tcam_map_oxbar_ctl_5_enable(&v_tcam_map_oxbar_ctl, enable5);
  setp_tcam_map_oxbar_ctl_tcam_map_oxbar_ctl_5_select(&v_tcam_map_oxbar_ctl, select5);
  int      physres_to_sel6[8] = { 0, -1, -1, -1,  1, -1,  2, -1  };
  uint32_t enable6 = (ltcam_to_physres_[6] != 0xFF) ?1u :0u;
  uint32_t select6 = (enable6 == 1u) ?physres_to_sel6[ ltcam_to_physres_[6] ] :0;
  RMT_ASSERT(select6 >= 0);
  setp_tcam_map_oxbar_ctl_tcam_map_oxbar_ctl_6_enable(&v_tcam_map_oxbar_ctl, enable6);
  setp_tcam_map_oxbar_ctl_tcam_map_oxbar_ctl_6_select(&v_tcam_map_oxbar_ctl, select6);
  int      physres_to_sel7[8] = { 0, -1, -1, -1,  1, -1,  2,  3  };
  uint32_t enable7 = (ltcam_to_physres_[7] != 0xFF) ?1u :0u;
  uint32_t select7 = (enable7 == 1u) ?physres_to_sel7[ ltcam_to_physres_[7] ] :0;
  RMT_ASSERT(select7 >= 0);
  setp_tcam_map_oxbar_ctl_tcam_map_oxbar_ctl_7_enable(&v_tcam_map_oxbar_ctl, enable7);
  setp_tcam_map_oxbar_ctl_tcam_map_oxbar_ctl_7_select(&v_tcam_map_oxbar_ctl, select7);

  // Now configurate tcam_result_ctl
  uint8_t pri_l1 = 0, pri_l2 = 0, pri_l3 = 0;
  uint8_t bmp_l1 = 0, bmp_l2 = 0, bmp_l3 = 0;
  for (int i = 0; i < 4; i++) {
    int i0246 = i + i; // So for 0 2 4 6
    pri_l1 |= (!res_[i0246].is_bitmap_ && (res_[i0246].width_ > 1)) ?0<<i :1<<i;
    bmp_l1 |= ( res_[i0246].is_bitmap_ && (res_[i0246].width_ > 1)) ?0<<i :1<<i;
  }
  for (int i = 0; i < 2; i++) {
    int i04 = i + i + i + i; // So for 0 and 4
    pri_l2 |= (!res_[i04].is_bitmap_ && (res_[i04].width_ > 2)) ?0<<i :1<<i;
    bmp_l2 |= ( res_[i04].is_bitmap_ && (res_[i04].width_ > 2)) ?0<<i :1<<i;
  }
  int i = 0, i0 = 0;
  pri_l3 |= (!res_[i0].is_bitmap_ && (res_[i0].width_ > 4)) ?0<<i :1<<i;
  bmp_l3 |= ( res_[i0].is_bitmap_ && (res_[i0].width_ > 4)) ?0<<i :1<<i;
  uint8_t pri_bmp_sel = 0;
  for (int p = 0; p < TcamConsts::kNumPhysResults; p++) {
    pri_bmp_sel |= (res_[p].is_bitmap_) ?(1<<p) :0;
  }
  setp_tcam_result_ctl_tcam_result_ctl_pri_l1(&v_tcam_result_ctl, pri_l1);
  setp_tcam_result_ctl_tcam_result_ctl_bmp_l1(&v_tcam_result_ctl, bmp_l1);
  setp_tcam_result_ctl_tcam_result_ctl_pri_l2(&v_tcam_result_ctl, pri_l2);
  setp_tcam_result_ctl_tcam_result_ctl_bmp_l2(&v_tcam_result_ctl, bmp_l2);
  setp_tcam_result_ctl_tcam_result_ctl_pri_l3(&v_tcam_result_ctl, pri_l3);
  setp_tcam_result_ctl_tcam_result_ctl_bmp_l3(&v_tcam_result_ctl, bmp_l3);
  setp_tcam_result_ctl_tcam_result_ctl_sel(&v_tcam_result_ctl, pri_bmp_sel);

  // Zeroise all if no LTCAM
  if (ltcams_ == 0) v_tcam_map_oxbar_ctl = v_tcam_result_ctl = 0u;

  tu_->OutWord((void*)a_tcam_map_oxbar_ctl, v_tcam_map_oxbar_ctl);
  tu_->OutWord((void*)a_tcam_result_ctl, v_tcam_result_ctl);
}
#else
void TcamWrap::pick_result_config() {
  if (ltcam_ == 0xFF) return;
  ltcam_to_physres_[ltcam_] = 0;
  physres_to_ltcam_[0] = ltcam_;
  // Always single physical result on Tofino/JBay
  // Recurse setting widths so all phys results combine.
  // Also setup all phys results to be pri-encoded
  recurse_setting_width_type(0, 8, true /*pri*/);
}
void TcamWrap::install_config_chip() {
  pick_result_config();
}
#endif

void TcamWrap::install_config() {
  // Setup tcam_mode etc and program H/W
  //
  // tcam_chain_out_enable, tcam_match_out_enable - see flags
  // tcam_ingress 1, tcam_egress 0 - see flags
  // tcam_logical_table/tcam_vpn - deterministic value from input ltcams
  //
  auto& mau_base = RegisterUtils::ref_mau(pipe_,stage_);
  auto& tcam_regs = mau_base.tcams.col[col_];
  auto a_mode = &tcam_regs.tcam_mode[row_];
  uint32_t v_mode = tu_->InWord((void*)a_mode);
  setp_tcam_mode_tcam_logical_table(&v_mode, logical_table_);
  setp_tcam_mode_tcam_vpn(&v_mode, vpn_);
  setp_tcam_mode_tcam_ingress(&v_mode, ingress() ?1 :0);
  setp_tcam_mode_tcam_egress(&v_mode, egress() ?1 :0);
  setp_tcam_mode_tcam_chain_out_enable(&v_mode, (chain_out()) ?1 :0);
  setp_tcam_mode_tcam_match_output_enable(&v_mode, (match_out()) ?1 :0);
  setp_tcam_mode_tcam_vbit_dirtcam_mode(&v_mode, 0); // No DirtCAM
  setp_tcam_mode_tcam_data_dirtcam_mode(&v_mode, 0);

  if (ltcams_ == 0) v_mode = 0; // Zeroise all if no LTCAM

  tu_->OutWord((void*)a_mode, v_mode);
  install_config_chip();
}

void TcamWrap::install_data() {
  // Calculate and install W0/W1 to allow 'proper' lookup in real RefModel TCAM
  for (int entry = 0; entry < TcamConsts::kEntries; entry++) {
    uint64_t w0 = UINT64_C(0), w1 = UINT64_C(0);
    uint8_t  abit = 0;
    if (ltcams_ != 0) do_calc_w0_w1(entry, &w0, &w1, &abit);
    // todo? should allow MRD spread
    // maybe setup deterministically positioned 2/4/8-entry MRD blocks?
    uint8_t mrd0 = 1; // 1=>boundary set, 0=>no boundary (so MRD spread occurs)
    tu_->tcam_write(pipe_, stage_, row_, col_, entry, w0, w1, mrd0, abit);
  }
}

int TcamWrap::lookup(const uint8_t thread, const uint64_t word_in,
                     BV512 *matches, int debug) {
  if (ltcams_ == 0) return 0;
  if ((threads() & thread) == 0) return 0;

  // Do lookup locally - regenerate val/mask/w0/w1 then check for match
  int      n_matches = 0;
  uint64_t s0 = do_get_s0(word_in) & TcamConsts::kMask;
  uint64_t s1 = do_get_s1(word_in) & TcamConsts::kMask;
  // Calculate matches
  for (int entry = 0; entry < TcamConsts::kEntries; entry++) {
    uint64_t w0, w1;
    uint8_t  abit;
    do_calc_w0_w1(entry, &w0, &w1, &abit);
    if (do_match(w0, w1, s0, s1, debug)) {
      do_calc_w0_w1(entry, &w0, &w1, &abit, debug, "LOOKUP_HIT", word_in);
      matches->set_bit(entry);
      n_matches++;
    }
  }
  return n_matches;
}

void TcamWrap::do_apply_mrd(const BV512 &matches, BV512 *mrd) {
  mrd->copy_from(matches);
  // todo? should expand hits in mrd using MRD
}
void TcamWrap::do_apply_mask(const BV512 &input, const BV512 &mask, BV512 *output) {
  output->copy_from(input);
  output->mask(mask);
}

int TcamWrap::lookup(const uint8_t thread,
                     const uint64_t word_in, // word to lookup
                     const BV512 &hits_in,   // chained hits in
                     const BV512 &hits_mask, // mask to apply before hits_out
                     BV512 *hits_out, int debug) {

  last_word_in_ = word_in;
  last_hits_in_.copy_from(hits_in);
  last_hits_mask_.copy_from(hits_mask);
  BV512 matches(UINT64_C(0));

  last_n_matches_ = lookup(thread, word_in, &matches, debug);
  if (last_n_matches_ > 0) {
    // Stash a copy
    last_matches_.copy_from(matches);
    // Maybe MRD expand
    do_apply_mrd(matches, &last_matches_post_mrd_);
    // Mask twice, first with chained hits in, second with hits_mask out
    do_apply_mask(last_matches_post_mrd_, hits_in, &last_matches_combine_);
    do_apply_mask(last_matches_combine_, hits_mask, &last_hits_out_);

    hits_out->copy_from(last_hits_out_);

    reset_phys_results();
    if (match_out()) calc_phys_results(last_hits_out_);

  } else {
    last_matches_.fill_all_zeros();
    last_matches_post_mrd_.fill_all_zeros();
    last_matches_combine_.fill_all_zeros();
    last_hits_out_.fill_all_zeros();

    hits_out->fill_all_zeros();

    reset_phys_results();
  }
  return last_n_matches_;
}

bool TcamWrap::get_result(int ltcam, uint32_t *addr, uint8_t *abit, int *prio) {
  if (((ltcams_ >> ltcam) & 1) == 0) return false;
  if (!match_out()) return false;

  RMT_ASSERT((ltcam >= 0) && (ltcam < TcamConsts::kNumLogTcams));
  int physres = ltcam_to_physres_[ltcam];
  if (physres == 0xFF) return false; // LTCAM invalid this TCAM
  RMT_ASSERT((physres >= 0) && (physres < TcamConsts::kNumPhysResults));
  maybe_set_abit(physres);

  if (!res_[physres].hit_) return false;

  *addr = res_[physres].fin_addr_;
  *abit = res_[physres].abit_;
  *prio = priority_;
  return true;
}

void TcamWrap::get_result_bv(int which, BV512 *bv) {
  switch (which) {
    case ResCtl::kWordIn:         bv->set_word(last_word_in_, 0, 64);    break;
    case ResCtl::kHitsIn:         bv->copy_from(last_hits_in_);          break;
    case ResCtl::kHitsMask:       bv->copy_from(last_hits_mask_);        break;
    case ResCtl::kMatches:        bv->copy_from(last_matches_);          break;
    case ResCtl::kMatchesPostMrd: bv->copy_from(last_matches_post_mrd_); break;
    case ResCtl::kMatchesCombine: bv->copy_from(last_matches_combine_);  break;
    case ResCtl::kHitsOut:        bv->copy_from(last_hits_out_);         break;
  }
}




TcamColWrap::TcamColWrap(TestUtil *tu, int pipe, int stage, int col)
    : tu_(tu), pipe_(pipe), stage_(stage), col_(col) {
  create();
}
TcamColWrap::~TcamColWrap() {
  destroy();
}

void TcamColWrap::create() {
  debug_ = 0;
  array_seed_ = UINT64_C(0);
  seed_ = UINT64_C(0);
  for (int r = 0; r < TcamConsts::kNumRows; r++) {
    rows_[r] = new TcamWrap(tu_, pipe_, stage_, col_, r);
  }
}
void TcamColWrap::destroy() {
  for (int r = 0; r < TcamConsts::kNumRows; r++) {
    if (rows_[r] != nullptr) delete rows_[r];
    rows_[r] = nullptr;
  }
}
void TcamColWrap::set_debug(int debug) {
  debug_ = debug;
  for (int r = 0; r < TcamConsts::kNumRows; r++) rows_[r]->set_debug(debug);
}
void TcamColWrap::set_seed(uint64_t array_seed, uint64_t seed) {
  array_seed_ = seed;
  seed_ = seed;
  // Generate some more seeds deterministically
  for (int r = 0; r < TcamConsts::kNumRows; r++) {
    rows_[r]->set_seed( array_seed, tu_->xrand64(seed, (col_ * TcamConsts::kNumRows) + r) );
  }
}


void TcamColWrap::configure_tcams_top(int rowHi, int rowLo, uint16_t row_mask,
                                      uint32_t flags, uint8_t ltcams, uint64_t info) {
  for (int r = rowHi; r >= rowLo; r--) {
    if (r == rowLo) flags = TcamCtl::top_flags(flags);
    rows_[r]->configure(row_mask, flags, ltcams, info);
  }
}
void TcamColWrap::configure_tcams_bot(int rowLo, int rowHi, uint16_t row_mask,
                                      uint32_t flags, uint8_t ltcams, uint64_t info) {
  for (int r = rowLo; r <= rowHi; r++) {
    if (r == rowHi) flags = TcamCtl::bot_flags(flags);
    rows_[r]->configure(row_mask, flags, ltcams, info);
  }
}
void TcamColWrap::configure_tcams(int rowStart, int rowEnd,
                                  uint32_t flags, uint8_t ltcams, uint64_t info) {
  int rowHi = (rowStart > rowEnd) ?rowStart :rowEnd;
  int rowLo = (rowStart < rowEnd) ?rowStart :rowEnd;
  uint16_t row_mask = (0xFFFF >> (16 - (1 + rowHi - rowLo))) << rowLo;
  if ((flags & TcamCtl::kChainOutput) != 0u) {
    // Chain output may get cleared so set wide flag to track row was part of chain
    flags |= TcamCtl::kWideFlag;
  }
  if ((rowHi >= TcamConsts::kMidRowUpper) && (rowLo <= TcamConsts::kMidRowLower)) {
    // Rows cross middle
    configure_tcams_top(rowHi, TcamConsts::kMidRowUpper, row_mask, flags, ltcams, info);
    configure_tcams_bot(rowLo, TcamConsts::kMidRowLower, row_mask, flags, ltcams, info);

  } else if (rowLo >= TcamConsts::kMidRowUpper) { // All top rows
    configure_tcams_top(rowHi, rowLo, row_mask, flags, ltcams, info);

  } else if (rowHi <= TcamConsts::kMidRowLower) { // All bot rows
    configure_tcams_bot(rowLo, rowHi, row_mask, flags, ltcams, info);
  }
}

#if MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd0)
void TcamColWrap::install_config_chip() {
  // Nothing to do in the case of WIP
}
#else
void TcamColWrap::install_config_chip() {
  // Tofino/JBay need to set up tcam_table_map
  auto& mau_base = RegisterUtils::ref_mau(pipe_,stage_);
  auto& tcam_regs = mau_base.tcams.col[col_];
  std::array< uint16_t, TcamConsts::kNumLogTcams > rows_per_ltcam = { 0 };
  // Work out what LTCAMs our TCAMs were configured to use
  for (int r = 0; r < TcamConsts::kNumRows; r++) {
    bool output = rows_[r]->match_out();
    uint8_t ltc = rows_[r]->ltcam();
    if ((output) && (ltc < TcamConsts::kNumLogTcams)) rows_per_ltcam[ltc] |= 1<<r;
  }
  // Then setup tcam_table_map to reflect this
  for (int ltc = 0; ltc < TcamConsts::kNumLogTcams; ltc++) {
    auto a_tablemap = &tcam_regs.tcam_table_map[ltc];
    tu_->OutWord((void*)a_tablemap, rows_per_ltcam[ltc]);
  }
}
#endif
void TcamColWrap::install_config() {
  for (int r = 0; r < TcamConsts::kNumRows; r++) rows_[r]->install_config();
  install_config_chip();
}

void TcamColWrap::install_data() {
  for (int r = 0; r < TcamConsts::kNumRows; r++) rows_[r]->install_data();
}

uint64_t TcamColWrap::do_get_input_word(int row, const BV528 &input) {
  uint64_t word = input.get_word(row * TcamConsts::kWidth, TcamConsts::kWidth);
  if ((row % 2) == 0) {
    // Even rows are straightforward given our fixed linear mapping
    // They get 44b at offset row*44
  } else {
    // Odd words a little more tricky
    // They still get the 44b at offset row*44
    // However the low 4b appear as the high 4b of the word!
    int      hi_shift_r = TcamConsts::kOddRowShuffle;      // ShiftR  4  [43:4] to  [39:0]
    int      lo_shift_l = TcamConsts::kWidth - hi_shift_r; // ShiftL 40   [3:0] to [43:40]
    uint64_t hi_mask = UINT64_C(0xFFFFFFFFFFFFFFFF) >> (64-lo_shift_l); // 0xFFFFFFFFFF
    uint64_t lo_mask = UINT64_C(0xFFFFFFFFFFFFFFFF) >> (64-hi_shift_r); // 0xF
    word = ((word & lo_mask) << lo_shift_l) | ((word >> hi_shift_r) & hi_mask);
  }
  return word;
}

void TcamColWrap::lookup(const uint8_t thread, const BV528 &input, int debug) {
  BV512 hits_in, hits_mask, hits_out_bot, hits_out_top;
  bool  and_top_bot = (rows_[TcamConsts::kMidRowLower]->chain_out() &&
                       rows_[TcamConsts::kMidRowUpper]->chain_out());

  hits_mask.fill_all_ones();
  hits_in.fill_all_ones();

  for (int r = TcamConsts::kBotRow; r <= TcamConsts::kMidRowLower; r++) {
    // Extract word_in from BV528 input
    uint64_t word_in = do_get_input_word(r, input);

    hits_out_bot.fill_all_zeros();
    rows_[r]->lookup(thread, word_in, hits_in, hits_mask, &hits_out_bot, debug);
    if (rows_[r]->chain_out()) {
      hits_in.copy_from(hits_out_bot);
    } else {
      hits_in.fill_all_ones();
    }
  }

  hits_in.fill_all_ones();

  for (int r = TcamConsts::kTopRow; r >= TcamConsts::kMidRowUpper; r--) {
    // Extract word_in from BV528 input
    uint64_t word_in = do_get_input_word(r, input);

    hits_out_top.fill_all_zeros();
    // At final top row (MidRowUpper) we may mask with final bottom row hits_out
    if ((r == TcamConsts::kMidRowUpper) && (and_top_bot)) hits_mask.copy_from(hits_out_bot);

    rows_[r]->lookup(thread, word_in, hits_in, hits_mask, &hits_out_top, debug);
    if (rows_[r]->chain_out()) {
      hits_in.copy_from(hits_out_top);
    } else {
      hits_in.fill_all_ones();
    }
  }
}

bool TcamColWrap::get_result(int ltcam, uint32_t *addr, uint8_t *abit, int *prio) {
  uint32_t tmp_addr = 0u, ret_addr = 0u;
  uint8_t tmp_abit = 0, ret_abit = 0;
  int tmp_prio = -1, ret_prio = -1;
  bool ret_hit = false;
  for (int r = 0; r < TcamConsts::kNumRows; r++) {
    if (rows_[r]->get_result(ltcam, &tmp_addr,  &tmp_abit, &tmp_prio)) {
      ret_hit = true;
      if (tmp_prio > ret_prio) { // Keep max_pri hit
        ret_prio = tmp_prio;
        ret_addr = tmp_addr;
        ret_abit = tmp_abit;
      }
    }
  }
  if (ret_hit) {
    *addr = ret_addr;
    *abit = ret_abit;
    *prio = ret_prio;
  }
  return ret_hit;
}

void TcamColWrap::get_result_bv(int row, int which, BV512 *bv) {
  rows_[row]->get_result_bv(which, bv);
}







TcamArrayWrap::TcamArrayWrap(TestUtil *tu)
    : tu_(tu),
      chip_(tu_->get_chip()),
      pipe_(tu_->get_pipe()),
      stage_(tu_->get_stage()) {
  RMT_ASSERT(stage_ == 0); // Has to be stage 0
  create();
}
TcamArrayWrap::~TcamArrayWrap() {
  destroy();
}

void TcamArrayWrap::create() {
  debug_ = 0;
  total_ltcam_hits_ = 0;
  ingress_ltcams_ = egress_ltcams_ = 0;
  seed_ = UINT64_C(0);
  last_data_input_seed_ = UINT64_C(0);
  last_output_.fill_all_zeros();
  for (int c = 0; c < TcamConsts::kNumCols; c++) {
    cols_[c] = new TcamColWrap(tu_, pipe_, stage_, c);
  }
}
void TcamArrayWrap::destroy() {
  for (int c = 0; c < TcamConsts::kNumCols; c++) {
    if (cols_[c] != nullptr) delete cols_[c];
    cols_[c] = nullptr;
  }
}
void TcamArrayWrap::set_debug(int debug) {
  debug_ = debug;
  for (int c = 0; c < TcamConsts::kNumCols; c++) cols_[c]->set_debug(debug);
}
void TcamArrayWrap::set_seed(uint64_t seed) {
  total_ltcam_hits_ = 0;
  ingress_ltcams_ = egress_ltcams_ = 0;
  seed_ = seed;
  last_data_input_seed_ = UINT64_C(0);
  last_output_.fill_all_zeros();
  // Generate some more seeds
  for (int c = 0; c < TcamConsts::kNumCols; c++)
    cols_[c]->set_seed( seed, tu_->xrand64(seed_, c) );
}
void TcamArrayWrap::configure_vh_xbar() {
  // Want to setup a fixed tcam_vh_xbar
  // Just linearly map input xbar ternary bytes
  // Every 6th byte get split into hi/lo nibbles with lo nibble on rowX, hi nibble on X+1
  // -- so each row gets 44b (though we'll prob only use 40)(so will ignore 6th bytes)
  for (int c = 0; c < TcamConsts::kNumCols; c++) {
    for (int r = 0; r < TcamConsts::kNumRows; r++) {
      int main_src = r;   // So row N gets main src N
      int xtra_src = r/2; // So row N gets xtra src N/2
      int xtra_nib = r%2; // So even rows get lo nibble, odd rows get hi nibble
      int bus      = c;   // Col0 uses bus0, col1 uses bus1
      tcam_row_vh_util::set_input_src_simple(chip_, pipe_, stage_, r,
                                             bus, true /*enable*/,
                                             main_src, xtra_src, xtra_nib);
    }
  }
}
void TcamArrayWrap::configure_other_csrs() {
  // Need to setup:
  // 0. Some relaxations
  MauDependencies::kRelaxThreadCheck = true;
  MauPredication::kRelaxPredicationCheck = true;
  Mau::kLookupUnusedLtcams = false;

  // 1. predication_ctl
  // 2. mpr_always_run/pred_always_run
  // 3. tcam_hit_to_logical_table_ixbar_outputmap
  // NB. We use just 8 LTs with each LT using identically numbered LTCAM/TernaryOutputBus
  //
  for (int lt = 0; lt < TcamConsts::kNumLogTables; lt++) {
    int ltc = lt; // Always 1-1 mapping
    bool ingress = (((ingress_ltcams_ >> ltc) & 1) == 1);
    bool egress  = (((egress_ltcams_  >> ltc) & 1) == 1);
    RMT_ASSERT( ! (ingress && egress) );
    tu_->table_config(pipe_, stage_, lt, ingress);
    tu_->set_table_default_regs(pipe_, stage_, lt);
  }
  for (int ltc = 0; ltc < TcamConsts::kNumLogTcams; ltc++) {
    tu_->ltcam_config(pipe_, stage_, ltc, ltc, ltc, 0 /*match_addr_shift*/);
  }
}
void TcamArrayWrap::configure(uint8_t ingress_ltcams, uint8_t egress_ltcams) {
  RMT_ASSERT((ingress_ltcams & egress_ltcams) == 0);
  ingress_ltcams_ = ingress_ltcams;
  egress_ltcams_ =  egress_ltcams;
  configure_vh_xbar();
  configure_other_csrs();
}
void TcamArrayWrap::configure_tcams(int colStart, int colEnd, int rowStart, int rowEnd,
                                    uint32_t flags, uint8_t ltcams, uint64_t info) {
  // Check only programming a single gress at a time
  bool ingress = ((ltcams & ingress_ltcams_) != 0);
  bool egress  = ((ltcams &  egress_ltcams_) != 0);
  RMT_ASSERT( ! (ingress && egress) );
  // Automagically switch on egress flag based on ltcams being used
  if (egress) flags |= TcamCtl::kEgressFlag; else flags &= ~TcamCtl::kEgressFlag;
  if (colStart <= colEnd) {
    for (int c = colStart; c <= colEnd; c++) {
      cols_[c]->configure_tcams(rowStart, rowEnd, flags, ltcams, info);
    }
  } else {
    for (int c = colStart; c >= colEnd; c--) {
      cols_[c]->configure_tcams(rowStart, rowEnd, flags, ltcams, info);
    }
  }
}


void TcamArrayWrap::install_config() {
  for (int c = 0; c < TcamConsts::kNumCols; c++) cols_[c]->install_config();

  // Also setup tcam_match_addr_shift to be max value 4 to provoke
  // warnings about address bits being lost when we use bitmaps with 16b
  // of output. This only on WIP but program it unconditionally
  auto& mau_base = RegisterUtils::ref_mau(pipe_,stage_);
  auto& tcams = mau_base.tcams;
  for (int row = 0; row < 8; row++) {
    auto a_tcam_match_adr_shift = &tcams.tcam_match_adr_shift[row];
    tu_->OutWord((void*)a_tcam_match_adr_shift, TcamConsts::kMatchAddrShift);
  }
}
void TcamArrayWrap::install_data() {
  for (int c = 0; c < TcamConsts::kNumCols; c++) cols_[c]->install_data();
}

void TcamArrayWrap::install() {
  install_config();
  install_data();
}

bool TcamArrayWrap::get_result(int ltcam, uint32_t *addr, uint8_t *abit, int *prio) {
  uint32_t tmp_addr = 0u, ret_addr = 0u;
  uint8_t tmp_abit = 0, ret_abit = 0;
  int tmp_prio = -1, ret_prio = -1;
  bool ret_hit = false;
  for (int c = 0; c < TcamConsts::kNumCols; c++) {
    if (cols_[c]->get_result(ltcam, &tmp_addr, &tmp_abit, &tmp_prio)) {
      ret_hit = true;
      if (tmp_prio > ret_prio) { // Keep max_pri hit
        ret_prio = tmp_prio;
        ret_addr = tmp_addr;
        ret_abit = tmp_abit;
      }
    }
  }
  if (ret_hit) {
    *addr = ret_addr;
    *abit = ret_abit;
    if (prio != nullptr) *prio = ret_prio;
  }
  return ret_hit;
}

void TcamArrayWrap::get_result_bv(int col, int row, int which, BV512 *bv) {
  cols_[col]->get_result_bv(row, which, bv);
}

uint64_t TcamArrayWrap::get_lookup_seed() {
  last_data_input_seed_ = tu_->mmix_rand64(last_data_input_seed_);
  return last_data_input_seed_;
}

void TcamArrayWrap::get_lookup_bv(uint64_t seed, BV528 *input) {
  // Fill in BV with random data derived from seed
  seed = (seed * UINT64_C(777011)) + UINT64_C(777013);
  for (int off = 0; off <= TcamConsts::kInputWidth; off += 64) {
    input->set_word(seed, off, 64);
    seed = tu_->mmix_rand64(seed);
  }
}

void TcamArrayWrap::lookup_tcams(const uint8_t thread, const BV528 &input, int debug) {
  for (int c = 0; c < TcamConsts::kNumCols; c++)
    cols_[c]->lookup(thread, input, debug);
}

void TcamArrayWrap::display_output(const uint8_t thread, const BVTcamTot &output,
                                   uint8_t diffs, int debug, const char *str) {
  int width = TcamConsts::kTcamOutWidth;
  for (int ltc = 0; ltc < TcamConsts::kNumLogTcams; ltc++) {
    uint32_t out = static_cast<uint32_t>( output.get_word(ltc * width, width) );
    uint32_t addr = PhvMauData::tcam_match_addr(out);
    uint8_t  hit  = PhvMauData::tcam_match_hit(out);
    uint8_t  abit = PhvMauData::tcam_match_abit(out);
    // Always print diffs - maybe print everything
    if ( (debug >= 3) ||  (((diffs >> ltc) & 1) == 1) )
      printf("%c%c %sLTC[%d] %s addr=0x%08x abit=%d\n",
             ((thread & TcamConsts::kIngress) != 0)?'I':' ',
             ((thread & TcamConsts::kEgress)  != 0)?'E':' ',
             (str==nullptr)?"":str, ltc, (hit==1)?" HIT":"MISS", addr, abit);
  }
}

void TcamArrayWrap::lookup_internal(const uint8_t thread, const BV528 &input,
                                    BVTcamTot *output, int debug) {
  int width = TcamConsts::kTcamOutWidth;
  // Do lookups
  lookup_tcams(thread, input, debug);
  // Find internal results and add to output BV
  for (int ltc = 0; ltc < TcamConsts::kNumLogTcams; ltc++) {
    int      prio = -1;
    uint32_t addr = 0u;
    uint8_t  abit = 0;
    bool     hit = get_result(ltc, &addr, &abit, &prio);
    uint64_t out = PhvMauData::make_tcam_match(addr, hit?1:0, abit);
    output->set_word(out, ltc * width, width);
    if ((out != UINT64_C(0)) && (debug >= 3)) {
      printf("TCAM[LTC=%d] Hit=%c Abit=%d Addr=0x%x PRIO=%d(0x%x)\n",
             ltc, hit?'T':'F', abit, addr, prio, prio);
    }
  }
}

void TcamArrayWrap::lookup_external1(const uint8_t thread, const BV528 &input,
                                     BVTcamTot *output) {
  if (((thread & TcamConsts::kIngress) == 0) && ((thread & TcamConsts::kEgress) == 0)) {
    // No thread - nothing to do
    last_output_.fill_all_zeros();
    output->fill_all_zeros();

  } else {
    const int width = TcamConsts::kTcamOutWidth;

    // Setup PHV with PHV pipe data containing:
    //  a) BV528 of TCAM Array input data (and set kTmIxbarData to be loadFromPhv)
    //  b) space for TcamResults (kTcamMatchAddr is calcAndStore)
    //
    Phv *phv = tu_->phv_alloc();
    if ((thread & TcamConsts::kIngress) != 0) phv->set_ingress();
    if ((thread & TcamConsts::kEgress)  != 0) phv->set_egress();

    phv->set_pipe_data_ctrl(stage_, PhvData::kIxbarTmData, PhvDataCtrl::kLoadFromPhv);
    for (int off = 0; off < TcamConsts::kInputWidth; off += 64) {
      phv->set_pipe_data(stage_, PhvData::kIxbarTmData, off, input.get_word(off), 64);
    }
    phv->set_pipe_data_ctrl(stage_, PhvData::kTcamMatchAddr, PhvDataCtrl::kCalcAndStore);

    int port_index = Port::make_port_index(pipe_, 1, 0);
    Port *port = tu_->get_objmgr()->port_get(port_index);
    (void)tu_->port_process_inbound(port, phv);

    // Extract results from PhvPipeData and add to output BV
    for (int ltc = 0; ltc < TcamConsts::kNumLogTcams; ltc++) {
      uint64_t out = phv->get_pipe_data(stage_, PhvData::kTcamMatchAddr, ltc * width, width);
      output->set_word(out, ltc * width, width);
    }
    phv->free_pipe_data();
    tu_->phv_free(phv);

    // Stash a copy of last_output - lookup_external2 replays it
    last_output_.copy_from(*output);
  }
}
void TcamArrayWrap::lookup_external2(const uint8_t thread, const BV528 &input,
                                     BVTcamTot *output) {
  // This func is virtual and it's anticipated it would be overridden
  // if anyone (eg DV) wanted to do a lookup in some other impl (eg RTL)
  // for comparsion. So this impl just duplicates result of external1 impl
  output->copy_from(last_output_);
}

uint8_t TcamArrayWrap::compare_outputs(const BVTcamTot &out1, const BVTcamTot &out2,
                                       int *ltcam_hits) {
  const int width = TcamConsts::kTcamOutWidth;
  int     hits = 0;
  uint8_t diffs = 0;
  for (int ltc = 0; ltc < TcamConsts::kNumLogTcams; ltc++) {
    uint64_t w1 = out1.get_word(ltc * width, width);
    uint64_t w2 = out2.get_word(ltc * width, width);
    if (w1 == w2) {
      if (w1 != UINT64_C(0)) hits++;
    } else {
      diffs |= (1<<ltc);
    }
  }
  if (ltcam_hits != nullptr) *ltcam_hits = hits;
  return diffs;
}

bool TcamArrayWrap::lookup_bv(const uint8_t thread, const BV528 &input, int debug) {
  // Maybe temporarily increase global debug
  int prev_debug = debug_;
  if (debug > debug_) set_debug(debug);

  // Lookup in internal TcamArray implementation
  BVTcamTot in_output;
  lookup_internal(thread, input, &in_output, debug);

  // Lookup in 2x external TcamArray implementations (RefModel, DV?)
  BVTcamTot ex1_output, ex2_output;
  lookup_external1(thread, input, &ex1_output);
  lookup_external2(thread, input, &ex2_output);

  // Check result of internal/external lookups the same
  int     ltcam_hits = 0;
  uint8_t diffs1 = compare_outputs(in_output, ex1_output, &ltcam_hits);
  uint8_t diffs2 = compare_outputs(in_output, ex2_output);
  uint8_t diffs  = diffs1 | diffs2;
  bool    is_same = (diffs == 0);

  // Keep track of total hits seen
  total_ltcam_hits_ += ltcam_hits;

  display_output(thread,  in_output, diffs, debug, "INT  ");
  display_output(thread, ex1_output, diffs, debug, "EXT1 ");
  if ( (debug >= 5) || ((diffs2 != 0) && (diffs1 != diffs2)) )
    display_output(thread, ex2_output, diffs, debug, "EXT2 ");

  // Restore global debug to what it was
  if (prev_debug < debug_) set_debug(prev_debug);
  return is_same;
}

bool TcamArrayWrap::lookup_seed(const uint8_t thread, uint64_t seed, int debug) {
  BV528 input;
  get_lookup_bv(seed, &input);
  return lookup_bv(thread, input, debug);
}

bool TcamArrayWrap::lookup(const uint8_t thread, int debug) {
  return lookup_seed(thread, get_lookup_seed(), debug);
}


} // namespace
