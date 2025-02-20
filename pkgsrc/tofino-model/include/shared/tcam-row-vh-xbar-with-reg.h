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

#ifndef _SHARED_TCAM_ROW_VH_WITH_REG_H__
#define _SHARED_TCAM_ROW_VH_WITH_REG_H__

#include <array>
#include <cstdint>
#include <rmt-log.h>
#include <mau-defs.h>
#include <rmt-object.h>
#include <bitvector.h>
#include <register_includes/tcam_row_halfbyte_mux_ctl.h>
#include <register_includes/tcam_row_output_ctl.h>
#include <register_includes/tcam_validbit_xbar_ctl_array3.h>
#include <register_includes/tcam_extra_byte_ctl.h>
#include <register_includes/tcam_extra_bit_mux_ctl.h>

namespace MODEL_CHIP_NAMESPACE {

class TcamRowVhWithReg {
public:
  static constexpr int kTotalMatchBytes        = MauDefs::kExactMatchBytes + MauDefs::kTernaryMatchBytes;
  static constexpr int kTotalMatchBits         = kTotalMatchBytes*8;

  static constexpr int kOutputBits             = MauDefs::kTernaryRowMatchBits;
  static constexpr int kTernaryFirstByte       = MauDefs::kTernaryFirstByte;

  static constexpr int kVersionDataWidth       = MauDefs::kVersionBits;
  static constexpr int kFirstRankBytes         = 11;
  static constexpr int kSecondRankValids       = 8;

  // This outputs to 2 rows of the 16, so specify which half_row (0 to 7) it is
  //  and which bus (0 or 1) it drives on each of the two rows.
  TcamRowVhWithReg( int chip, int pipe, int mau, int half_row, int bus);
  ~TcamRowVhWithReg() {};

  // Takes all of the output of the input mux data and valids (including the
  //   exact match data and valids)
  void CalculateSearchData(
      const BitVector<kTotalMatchBits>&  input,
      const BitVector<kTotalMatchBytes>& input_valid,
      const BitVector<kVersionDataWidth>& ingress_version_bits,
      const BitVector<kVersionDataWidth>& egress_version_bits,
      BitVector<kOutputBits> *search_data_even,
      BitVector<kOutputBits> *search_data_odd );

private:
  const uint half_row_;
  const uint bus_;
  bool reset_complete_=false;

  // registers
  std::array<register_classes::TcamRowOutputCtl,2> row_output_ctl_;
  std::array<register_classes::TcamRowHalfbyteMuxCtl,2> half_byte_mux_ctl_;
  register_classes::TcamExtraByteCtl extra_byte_ctl_;
  register_classes::TcamRowHalfbyteMuxCtl half_byte_mux_ctl_odd_;

  // just need one slice of this array, but it's all or nothing at the moment
  // no longer used in jbay
  register_classes::TcamValidbitXbarCtlArray3 valid_bit_xbar_ctl_array_;

  // added in jbay
  std::array<register_classes::TcamExtraBitMuxCtl,2> extra_bit_mux_ctl_;

  // control values extracted from the array registers
  std::array< int, kSecondRankValids > valid_bit_sel_;
  std::array< bool, kSecondRankValids > valid_bit_enable_;

  std::array< int, kFirstRankBytes >   output_byte_source_;
  std::array< bool, kFirstRankBytes >   output_byte_enable_;

  void ValidBitXbarArrayCallback(uint32_t a2, uint32_t a1, uint32_t a0);

  void CalculateSources();

  uint8_t CalculateTopNibble(int bus,
                             const BitVector<kTotalMatchBits>&  input,
                             const BitVector<kTotalMatchBytes>& input_valid,
                             const BitVector<kVersionDataWidth>& ingress_version_bits,
                             const BitVector<kVersionDataWidth>& egress_version_bits);

private:
  DISALLOW_COPY_AND_ASSIGN(TcamRowVhWithReg);
};

}

#endif // _SHARED_TCAM_ROW_VH_WITH_REG_H__
