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

#include <string>
#include <common/rmt-assert.h>
#include <rmt-log.h>
#include <rmt-object-manager.h>
#include <learning-filter.h>
#include <learning-filter-reg.h>
#include <model_core/model.h>

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;

namespace MODEL_CHIP_NAMESPACE {

LearningFilter::LearningFilter(RmtObjectManager *om, int pipeIndex)
      : PipeObject(om,pipeIndex), regs_(chip_index(), pipeIndex, this),
      lq_push_timer_([this](uint64_t tid){this->drain(tid);})
{
  RMT_LOG(RmtDebug::verbose(),"LearningFilter::create pipe_index:%d\n",pipe_index());
  for (int f=0; f < RmtDefs::kLearnFilterNumBloomFilter; f++) {
    bloomFilter_[f].clear();
  }
  active_filter_id_ = 0;
  bloomFilter_[active_filter_id_].start_fill();
  lq_count_dropped_buffer_full_ = 0;
  lq_count_total_ = 0;
  lq_count_new_ = 0;
  lq_count_invalid_ = 0;
  lf_hash_initialized_ = false;
}

LearningFilter::~LearningFilter() {
  RMT_LOG(RmtDebug::verbose(),"LearningFilter::delete pipe_index:%d\n",pipe_index());
}

/**
 * @param lq a LearnQuantumType
 * @return `true` if `lq` resulted in a new Learning Quantum, `false`
 *         otherwise.
 */
bool LearningFilter::learn( const LearnQuantumType& lq)
{
  bool result = false;
  lf_mutex_.lock(); // lock-out the CPU thread from messing with the filter
  if (regs_.common_ctrl_.learn_dis()) {
    RMT_LOG(RmtDebug::info(),"LearningFilter::learn:Disabled !!");
    goto unlock_return;
  }
  if (lq.valid && lq.length > 0) {
    lq_count_total_++; // XXX increment for invalid LQs too??
  } else {
    lq_count_invalid_++;
    RMT_LOG(RmtDebug::info(),"LearningFilter::learn:Invalid LQ %" PRIx64 "\n", lq_count_invalid_);
    goto unlock_return;
  }
  // check if there is some space in the active buffer
  if (!lq_buffer_available()) {
    // update stats
    lq_count_dropped_buffer_full_++;
    // XXX change to register.increment() when implemented in csrtocpp.pl
    regs_.lq_dropped_.ctr48(lq_count_dropped_buffer_full_);
    RMT_P4_LOG_INFO("LearningFilter::learn: Dropped LQ #%" PRIx64 " Total LQs %" PRIx64 "\n",
                       lq_count_dropped_buffer_full_, lq_count_total_);
    goto unlock_return;
  }

  if (lq_is_seen(lq)) {
    RMT_P4_LOG_INFO("LearningFilter::learn:LQ is seen %" PRIx64 "\n", lq_count_total_);
    RMT_LOG(RmtDebug::verbose(),"LearningFilter::learn:LQ is seen %" PRIx64 "\n", lq_count_total_);
    goto unlock_return;
  }

  // new LQ : Add it to the buffer associated with the active filter
  lq_add(lq);
  result = true;

  RMT_LOG(RmtDebug::verbose(),
          "LearningFilter::learn: filter_id %d New LQ #%" PRIx64 ", Total LQs %" PRIx64 "\n",
          active_filter_id_, lq_count_new_, lq_count_total_);
  RMT_P4_LOG_INFO("LearningFilter::learn: filter_id %d New LQ #%" PRIx64 " Total LQs %" PRIx64 "\n",
                  active_filter_id_, lq_count_new_, lq_count_total_);

  // If buffer is full, send to CPU (DRAIN)
  // If the other buffer is not in use, set it to active
  // XXX timer based drain can be simulated by using user programmed timer val
  // as packet count
  if (lq_buffer_full() || lq_push_timer_expired()) {
    // XXX - can we drain second buffer if the first one is not yet cleared by the CPU?
    // If not then we can schedule the drain for second buffer on clear of the 1st one
    // hardware will send it to DMA engine
    drain_ (active_filter_id_);
    // stop the timer
    lq_push_timer_.stop();

    for (int f=0; f < RmtDefs::kLearnFilterNumBloomFilter; f++) {
      if (bloomFilter_[f].is_empty()) {
        active_filter_id_ = f;
        bloomFilter_[f].start_fill();
        break;
      }
    }
  }
unlock_return:
  lf_mutex_.unlock();
  return result;
}

bool LearningFilter::lq_push_timer_expired ()
{
  uint32_t timer_threshold = regs_.lqt_timeout_.timeout();
  RMT_LOG(RmtDebug::verbose(),"LearningFilter::lq_push_timer_expired threshold = %d", timer_threshold);
  if (!timer_threshold ||
      !(bloomFilter_[active_filter_id_].lq_buf_get_size() % timer_threshold))
  {
      return true;
  }
  return false;
}

void LearningFilter::clear ()
{
    if (regs_.bft_ctrl_.clear())
    { // if bit is set, clear the 'other' filter, reset the bit
      // lockout the packet processing thread when CPU is clearing the filter
      lf_mutex_.lock();

      int fid = 1-active_filter_id_;
      if (bloomFilter_[fid].is_drain()) {
        RMT_LOG(RmtDebug::info(),"LearningFilter::clear: filter_id %d", fid);
        RMT_P4_LOG_INFO("LearningFilter::clear: filter_id %d", fid);
        bloomFilter_[fid].clear();
        // if current filter is not in fill state
        // switch to the one that was just cleared
        if (! bloomFilter_[active_filter_id_].is_fill()) {
          active_filter_id_ = fid;
          bloomFilter_[active_filter_id_].start_fill();
        }
      } else {
        RMT_LOG(RmtDebug::info(),"LearningFilter::clear : Excessive clearing !!! Nothing to clear");
        RMT_P4_LOG_INFO("LearningFilter::clear : Excessive clearing of filter %d !!! Nothing to clear", fid);
      }
      lf_mutex_.unlock();
    }
    return;
}

// internal drain_ interface.
void LearningFilter::drain_ (int32_t filter_id)
{
    uint8_t *learn_filter_data;
    int32_t dma_bytes = 0;
    learn_filter_data = bloomFilter_[filter_id].get_lq_buffer(dma_bytes);
    if (learn_filter_data) {
      // send data to DMA engine
      RMT_LOG(RmtDebug::info(),"LearningFilter::drain: filter_id %d\n",
              active_filter_id_);
      RMT_P4_LOG_INFO("LearningFilter::drain: filter_id %d\n", active_filter_id_);
      GLOBAL_MODEL->dru_learn_callback(chip_index(),
                                                learn_filter_data,
                                                dma_bytes,
                                                pipe_index());
      // XXX since the buffer can be upto 96KB,
      // Talk to driver team to see if we can avoid extra copy and malloc/free,
      // let the dru_learn_callback free the buffer
      // OR use pre-allocated buffers (still copy overhead)
      free (learn_filter_data);
    }
}
// external drain interface (to be used from timer)
void LearningFilter::drain (uint64_t tid)
{
  // drain active filter (even if it is not full)
  // switch the active_buffer if other filter is avaialble
  lf_mutex_.lock();
  drain_(active_filter_id_);
  // stop the timer
  RMT_ASSERT(lq_push_timer_.is_running());
  lq_push_timer_.stop();

  if (bloomFilter_[1-active_filter_id_].is_empty()) {
    active_filter_id_ = 1-active_filter_id_;
    bloomFilter_[active_filter_id_].start_fill();
  }
  lf_mutex_.unlock();
}

void LearningFilter::lq_add (const LearnQuantumType& lq)
{
  RMT_ASSERT(!lq_buffer_full());
  bloomFilter_[active_filter_id_].lq_buffer_add(lq);
  if (bloomFilter_[active_filter_id_].lq_buf_get_size() == 1) {
    // added 1st LQ, start the timer
    // push timer is currently set to the order of 500us.. which is
    // smaller than the timer granularity (1ms).
    // Since test infra is slow, slow down the push timer by 1000x
    uint64_t timer_threshold = regs_.lqt_timeout_.timeout(); // make sure it is not 0
    lq_push_timer_.run(timer_threshold * 1000, model_timer::Timer::Once);
  }
  lq_count_new_++;
}

uint16_t LearningFilter::lf_hash_compute (int32_t hash_id, const LearnQuantumType& lq)
{
  // convert registers to bit vectors for ease of use (done only first time)
  // XXX Can be done based on wr-callback on enable register and make a rule that
  // this register must be written after config ??? - TBD
  // lf_hash[0-4].gf_bv[0-13]

  if (!lf_hash_initialized_) {
    lf_hash_init();
  }

  uint16_t hash_idx = 0;

  for (int b=0; b < RmtDefs::kLearnFilterHashIndexBits; b++)
  {
      bool p = false;
      p = lq.data.masked_parity<0, RmtDefs::kLearnQuantumWidth>(lf_hash_[hash_id].gf_bv_[b]);
      hash_idx |= (p << b);
  }
  uint16_t seed = regs_.hash_seed_.seed(hash_id);
  hash_idx ^= seed;
  RMT_LOG(RmtDebug::verbose(),"LearningFilter::lf_hash_compute: hash_id %d, idx %d\n", hash_id, hash_idx);

  return hash_idx;
}

bool LearningFilter::lq_is_seen (const LearnQuantumType& lq)
{
  bool     hit = false;
  uint16_t idx[RmtDefs::kLearnFilterNumHash];

  // compute all 4 hash indices
  for (int i=0; i < RmtDefs::kLearnFilterNumHash; i++) {
    idx[i] = lf_hash_compute(i, lq);
  }
  // If any filter has hit for all 4 hashes, then LQ is seen
  for (int f=0; f < RmtDefs::kLearnFilterNumBloomFilter; f++) {
    bool filter_hit = true;
    // hit from any one bloom-filter = all 4 hashes are a hit
    for (int i=0; i < RmtDefs::kLearnFilterNumHash; i++) {
      if (! bloomFilter_[f].is_hit(i, idx[i])) {
        filter_hit = false;
      }
    }
    hit = filter_hit || hit;  // hit = hit0 OR hit1
  }

  if (!hit) {
    // Set the 4 bits on the filter that is in FILL state for future
    for (int f = 0; f < RmtDefs::kLearnFilterNumBloomFilter; f++) {
      for (int i = 0; i < RmtDefs::kLearnFilterNumHash; i++) {
        if (bloomFilter_[f].is_fill()) {
          bloomFilter_[f].set_bit(i, idx[i]);
        }
      }
    }
  }
  return hit;
}

void LearningFilter::lf_hash_init ()
{
  RMT_LOG(RmtDebug::info(),"LearningFilter::lf_hash_init");
  for (int h=0; h<RmtDefs::RmtDefs::kLearnFilterNumHash; h++) {
    for (int g=0; g<RmtDefs::kLearnFilterHashIndexBits; g++) {
      register_classes::LfltrHashArray& hash = regs_.hash_.hash_array(h, g);
      for (int b=0; b<RmtDefs::kLearnQuantumWidth; b++) {
        uint8_t& sel = hash.sel(b);
        if (sel) {
          lf_hash_[h].gf_bv_[g].set_bit(b);
        } else {
          lf_hash_[h].gf_bv_[g].clear_bit(b);
        }
      }
    }
  }
  lf_hash_initialized_ = true;
}

bool LearningFilter::LfBloomFilter::is_hit (int32_t hash_id, uint16_t bit_idx)
{
  if (bf_tables_[hash_id].get_bit(bit_idx)) {
    return true;
  }
  return false;
};

void LearningFilter::LfBloomFilter::start_fill ()
{
  RMT_ASSERT(state_ == LF_BF_STATE_EMPTY);
  state_ = LF_BF_STATE_FILL;
}

uint8_t * LearningFilter::LfBloomFilter::get_lq_buffer (int32_t & dma_bytes)
{
  // copy out all the LQs to a buffer
  // use some of the existing code
  // TODO: should this use TOFINO_MALLOC? It didn't work
  int lq_count = lq_buffer_.size();
  dma_bytes = lq_count * ((RmtDefs::kLearnQuantumWidth+7)/8);
  uint8_t *learn_filter_data = NULL;
  if (lq_count) {
    learn_filter_data = static_cast<uint8_t*>( malloc( dma_bytes ) );
    uint8_t *data_p = learn_filter_data;
    if (learn_filter_data) {
      for (int l=0; l < lq_count; l++) {
        for (int i=0;i<48;++i) {
          // Fix in Deparser (match h/w) to copy the bytes from 47-0 where
          // LQbyte0 is byte47 of the bitvector.
          // It also fills unused bytes to 0, no need to do it here.
          data_p[i] = lq_buffer_[l].data.get_byte(i);
        }
        data_p += ((RmtDefs::kLearnQuantumWidth+7)/8);
      }
    }
    state_ = LF_BF_STATE_DRAIN;
  } else {
    // CPU is not notified when buffer is empty
    // directly go to empty state
    state_ = LF_BF_STATE_EMPTY;
  }
  return learn_filter_data;
}

void LearningFilter::LfBloomFilter::clear ()
{
  // clear the bits and the lq_buffer and set state to EMPTY
  for (int i=0; i < RmtDefs::kLearnFilterNumHash; i++) {
      bf_tables_[i].fill_all_zeros();
  }
  lq_buffer_.clear();
  state_ = LF_BF_STATE_EMPTY;
}

}
