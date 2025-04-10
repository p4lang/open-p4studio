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

#ifndef _TOFINO_LFSR_
#define _TOFINO_LFSR_
#include <mutex>

namespace MODEL_CHIP_NAMESPACE {
	class tofino_lfsr {
		public:
			tofino_lfsr(uint64_t seed):value_(seed) {}
			uint64_t get_value(void) const { return value_; }
			void clock(uint32_t clocks);
			void reset(uint64_t value) { value_ = value; }
		private:
			std::mutex mutex_;
			uint64_t value_;
	};
};



#endif // _TOFINO_LFSR_
