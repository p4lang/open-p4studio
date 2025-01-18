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

#ifndef _JBAY_DEPARSER_INVERT_CSUM_CTL_H_
#define _JBAY_DEPARSER_INVERT_CSUM_CTL_H_

namespace MODEL_CHIP_NAMESPACE {

class DeparserInvertCsumCtl {

 public:
  DeparserInvertCsumCtl(int chip, int pipe) { }
  ~DeparserInvertCsumCtl() { }

  uint32_t get_csum_invert(int csum_eng)                    const { return 0u; }
  bool     get_csum_invert_clot(int csum_eng, int clot_num) const { return GLOBAL_FALSE; }
  bool     get_csum_invert_phv(int csum_eng, int phv_num)   const { return GLOBAL_FALSE; }
};

} // namespace MODEL_CHIP_NAMESPACE

#endif // _JBAY_DEPARSER_INVERT_CSUM_CTL_H_
