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

#ifndef _UTESTS_TCAM_ROW_VH_UTIL_
#define _UTESTS_TCAM_ROW_VH_UTIL_

#include <utests/test_namespace.h>
#include <model_core/model.h>
#include <register_includes/reg.h> 
#include <rmt-defs.h>

namespace MODEL_CHIP_TEST_NAMESPACE {
  
  namespace tcam_row_vh_util {

    // set the input source for a bus (0 or 1) on a tcam row (0->15)
    //  main_src 0->15 gets 5 bytes from input xbar as follows:
    //    0   ->    128 .. 132    (ie bytes 128,129,130,131,132  then 133 is extra byte)
    //    1   ->    134 .. 138
    //    2   ->    139 .. 143    (144 is extra byte)
    //    3   ->    145 .. 149
    //  etc
    //  extra_byte_src 0->7 selects bytes:
    //    0   ->  133
    //    1   ->  144
    //    2   ->  155
    //   etc
    //  extra_nibble selects where the extra nibble comes from:
    //   0 = low nibble of extra byte
    //   1 = high nibble of extra byte
    //   2 = valid bit xbar output, low nibble for even rows, high nibble for odd rows
    //   3 = valid on bits 1:0, version on bits 3:2
    void set_input_src_simple( int chip, int pipe, int mau, int row, int bus ,
                               bool enable, int main_src, int extra_byte_src, int extra_nibble );
  }
}

#endif
