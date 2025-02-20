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

#ifndef _SHARED_PHV_PIPE_DATA_
#define _SHARED_PHV_PIPE_DATA_

#include <rmt-defs.h>
#include <bitvector.h>


namespace MODEL_CHIP_NAMESPACE {


class PhvDataCtrl {
 public:
  static constexpr uint8_t kCalcOnly       = 0;
  static constexpr uint8_t kCalcAndStore   = 1;
  static constexpr uint8_t kLoadFromPhv    = 2;
  static constexpr uint8_t kCalcComparePhv = 3; // TODO: Add this?
  static bool do_calc(uint8_t ctl)  { return (ctl == kCalcOnly); }
  static bool do_store(uint8_t ctl) { return (ctl == kCalcAndStore); }
  static bool do_load(uint8_t ctl)  { return (ctl == kLoadFromPhv);  }
};


class PhvData {
 public:
  static constexpr int kMin           =  1;
  static constexpr int kIxbarData     =  1;
  static constexpr int kIxbarXmData   =  2;
  static constexpr int kIxbarTmData   =  3;
  static constexpr int kTcamMatchAddr =  4;
  static constexpr int kXmHash        =  5;
  static constexpr int kActionHv      =  6;
  static constexpr int kInstrAddr     =  7;
  static constexpr int kMax           =  8;
};


class PhvMauData {
  // MAU Input XBar
  static constexpr int kExactMatchBytes       = MauDefs::kExactMatchBytes;
  static constexpr int kTernaryMatchBytes     = MauDefs::kTernaryMatchBytes;
  static constexpr int kExactMatchInputBits   = MauDefs::kExactMatchInputBits;
  static constexpr int kTernaryMatchInputBits = MauDefs::kTernaryMatchInputBits; // Post swizzle
  static constexpr int kExactFirstBit         = 0;
  static constexpr int kTernaryFirstBit       = MauDefs::kTernaryFirstBit;
  static constexpr int kTotalMatchInputBits   = MauDefs::kTotalMatchInputBits;
  // LTCAM match addresses
  static constexpr int kLogicalTcams         = MauDefs::kLogicalTcamsPerMau;
  static constexpr int kMatchAddrWidth       = MauDefs::kTableResultMatchAddrWidth;
  // Tokens to allow 64b tcam_match_data to be split into Abit[20] Hit[19] Addr[18:0}
  static constexpr int      kTcamMatchAddrOff   = 0;
  static constexpr int      kTcamMatchAddrWidth = kMatchAddrWidth;
  static constexpr uint32_t kTcamMatchAddrMask  = 0xFFFFFFFFu >> (32-kTcamMatchAddrWidth);
  static constexpr int      kTcamMatchHitOff    = kTcamMatchAddrOff + kTcamMatchAddrWidth;
  static constexpr int      kTcamMatchHitWidth  = 1;
  static constexpr uint8_t  kTcamMatchHitMask   = 0xFF >> (8-kTcamMatchHitWidth);
  static constexpr int      kTcamMatchAbitOff   = kTcamMatchHitOff + kTcamMatchHitWidth;
  static constexpr int      kTcamMatchAbitWidth = 1;
  static constexpr uint8_t  kTcamMatchAbitMask  = 0xFF >> (8-kTcamMatchAbitWidth);
  static constexpr int      kTcamMatchNextOff   = kTcamMatchAbitOff + kTcamMatchAbitWidth;

 public:
  // Static funcs to extract/form tcam_match_data
  static uint32_t tcam_match_addr(uint64_t d) {
    return static_cast<uint32_t>(d >> kTcamMatchAddrOff) & kTcamMatchAddrMask;
  }
  static uint8_t tcam_match_hit(uint64_t d) {
    return static_cast<uint8_t>(d >> kTcamMatchHitOff) & kTcamMatchHitMask;
  }
  static uint8_t tcam_match_abit(uint64_t d) {
    return static_cast<uint8_t>(d >> kTcamMatchAbitOff) & kTcamMatchAbitMask;
  }
  static uint64_t make_tcam_match(uint32_t addr, uint8_t hit, uint8_t abit) {
    return ( ((addr & kTcamMatchAddrMask) << kTcamMatchAddrOff) |
             ((hit  & kTcamMatchHitMask)  << kTcamMatchHitOff) |
             ((abit & kTcamMatchAbitMask) << kTcamMatchAbitOff) );
  }
  static constexpr int kTcamMatchOutputWidth = kTcamMatchNextOff;
  static constexpr int kTcamMatchTotalWidth  = kTcamMatchOutputWidth * kLogicalTcams;

  PhvMauData(int mau);
  ~PhvMauData();

  void     set_ixbar_data(int bit_offset, uint64_t data, int width);
  void     set_ixbar_xm_data(int bit_offset, uint64_t data, int width);
  void     set_ixbar_tm_data(int bit_offset, uint64_t data, int width);
  void     set_tcam_match_data(int bit_offset, uint64_t data, int width);
  void     set_tcam_match_addr(int ltcam, uint32_t match_addr, uint8_t hit, uint8_t action_bit);

  uint64_t get_ixbar_data(int bit_offset, int width);
  uint64_t get_ixbar_xm_data(int bit_offset, int width);
  uint64_t get_ixbar_tm_data(int bit_offset, int width);
  uint64_t get_tcam_match_data(int bit_offset, int width);
  void     get_tcam_match_addr(int ltcam, uint32_t *match_addr, uint8_t *hit, uint8_t *action_bit);

  void     set_mau_data(int what_data, int bit_offset, uint64_t data, int width);
  uint64_t get_mau_data(int what_data, int bit_offset, int width);

  void     set_mau_data_ctrl(int what_data, uint8_t ctrl);
  uint8_t  get_mau_data_ctrl(int what_data);

 private:
  int                                   mau_;
  std::array< uint8_t, PhvData::kMax >  ctrl_;
  BitVector<kTotalMatchInputBits>       ixbar_data_;
  BitVector<kTcamMatchTotalWidth>       tcam_match_;
};


class PhvPipeData {
  static constexpr int kStages = RmtDefs::kStagesMax;

 public:
  PhvPipeData(int pipe);
  ~PhvPipeData();

 private:
  PhvMauData *phv_mau_data_create(int mau);
  PhvMauData *phv_mau_data_lookup(int mau);
  PhvMauData *phv_mau_data_set(int mau, PhvMauData *mau_data);
  PhvMauData *phv_mau_data_get(int mau);

 public:
  void     set_pipe_data(int mau, int what_data, int bit_offset, uint64_t data, int width);
  uint64_t get_pipe_data(int mau, int what_data, int bit_offset, int width);

  void     set_pipe_data_tcam_match_addr(int mau, int ltcam,
                                         uint32_t match_addr, uint8_t hit, uint8_t action_bit);
  void     get_pipe_data_tcam_match_addr(int mau, int ltcam,
                                         uint32_t *match_addr, uint8_t *hit, uint8_t *action_bit);

  void     set_pipe_data_ctrl(int mau, int what_data, uint8_t ctrl);
  uint8_t  get_pipe_data_ctrl(int mau, int what_data);

 private:
  int                                  pipe_;
  std::array< PhvMauData*, kStages >   maus_;
};


}

#endif // _SHARED_PHV_PIPE_DATA_
