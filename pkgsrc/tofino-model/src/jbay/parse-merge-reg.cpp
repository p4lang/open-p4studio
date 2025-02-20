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

#include <parse-merge-reg-chip.h>

namespace MODEL_CHIP_NAMESPACE {

ParseMergeRegJbay::ParseMergeRegJbay(int chipIndex, int pipeIndex) :
    ParseMergeReg(chipIndex, pipeIndex) { }

bool ParseMergeRegJbay::phv_get_tm_status_phv(int pipe, uint8_t *off16) {
  RMT_ASSERT((pipe >= 0) && (pipe < RmtDefs::kPipesMax) && (off16 != NULL));
  if ((prs_merge_pps_tm_status_phv_.en() == 0) ||
      (((prs_merge_pps_tm_status_phv_.pipe_mask() >> pipe) & 1) == 0)) return false;
  // XXX: the lsb of the off16 is ignored since this always refers to a
  // pair of 16b words
  *off16 = prs_merge_pps_tm_status_phv_.phv() & 0xfe;
  return true;
}

void ParseMergeRegJbay::phv_set_tm_status_phv(int pipe, uint8_t off16) {
  RMT_ASSERT((pipe >= 0) && (pipe < RmtDefs::kPipesMax));
  prs_merge_pps_tm_status_phv_.en(1);
  prs_merge_pps_tm_status_phv_.pipe_mask(1<<pipe);
  prs_merge_pps_tm_status_phv_.phv(off16);
}

bool ParseMergeRegJbay::phv_get_tm_status_phv_sec(int pipe,
                                              uint8_t *off16,
                                              uint8_t *which_half) {
  (void)pipe;
  (void)off16;
  (void)which_half;
  // phv_sec does not exist for jbay
  return false;
}

void ParseMergeRegJbay::phv_set_tm_status_phv_sec(uint8_t off16,
                                              uint8_t which_half) {
  (void)off16;
  (void)which_half;
  // phv_sec does not exist for jbay
}

}
