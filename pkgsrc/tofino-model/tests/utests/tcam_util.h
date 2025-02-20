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

#ifndef _TCAM_UTIL_
#define _TCAM_UTIL_

#include <utests/test_util.h>

#include <rmt-defs.h>
#include <rmt-object-manager.h>
#include <bitvector.h>
#include <port.h>
#include <phv.h>
#include <phv-pipe-data.h>



namespace MODEL_CHIP_TEST_NAMESPACE {

using namespace MODEL_CHIP_NAMESPACE;

class ResCtl { public:
  static constexpr int kWordIn         = 0;
  static constexpr int kHitsIn         = 1;
  static constexpr int kHitsMask       = 2;
  static constexpr int kMatches        = 3;
  static constexpr int kMatchesPostMrd = 4;
  static constexpr int kMatchesCombine = 5;
  static constexpr int kHitsOut        = 6;
};
class TcamCtl { public:
  static constexpr uint32_t kChainOutput   = 0x00000001u;
  static constexpr uint32_t kMatchOutput   = 0x00000002u;
  static constexpr uint32_t kChainLoFinal  = 0x00000100u; // Last bot row gets flags>>8
  static constexpr uint32_t kMatchLoFinal  = 0x00000200u;
  static constexpr uint32_t kChainHiFinal  = 0x00010000u; // Last top row gets flags>>16
  static constexpr uint32_t kMatchHiFinal  = 0x00020000u;
  static constexpr uint32_t kWideFlag      = 0x01000000u; // Even set for last TCAM in chain
  static constexpr uint32_t kEgressFlag    = 0x02000000u; // Determines whether TCAM I or E
  static constexpr uint32_t kNoAbit        = 0x10000000u; // Only relevant on Tofino/JBay
  static constexpr uint32_t kNoBitmap      = 0x20000000u; // Only relevant on WIP
  static constexpr uint32_t kUseResult0    = 0x40000000u; // Only relevant on WIP
  static constexpr uint32_t kStrictRowPrio = 0x80000000u; // Only relevant on WIP
  static uint32_t  bot_flags(uint32_t f) { return (f & 0xFFFFFF00u) | ((f>> 8) & 0xFFu); }
  static uint32_t  top_flags(uint32_t f) { return (f & 0xFFFFFF00u) | ((f>>16) & 0xFFu); }
};
class TcamConsts { public:
  static constexpr uint8_t  kIngress        = 1;
  static constexpr uint8_t  kEgress         = 2;

  static constexpr int      kNumCols        = 2;
  static constexpr int      kNumRows        = 12;
  static constexpr int      kInputWidth     = 528;
  static constexpr int      kWidth          = kInputWidth/kNumRows; // 44
  static constexpr int      kOddRowShuffle  = 4;
  static constexpr uint64_t kMask           = UINT64_C(0xFFFFFFFFFFFFFFFF) >> (64-kWidth);

  static constexpr int      kTopRow         = kNumRows - 1;
  static constexpr int      kMidRowUpper    = kNumRows/2;
  static constexpr int      kMidRowLower    = kMidRowUpper-1;
  static constexpr int      kBotRow         = 0;
  static constexpr uint16_t kRowMask        = 0xFFFF >> (16-kNumRows);

  static constexpr int      kNumLogTables   = 8;
  static constexpr int      kNumLogTcams    = 8;
  static constexpr int      kAddrWidth      = 9;
  static constexpr int      kEntries        = 1<<kAddrWidth; // 512

  static constexpr int      kEntryBitsSet   = 8; // How many bits to compare in w0/w1

  static constexpr int      kNumPhysResults = 8;

  static constexpr int      kMatchAddrShift = 4;
  static constexpr int      kTcamOutWidth   = PhvMauData::kTcamMatchOutputWidth;
  static constexpr int      kTotalOutWidth  = PhvMauData::kTcamMatchTotalWidth;

  static_assert( ((kNumRows % 2) == 0), "Code assumes even number Tcam rows");
  static_assert( ((kInputWidth % kNumRows) == 0),
                 "Tcam Array InputWidth should be multiple of num rows");
  static_assert( ((kTcamOutWidth * kNumLogTcams) == kTotalOutWidth),
                 "TcamOutWidth * NumLogTcams != kTotalOutWidth");
  static_assert( ((kNumPhysResults * 64) == kEntries),
                 "Sum of all phys results should fill TCAM");
};



class TcamPhysResults { public:
  uint8_t  width_;
  bool     is_bitmap_;
  bool     hit_;
  uint8_t  abit_;
  uint64_t raw_;
  int16_t  pri_l0_;
  int16_t  pri_l1_;
  int16_t  pri_l2_;
  int16_t  pri_l3_;
  uint16_t bmp_l0_;
  uint16_t bmp_l1_;
  uint16_t bmp_l2_;
  uint16_t bmp_l3_;
  uint16_t fin_;
  uint32_t fin_addr_;

  TcamPhysResults()  { width_ = 0; is_bitmap_ = false; reset(); }
  ~TcamPhysResults() { }

