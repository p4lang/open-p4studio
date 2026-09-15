// SPDX-FileCopyrightText: 2021 Intel Corporation
// Copyright (C) 2021 Intel Corporation.
//
// SPDX-License-Identifier: Apache-2.0
#ifndef _TDI_UTILS_HPP
#define _TDI_UTILS_HPP

#include <queue>
#include <mutex>
#include <future>
#include <thread>
#include <condition_variable>
#include <functional>
#include <cstring>

#include <target-sys/bf_sal/bf_sys_intf.h>
#include <tdi/common/tdi_table.hpp>

#define LOG_CRIT(...) LOG_COMMON(BF_LOG_CRIT, __VA_ARGS__)
#define LOG_ERROR(...) LOG_COMMON(BF_LOG_ERR, __VA_ARGS__)
#define LOG_WARN(...) LOG_COMMON(BF_LOG_WARN, __VA_ARGS__)
#define LOG_TRACE(...) LOG_COMMON(BF_LOG_INFO, __VA_ARGS__)
#define LOG_DBG(...) LOG_COMMON(BF_LOG_DBG, __VA_ARGS__)

#define LOG_COMMON(LOG_LEVEL, ...)                                \
  do {                                                            \
    if (bf_sys_log_is_log_enabled(BF_MOD_BFRT, LOG_LEVEL) == 1) { \
      bf_sys_log_and_trace(BF_MOD_BFRT, LOG_LEVEL, __VA_ARGS__);  \
    }                                                             \
  } while (0);

#define TDI_ASSERT bf_sys_assert
#define TDI_DBGCHK bf_sys_dbgchk

namespace tdi {

// This is a class to initialize a generalized thread pool with a specific
// number of worker threads given at time of creation of the thread pool.
// The thread pool uses a thread safe queue to manage the producer-consumer
// pipeline. This thread safe queue is just a wrapper on std::queue<T>.
// The queue is templatized over std::function<void()> and thus holds function
// objects. It is the job of the submitTask function to package the function F
// and the variadic arguments that are passed to it by the user into a
// std::function<void()> function object and enqueue it in the queue.
// Any producer can first create the thread pool and then use the submitTask
// function to submit tasks which will be enqueued in the queue. The worker
// thread/threads are waiting for any tasks to appear in the queue and pick
// them up as soon as the tasks are enqueued. During the destruction of the
// thread pool, the "shutdown_" flag is set which indicates to the worker
// threads that they need to stop processing the tasks in the queue and return.
// These worker threads are then subsequently joined.
// The implementation of this thread pool is inspired from the following.
// https://github.com/mtrebi/thread-pool
class TdiThreadPool {
 private:
  // Wrapper class over std::queue to make it thread safe
  template <typename T>
  class ThreadSafeQueue {
   public:
    ThreadSafeQueue(){};

    void enqueue(T &t) {
      std::lock_guard<std::mutex> lg(mtx_);
      queue_.push(t);
    }

    bool dequeue(T *t) {
      std::lock_guard<std::mutex> lg(mtx_);
      if (queue_.empty()) {
        return false;
      }
      *t = queue_.front();
      queue_.pop();
      return true;
    }

    bool empty() const {
      std::lock_guard<std::mutex> lg(mtx_);
      return queue_.empty();
    }

    size_t size() const {
      std::lock_guard<std::mutex> lg(mtx_);
      return queue_.size();
    }

   private:
    std::queue<T> queue_;
    // Lock to protect against concurrent accesses to std::queue
    mutable std::mutex mtx_;
  };  // ThreadSafeQueue

  // Functor which will be executed by each worker thread
  class WorkerThread {
   public:
    WorkerThread(const int &id, TdiThreadPool *thread_pool)
        : id_(id), thread_pool_(thread_pool) {}

