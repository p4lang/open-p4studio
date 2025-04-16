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

#ifndef _MODEL_CORE_EVENT_IMPL_
#define _MODEL_CORE_EVENT_IMPL_

namespace model_core {

/* Event ctor */
template<typename W>
Event<W>::Event(const uint64_t sim_time, const std::chrono::system_clock::time_point real_time, bool use_local_time)
    : real_time_(real_time),
      sim_time_(sim_time),
      use_local_time_(use_local_time) {}

template<typename W>
Event<W>::Event(const uint64_t sim_time)
    : Event(sim_time, std::chrono::system_clock::now(), true) {}

/* EventMessage ctor */
template<typename W>
EventMessage<W>::EventMessage(const uint64_t time, const Severity severity, const std::string& message)
    : Event<W>(time), severity_(severity), message_(message) {}

/* EventChipLifetime ctor */
template<typename W>
EventChipLifetime<W>::EventChipLifetime(const uint64_t time, const int index, const uint8_t type, bool destroyed)
    : Event<W>(time), index_(index), type_(type), chip_destroyed_(destroyed) {}

/* Get real_time string from event (required for testing) */
template<typename W>
std::string Event<W>::real_time() const {
  return iso8601(real_time_, use_local_time_);
}

/* Serialize Event members */
template<typename W>
void Event<W>::Serialize(W& writer) const {
  // TODO check that writer has_root before entering data
  // to avoid crashing the model
  writer.Key("real_time");
  std::string real_time_str = real_time();
  writer.String(real_time_str.c_str(), static_cast<size_t>(real_time_str.length()));
  writer.Key("sim_time");
  writer.Uint64(sim_time_);
}

/* Serialize EventMessage */
template<typename W>
void EventMessage<W>::Serialize(W& writer) const {
  writer.StartObject();
  Event<W>::Serialize(writer);
  writer.Key("severity");
  writer.String(model_core::severityToString(severity_));
  writer.Key("message");
  writer.String(message_.c_str(), static_cast<size_t>(message_.length()));
  writer.EndObject();
}

/* Serialize EventChipLifetime */
template<typename W>
void EventChipLifetime<W>::Serialize(W& writer) const {
  writer.StartObject();
  Event<W>::Serialize(writer);
  if (chip_destroyed_) {
    writer.Key("chip_destroyed");
  } else {
    writer.Key("chip_created");
  }
  writer.StartObject();
  writer.Key("index");
  writer.Int(index_);
  writer.Key("type");
  writer.String(chipTypeToString(type_));
  writer.EndObject();
  writer.EndObject();
}

}  // namespace model_core

#endif // _MODEL_CORE_EVENT_IMPL_
