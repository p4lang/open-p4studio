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

#ifndef _BOUNDED_QUEUE
#define _BOUNDED_QUEUE

#include <queue>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <chrono>
#include <functional>

namespace model_common {
  template<typename T>
  class BoundedQueue {

    public:

      BoundedQueue(int cap = 0, int tmo_in = 0, int tmo_out = 0);
      bool enqueue(const T& var);
      bool dequeue(T& var);
      bool peek(std::function<void (T& var)>r_func);


      uint64_t size() const;
      uint64_t capacity() const;
      void set_peek(bool en_) { peek_ = en_; }
      ~BoundedQueue()=default;

    private:
      BoundedQueue(const BoundedQueue& other)=delete;
      BoundedQueue& operator=(const BoundedQueue& other)=delete;

      std::queue<T> q_;
      std::atomic<uint64_t> curr_cnt;
      uint64_t max_sz;
      std::condition_variable not_full;
      std::condition_variable not_empty;
      std::condition_variable snoop_cv;
      std::mutex q_lock;
      int tmo_in_;
      int tmo_out_;
      bool peek_;
      std::atomic<bool> peek_done_;
  };

  /* Definition of the methods must be included in the template header */

  template<typename T>
  BoundedQueue<T>::BoundedQueue(int cap, int tmo_in, int tmo_out):curr_cnt(0), max_sz(cap), tmo_in_(tmo_in), tmo_out_(tmo_out), peek_(false) {
  }

  template<typename T>
  bool BoundedQueue<T>::enqueue(const T& var) {

    std::unique_lock<std::mutex> l(q_lock);
    bool rval = false;
    if (tmo_in_) {
      rval = not_full.wait_for(l, std::chrono::microseconds(tmo_in_), [this]() { return ((max_sz == 0) || (curr_cnt.load() != max_sz)); });
    } else {
      not_full.wait(l, [this]() { return ((max_sz == 0) || (curr_cnt.load() != max_sz)); });
      rval = true;
    }
    if (rval) {
      q_.push(var);
      ++curr_cnt;
      not_empty.notify_all();
    }
    return rval;
  }

  template<typename T>
  bool BoundedQueue<T>::dequeue(T& ref) {

    std::unique_lock<std::mutex> l(q_lock);
    bool rval = false;

    if (tmo_out_) {
      rval = not_empty.wait_for(l, std::chrono::microseconds(tmo_out_), [this]() { return (curr_cnt.load() != 0); });
    } else {
      not_empty.wait(l, [this]() { return (curr_cnt.load() != 0); });
      rval = true;
    }
    if (rval ) {
      ref = q_.front();
      if (peek_) {
        peek_done_ = false;
        if (tmo_out_) {
          rval = snoop_cv.wait_for(l, std::chrono::microseconds(tmo_out_), [&]() { return peek_done_.load(); });
        } else {
          snoop_cv.wait(l, [&]() { return peek_done_.load(); });
          rval = true;
        }
      } else rval = true;
      if (rval) {
        q_.pop();
        --curr_cnt;
        not_full.notify_all();
      }
    }
    return rval;
  }

  template<typename T>
  bool BoundedQueue<T>::peek(std::function<void (T& ref)> r_func) {

    std::unique_lock<std::mutex> l(q_lock);
    bool rval = false;

    if (tmo_out_) {
      rval = not_empty.wait_for(l, std::chrono::microseconds(tmo_out_), [this]() { return (curr_cnt.load() != 0); });
    } else {
      not_empty.wait(l, [this]() { return (curr_cnt.load() != 0); });
      rval = true;
    }
    if (rval) {
      r_func(q_.front());
      peek_done_ = true;
    }
    return rval;
  }

  template<typename T>
  uint64_t BoundedQueue<T>::size(void) const {
    return curr_cnt.load();
  }

  template<typename T>
  uint64_t BoundedQueue<T>::capacity(void) const {
    return max_sz;
  }

};

#endif




