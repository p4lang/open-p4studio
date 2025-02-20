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

#ifndef _UTESTS_INPUT_XBAR_UTIL_
#define _UTESTS_INPUT_XBAR_UTIL_

#include <utests/test_namespace.h>

namespace MODEL_CHIP_TEST_NAMESPACE {

  namespace input_xbar_util {

    // set a particular output byte's source to a particular phv word. For 16 and 32 bit words you don't
    //   get to choose which byte within the word, you just get the one the xbar gives you. And no
    //   swizzling is done for the Ternary bytes (and of course there is no swizzler for the exact match
    //   bytes) - for swizzling use one of the later functions
    void set_byte_src(int chip,int pipe, int mau, int output_byte, bool enable, int which_phv_word );

    // Sets a four consecutive output bytes to output a particular 32 bit phv word. If the output bytes are
    //  in the ternary part it also sets the ternary swizzlers to swizzle the bytes back into the correct
    //  order.
    // Probably won't work if the bytes straddle the exact match and ternary parts.
    void set_32_bit_word_src(int chip,int pipe, int mau, int start_output_byte, bool enable, int which_phv_word );

    //  in the ternary part it also sets the ternary swizzlers to swizzle the bytes back into the correct
    //  order.
    // Probably won't work if the bytes straddle the exact match and ternary parts.
    void set_16_bit_word_src(int chip,int pipe, int mau, int start_output_byte, bool enable, int which_phv_word );

  }
}
#endif