    void operator()() {
      // continue processing tasks from the queue until the thread pool is
      // shutdown
      while (!thread_pool_->shutdown_) {
        bool is_dequeued = false;
        std::function<void()> fn;
        {
          std::unique_lock<std::mutex> lock(thread_pool_->mtx_);
          if (thread_pool_->queue_.empty()) {
            // Wait until there is work to be performed
            thread_pool_->cond_var_.wait(lock);
          }
        }
        // Get a task from the queue
        is_dequeued = thread_pool_->queue_.dequeue(&fn);
        if (is_dequeued) {
          // Perform the task
          fn();
        }
      }
    }

   private:
    const int id_{0};
    TdiThreadPool *thread_pool_{nullptr};
  };  // WorkerThread

  std::vector<std::thread> threads_;  // Vector to keep track of threads
  bool shutdown_{false};              // Flag to shutdown the pool
  ThreadSafeQueue<std::function<void()>> queue_;

  // We use condition variable so that the worker threads can be signalled
  // whenever there are tasks to be performed in the queue and thus they
  // don't need to spin on the queue waiting for tasks to show up
  std::mutex mtx_;
  std::condition_variable cond_var_;

 public:
  TdiThreadPool(const size_t &num_threads = 1)
      : threads_(std::vector<std::thread>(num_threads)) {
    for (size_t i = 0; i < threads_.size(); i++) {
      threads_[i] = std::thread(WorkerThread(i, this));
    }
  }
  ~TdiThreadPool() {
    // Stop processing any more tasks
    shutdown_ = true;
    // Wake up all threads so that break from their respective while loops
    // and return
    cond_var_.notify_all();
    for (size_t i = 0; i < threads_.size(); i++) {
      if (threads_[i].joinable()) {
        // Wait for the thread to finish
        threads_[i].join();
      }
    }
  }

  size_t getQueueSize() { return queue_.size(); }

  // This function is responsible for packaging the function 'F' and the
  // variadic arguments 'args' that are passed to it into a generic
  // function object 'std::function<void()>' and enqueue it in the queue.
  // These packaged function objects will subsquently be processed by the
  // worker threads
  template <typename F, typename... Args>
  auto submitTask(F &&f, Args &&... args) -> std::future<decltype(f(args...))> {
    // Construct a function object
    std::function<decltype(f(args...))()> fn_obj =
        std::bind(std::forward<F>(f), std::forward<Args>(args)...);
    // Make a packaged task. We need a task object so that we retrieve the
    // result once the worker thread has finished execution
    auto task_ptr =
        std::make_shared<std::packaged_task<decltype(f(args...))()>>(fn_obj);

    // Construct a generic void function
    std::function<void()> fn_wrapper = [task_ptr]() { (*task_ptr)(); };

    // Enqueue the generic void function
    queue_.enqueue(fn_wrapper);

    // Wake up any one thread waiting
    cond_var_.notify_one();

    // Return the future for the packaged_task shared state
    return task_ptr->get_future();
  }

  // Delete the copy constructor and the assignment operator
  TdiThreadPool(const TdiThreadPool &) = delete;
  TdiThreadPool(TdiThreadPool &&) = delete;
  TdiThreadPool &operator=(const TdiThreadPool &) = delete;
  TdiThreadPool &operator=(TdiThreadPool &&) = delete;
};  // TdiThreadPool

class TdiEndiannessHandler {
 public:
  TdiEndiannessHandler() = delete;

  static void toHostOrder(const size_t &size,
                          const uint8_t *value_ptr,
                          uint64_t *out_data) {
    uint64_t local_value = 0;
    uint8_t *dst = reinterpret_cast<uint8_t *>(&local_value);
    if (size > 8) {
      LOG_ERROR(
          "ERROR: %s:%d Trying to convert a network order byte stream of more "
          "than 8 bytes (%zu) into 64 bit data",
          __func__,
          __LINE__,
          size);
      TDI_DBGCHK(0);
      return;
    }
    local_value = 0;
    // Before memcpy, move the dst pointer offset deep enough
    dst += (8 - size);
    std::memcpy(dst, value_ptr, size);
    local_value = be64toh(local_value);

    *out_data = local_value;
  }

