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

#ifndef _SHARED_LEARNING_FILTER_
#define _SHARED_LEARNING_FILTER_

#include <string>
#include <cstdint>
#include <mutex>
#include <vector>
#include <model_core/timer-manager.h>
#include <rmt-defs.h>
#include <pipe-object.h>
#include <deparser-metadata.h>
#include <learning-filter-reg.h>

namespace MODEL_CHIP_NAMESPACE {

  class LearningFilter : public PipeObject {

 public:
    LearningFilter(RmtObjectManager *om, int pipeIndex);
    virtual ~LearningFilter();

    bool learn( const LearnQuantumType& lq);
    // CPU interface
    void clear();
    // drain (to be) called from timeout
    void drain(uint64_t tid);

    // All registers for the LearningFilter for config and control
    LfRegs    regs_;

    // internal stats getters
    uint64_t&  lq_count_dropped_buffer_full() { return lq_count_dropped_buffer_full_; }
    uint64_t&  lq_count_total() { return lq_count_total_; }
    uint64_t&  lq_count_new() { return lq_count_new_; }
    uint64_t&  lq_count_invalid() { return lq_count_invalid_; }

 private:
    class LfBloomFilter {
      // Bloom filter logic and associated LQ buffer is contained inside LearningFilter
      public:
        LfBloomFilter() {};
        virtual ~LfBloomFilter() {};

        // helper/accessor functions
        void    set_bit (int hash_id, uint16_t bit_idx) { bf_tables_[hash_id].set_bit(bit_idx); }
        bool    is_hit (int32_t hash_id, uint16_t bit_idx);
        void    lq_buffer_add (const LearnQuantumType& lq) { lq_buffer_.push_back(lq); }
        bool    lq_buffer_full() { return lq_buffer_.size() == RmtDefs::kLearnFilterLqBufferSize; }
        bool    is_empty () { return state_ == LF_BF_STATE_EMPTY; }
        bool    is_drain () { return state_ == LF_BF_STATE_DRAIN; }
        bool    is_fill () { return state_ == LF_BF_STATE_FILL; }
        bool    is_learning () { return state_ != LF_BF_STATE_CLEAR; }

        void    clear (void);
        void    start_fill (void);
        uint8_t *get_lq_buffer (int32_t & dma_bytes); // used during the drain
        int32_t lq_buf_get_size() {return lq_buffer_.size(); }

      private:
        typedef enum bf_state {
          LF_BF_STATE_CLEAR=0,
          LF_BF_STATE_EMPTY,
          LF_BF_STATE_FILL,
          LF_BF_STATE_DRAIN,
        } bf_state_e;
        bf_state_e                      state_{LF_BF_STATE_CLEAR};
        // each filter has 4x16k tables
        BitVector<RmtDefs::kLearnFilterHashTableSize>    bf_tables_[RmtDefs::kLearnFilterNumHash];
        // Buffers to hold the LQs to be sent to CPU
        std::vector<LearnQuantumType>   lq_buffer_;
    };

    int32_t         active_filter_id_;
    // Two bloom filters per LF block
    LfBloomFilter   bloomFilter_[RmtDefs::kLearnFilterNumBloomFilter];

    // utility functions
    uint16_t  lf_hash_compute (int id, const LearnQuantumType& lq);  // compute hash LQ (384 bits) => 14 bit
    bool      lq_buffer_full () { return  bloomFilter_[active_filter_id_].lq_buffer_full(); }
    bool      lq_is_seen (const LearnQuantumType& lq);
    void      lq_add (const LearnQuantumType& lq);
    bool      lq_push_timer_expired();
    bool      lq_buffer_available() {
                return bloomFilter_[active_filter_id_].is_fill() && !lq_buffer_full();
              }

    // stats - keep it public ? Find register for it
    uint64_t  lq_count_dropped_buffer_full_;
    uint64_t  lq_count_total_;               // total lqs seen
    uint64_t  lq_count_new_;                 // lqs accepted for learning
    uint64_t  lq_count_invalid_;             // XXX - not sure if this is needed or present in h/w

    // hash related bitmaps
    bool      lf_hash_initialized_;
    void      lf_hash_init();

    typedef struct    lf_gf_matrix {
      // 384 x 14 GF matrix
      std::array < BitVector<RmtDefs::kLearnQuantumWidth>, RmtDefs::kLearnFilterHashIndexBits > gf_bv_;
    } lf_gf_matrix_t;

    std::array<lf_gf_matrix_t, RmtDefs::kLearnFilterNumHash> lf_hash_;

    // internal drain_ function
    void drain_(int32_t filter_id);
    // mutex for cpu clear and pakcket processing threads as they update the bloomFilter bitmaps
    // and LQ FIFOs
    std::mutex              lf_mutex_;
    // only one timer per LF not needed per buffer
    model_timer::Timer      lq_push_timer_;
  };
}
#endif // _SHARED_LEARNING_FILTER_
