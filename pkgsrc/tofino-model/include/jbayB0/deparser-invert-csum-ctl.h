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

#ifndef _JBAY_B0_DEPARSER_INVERT_CSUM_CTL_H_
#define _JBAY_B0_DEPARSER_INVERT_CSUM_CTL_H_

#include <pipe-csr.h>


namespace MODEL_CHIP_NAMESPACE {

class DeparserInvertCsumCtl {
  static constexpr int kDprsrIppScratchAddr  = static_cast<int>(BFN_REG_TOP(pipes_pardereg_dprsrreg_dprsrreg_inp_ipp_scratch_address));
  static constexpr int kDprsrIcrScratchAddr  = static_cast<int>(BFN_REG_TOP(pipes_pardereg_dprsrreg_dprsrreg_inp_icr_scratch_address));
  static constexpr int kDprsrIcrScratch2Addr = static_cast<int>(BFN_REG_TOP(pipes_pardereg_dprsrreg_dprsrreg_inp_icr_scratch2_address));
  static constexpr int kBaseClotCsum         = 0;
  static constexpr int kNumClotCsums         = 16;
  static constexpr int kBasePhvCsum          = kNumClotCsums;
  static constexpr int kNumPhvCsums          = 8;

 public:
  DeparserInvertCsumCtl(int chip, int pipe)
      : ipp_scratch_(chip, pipe, kDprsrIppScratchAddr, "ipp_scratch"),
        icr_scratch_(chip, pipe, kDprsrIcrScratchAddr, "icr_scratch"),
        icr_scratch2_(chip, pipe, kDprsrIcrScratch2Addr, "icr_scratch2")
  {
    ipp_scratch_.reset();
    icr_scratch_.reset();
    icr_scratch2_.reset();
  }
  ~DeparserInvertCsumCtl() { }

  uint32_t get_csum_invert(int csum_eng) const {
    switch (csum_eng) {
      case 0: return icr_scratch_.value() & 0xFFFFFFu;
      case 1: return ((icr_scratch2_.value() & 0xFFFFu) << 8) | (icr_scratch_.value() >> 24);
      case 2: return ((ipp_scratch_.value() & 0xFFu) << 16) | (icr_scratch2_.value() >> 16);
      case 3: return ipp_scratch_.value() >> 8;
      case 4: case 5: case 6: case 7: return 0u; // Invert not supported for these
      default: RMT_ASSERT(0 && "Bad dprsr csum engine");
    }
  }
  bool get_csum_invert_clot(int csum_eng, int clot_num) const {
    RMT_ASSERT(((clot_num >= 0) && (clot_num < kNumClotCsums)) && "Bad dprsr csum clot");
    return (((get_csum_invert(csum_eng) >> (clot_num + kBaseClotCsum)) & 1) == 1);
  }
  bool get_csum_invert_phv(int csum_eng, int phv_num) const {
    RMT_ASSERT(((phv_num >= 0) && (phv_num < kNumPhvCsums)) && "Bad dprsr csum phv");
    return (((get_csum_invert(csum_eng) >> (phv_num + kBasePhvCsum)) & 1) == 1);
  }

 private:
  register_classes::PipeCsr ipp_scratch_;
  register_classes::PipeCsr icr_scratch_;
  register_classes::PipeCsr icr_scratch2_;

};


} // namespace MODEL_CHIP_NAMESPACE

#endif // _JBAY_B0_DEPARSER_INVERT_CSUM_CTL_H_
