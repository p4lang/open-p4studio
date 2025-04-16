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

#ifndef _SHARED_EVENT_
#define _SHARED_EVENT_

#include <model_core/event.h>
#include <packet.h>
#include <port.h>
#include <phv.h>
#include <i2queueing-metadata.h>
#include <e2mac-metadata.h>
#include <rmt-types.h>
#include <cinttypes>
#include <rapidjson/writer.h>
#include <sstream>
#include <iostream>
#include <string>

namespace MODEL_CHIP_NAMESPACE {

using Severity = model_core::Severity;

enum class Gress {
  undef,
  ingress,
  egress,
  both
};

/* Utility to convert Gress enum to JSON enum names */
const char* gressToString(const Gress gress);

template<typename W>
class EventRmt : public model_core::Event<W> {
 public:
  EventRmt(const uint64_t time, const uint64_t pkt_id, const int chip, const int pipe, const Gress gress, const int rmt_type);
  EventRmt(EventRmt&& other) = default;
  EventRmt& operator=(EventRmt&& other) = default;
  virtual ~EventRmt() {}
  virtual void Serialize(W& writer) const;
 protected:
  uint64_t pkt_id_;
  int chip_;
  int pipe_;
  Gress gress_;
  const char* rmt_type_;
};

template<typename W>
class EventMessage : public EventRmt<W> {
 public:
  EventMessage(const uint64_t time, const uint64_t pkt_id, const int chip, const int pipe, const Gress gress, const int rmt_type, const Severity severity, const std::string& message);
  EventMessage(EventMessage&& other)
      : EventRmt<W>(std::move(other)), severity_(other.severity_), message_(std::move(other.message_)) {}
  virtual ~EventMessage() {}
  EventMessage& operator=(EventMessage&& other) {
    EventRmt<W>::operator=(std::move(other));
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
class EventPhv : public EventRmt<W> {
 public:
  EventPhv(const uint64_t time, const int chip, const int rmt_type, const Phv& phv);
  EventPhv(const uint64_t time, const uint64_t pkt_id, const int chip, const Gress gress, const int rmt_type, const Phv& phv);
  EventPhv(EventPhv&& o)
      : EventRmt<W>(std::move(o)), phv_(o.phv_) {}
  virtual ~EventPhv() {}
  EventPhv& operator=(EventPhv&& o) {
    EventRmt<W>::operator=(std::move(o));
    phv_ = o.phv_;
    return *this;
  }
  virtual void Serialize(W& writer) const;
 private:
  EventPhv(const EventPhv&) = delete;
  EventPhv& operator=(const EventPhv&) = delete;

  const Phv* phv_;
};

template<typename W>
class EventPacket : public EventRmt<W> {
 public:
  EventPacket(const uint64_t time, const int chip, const int rmt_type, const Packet& packet);
  EventPacket(const uint64_t time, const int chip, const int pipe, const int rmt_type, const Packet& packet);
  EventPacket(EventPacket&& o)
      : EventRmt<W>(std::move(o)), packet_(o.packet_) {}
  virtual ~EventPacket() {}
  EventPacket& operator=(EventPacket&& o) {
    EventRmt<W>::operator=(std::move(o));
    packet_ = o.packet_;
    return *this;
  }
  virtual void Serialize(W& writer) const;
 private:
  EventPacket(const EventPacket&) = delete;
  EventPacket& operator=(const EventPacket&) = delete;

  const Packet* packet_;
};

template<typename W>
class EventParserState : public EventRmt<W> {
 public:
  EventParserState(const uint64_t time, const uint64_t pkt_id, const int chip, const int pipe, const Gress gress, const int state);
  EventParserState(EventParserState&& other)
      : EventRmt<W>(std::move(other)), state_(other.state_) {}
  virtual ~EventParserState() {}
  EventParserState& operator=(EventParserState&& other) {
    EventRmt<W>::operator=(std::move(other));
    state_ = other.state_;
    return *this;
  }
  virtual void Serialize(W& writer) const;
 private:
  EventParserState(const EventParserState&) = delete;
  EventParserState& operator=(const EventParserState&) = delete;

  int state_;
};

template<typename W>
class EventParserExtract : public EventRmt<W> {
 public:
  EventParserExtract(const uint64_t time, const uint64_t pkt_id, const int chip, const int pipe, const Gress gress, const int phv_word, const int data, const bool tag_along);
  EventParserExtract(EventParserExtract&& other)
      : EventRmt<W>(std::move(other)), phv_word_(other.phv_word_), data_(other.data_), tag_along_(other.tag_along_) {}
  virtual ~EventParserExtract() {}
  EventParserExtract& operator=(EventParserExtract&& other) {
    EventRmt<W>::operator=(std::move(other));
    phv_word_ = other.phv_word_;
    data_ = other.data_;
    tag_along_ = other.tag_along_;
    return *this;
  }
  virtual void Serialize(W& writer) const;
 private:
  EventParserExtract(const EventParserExtract&) = delete;
  EventParserExtract& operator=(const EventParserExtract&) = delete;

  int phv_word_;
  int data_;
  bool tag_along_;
};

template<typename W>
class EventParserTcamMatch : public EventRmt<W> {
 public:
  EventParserTcamMatch(const uint64_t time, const uint64_t pkt_id, const int chip, const int pipe, const Gress gress, const int index, const std::string& lookup);
  EventParserTcamMatch(EventParserTcamMatch&& other)
      : EventRmt<W>(std::move(other)), index_(other.index_), lookup_(std::move(other.lookup_)) {}
  virtual ~EventParserTcamMatch() {}
  EventParserTcamMatch& operator=(EventParserTcamMatch&& other) {
    EventRmt<W>::operator=(std::move(other));
    index_ = other.index_;
    lookup_ = std::move(other.lookup_);
    return *this;
  }
  virtual void Serialize(W& writer) const;
 private:
  EventParserTcamMatch(const EventParserTcamMatch&) = delete;
  EventParserTcamMatch& operator=(const EventParserTcamMatch&) = delete;

  int index_;
  std::string lookup_;
};

template<typename W>
class EventDeparserMetadataToTm : public EventRmt<W> {
 public:
  EventDeparserMetadataToTm(const uint64_t time, const uint64_t pkt_id, const int chip, const int pipe, const I2QueueingMetadata& metadata);
  EventDeparserMetadataToTm(EventDeparserMetadataToTm&& other)
      : EventRmt<W>(std::move(other)), m_(other.m_) {}
  virtual ~EventDeparserMetadataToTm() {}
  EventDeparserMetadataToTm& operator=(EventDeparserMetadataToTm&& other) {
    EventRmt<W>::operator=(std::move(other));
    m_ = other.m_;
    return *this;
  }
  virtual void Serialize(W& writer) const;
 private:
  EventDeparserMetadataToTm(const EventDeparserMetadataToTm&) = delete;
  EventDeparserMetadataToTm& operator=(const EventDeparserMetadataToTm&) = delete;
  const I2QueueingMetadata* m_;
};

template<typename W>
class EventDeparserMetadataToMac : public EventRmt<W> {
 public:
  EventDeparserMetadataToMac(const uint64_t time, const uint64_t pkt_id, const int chip, const int pipe, const E2MacMetadata& metadata);
  EventDeparserMetadataToMac(EventDeparserMetadataToMac&& other)
      : EventRmt<W>(std::move(other)), m_(other.m_) {}
  virtual ~EventDeparserMetadataToMac() {}
  EventDeparserMetadataToMac& operator=(EventDeparserMetadataToMac&& other) {
    EventRmt<W>::operator=(std::move(other));
    m_ = other.m_;
    return *this;
  }
  virtual void Serialize(W& writer) const;
 private:
  EventDeparserMetadataToMac(const EventDeparserMetadataToMac&) = delete;
  EventDeparserMetadataToMac& operator=(const EventDeparserMetadataToMac&) = delete;
  const E2MacMetadata* m_;
};

template<typename W>
class EventMau : public EventRmt<W> {
 public:
  EventMau(const uint64_t time, const uint64_t pkt_id, const int chip, const int pipe, const Gress gress, const int stage, const int table);
  EventMau(EventMau&& other)
      : EventRmt<W>(std::move(other)),
        stage_(other.stage_), table_(other.table_) {}
  virtual ~EventMau() {}
  EventMau& operator=(EventMau&& other) {
    EventRmt<W>::operator=(std::move(other));
    stage_ = other.stage_;
    table_ = other.table_;
    return *this;
  }
  virtual void Serialize(W& writer) const;
 private:
  EventMau(const EventMau&) = delete;
  EventMau& operator=(const EventMau&) = delete;
  int stage_;
  int table_;
};

template<typename W>
class EventMauPhv : public EventMau<W> {
 public:
  EventMauPhv(const uint64_t time, const int chip, const int stage, const Phv& phv);
  EventMauPhv(const uint64_t time, const uint64_t pkt_id, const int chip, const Gress gress, const int stage, const Phv& phv);
  EventMauPhv(EventMauPhv&& other)
      : EventMau<W>(std::move(other)), phv_(other.phv_) {}
  virtual ~EventMauPhv() {}
  EventMauPhv& operator=(EventMauPhv&& other) {
    EventMau<W>::operator=(std::move(other));
    phv_ = other.phv_;
    return *this;
  }
  virtual void Serialize(W& writer) const;
 private:
  EventMauPhv(const EventMauPhv&) = delete;
  EventMauPhv& operator=(const EventMauPhv&) = delete;
  const Phv* phv_;
};

template<typename W>
class EventMauGateway : public EventMau<W> {
 public:
  EventMauGateway(const uint64_t time, const uint64_t pkt_id, const int chip, const int pipe, const Gress gress, const int stage, const int table, const bool enabled, const bool match, const int next_table_stage, const int next_table_table);
  EventMauGateway(const uint64_t time, const uint64_t pkt_id, const int chip, const int pipe, const Gress gress, const int stage, const int table, const uint32_t action_instr_addr, const int next_table_stage, const int next_table_table);
  EventMauGateway(EventMauGateway&& other)
      : EventMau<W>(std::move(other)),
        enabled_(other.enabled_), match_(other.match_),
        action_instr_addr_(other.action_instr_addr_),
        next_table_stage_(other.next_table_stage_),
        next_table_table_(other.next_table_table_) {}
  virtual ~EventMauGateway() {}
  EventMauGateway& operator=(EventMauGateway&& other) {
    EventMau<W>::operator=(std::move(other));
    enabled_ = other.enabled_;
    match_ = other.match_;
    action_instr_addr_ = other.action_instr_addr_;
    next_table_stage_ = other.next_table_stage_;
    next_table_table_ = other.next_table_table_;
    return *this;
  }
  virtual void Serialize(W& writer) const;
 private:
  EventMauGateway(const EventMauGateway&) = delete;
  EventMauGateway& operator=(const EventMauGateway&) = delete;
  bool enabled_;
  bool match_;
  uint32_t action_instr_addr_;
  int next_table_stage_;
  int next_table_table_;
};

template<typename W>
class EventMauTableHit : public EventMau<W> {
 public:
  EventMauTableHit(const uint64_t time, const uint64_t pkt_id, const int chip, const int pipe, const Gress gress, const int stage, const int table, const int next_table_stage, const int next_table_table, const uint32_t action_instr_addr, const uint32_t stats_addr, const bool stats_addr_consumed);
  EventMauTableHit(EventMauTableHit&& other)
      : EventMau<W>(std::move(other)),
        next_table_stage_(other.next_table_stage_),
        next_table_table_(other.next_table_table_),
        action_instr_addr_(other.action_instr_addr_),
        stats_addr_(other.stats_addr_),
        stats_addr_consumed_(other.stats_addr_consumed_) {}
  virtual ~EventMauTableHit() {}
  EventMauTableHit& operator=(EventMauTableHit&& other) {
    EventMau<W>::operator=(std::move(other));
    next_table_stage_ = other.next_table_stage_;
    next_table_table_ = other.next_table_table_;
    action_instr_addr_ = other.action_instr_addr_;
    stats_addr_ = other.stats_addr_;
    stats_addr_consumed_ = other.stats_addr_consumed_;
    return *this;
  }
  virtual void Serialize(W& writer) const;
 private:
  EventMauTableHit(const EventMauTableHit&) = delete;
  EventMauTableHit& operator=(const EventMauTableHit&) = delete;
  int next_table_stage_;
  int next_table_table_;
  uint32_t action_instr_addr_;
  uint32_t stats_addr_;
  bool stats_addr_consumed_;
};

template <typename W>
class EventMauStatefulAlu : public EventMau<W> {
 public:
  EventMauStatefulAlu(const uint64_t time, const uint64_t pkt_id, const int chip, const int pipe, const Gress gress, const int stage, const int table, const int meter_alu, const int stateful_instr);
  EventMauStatefulAlu(EventMauStatefulAlu&& other)
      : EventMau<W>(std::move(other)),
        meter_alu_(other.meter_alu_), stateful_instr_(other.stateful_instr_) {}
  virtual ~EventMauStatefulAlu() {}
  EventMauStatefulAlu& operator=(EventMauStatefulAlu&& other) {
    EventMau<W>::operator=(std::move(other));
    meter_alu_ = other.meter_alu_;
    stateful_instr_ = other.stateful_instr_;
    return *this;
  }
  virtual void Serialize(W& writer) const;
 private:
  EventMauStatefulAlu(const EventMauStatefulAlu&) = delete;
  EventMauStatefulAlu& operator=(const EventMauStatefulAlu&) = delete;
  int meter_alu_;
  int stateful_instr_;
};

}  // MODEL_CHIP_NAMESPACE

#include <event.impl.h>

#endif // _SHARED_EVENT__
