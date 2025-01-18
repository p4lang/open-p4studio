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

#include <ostream>
#include <iostream>
#include <string>
#include <common/rmt-assert.h>
#include <model_core/timer.h>
#include <model_core/timer-manager.h>

using namespace std;
using namespace model_timer;

Timer::Timer (TimerCallback & cb, TimerManager *tm) :
    CallBack(cb)
{
    if (!tm) {
      tm_ = getTimerManager();
    } else {
      tm_ = tm;
    }
    id_ = tm_->allocate_id();
    timeout_ = 0;
    remaining_time_ = 0;
    type_ = Once;
    state_ = TIMER_STOPPED;
}

Timer::Timer (const Timer& rhs) : CallBack(rhs.cb_)
{
    state_ = rhs.state_;
    id_ = rhs.id_;
    timeout_ = rhs.timeout_;
    remaining_time_ = rhs.remaining_time_;
    type_ = rhs.type_;
    tm_ = rhs.tm_;
}

Timer& Timer::operator=(const Timer& rhs)
{
    state_ = rhs.state_;
    id_ = rhs.id_;
    timeout_ = rhs.timeout_;
    remaining_time_ = rhs.remaining_time_;
    type_ = rhs.type_;
    tm_ = rhs.tm_;

    return *this;

}
void
Timer::run(uint64_t time, timer_type_e type)
{
  if (!tm_) {
    return;  // not initialized
  }
  if (state_ == TIMER_RUNNING || time == 0) {
    // already running.. cannot change
    // zero time is not allowed as it can hog everyting if recursive
    return;
  }
  timeout_ = time;
  type_ = type;
  state_ = TIMER_RUNNING;
  tm_->timer_add(*this);
}

void
Timer::stop()
{
  if (state_ == TIMER_RUNNING) {
    tm_->timer_stop(id_);
  }
  state_ = TIMER_STOPPED;
}

void
Timer::print()
{
  std::cout << "Timer id: " << id_ << " val: " << timeout_ << " remaining: "<< remaining_time_ << endl;
}
