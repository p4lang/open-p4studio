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

#ifndef _SHARED_MAU_ATOMIC_WRITES_
#define _SHARED_MAU_ATOMIC_WRITES_

#include <atomic>
#include <rmt-defs.h>
#include <mau-defs.h>
#include <register_utils.h>
#include <indirect-addressing.h>
#include <model_core/register_block.h>
#include <rmt-log.h>


namespace MODEL_CHIP_NAMESPACE {


  struct MauCsrWrite {
    uint32_t offset_, data_;
  };


  class MauAtomicWrites : public model_core::RegisterBlock<RegisterCallback> {

    static uint32_t mau_offset(int p, int s) {
      return RegisterUtils::dpi_addr_mau_first(p, s);
    }
    static uint32_t mau_size(int p, int s)   { return 1 +
          RegisterUtils::dpi_addr_mau_last(p, s) -
          RegisterUtils::dpi_addr_mau_first(p, s);
    }

 public:
    static constexpr int kNumAtomicCsrWrites = 64;

    MauAtomicWrites(int chipIndex, int pipeIndex, int stageIndex,
                    RegisterCallback& write_callback = 0, RegisterCallback& read_callback = 0)
        : RegisterBlock(chipIndex, mau_offset(pipeIndex, stageIndex),
                        mau_size(pipeIndex, stageIndex), false,
                        write_callback, read_callback, "MauAtomicWrites"),
        base_addr_(mau_offset(pipeIndex, stageIndex)),
        buffer_writes_(false), n_writes_(0), n_csr_writes_(0), csr_writes_() {
    }
    virtual ~MauAtomicWrites() {
    }

    int  n_writes()       const { return n_writes_; }
    int  n_csr_writes()   const { return n_csr_writes_; }
    bool full()           const { return (n_csr_writes_ == kNumAtomicCsrWrites); }
    bool overflowed()     const { return (n_writes_ > n_csr_writes_); }
    void buffer_writes(bool tf) { buffer_writes_ = tf; }

    bool get_write(int i, uint32_t *addr, uint32_t *data) {
      if (i < 0) i += n_csr_writes_; // Allow access to last
      if ((i < 0) || (i >= n_csr_writes_)) return false;
      *addr = base_addr_ + csr_writes_[i].offset_;
      *data = csr_writes_[i].data_;
      return true;
    }
    void reset() {
      buffer_writes_ = false;
      n_writes_ = 0;
      n_csr_writes_ = 0;
    }

    bool read(uint32_t offset, uint32_t* data) const {
      // Always return false so InWord tries again
      if (read_callback_) read_callback_();
      return false;
    }
    bool write(uint32_t offset, uint32_t data) {
      // Bail if we're not stashing CSR writes so OutWord tries again
      if (!buffer_writes_) return true;
      n_writes_++;
      if (n_csr_writes_ < kNumAtomicCsrWrites) {
        MauCsrWrite w;
        w.offset_ = offset; w.data_ = data;
        csr_writes_[n_csr_writes_] = w;
        n_csr_writes_++;
      }
      if (write_callback_) write_callback_();
      return false;
    }
    std::string to_string(bool print_zeros = false, std::string indent_string = "") const {
      return "";
    }
    std::string to_string(uint32_t offset, bool print_zeros = false, std::string indent_string = "") const {
      return "";
    }


 private:
    uint32_t                                        base_addr_;
    bool                                            buffer_writes_;
    int                                             n_writes_;
    int                                             n_csr_writes_;
    std::array< MauCsrWrite, kNumAtomicCsrWrites >  csr_writes_;


  };

}
#endif // _SHARED_MAU_ATOMIC_WRITES_
