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

#ifndef MODEL_COMMON_EMPLACE_ARRAY_
#define MODEL_COMMON_EMPLACE_ARRAY_

#include <array>
#include <cstddef>
#include <type_traits>
#include <functional>

#include "common/missing_std_features.h"  // std::enable_if_t

namespace model_common {

// Helper templates for recursively creating the initialisation list.
template<std::size_t N, typename T, typename... Ts>
struct ArrayEmplacer {
    // Recursively call emplace(), adding an extra `T` to the `Ts`, until T+Ts is N in length.
    template <unsigned NTs=N-1, std::enable_if_t<sizeof...(Ts) < NTs, bool> = true>
    static std::array<T, N> emplace(std::function<T()> ctor) {
        // The added 'T' will be pushed into the 'Ts' parameter pack.
        return ArrayEmplacer<N, T, T, Ts...>::emplace(ctor);
    }
    // Stop when we have enough `T` parameters in total.
    template <unsigned NTs=N-1, std::enable_if_t<sizeof...(Ts) == NTs, bool> = true>
    static std::array<T, N> emplace(std::function<T()> ctor) {
        // Construct the std::array using an initialisation list.
        // Return-Value-Optimisation will do the rest.
        return std::array<T, N>{ T(ctor()), Ts(ctor())... };
    }
};

// Returns an initialised std::array<T, N>, via Return-Value-Optimisation.
// The functor `ctor` is called to create each element.
// See `EBufRegArray::operator()` for example usage.
template<typename T, std::size_t N>
std::array<T, N> emplace_array(std::function<T(void)> ctor) {
    return ArrayEmplacer<N, T>::emplace(ctor);
}

}  // namespace model_common

#endif // MODEL_COMMON_EMPLACE_ARRAY_
