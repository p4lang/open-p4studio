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
#include <cstdint>
#include <cinttypes>
#include <common/rmt-assert.h>
#include <model_core/timer.h>
#include <model_core/timer-manager.h>


using namespace model_timer;

model_timer::TimerManager *TimerManagerP(nullptr);

model_timer::TimerManager *model_timer::getTimerManager()
{
  if (TimerManagerP == nullptr) {
    TimerManagerP = new TimerManager();
  }
  return TimerManagerP;
}

// global call back function called my the external timer thread, each time timer expires.
namespace model_timer {

void ModelTimerIncrement(uint64_t units)
{
    // decrement the remaining time from the timer element at the head
    // remove it from the list and run callback if remainning time is 0
    if (! getTimerManager()) { return; }
    getTimerManager()->advance_time(units);
}

uint64_t ModelTimerPicoSecToClocks(uint64_t pico)
{
    uint64_t pico_per_clk = 1000000000000/TimerManager::kClocksPerSec;
    return (pico + pico_per_clk - 1) / pico_per_clk; // round up
}

uint64_t ModelTimerClocksToPicoSec(uint64_t clks)
{
    uint64_t pico_per_clk = 1000000000000/TimerManager::kClocksPerSec;
    return clks * pico_per_clk;
}

void ModelTimerSetTime (uint64_t new_time)
{
  if (! getTimerManager()) { return; }
  getTimerManager()->set_time(new_time);
  return;
}
void ModelTimerGetTime (uint64_t& current_time)
{
  if (! getTimerManager()) { return; }
  getTimerManager()->get_time(current_time);
  return;
}
void ModelTimerClear ()
{
  if (! getTimerManager()) { return; }
  getTimerManager()->clear();
  return;
}
////
} //namespace

TimerManager::TimerManager (uint32_t scale) {
  scale_ = scale;  // runnnig less than 1ms timer may be too taxing for the model
  last_timeout_ =0;
  up_time_ = 0;
}

TimerManager::~TimerManager () {
  clear();
  if (TimerManagerP == this) {
    TimerManagerP = nullptr;
  }
}

void TimerManager::clear () {
  std::lock_guard<std::mutex> lock(list_mutex_);
  timer_list_.clear();
  last_timeout_ =0;
  up_time_ = 0;
}

void TimerManager::advance_time (uint64_t units) {
  std::lock_guard<std::mutex> lock(time_mutex_);
  advance_time_internal(units);
}

void TimerManager::advance_time_internal (uint64_t units)
{
  // time lock held by caller
  while (units) {
    uint64_t decrement = units;
    update_timers(decrement);
    up_time_ += decrement;
    units    -= decrement;
    run_expired_timer_cb();
  }
}

void
TimerManager::set_time (uint64_t new_time)
{
  std::lock_guard<std::mutex> lock(time_mutex_);
  if (new_time > up_time_) {
    uint64_t delta = new_time - up_time_;
    advance_time_internal(delta);
    up_time_ = new_time;
  } else {
    // rewind the time
    uint64_t delta = up_time_ - new_time;
    // just add it to the first timer and total
    list_mutex_.lock();
    std::list<Timer>::iterator    t = timer_list_.begin();
    if (t != timer_list_.end()) {
        t->remaining_time_ += delta;
        last_timeout_ += delta;
    }
    list_mutex_.unlock();
    up_time_ = new_time;
  }
}

void
TimerManager::get_time (uint64_t& time)
{
  // problem when called from timer callback
  // no time_mutex taken
  //std::lock_guard<std::mutex> lock(time_mutex_);
  time = up_time_;
}

void
TimerManager::run_expired_timer_cb ()
{
  while (1) {
    list_mutex_.lock();
    std::list<Timer>::iterator    t = timer_list_.begin();
    if (t == timer_list_.end()) {
      list_mutex_.unlock();
      break;
    }
    if (t->remaining_time_ == 0) {
      t->state_ = Timer::TIMER_STOPPED;
      Timer texp(*t);
      timer_list_.pop_front();
      list_mutex_.unlock();

      // add recurring timer back to the queue so that
      // WIP function has a chance to cancel it
      if (texp.type_ == Timer::Recurring) {
        // add it back
        timer_add(texp);
        // print();
      }
      // callback with no locks held
      if (texp.cb_) {
        //printf("Running expired timer callback %" PRId64 "\n", up_time_ );
        //print();
        //texp.print();
        texp.cb_(texp.id());
      }
    } else {
      list_mutex_.unlock();
      break;
    }
  }
  return;
}

