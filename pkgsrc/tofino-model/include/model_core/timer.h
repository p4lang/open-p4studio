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

#ifndef _MODEL_CORE_MODEL_TIMER_H_
#define _MODEL_CORE_MODEL_TIMER_H_

#include <string>
#include <cstdint>
#include <mutex>
#include <functional>


namespace model_timer {

class TimerManager;
extern TimerManager *getTimerManager();

typedef std::function< void(uint64_t)> const TimerCallback;
template <typename CB_TYPE>
class CallBack {
  public:
    CallBack(CB_TYPE cb=0) :  cb_(cb) { }
    CB_TYPE cb_;
};


class Timer : public CallBack < TimerCallback > {
public:
    typedef enum {
      Once,
      Recurring,
    } timer_type_e;

    typedef enum {
      TIMER_STOPPED,
      TIMER_RUNNING,
      TIMER_INTERNAL, // copy managed my timer manager
    } timer_state_e;


    Timer (TimerCallback & cb=0, TimerManager *tm = 0);
    Timer (const Timer &rhs);
    
    ~Timer() { if (state_ ==  TIMER_RUNNING && tm_) { this->stop(); } return; }

    Timer& operator= (const Timer &rhs);

    void      run(uint64_t timeout = 1, timer_type_e type=Once);
    void      stop();
    uint64_t  id() {return id_; }
    uint64_t  timeout() {return timeout_;}
    bool      is_running() { return state_ == TIMER_RUNNING; }
    void      print();

private:
    timer_state_e state_;
    uint64_t      id_;
    uint64_t      timeout_;
    uint64_t      remaining_time_;
    timer_type_e  type_;
    TimerManager  *tm_;

    friend class TimerManager;
};

}
#endif //_MODEL_CORE_MODEL_TIMER_H_