  static void toNetworkOrder(const size_t &size,
                             const uint64_t &in_data,
                             uint8_t *value_ptr) {
    if (size > 8) {
      LOG_ERROR(
          "ERROR: %s:%d Trying to convert a 64 bit data into a network order "
          "byte stream of more than 8 bytes (%zu)",
          __func__,
          __LINE__,
          size);
      TDI_DBGCHK(0);
      return;
    }
    auto local_val = in_data;
    local_val = htobe64(local_val);
    uint8_t *src = reinterpret_cast<uint8_t *>(&local_val);
    // Before memcpy, move the src pointer offset deep enough
    src += (8 - size);
    std::memcpy(value_ptr, src, size);
  }
};

namespace utils {

class TableFieldUtils {
 public:
  TableFieldUtils() = delete;

static inline tdi_status_t keyFieldSafeGet(
      const tdi_id_t &field_id,
      const tdi::KeyFieldInfo **key_field,
      const tdi::KeyFieldValue *field_value,
      const tdi::Table *table) {
    *key_field = table->tableInfoGet()->keyFieldGet(field_id);
    if (!key_field) {
      LOG_ERROR("%s:%d %s Unable to find key for key field_id %d, ",
                __func__,
                __LINE__,
                table->tableInfoGet()->nameGet().c_str(),
                field_id);
      return TDI_OBJECT_NOT_FOUND;
    }

    // If the key_field type is not what is required here, then just return
    // error
    if ((*key_field)->matchTypeGet() != field_value->matchTypeGet()) {
      LOG_ERROR("%s:%d %s Incorrect key type provided for key field_id %d, ",
                //        "expected type: %s received type: %s",
                __func__,
                __LINE__,
                table->tableInfoGet()->nameGet().c_str(),
                field_id
      );
      return TDI_INVALID_ARG;
    }
    if ((*key_field)->isPtrGet() && field_value->size_ < 64) {
      LOG_ERROR(
          "%s:%d Field size is greater than 64 bits. Please use byte arrays in "
          "the KeyFieldValue for field %d",
          __func__,
          __LINE__,
          field_id);
      return TDI_NOT_SUPPORTED;
    }
    return TDI_SUCCESS;
  }

  // To be used with DataFieldInfo and KeyFieldInfo classes.
  template <class T>
  static tdi_status_t boundsCheck(const tdi::Table &table,
                                  const T &field,
                                  const uint64_t &value,
                                  const uint8_t *value_ptr,
                                  const size_t &s) {
    size_t field_size = field.sizeGet();

    if (!value_ptr) {
      // if field_size == 64 - automatically will fit,
      // if > 64 fieldTypeCompatibilityCheck check will catch it
      if (field_size < 64) {
        auto limit = (1ULL << field_size) - 1;
        if (value > limit) {
	 LOG_ERROR("ERROR: %s:%d %s : Value of %lu"
                    " exceeds the size of the field with id %d ",
                    __func__,
                    __LINE__,
                    table.tableInfoGet()->nameGet().c_str(),
                    value,
                    field.idGet());
          return TDI_INVALID_ARG;
        }
      }
    } else {
      if (s != (field_size + 7) / 8) {
        LOG_ERROR(
            "ERROR: %s:%d %s: Array size of %zu bytes isn't ==  size of the "
            "field %zu with id %d ",
            __func__,
            __LINE__,
            table.tableInfoGet()->nameGet().c_str(),
            s,
            (field_size + 7) / 8,
            field.idGet());
        return TDI_INVALID_ARG;
      }
      int shift = field_size % 8;
      if (shift != 0 && (value_ptr[0] >> shift) > 0) {
        LOG_ERROR(
            "ERROR: %s:%d %s: Specified value is greater than what field size"
            " %zu bits allows for field id %d",
            __func__,
            __LINE__,
            table.tableInfoGet()->nameGet().c_str(),
            field_size,
            field.idGet());
        return TDI_INVALID_ARG;
      }
    }
    return TDI_SUCCESS;
  }