void
TimerManager::update_timers (uint64_t& decrement)
{
  // if there are any timers, decrement the first timer's remaing time
  // and last_timeout_ value
  list_mutex_.lock();
  std::list<Timer>::iterator    t = timer_list_.begin();
  if (t != timer_list_.end()) {
    if (t->remaining_time_ < decrement) {
      decrement = t->remaining_time_;
    }
    t->remaining_time_ -= decrement;
    last_timeout_ -= decrement;
  }
  list_mutex_.unlock();
  return;
}

void
TimerManager::timer_stop (uint64_t id)
{
    uint64_t remove_time = 0;
    std::list<Timer>::iterator    t;
    std::list<Timer>::iterator    tmp;
    list_mutex_.lock();    
    std::list<Timer>::iterator    next = timer_list_.end();
    for (t=timer_list_.begin(); t!=timer_list_.end(); t++) {
      if (t->id_ == id) {
        remove_time = t->remaining_time_;
        t->state_ = Timer::TIMER_STOPPED; // to avoid ~Timer to call stop again
        tmp = t;
        next = ++tmp;
        timer_list_.erase(t);
        break;
      }
    }
    if (remove_time == 0) {
      // timer not found
      list_mutex_.unlock();    
      return;
    }
    if (next != timer_list_.end()) {
      // there is a timer after this
      // add the delta to it
      next->remaining_time_ += remove_time;
    } else {
      // this was the last timer in the list
      last_timeout_ -= remove_time;
    }
    list_mutex_.unlock();    
    return;
}

void
TimerManager::timer_add (Timer& timer)
{
    uint64_t new_timeout = timer.timeout_ * scale_;
    Timer::timer_state_e state = timer.state_;
    timer.state_ = Timer::TIMER_INTERNAL;

    //printf("timer_add %" PRId64 " list before:\n", new_timeout );
    //print();

    list_mutex_.lock();    
    std::list<Timer>::iterator    t = timer_list_.begin();
    timer.remaining_time_ = new_timeout;
    if (timer.remaining_time_ >= last_timeout_) {
      // avoid going thru the list if this is larger than any pending timer
      timer.remaining_time_ -= last_timeout_;
      last_timeout_ += timer.remaining_time_;
      timer_list_.insert(timer_list_.end(), timer);
    } else {
      while (t != timer_list_.end()) {
        if (t->remaining_time_ <= timer.remaining_time_) {
          timer.remaining_time_ -= t->remaining_time_;
        } else {
          // insert it here, update the next timers remaining time
          t->remaining_time_ -= timer.remaining_time_;
          timer_list_.insert(t, timer);
          break;
        }
        t++;
      }
      // must insert somewhere in the middle
      // Inserting at the end is handled before this
      // RMT_ASSERT (t != timer_list_.end());  
    }
    list_mutex_.unlock();    
    timer.state_ = state;
    //printf("timer_add list after:\n" );
    //print();
};

void
TimerManager::set_scale(uint32_t scale) {
  std::list<Timer>::iterator    t = timer_list_.begin();
  while (t != timer_list_.end()) {
    t->remaining_time_ = (t->remaining_time_ / scale_) * scale;
    t++;
  }
  scale_ = scale;
}

void
TimerManager::print() {
  std::list<Timer>::iterator    t = timer_list_.begin();

  printf("TimerManager time=%" PRId64 " last_timeout_=%" PRId64 " \n",up_time_,last_timeout_);
  while (t != timer_list_.end()) {
    t->print();
    t++;
  }
}
static uint64_t timer_id = 0;
uint64_t
TimerManager::allocate_id()
{
  return ++timer_id;
}
