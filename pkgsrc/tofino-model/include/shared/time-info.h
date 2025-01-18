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

#ifndef _SHARED_TIME_INFO_
#define _SHARED_TIME_INFO_

#include <array>
#include <string>
#include <cstdint>
#include <common/rmt-assert.h>
#include <random>
#include <limits>
#include <rmt-defs.h>
#include <mau-defs.h>

namespace MODEL_CHIP_NAMESPACE {

template <class TICK_TYPE,class RANDOM_TYPE>
class TimeInfo {
 public:
  TimeInfo() : relative_time_(0), relative_time_valid_(false) {}
  ~TimeInfo() {}

 private:
  static constexpr int  kMauMax = RmtDefs::kStagesMax;
  static constexpr int  kNumMeterAlus = MauDefs::kNumMeterAlus;
  static constexpr int  kMauApproxLatency = 20;

 public:
  // use set_mau_tick_time and set_mau_random_value to set
  //  specific values to match the RTL simulation
  void set_meter_tick_time(int mau_index,int meter_index, TICK_TYPE tick_time) {
    RMT_ASSERT( mau_index>=0 && mau_index<kMauMax );
    RMT_ASSERT( meter_index>=0 && meter_index<kNumMeterAlus );
    mau_tick_time_[mau_index][meter_index]=tick_time;
  }
  void set_meter_random_value(int mau_index,int meter_index, RANDOM_TYPE random_value) {
    RMT_ASSERT( mau_index>=0 && mau_index<kMauMax );
    RMT_ASSERT( meter_index>=0 && meter_index<kNumMeterAlus );
    mau_random_value_[mau_index][meter_index]=random_value;
  }
  // Likewise for MAU immediate_data RNG
  void set_immediate_data_random_value(int mau_index, RANDOM_TYPE random_value) {
    RMT_ASSERT( mau_index>=0 && mau_index<kMauMax );
    immediate_data_random_value_[mau_index]=random_value;
  }
  // use this to initialise to plausible values, first_tick_time is
  //  used as the tick time for the first mau and the seed for the
  //  random numbers (it doesn't bother to do the random accurately)
  void set_all(TICK_TYPE first_tick_time) {
    relative_time_ = first_tick_time;
    relative_time_valid_ = true;
    std::default_random_engine generator (first_tick_time);
    std::uniform_int_distribution<RANDOM_TYPE> distribution(std::numeric_limits<RANDOM_TYPE>::min(),
                                                            std::numeric_limits<RANDOM_TYPE>::max());
    for (int i=0;i<kMauMax;++i) {
      for (int j=0;j<kNumMeterAlus;++j) {
        mau_tick_time_[i][j]  = first_tick_time;
        mau_random_value_[i][j] = distribution(generator);
      }
      immediate_data_random_value_[i] = distribution(generator);
      first_tick_time += kMauApproxLatency;
    }
  }

  void set_from(const TimeInfo &other) {
    for (int i=0;i<kMauMax;++i) {
      for (int j=0;j<kNumMeterAlus;++j) {
        mau_tick_time_[i][j]  = other.mau_tick_time_[i][j];
        mau_random_value_[i][j] = other.mau_random_value_[i][j];
      }
      immediate_data_random_value_[i] = other.immediate_data_random_value_[i];
    }
    relative_time_ = other.relative_time_;
    relative_time_valid_ = other.relative_time_valid_;
  }

  void time_incr(TICK_TYPE delta, bool just_relative_time=false) {
    relative_time_valid_ = true;
    relative_time_ += delta;
    if (just_relative_time) return;
    for (int i=0;i<kMauMax;++i) {
      for (int j=0;j<kNumMeterAlus;++j) mau_tick_time_[i][j] += delta;
    }
  }
  bool is_before(TimeInfo &other) const {
    // Return true if ANY time val in this < other
    if ((relative_time_valid_) && (other.relative_time_valid_) &&
        (relative_time_ < other.relative_time_)) return true;
    for (int i=0;i<kMauMax;++i) {
      for (int j=0;j<kNumMeterAlus;++j) {
        if (mau_tick_time_[i][j] < other.mau_tick_time_[i][j]) return true;
      }
    }
    return false;
  }

  TICK_TYPE get_meter_tick_time(int mau_index,int meter_index) {
    RMT_ASSERT( mau_index>=0 && mau_index<kMauMax );
    return mau_tick_time_[mau_index][meter_index];
  }
  RANDOM_TYPE get_meter_random_value(int mau_index,int meter_index) {
    RMT_ASSERT( mau_index>=0 && mau_index<kMauMax );
    return mau_random_value_[mau_index][meter_index];
  }
  RANDOM_TYPE get_immediate_data_random_value(int mau_index) {
    RMT_ASSERT( mau_index>=0 && mau_index<kMauMax );
    return immediate_data_random_value_[mau_index];
  }

  // used for pipeline delays, absolute time doesn't matter, just relative between
  //  phv and eop etc.
  void set_relative_time(TICK_TYPE relative_time) {
    relative_time_ = relative_time;
    relative_time_valid_ = true;
  }
  bool relative_time_valid()    const { return relative_time_valid_; }
  TICK_TYPE get_relative_time() const { return relative_time_; }

 private:
  std::array<std::array<TICK_TYPE, kNumMeterAlus>, kMauMax>   mau_tick_time_{};
  std::array<std::array<RANDOM_TYPE, kNumMeterAlus>, kMauMax> mau_random_value_{};
  std::array<RANDOM_TYPE, kMauMax>                 immediate_data_random_value_{};

  TICK_TYPE relative_time_;
  bool relative_time_valid_;

};

using TimeInfoType = TimeInfo<uint64_t,uint64_t>;

}

#endif // _SHARED_TIME_INFO_