  // To be used with DataFieldInfo and KeyFieldInfo classes.
  template <class T>
  static tdi_status_t fieldTypeCompatibilityCheck(const tdi::Table &table,
                                                  const T &field,
                                                  const uint64_t * /*value*/,
                                                  const uint8_t *value_ptr,
                                                  const size_t &size) {
    size_t field_size = field.sizeGet();

    if (!value_ptr) {
      // The field size in this case must be less than 64
      if (field_size > 64) {
        LOG_ERROR(
            "ERROR: %s:%d %s: Field size %zu for field with id %d is greater "
            "than 64. Hence please use the pointer variant of the API",
            __func__,
            __LINE__,
            table.tableInfoGet()->nameGet().c_str(),
            field_size,
            field.idGet());
        return TDI_INVALID_ARG;
      }
    } else {
      // Now the field_size must be equal to the size passed in
      if (size != (field_size + 7) / 8) {
        LOG_ERROR(
            "ERROR: %s:%d %s: Array size of %zu bytes isn't == size of the "
            "field %zu with id %d",
            __func__,
            __LINE__,
            table.tableInfoGet()->nameGet().c_str(),
            size,
            (field_size + 7) / 8,
            field.idGet());
        return TDI_INVALID_ARG;
      }
    }
    return TDI_SUCCESS;
  }

  // To be used with DataFieldInfo and KeyFieldInfo classes.
  template <class T>
  static void toHostOrderData(const T &field,
                              const uint8_t *value_ptr,
                              uint64_t *out_data) {
    auto field_size = field.sizeGet();
    auto size = (field_size + 7) / 8;
    TdiEndiannessHandler::toHostOrder(size, value_ptr, out_data);
  }

  // To be used with DataFieldInfo and KeyFieldInfo classes.
  template <class T>
  static void toNetworkOrderData(const T &field,
                                 const uint64_t &in_data,
                                 uint8_t *value_ptr) {
    auto field_size = field.sizeGet();
    auto size = (field_size + 7) / 8;
    TdiEndiannessHandler::toNetworkOrder(size, in_data, value_ptr);
  }

};  // class TableFieldUtils

  //Explicit Instantiation of template functions for DataFieldInfo and KeyFieldInfo classes.
  template void TableFieldUtils::toNetworkOrderData(const tdi::DataFieldInfo &field,
		  const uint64_t &in_data,
		  uint8_t *out_data);
  template void TableFieldUtils::toNetworkOrderData(const tdi::KeyFieldInfo &field,
		  const uint64_t &in_data,
		  uint8_t *out_data);


  template void TableFieldUtils::toHostOrderData(const tdi::DataFieldInfo &field,
                              const uint8_t *value_ptr,
                              uint64_t *out_data);
  template void TableFieldUtils::toHostOrderData(const tdi::KeyFieldInfo &field,
                              const uint8_t *value_ptr,
                              uint64_t *out_data);

  template tdi_status_t TableFieldUtils::fieldTypeCompatibilityCheck(const tdi::Table &table,
                                                  const tdi::DataFieldInfo &field,
                                                  const uint64_t * /*value*/,
                                                  const uint8_t *value_ptr,
                                                  const size_t &size);
  template tdi_status_t TableFieldUtils::fieldTypeCompatibilityCheck(const tdi::Table &table,
                                                  const tdi::KeyFieldInfo &field,
                                                  const uint64_t * /*value*/,
                                                  const uint8_t *value_ptr,
                                                  const size_t &size);

  template tdi_status_t TableFieldUtils::boundsCheck(const tdi::Table &table,
                                  const tdi::DataFieldInfo &field,
                                  const uint64_t &value,
                                  const uint8_t *value_ptr,
                                  const size_t &s);
  template tdi_status_t TableFieldUtils::boundsCheck(const tdi::Table &table,
                                  const tdi::KeyFieldInfo &field,
                                  const uint64_t &value,
                                  const uint8_t *value_ptr,
                                  const size_t &s);
}  // namespace utils
}  // namespace tdi

#endif  // _TDI_UTILS_HPP
