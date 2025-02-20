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

#include <atomic>
#include <model_core/spinlock.h>

namespace model_core {

Spinlock::Spinlock() = default;

void Spinlock::lock() {
  while (std::atomic_flag_test_and_set_explicit(
      &lock_, std::memory_order_acquire))
    ; // spin until the lock is acquired
}

void Spinlock::unlock() {
  std::atomic_flag_clear_explicit(
      &lock_, std::memory_order_release);
}

void Spinlock::clear() {
  lock_.clear();
}

}  // model_core

