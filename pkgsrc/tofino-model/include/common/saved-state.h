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

#ifndef _SAVED_STATE
#define _SAVED_STATE

#include "missing_std_features.h"
#include <memory>
#include <vector>

namespace model_common {

template <typename T> class SavedStateManagerInterface {
protected:
  std::unique_ptr<std::vector<T>> states_ = std::make_unique<std::vector<T>>();
  bool saving_ = true;

public:
  virtual void replay() = 0;
  inline const bool is_saving() { return saving_; };
  void enable_saving() { saving_ = true; };
  void disable_saving() { saving_ = false; };
  size_t size() { return states_->size(); };
  void add_state(T state) {
    if (is_saving()) {
      states_->push_back(state);
    }
  };

};

} // namespace model_common

#endif
