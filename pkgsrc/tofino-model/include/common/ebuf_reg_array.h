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

#ifndef MODEL_COMMON_EBUF_REG_ARRAY_
#define MODEL_COMMON_EBUF_REG_ARRAY_

#include <array>
#include <cstddef>
#include <type_traits>
#include <functional>

#include "common/emplace_array.h"
#include "common/rmt-assert.h"

namespace model_common {

// Helper class for filling in EgressBufChanGroupImpl arrays.
template<
    typename T,          // EgressBufChanGroupImpl<register_classes::Ebuf...>
    unsigned NumSlices,  // e.g. RmtDefs::kEgressBufSlicesPerPipe
    unsigned NumEbufs,   // e.g. RmtDefs::kEgressBufEbuf400PerSlice
    unsigned NumChans,   // e.g. RmtDefs::kEgressBufChannelsPerEbuf400
    std::size_t N = NumSlices * NumEbufs * NumChans // don't override!
>
class EBufRegArray {
public:
    // The helper function.
    // See egress-buf.cpp for example usage.
    //
    // create() instantiates the EBufRegArray functor,
    // and calls emplace_array, passing the functor as an argument.
    static std::array<T, N>
    create(int chipIndex, int pipeIndex) {
        std::function<T()> ctor = EBufRegArray(chipIndex, pipeIndex);
        return emplace_array<T, N>(ctor);
    }

    // Turn EBufRegArray into a functor of type `std::function<T(void)>`.
    // The functor creates each EgressBufChanGroupImpl<> element, for emplace_array to emplace.
    // Depending upon NumEbufs, the "operator()" with or without ebufIndex_ will be picked (SFINAE).
    template <unsigned U=NumEbufs, std::enable_if_t<U != 1, bool> = true>
    T operator()() {
        auto t = T(chipIndex_, pipeIndex_, sliceIndex_, ebufIndex_, chanIndex_);
        increment();
        return t;
    };
    template <unsigned U=NumEbufs, std::enable_if_t<U == 1, bool> = true>
    T operator()() {
        auto t = T(chipIndex_, pipeIndex_, sliceIndex_, chanIndex_);
        increment();
        return t;
    };

private:
    const int chipIndex_;
    const int pipeIndex_;
    unsigned sliceIndex_ = 0;
    unsigned ebufIndex_ = 0;
    unsigned chanIndex_ = 0;

    EBufRegArray(int chipIndex, int pipeIndex) : chipIndex_(chipIndex), pipeIndex_(pipeIndex) {
        RMT_ASSERT(NumSlices && NumEbufs && NumChans && "EBufRegArray invalid template parameters");
    }

    void increment() {
        // Check that the indices are valid on this call.
        RMT_ASSERT(sliceIndex_ < NumSlices && "EBufRegArray operator() called too many times");
        // Increment for next time we are called.
        if (++chanIndex_ == NumChans) {
            chanIndex_ = 0;
            if (++ebufIndex_ == NumEbufs) {
                ebufIndex_ = 0;
                ++sliceIndex_;  // Check value on recall.
            }
        }
    }
};

}  // namespace model_common

#endif // MODEL_COMMON_EBUF_REG_ARRAY_
