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


#include <tofino_lfsr.h>
#include <iostream>
using namespace std;
/*
 * LFSR for Tofino
 * Starts with zero, bit 0, 2, 3, 4 of current value are XORED
 * Current value is shifted right by 1
 * and the negative of XORED bit is inserted into MSB (64)
 */

namespace MODEL_CHIP_NAMESPACE {

void tofino_lfsr::clock(uint32_t clocks) {
  // The first array element is used as a mask below, not a tap - I am not sure what the
  //  point of this is, because the length of the LFSR is hard coded at 64 in other code below.
	const uint64_t lfsr_taps_[] = {0xFFFFFFFFFFFFFFFF,  (1<<4), (1<<3), (1<<2), (1<<0), 0};

	std::lock_guard<std::mutex> l(mutex_);
    // Run the random LFSR once per expired clock
    //clocks = (clocks % 4) + 1;
    clocks = 1;
	while(clocks) {
		uint64_t tap = 0;
		int i = 1;

        // This used to do which is incorrect and results in only a 128 cycle of values:
        //  tap ^= (lfsr_taps_[i++] & value_);
		while(lfsr_taps_[i])
                  tap ^= (lfsr_taps_[i++] & value_) ? UINT64_C(1) : UINT64_C(0);

		value_ >>= 1;
                value_ |= ((~tap) << 63);
		value_ &= lfsr_taps_[0];
		--clocks;
	}
}

}
