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

#ifndef _SHARED_PIPE_CSR_H_
#define _SHARED_PIPE_CSR_H_

#include <cassert>
#include <simple-csr.h>
#include <register_utils.h>


namespace MODEL_CHIP_NAMESPACE {
namespace register_classes {

class PipeCsr : public SimpleCsr {

  static constexpr uint64_t kPipeStride = static_cast<uint64_t>(BFN_REG_TOP(pipes_array_element_size));

 public:
  PipeCsr(int chipNumber, int pipeNumber, int pipe0_addr, const char *name,
          RegisterCallback& write_callback = 0, RegisterCallback& read_callback = 0)
      : SimpleCsr(chipNumber, StartOffset(pipe0_addr, pipeNumber), name, write_callback, read_callback) {
  }
  PipeCsr()
      : SimpleCsr() {
  }

 private:
  static int StartOffset(int pipe0_addr, int pipeNumber) {
    uint64_t off64 = UINT64_C(0);
    assert(pipe0_addr >= 0);
    off64 += pipe0_addr; // to get to pipe0
    assert((pipeNumber >= 0) && (pipeNumber < RegisterUtils::kPipesMax));
    off64 += pipeNumber * RegisterUtils::kPipeStride; // to get to pipeN
    assert(off64 <= UINT64_C(0x7FFFFFFF));
    return static_cast<int>(off64);
  }
};


} // namespace register_classes
} // namespace MODEL_CHIP_NAMESPACE

#endif // _SHARED_PIPE_CSR_H_
