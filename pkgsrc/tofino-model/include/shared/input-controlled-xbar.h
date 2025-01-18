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

#ifndef __INPUT_CONTROLLED_XBAR_H__
#define __INPUT_CONTROLLED_XBAR_H__

#include <iostream>
#include <array>
#include <cinttypes>
#include <common/rmt-assert.h>

namespace MODEL_CHIP_NAMESPACE {

template <int N_INPUTS,int N_OUTPUTS,int WORD_WIDTH,
      bool CHECK_SINGLE_DRIVE=true>
 class InputControlledXbar {
 public:

 InputControlledXbar() {}
 ~InputControlledXbar() {}

 calculateOutputs( std::array<BitVector<WORD_WIDTH>,N_INPUTS> const& inputs,
                   std::array<BitVector<WORD_WIDTH>,N_OUTPUTS>& outputs,
                   std::array<int,N_INPUTS>  const& input_controls,
                   std::array<bool,N_INPUTS> const& input_enables ) {
   std::array<bool,N_OUTPUTS> written{};

   for (int i=0;i<N_INPUTS;++i) {
     if (input_enables[i]) {
       int output = input_controls[i];
       RMT_ASSERT( output < N_OUTPUTS );
       if (CHECK_SINGLE_DRIVE) {
         RMT_ASSERT( ! written[output] );
         written[output] = true;
       }
       outputs[output].copy_from( inputs[i] );
     }
     else {
       outputs[output].fill_all_zeros();
     }
   }
 }
                    
private:    
 DISALLOW_COPY_AND_ASSIGN(InputControlledXbar);
};
}

#endif