  void reset() {
    // Just reset dynamic vars
    hit_ = false;
    abit_ = 0;
    raw_ = UINT64_C(0);
    pri_l0_ = pri_l1_ = pri_l2_ = pri_l3_ = -1;
    bmp_l0_ = bmp_l1_ = bmp_l2_ = bmp_l3_ = 0;
    fin_ = 0; fin_addr_ = 0u;
  }
};



class TcamWrap {
 public:
  using BV512 = MODEL_CHIP_NAMESPACE::BitVector<512>;

  TcamWrap(TestUtil *tu, int pipe, int stage, int col, int row);
  ~TcamWrap();

  int      priority()   const { return priority_; }
  uint64_t seed()       const { return seed_; }
  uint8_t  ltcams_in()  const { return ltcams_input_; }
  uint8_t  ltcams()     const { return ltcams_; }
  uint8_t  ltcam()      const { return ltcam_; }
  uint8_t  log_table()  const { return logical_table_; }
  uint8_t  vpn()        const { return vpn_; }
  uint32_t flags()      const { return flags_; }
  uint64_t info()       const { return info_; }
  bool     chain_out()  const { return ((flags_ & TcamCtl::kChainOutput) != 0); }
  bool     match_out()  const { return ((flags_ & TcamCtl::kMatchOutput) != 0); }
  bool     wide()       const { return ((flags_ & TcamCtl::kWideFlag)    != 0); }
  bool     ingress()    const { return ((flags_ & TcamCtl::kEgressFlag)  == 0); }
  bool     egress()     const { return ((flags_ & TcamCtl::kEgressFlag)  != 0); }
  uint8_t  threads()    const { return ((ingress() ?TcamConsts::kIngress :0) |
                                        ( egress() ?TcamConsts::kEgress  :0)); }

  void     reset();
  void     create();
  void     destroy();
  void     set_debug(int debug);
  void     set_seed(uint64_t array_seed, uint64_t seed);
  void     configure(uint16_t row_mask, uint32_t flags, uint8_t ltcams, uint64_t info);
  void     install_config_chip();
  void     install_config();
  void     install_data();
  int      lookup(const uint8_t thread, const uint64_t word_in, BV512 *matches, int debug);
  int      lookup(const uint8_t thread, const uint64_t word_in,
                  const BV512 &hits_in, const BV512 &hits_mask, BV512 *hits_out, int debug);
  bool     get_result(int ltcam, uint32_t *addr, uint8_t *abit, int *prio);
  void     get_result_bv(int which, BV512 *bv);


 private:
  int16_t  combine_pri(int16_t hi, int16_t lo, int16_t inc);
  uint16_t combine_bmp(uint16_t hi, uint16_t lo);
  void     reset_phys_results();
  void     calc_phys_results(const BV512 &hits);
  void     maybe_set_abit(int result);
  int      do_set_priority(uint32_t flags);
  int      do_get_mask_bits();
  uint64_t do_calc_mask(uint64_t seed, int n_bits, uint64_t mask0);
  uint64_t do_calc_value(uint64_t seed, uint64_t mask);
  uint64_t do_calc_w0(uint64_t value, uint64_t mask);
  uint64_t do_calc_w1(uint64_t value, uint64_t mask);
  uint8_t  do_calc_abit(uint64_t value);
  uint64_t do_get_s0(uint64_t search);
  uint64_t do_get_s1(uint64_t search);
  void     do_calc_w0_w1(int i, uint64_t *w0, uint64_t *w1, uint8_t *abit,
                         int debug=0, const char *str=nullptr, uint64_t w64=UINT64_C(0));
  int      pick_result(uint8_t avail, uint8_t usable, uint8_t *still_avail);
  int      pick_width(uint8_t avail_from_res, uint8_t max_width);
  bool     pick_pri(int physres);
  void     recurse_setting_width_type(int result, int width, bool pri);
  void     pick_result_config();
  uint64_t do_compare(uint64_t w0, uint64_t w1, uint64_t s0, uint64_t s1);
  bool     do_match(uint64_t w0, uint64_t w1, uint64_t s0, uint64_t s1, int debug);
  void     do_apply_mrd(const BV512 &matches, BV512 *mrd);
  void     do_apply_mask(const BV512 &input, const BV512 &mask, BV512 *output);

 private:
  TestUtil  *tu_;
  int        pipe_;
  int        stage_;
  int        col_;
  int        row_;
  int        debug_;
  int        priority_;
  uint64_t   array_seed_;
  uint64_t   seed_;
  uint64_t   cnf_seed_;
  uint64_t   value_seed_;
  uint64_t   mask_seed_;

  uint8_t    ltcams_input_;
  uint8_t    ltcams_;
  uint8_t    ltcam_;
  uint8_t    logical_table_;
  uint8_t    vpn_;
  uint16_t   row_mask_;
  uint32_t   flags_;
  uint64_t   info_;

