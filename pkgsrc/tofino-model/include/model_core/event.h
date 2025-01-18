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

#ifndef _MODEL_CORE_EVENT_
#define _MODEL_CORE_EVENT_

#include <cinttypes>
#include <rapidjson/writer.h>
#include <chrono>
#include <string>

namespace model_core {

/* Enum for JSON messages */
enum class Severity {
  kUnknown,
  kDebug,
  kInfo,
  kWarn,
  kError
};

/* Utility to convert Severity enum to JSON enum names */
const char* severityToString(const Severity severity);

/* Utility to convert ChipType to JSON enum names */
const char* chipTypeToString(const uint8_t type);

/* Get the current time in iso8601 format */
std::string iso8601(std::chrono::system_clock::time_point time_pt,
                    bool use_local_time);

/* Base class for Event JSON objects */
template<typename W>
class Event {
 public:
  virtual ~Event() {}
  virtual void Serialize(W& writer) const;
  virtual std::string real_time() const;  // required for testing
 private:
  // Prohibit copy & assignment ctors
  Event(const Event&) = delete;
  Event& operator=(const Event&) = delete;
  std::chrono::system_clock::time_point real_time_;
  uint64_t sim_time_;
  bool use_local_time_;
 protected:
  Event(const uint64_t sim_time,
        std::chrono::system_clock::time_point real_time,
        bool use_local_time);
  Event(const uint64_t sim_time);
  Event(Event&& other) = default;
  Event& operator=(Event&& other) = default;

};

template<typename W>
class EventMessage : public Event<W> {
 public:
  EventMessage(const uint64_t time, const Severity severity, const std::string& message);
  EventMessage(EventMessage&& other)
      : Event<W>(std::move(other)), severity_(other.severity_), message_(std::move(other.message_)) {}
  virtual ~EventMessage() {}
  EventMessage& operator=(EventMessage&& other) {
    Event<W>::operator=(std::move(other));
    severity_ = other.severity_;
    message_ = std::move(other.message_);
    return *this;
  }
  virtual void Serialize(W& writer) const;
 private:
  EventMessage(const EventMessage&) = delete;
  EventMessage& operator=(const EventMessage&) = delete;
  Severity severity_;
  std::string message_;
};

template<typename W>
class EventChipLifetime : public Event<W> {
 public:
  EventChipLifetime(const uint64_t time, const int index, const uint8_t type, bool destroyed = false);
  EventChipLifetime(EventChipLifetime&& other)
      : Event<W>(std::move(other)), index_(other.index_), type_(other.type_) {}

  virtual ~EventChipLifetime() {}
  EventChipLifetime& operator=(EventChipLifetime&& other) {
    Event<W>::operator=(std::move(other));
    index_ = other.index_;
    type_ = other.type_;
    return *this;
  }
  virtual void Serialize(W& writer) const;
 private:
  EventChipLifetime(const EventChipLifetime&) = delete;
  EventChipLifetime& operator=(const EventChipLifetime&) = delete;
  int index_;
  uint8_t type_;
  bool chip_destroyed_;
};

}  // model_core

#include <model_core/event.impl.h>

#endif // _MODEL_CORE_EVENT__


