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

#ifndef MODEL_COMMON_MISSING_STD_FEATURES_
#define MODEL_COMMON_MISSING_STD_FEATURES_

// This file is here to ease the burden of not being able to update to a later
// compiler. N.B. All code must be conditionally included based on the
// __cplusplus version.

namespace std {

#if __cplusplus < 201402L

#ifndef ENABLE_IF_T_
#define ENABLE_IF_T_
template< bool B, class T = void >
using enable_if_t = typename enable_if<B,T>::type;
#endif // ENABLE_IF_T_

// <memory>
#ifndef DEFINED_MAKE_UNIQUE_
#define DEFINED_MAKE_UNIQUE_
template <typename T, typename... Args>
std::unique_ptr<T> make_unique_helper(std::false_type, Args &&... args) {
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
template <typename T, typename... Args>
std::unique_ptr<T> make_unique_helper(std::true_type, Args &&... args) {
  static_assert(
      std::extent<T>::value == 0,
      "make_unique<T[N]>() is forbidden, please use make_unique<T[]>().");
  using U = typename std::remove_extent<T>::type;
  return std::unique_ptr<T>(
      new U[sizeof...(Args)]{std::forward<Args>(args)...});
}
template <typename T, typename... Args>
std::unique_ptr<T> make_unique(Args &&... args) {
  return make_unique_helper<T>(std::is_array<T>(), std::forward<Args>(args)...);
}
#endif // DEFINED_MAKE_UNIQUE_

#endif

}  // namespace std

#endif // MODEL_COMMON_MISSING_STD_FEATURES_