  int        last_n_matches_;
  uint64_t   last_word_in_;           // Last search word (only 40b used)
  BV512      last_hits_in_;           // Hits in from chained TCAM (otherwise all ones)
  BV512      last_hits_mask_;         // Mask to apply prior to hit output (eg if 5/6 combine)
  BV512      last_matches_;           // Raw matches this TCAM
  BV512      last_matches_post_mrd_;  // Raw matches this TCAM plus MRD spread
  BV512      last_matches_combine_;   // matches_post_mrd combined with hits_in
  BV512      last_hits_out_;          // matches_combine further combined with hits_mask

  std::array< uint8_t,         TcamConsts::kNumLogTcams >    ltcam_to_physres_;
  std::array< uint8_t,         TcamConsts::kNumPhysResults > physres_to_ltcam_;
  std::array< TcamPhysResults, TcamConsts::kNumPhysResults > res_;
};




class TcamColWrap {
 public:
  using BV512 = MODEL_CHIP_NAMESPACE::BitVector<512>;
  using BV528 = MODEL_CHIP_NAMESPACE::BitVector<528>;

  TcamColWrap(TestUtil *tu, int pipe, int stage, int col);
  ~TcamColWrap();

  uint64_t seed()  const { return seed_; }

  void     create();
  void     destroy();
  void     set_debug(int debug);
  void     set_seed(uint64_t array_seed, uint64_t seed);
  void     configure_tcams_top(int rowHi, int rowLo, uint16_t row_mask,
                           uint32_t flags, uint8_t ltcams, uint64_t info);
  void     configure_tcams_bot(int rowLo, int rowHi, uint16_t row_mask,
                           uint32_t flags, uint8_t ltcams, uint64_t info);
  void     configure_tcams(int rowStart, int rowEnd,
                           uint32_t flags, uint8_t ltcams, uint64_t info);
  void     install_config_chip();
  void     install_config();
  void     install_data();
  void     lookup(const uint8_t thread, const BV528 &input, int debug);
  bool     get_result(int ltcam, uint32_t *addr, uint8_t *abit, int *prio);
  void     get_result_bv(int row, int which, BV512 *bv);


 private:
  uint64_t do_get_input_word(int row, const BV528 &input);

 private:
  TestUtil                                       *tu_;
  int                                             pipe_;
  int                                             stage_;
  int                                             col_;
  int                                             debug_;
  uint64_t                                        array_seed_;
  uint64_t                                        seed_;
  std::array < TcamWrap*, TcamConsts::kNumRows >  rows_;
};




class TcamArrayWrap {
 public:
  using BV512     = MODEL_CHIP_NAMESPACE::BitVector<512>;
  using BV528     = MODEL_CHIP_NAMESPACE::BitVector<528>;
  using BVTcamTot = MODEL_CHIP_NAMESPACE::BitVector<TcamConsts::kTotalOutWidth>;

  TcamArrayWrap(TestUtil *tu);
  ~TcamArrayWrap();

  uint64_t seed()        const { return seed_; }
  int      total_hits()  const { return total_ltcam_hits_; }

  void     create();
  void     destroy();
  void     set_debug(int debug);
  void     set_seed(uint64_t seed);
  void     configure_tcams(int colStart, int colEnd, int rowStart, int rowEnd,
                           uint32_t flags, uint8_t ltcams, uint64_t info);
  void     configure(uint8_t ingress_ltcams, uint8_t egress_ltcams);
  void     install_config();
  void     install_data();
  void     install();
  bool     get_result(int ltcam, uint32_t *addr, uint8_t *abit, int *prio=nullptr);
  void     get_result_bv(int col, int row, int which, BV512 *bv);

  uint64_t get_lookup_seed();
  void     get_lookup_bv(uint64_t seed, BV528 *input);
  void     lookup_tcams(const uint8_t thread, const BV528 &input, int debug);

  void     display_output(const uint8_t thread, const BVTcamTot &output,
                          uint8_t diffs, int debug, const char *str=nullptr);
  uint8_t  compare_outputs(const BVTcamTot &out1, const BVTcamTot &out2,
                           int *ltcam_hits=nullptr);
  bool     lookup_bv(const uint8_t thread, const BV528 &input, int debug);
  bool     lookup_seed(const uint8_t thread, uint64_t seed, int debug);
  bool     lookup(const uint8_t thread, int debug);

  virtual void configure_vh_xbar();
  virtual void configure_other_csrs();
  virtual void lookup_internal(const uint8_t thread, const BV528 &input,
                               BVTcamTot *output, int debug);
  virtual void lookup_external1(const uint8_t thread, const BV528 &input,
                                BVTcamTot *output);
  virtual void lookup_external2(const uint8_t thread, const BV528 &input, BVTcamTot *output);


 private:
  TestUtil                                          *tu_;
  int                                                chip_;
  int                                                pipe_;
  int                                                stage_;
  int                                                debug_;
  int                                                total_ltcam_hits_;
  uint8_t                                            ingress_ltcams_;
  uint8_t                                            egress_ltcams_;
  uint64_t                                           seed_;
  uint64_t                                           last_data_input_seed_;
  BVTcamTot                                          last_output_;
  std::array < TcamColWrap*, TcamConsts::kNumCols >  cols_;
};


}

#endif /*  _TCAM_UTIL_ */
