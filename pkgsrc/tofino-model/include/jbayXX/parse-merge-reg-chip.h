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

#ifndef _JBAY_PARSE_MERGE_REG_CHIP_H_
#define _JBAY_PARSE_MERGE_REG_CHIP_H_

#include <parse-merge-reg.h>

namespace MODEL_CHIP_NAMESPACE {

class ParseMergeRegJbay : public ParseMergeReg {
 public:
  ParseMergeRegJbay(int chipIndex, int pipeIndex);
  ~ParseMergeRegJbay() { }

  bool phv_get_tm_status_phv(int pipe, uint8_t *off16) override;
  void phv_set_tm_status_phv(int pipe, uint8_t off16) override;
  bool phv_get_tm_status_phv_sec(int pipe, uint8_t *off16, uint8_t *which_half) override;
  void phv_set_tm_status_phv_sec(uint8_t off16, uint8_t which_half) override;

};

typedef ParseMergeRegJbay ParseMergeRegType;

}

#endif //_JBAY_PARSE_MERGE_REG_CHIP_H_
