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

#ifndef _MODEL_CORE_SHADOW_COUNTER_
#define _MODEL_CORE_SHADOW_COUNTER_

#include <array>
#include <cstdint>
#include <functional>
#include <common/rmt-util.h>

namespace model_core {

using namespace model_common;

/**
 * Maintain an array of wrapping counters; the counter values can be sampled
 * via a given setter method.
 *
 * @tparam COUNTER_ARRAY_SIZE The size of the counter array.
 */
template <int COUNTER_ARRAY_SIZE>
class ShadowCounterArray {
 private:
  typedef std::function<void(uint32_t, uint64_t)> setter_func_t;
  std::array<uint64_t, COUNTER_ARRAY_SIZE> values_;
  int width_;
  setter_func_t setter_func_;

 public:
  /**
   * Constructor
   * @param width The width, in bits, of each wrapping counter in the array.
   * @param setter_func A function that will be called back with each counter's
   *    index and value when the `sample()` method is called.
   */
  ShadowCounterArray<COUNTER_ARRAY_SIZE>(int width,
                                         const setter_func_t& setter_func) :
      values_(),
      width_(width),
      setter_func_(setter_func) {
    RMT_ASSERT(nullptr != setter_func);
  }

  /**
   * Increment a counter with the given amount.
   * @param index Counter array index.
   * @param amount Amount by which to increment the counter.
   * @return The value of the counter after it has been incremented.
   */
  uint64_t increment(int index, uint64_t amount=1) {
    values_[index] = Util::increment_and_wrap(values_[index], width_, amount);
    return values_[index];
  }

  /**
   * Causes the `setter_func` callback function to be called for each counter
   * in the array.
   */
  void sample() {
    for (uint32_t i = 0; i < COUNTER_ARRAY_SIZE; i++) {
      setter_func_(i, values_[i]);
    }
  }
};

}
#endif //_MODEL_CORE_SHADOW_COUNTER_
