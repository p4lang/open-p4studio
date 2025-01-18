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

#ifndef _UTESTS_ACTION_HV_TRANSLATOR_
#define _UTESTS_ACTION_HV_TRANSLATOR_

#include <utests/test_namespace.h>

namespace MODEL_CHIP_TEST_NAMESPACE {

class TestUtil;

// Translates from old action hv config (ouput xbar) to new
//   config (input xbar)
// Old writes must be turned into ctl_ calls (use update_action_hv_cfg.pl)
//  then call do_writes to do the new writes.
class ActionHvTranslator {
 public:
  ActionHvTranslator() {}
  ~ActionHvTranslator() {}
  void ctl_word(int row,int side,int index,uint32_t value);
  void ctl_half(int row,int side,int index,uint32_t value);
  void ctl_byte(int row,int side,uint32_t value);
  void ctl_byte_enable(int row,int side,uint32_t value);
  void do_writes(TestUtil* tu);

  static constexpr int  kBytesIn  = 16;
  static constexpr int  kBytesOut = 160;
  static constexpr int  kXbarBytes     = 16;
  static constexpr int  kXbarHalfWords = 28;
  static constexpr int  kXbarWords     = 22;

 private:
  struct Src {
    bool enable=false;
    int  input=0;
  };
  
  Src byte_outputs[8][2][kXbarBytes] {};
  Src half_outputs[8][2][kXbarHalfWords] {};
  Src word_outputs[8][2][kXbarWords] {};

  // tells you which input byte is connected to a particular input of an output_byte's mux
  // taken from old action-output-hv-xbar.h
  int InputByte( int output_byte, int mux_input );

};
}

#endif // _UTESTS_ACTION_HV_TRANSLATOR_
