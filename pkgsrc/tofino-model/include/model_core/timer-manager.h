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

#ifndef _MODEL_CORE_TIMER_MANAGER_H_
#define _MODEL_CORE_TIMER_MANAGER_H_

#include <string>
#include <cstdint>
#include <mutex>
#include <list>
#include <model_core/timer.h>


namespace model_timer {

void ModelTimerIncrement(uint64_t units);
void ModelTimerSetTime (uint64_t new_time);
void ModelTimerGetTime (uint64_t& current_time);
void ModelTimerClear ();
uint64_t ModelTimerPicoSecToClocks(uint64_t pico);
uint64_t ModelTimerClocksToPicoSec(uint64_t clks);


class TimerManager {
  public:
    static constexpr uint64_t kClocksPerSec = 1250000000ull; // 1.25GHz

    TimerManager(uint32_t scale=1); // use larger# to increase(scale-up) timeout
    ~TimerManager();

    void       clear ();

    // called from timer expiry callback
    void          set_time (uint64_t t);
    void          get_time (uint64_t& t);
    void          advance_time (uint64_t units=1);

    // called from Timer class
    void          timer_add (Timer& timer);
    void          timer_stop (uint64_t id);
    uint64_t      allocate_id ();
    void          print();
    void          set_scale(uint32_t scale);

  private:
    void                update_timers (uint64_t& num_units);
    void                run_expired_timer_cb ();
    void                advance_time_internal (uint64_t units);

    // list sorted based on incremental remaining time
    std::list<Timer>    timer_list_;  //XXX optimize it later
    std::mutex          list_mutex_;
    uint32_t            scale_; // running less than 1ms timer may be too taxing for the model
    uint64_t            last_timeout_; // latest of all pending timers

    std::mutex          time_mutex_;  // for updating the up_time_
    uint64_t            up_time_;   // time since started

};

}
#endif //_MODEL_CORE_TIMER_MANAGER_H_
