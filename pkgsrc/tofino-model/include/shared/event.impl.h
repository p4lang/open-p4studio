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

#ifndef _SHARED_EVENT_IMPL_
#define _SHARED_EVENT_IMPL_

namespace MODEL_CHIP_NAMESPACE {

/* Construct EventRmt */
template<typename W>
EventRmt<W>::EventRmt(const uint64_t time, const uint64_t pkt_id, const int chip, const int pipe, const Gress gress, const int rmt_type)
    : model_core::Event<W>(time), pkt_id_(pkt_id), chip_(chip), pipe_(pipe), gress_(gress), rmt_type_(RmtTypes::toString(rmt_type)) {}

/* Constuct EventMessage */
template <typename W>
EventMessage<W>::EventMessage(const uint64_t time, const uint64_t pkt_id, const int chip, const int pipe, const Gress gress, const int rmt_type, const Severity severity, const std::string& message)
    : EventRmt<W>(time, pkt_id, chip, pipe, gress, rmt_type),
      severity_(severity),
      message_(message) {}

/* Construct EventPhv with Phv internals */
template<typename W>
EventPhv<W>::EventPhv(const uint64_t time, const int chip, const int rmt_type, const Phv& phv)
    : EventRmt<W>(time, 0, chip, phv.pipe_index(), Gress::undef, rmt_type),
      phv_(&phv) {
  if (phv.ingress() && phv.egress()) {
    this->gress_ = Gress::both;
    // TODO how to log correct pkt_id for this event
    if (phv.ingress_packet()) {
      this->pkt_id_ = phv.ingress_packet()->pkt_id();
    }
  } else if (phv.ingress()) {
    this->gress_ = Gress::ingress;
    if (phv.ingress_packet()) {
      this->pkt_id_ = phv.ingress_packet()->pkt_id();
    }
  } else if (phv.egress()) {
    this->gress_ = Gress::egress;
    if (phv.egress_packet()) {
      this->pkt_id_ = phv.egress_packet()->pkt_id();
    }
  } else if (phv.ghost()) {
    // Need to handle Ghost packets now - ingress only
    this->gress_ = Gress::ingress;
    if (phv.ingress_packet()) {
      this->pkt_id_ = phv.ingress_packet()->pkt_id();
    }
  } else {
    throw std::runtime_error(std::string("EventPhv could not determine pkt_id from phv."));
  }
}

/* Construct EventPhv with explicit pkt_id and gress */
template<typename W>
EventPhv<W>::EventPhv(const uint64_t time, const uint64_t pkt_id, const int chip, const Gress gress, const int rmt_type, const Phv& phv)
    : EventRmt<W>(time, pkt_id, chip, phv.pipe_index(), gress, rmt_type),
      phv_(&phv) {}

/* Construct EventPacket with internals of Packet */
template<typename W>
EventPacket<W>::EventPacket(const uint64_t time, const int chip, const int rmt_type, const Packet& packet)
    : EventRmt<W>(time, packet.pkt_id(), chip, 0, packet.is_egress() ? Gress::egress : Gress::ingress, rmt_type),
      packet_(&packet) {
  // we want to get pipe_index() from packet.port()
  // but packet.port() could be nullptr
  if (packet.port() != nullptr) {
    this->pipe_ = packet.port()->pipe_index();
  } else {
    this->pipe_ = -1;
  }
}
/* Construct EventPacket with pipe index */
template<typename W>
EventPacket<W>::EventPacket(const uint64_t time, const int chip, const int pipe, const int rmt_type, const Packet& packet)
    : EventRmt<W>(time, packet.pkt_id(), chip, pipe, packet.is_egress() ? Gress::egress : Gress::ingress, rmt_type),
      packet_(&packet) {}

/* Construct EventParserState */
template<typename W>
EventParserState<W>::EventParserState(const uint64_t time, const uint64_t pkt_id, const int chip, const int pipe_index, const Gress gress, const int state)
    : EventRmt<W>(time, pkt_id, chip, pipe_index, gress, RmtTypes::kRmtTypeParser),
      state_(state) {}

/* Constract EventParserExtract */
template<typename W>
EventParserExtract<W>::EventParserExtract(const uint64_t time, const uint64_t pkt_id, const int chip, const int pipe, const Gress gress, const int phv_word, const int data, const bool tag_along)
    : EventRmt<W>(time, pkt_id, chip, pipe, gress, RmtTypes::kRmtTypeParser),
      phv_word_(phv_word), data_(data), tag_along_(tag_along) {}

/* Construct EventParserTcamMatch */
template<typename W>
EventParserTcamMatch<W>::EventParserTcamMatch(const uint64_t time, const uint64_t pkt_id, const int chip, const int pipe, const Gress gress, const int index, const std::string& lookup)
    : EventRmt<W>(time, pkt_id, chip, pipe, gress, RmtTypes::kRmtTypeParser),
      index_(index), lookup_(lookup) {}

/* Construct EventDeparserMetadataToTm */
template<typename W>
EventDeparserMetadataToTm<W>::EventDeparserMetadataToTm(const uint64_t time, const uint64_t pkt_id, const int chip, const int pipe, const I2QueueingMetadata& metadata)
    : EventRmt<W>(time, pkt_id, chip, pipe, Gress::ingress, RmtTypes::kRmtTypeDeparser),
      m_(&metadata) {}

/* Construct EventDeparserMetadataToMac */
template<typename W>
EventDeparserMetadataToMac<W>::EventDeparserMetadataToMac(const uint64_t time, const uint64_t pkt_id, const int chip, const int pipe, const E2MacMetadata& metadata)
    : EventRmt<W>(time, pkt_id, chip, pipe, Gress::egress, RmtTypes::kRmtTypeDeparser),
      m_(&metadata) {}

/* Construct EventMau */
template<typename W>
EventMau<W>::EventMau(const uint64_t time, const uint64_t pkt_id, const int chip, const int pipe, const Gress gress, const int stage, const int table)
    : EventRmt<W>(time, pkt_id, chip, pipe, gress, RmtTypes::kRmtTypeMau),
      stage_(stage), table_(table) {}

/* Construct EventMauPhv */
template<typename W>
EventMauPhv<W>::EventMauPhv(const uint64_t time,  const int chip, const int stage, const Phv& phv)
    : EventMau<W>(time, 0, chip, phv.pipe_index(), Gress::undef, stage, 0),
      phv_(&phv) {
  if (phv.ingress() && phv.egress()) {
    this->gress_ = Gress::both;
    // TODO how to log correct pkt_id for this event
    if (phv.ingress_packet()) {
      this->pkt_id_ = phv.ingress_packet()->pkt_id();
    }
  } else if (phv.ingress()) {
    this->gress_ = Gress::ingress;
    if (phv.ingress_packet()) {
      this->pkt_id_ = phv.ingress_packet()->pkt_id();
    }
  } else if (phv.egress()) {
    this->gress_ = Gress::egress;
    if (phv.egress_packet()) {
      this->pkt_id_ = phv.egress_packet()->pkt_id();
    }
  } else if (phv.ghost()) {
    // Need to handle Ghost packets now - ingress only
    this->gress_ = Gress::ingress;
    if (phv.ingress_packet()) {
      this->pkt_id_ = phv.ingress_packet()->pkt_id();
    }
  } else {
    throw std::runtime_error(std::string("EventMauPhv could not determine pkt_id from phv."));
  }
}

/* Construct EventMauPhv with explicit pkt_id and gress */
template<typename W>
EventMauPhv<W>::EventMauPhv(const uint64_t time, const uint64_t pkt_id, const int chip, const Gress gress, const int stage, const Phv& phv)
    : EventMau<W>(time, pkt_id, chip, phv.pipe_index(), gress, stage, 0),
      phv_(&phv) {}

/* Construct EventMauGateway */
template<typename W>
EventMauGateway<W>::EventMauGateway(const uint64_t time, const uint64_t pkt_id, const int chip, const int pipe, const Gress gress, const int stage, const int table, const bool enabled, const bool match, const int next_table_stage, const int next_table_table)
    : EventMau<W>(time, pkt_id, chip, pipe, gress, stage, table),
      enabled_(enabled), match_(match),
      action_instr_addr_(0),
      next_table_stage_(next_table_stage),
      next_table_table_(next_table_table) {}
template<typename W>
EventMauGateway<W>::EventMauGateway(const uint64_t time, const uint64_t pkt_id, const int chip, const int pipe, const Gress gress, const int stage, const int table, const uint32_t action_instr_addr, const int next_table_stage, const int next_table_table)
    : EventMau<W>(time, pkt_id, chip, pipe, gress, stage, table),
      enabled_(true), match_(true),
      action_instr_addr_(action_instr_addr),
      next_table_stage_(next_table_stage),
      next_table_table_(next_table_table) {}

/* Construct EventMauTableHit */
template<typename W>
EventMauTableHit<W>::EventMauTableHit(const uint64_t time, const uint64_t pkt_id, const int chip, const int pipe, const Gress gress, const int stage, const int table, const int next_table_stage, const int next_table_table, const uint32_t action_instr_addr, const uint32_t stats_addr, const bool stats_addr_consumed)
    : EventMau<W>(time, pkt_id, chip, pipe, gress, stage, table),
      next_table_stage_(next_table_stage),
      next_table_table_(next_table_table),
      action_instr_addr_(action_instr_addr),
      stats_addr_(stats_addr),
      stats_addr_consumed_(stats_addr_consumed) {}

/* Construct EventMauStatefulAlu */
template<typename W>
EventMauStatefulAlu<W>::EventMauStatefulAlu(const uint64_t time, const uint64_t pkt_id, const int chip, const int pipe, const Gress gress, const int stage, const int table, const int meter_alu, const int stateful_instr)
    : EventMau<W>(time, pkt_id, chip, pipe, gress, stage, table),
      meter_alu_(meter_alu), stateful_instr_(stateful_instr) {}

/* Seralize EventRmt */
template<typename W>
void EventRmt<W>::Serialize(W& writer) const {
  writer.StartObject();   // context
  writer.Key("pkt");
  writer.StartObject();   // "pkt": { "id": 1, "flags": 300000}
  writer.Key("id");
  writer.Uint(pkt_id_ & Packet::kPktIdMask);
  writer.Key("flags");
  writer.Uint((pkt_id_ & ~Packet::kPktIdMask) >> Packet::kPktIdWidth);
  writer.EndObject();     // pkt
  writer.Key("chip");
  writer.Int(chip_);
  writer.Key("pipe");
  writer.Int(pipe_);
  writer.Key("gress");
  writer.String(gressToString(gress_));
  writer.Key("component");
  writer.String(rmt_type_); // TODO consider changing rmt_type_ to constexpr
  writer.EndObject();     // context
}

/* Serialize EventMessage */
template<typename W>
void EventMessage<W>::Serialize(W& writer) const {
  writer.StartObject();
  model_core::Event<W>::Serialize(writer);
  writer.Key("context");
  EventRmt<W>::Serialize(writer);
  writer.Key("severity");
  writer.String(model_core::severityToString(severity_));
  writer.Key("message");
  writer.String(message_.c_str(), static_cast<size_t>(message_.length()));
  writer.EndObject();
}

/* Serialize EventPhv */
template<typename W>
void EventPhv<W>::Serialize(W& writer) const {

  // Start writing JSON
  writer.StartObject(); // event
  model_core::Event<W>::Serialize(writer);     // time
  writer.Key("context");
  EventRmt<W>::Serialize(writer);
  writer.Key("phv");
  writer.StartObject(); // phv

  writer.Key("words");

  writer.StartArray();
  for (int i = 0; i < Phv::kWordsMax; i++) {
    std::stringstream ss;
    ss << &std::hex << phv_->get(i);
    writer.String(ss.str().c_str());
  }
  writer.EndArray();

  writer.EndObject();   // phv
  writer.EndObject();   // event
}


/* Serialize EventPacket */
template<typename W>
void EventPacket<W>::Serialize(W& writer) const {
  // Extract packet data
  int port = packet_->port() == nullptr ? -1 : packet_->port()->port_index();
  int length = packet_->len();
  uint8_t data[length];
  packet_->get_buf(data, 0, length);

  // Start writing JSON
  writer.StartObject(); // event
  model_core::Event<W>::Serialize(writer);     // time
  writer.Key("context");
  EventRmt<W>::Serialize(writer);
  writer.Key("packet");
  writer.StartObject(); // packet
  writer.Key("port");
  writer.Int(port);
  writer.Key("length");
  writer.Uint(length);
  writer.Key("data");
  writer.StartArray();
  for (auto d : data) {
    std::stringstream ss;
    ss << &std::hex << static_cast<int>(d);
    writer.String(ss.str().c_str());
  }
  writer.EndArray();
  writer.EndObject();   // packet
  writer.EndObject();   // event
}

/* Serialize EventParserState  */
template<typename W>
void EventParserState<W>::Serialize(W& writer) const {
  writer.StartObject();
  model_core::Event<W>::Serialize(writer);
  writer.Key("context");
  EventRmt<W>::Serialize(writer);
  writer.Key("parser_state");
  writer.Int(state_);
  writer.EndObject();
}

/* Serialize EventParserExtract */
template<typename W>
void EventParserExtract<W>::Serialize(W& writer) const {
  std::stringstream ss;
  ss << &std::hex << data_;

  writer.StartObject();
  model_core::Event<W>::Serialize(writer);
  writer.Key("context");
  EventRmt<W>::Serialize(writer);
  writer.Key("parser_extract");
  writer.StartObject();
  writer.Key("phv_word");
  writer.Int(phv_word_);
  writer.Key("data");
  writer.String(ss.str().c_str());
  writer.Key("tag_along");
  writer.Bool(tag_along_);
  writer.EndObject();
  writer.EndObject();
}

/* Serialize EventParserTcamMatch */
template<typename W>
void EventParserTcamMatch<W>::Serialize(W& writer) const {
  writer.StartObject();
  model_core::Event<W>::Serialize(writer);
  writer.Key("context");
  EventRmt<W>::Serialize(writer);
  writer.Key("parser_tcam_match");
  writer.StartObject();
  writer.Key("index");
  writer.Int(index_);
  writer.Key("lookup");
  writer.String(lookup_.c_str(), static_cast<size_t>(lookup_.length()));
  writer.EndObject();
  writer.EndObject();
}

/* Serialize EventDeparserMetadataToTm */
template<typename W>
void EventDeparserMetadataToTm<W>::Serialize(W& writer) const {
  writer.StartObject();
  model_core::Event<W>::Serialize(writer);
  writer.Key("context");
  EventRmt<W>::Serialize(writer);
  writer.Key("deparser_metadata_tm");
  writer.StartObject();
  writer.Key("version");
  writer.Uint(m_->version());
  writer.Key("icos");
  writer.Uint(m_->icos());
  writer.Key("cpu_needs_copy");
  writer.Bool(m_->cpu_needs_copy());
  writer.Key("needs_mc_copy");
  writer.Bool(m_->needs_mc_copy());
  writer.Key("cpu_cos");
  writer.Uint(m_->cpu_cos());
  writer.Key("is_egress_uc");
  writer.Bool(m_->is_egress_uc());
  writer.Key("egress_uc_port");
  writer.Uint(m_->egress_uc_port());
  writer.Key("has_mgid1");
  writer.Uint(m_->has_mgid1());
  writer.Key("has_mgid2");
  writer.Uint(m_->has_mgid2());
  writer.Key("mgid1");
  writer.Uint(m_->mgid1());
  writer.Key("mgid2");
  writer.Uint(m_->mgid2());
  writer.Key("hash1");
  writer.Uint(m_->hash1());
  writer.Key("hash2");
  writer.Uint(m_->hash2());
  writer.Key("physical_ingress_port");
  writer.Uint(m_->physical_ingress_port());
  writer.Key("pipe_mask");
  writer.Uint(m_->pipe_mask());
  writer.Key("ct_disable_mode");
  writer.Bool(m_->ct_disable_mode());
  writer.Key("ct_mcast_mode");
  writer.Bool(m_->ct_mcast_mode());
  writer.Key("dod");
  writer.Bool(m_->dod());
  writer.Key("meter_color");
  writer.Uint(m_->meter_color());
  writer.Key("multicast_pipe_vector");
  writer.Uint(m_->multicast_pipe_vector());
  writer.Key("qid");
  writer.Uint(m_->qid());
  writer.Key("irid");
  writer.Uint(m_->irid());
  writer.Key("use_yid_tbl");
  writer.Bool(m_->use_yid_tbl());
  writer.Key("bypass_egr_mode");
  writer.Bool(m_->bypass_egr_mode());
  writer.Key("xid");
  writer.Uint(m_->xid());
  writer.Key("yid");
  writer.Uint(m_->yid());
  writer.Key("afc");
  writer.Int64(m_->afc() ? m_->afc()->asUint64Val() : 0);
  writer.Key("mtu_trunc_len");
  writer.Int(m_->mtu_trunc_len() ? *m_->mtu_trunc_len() : 0);
  writer.Key("mtu_trunc_err_f");
  writer.Int(m_->mtu_trunc_err_f());
  writer.EndObject();
  writer.EndObject();
}

/* Serialize EventDeparserMetadataToMac */
template<typename W>
void EventDeparserMetadataToMac<W>::Serialize(W& writer) const {
  writer.StartObject();
  model_core::Event<W>::Serialize(writer);
  writer.Key("context");
  EventRmt<W>::Serialize(writer);
  writer.Key("deparser_metadata_mac");
  writer.StartObject();
  writer.Key("capture_tx_ts");
  writer.Bool(m_->capture_tx_ts());
  writer.Key("ecos");
  writer.Uint(m_->ecos());
  writer.Key("force_tx_error");
  writer.Bool(m_->force_tx_error());
  writer.Key("update_delay_on_tx");
  writer.Bool(m_->update_delay_on_tx());
  writer.Key("is_egress_uc");
  writer.Bool(m_->is_egress_uc());
  writer.Key("egress_unicast_port");
  writer.Uint(m_->egress_unicast_port());
  writer.Key("afc");
  writer.Int64(m_->afc() ? m_->afc()->asUint64Val() : 0);
  writer.Key("mtu_trunc_len");
  writer.Int(m_->mtu_trunc_len() ? *m_->mtu_trunc_len() : 0);
  writer.Key("mtu_trunc_err_f");
  writer.Int(m_->mtu_trunc_err_f());
  writer.EndObject();
  writer.EndObject();
}

/* Serialize EventMau */
template<typename W>
void EventMau<W>::Serialize(W& writer) const {
  writer.StartObject();   // context
  writer.Key("pkt");
  writer.StartObject();   // "pkt": { "id": 1, "flags": 300000}
  writer.Key("id");
  writer.Uint(this->pkt_id_ & Packet::kPktIdMask);
  writer.Key("flags");
  writer.Uint((this->pkt_id_ & ~Packet::kPktIdMask) >> Packet::kPktIdWidth);
  writer.EndObject();     // pkt
  writer.Key("chip");
  writer.Int(this->chip_);
  writer.Key("pipe");
  writer.Int(this->pipe_);
  writer.Key("gress");
  writer.String(gressToString(this->gress_));
  writer.Key("component");
  writer.String(this->rmt_type_);
  writer.Key("stage");
  writer.Int(stage_);
  writer.Key("table");
  writer.Int(table_);
  writer.EndObject();     // context
}

/* Serialize EventMauPhv */
template <typename W>
void EventMauPhv<W>::Serialize(W& writer) const {

  // Start writing JSON
  writer.StartObject(); // event
  model_core::Event<W>::Serialize(writer);     // time
  writer.Key("context");
  EventMau<W>::Serialize(writer);
  writer.Key("phv");
  writer.StartObject(); // phv

  writer.Key("words");

  writer.StartArray();
  for (int i = 0; i < Phv::kWordsMax; i++) {
    std::stringstream ss;
    ss << &std::hex << phv_->get(i);
    writer.String(ss.str().c_str());
  }
  writer.EndArray();

  writer.EndObject();   // phv
  writer.EndObject();   // event

}

/* Serialize EventMauGatway */
template<typename W>
void EventMauGateway<W>::Serialize(W& writer) const {
  writer.StartObject();     // event
  model_core::Event<W>::Serialize(writer);
  writer.Key("context");
  EventMau<W>::Serialize(writer);
  writer.Key("mau_gateway");
  writer.StartObject();     // mau_gateway
  writer.Key("enabled");
  writer.Bool(enabled_);
  writer.Key("match");
  writer.Bool(match_);
  if (enabled_ && match_) {
    std::stringstream ss;
    ss << &std::hex << action_instr_addr_;
    writer.Key("action_instr_addr");
    writer.String(ss.str().c_str());
  }
  writer.Key("next_table");
  writer.StartObject();     // next_table
  writer.Key("stage");
  writer.Int(next_table_stage_);
  writer.Key("table");
  writer.Int(next_table_table_);
  writer.EndObject();       // next_table
  writer.EndObject();       // mau_gateway
  writer.EndObject();       // event
}

/* Serialize EventMauTableHit */
template<typename W>
void EventMauTableHit<W>::Serialize(W& writer) const {
  std::stringstream action_instr_addr_hex, stats_addr_hex;
  action_instr_addr_hex << &std::hex << action_instr_addr_;
  stats_addr_hex << &std::hex << stats_addr_;

  writer.StartObject();     // event
  model_core::Event<W>::Serialize(writer);
  writer.Key("context");
  EventMau<W>::Serialize(writer);
  writer.Key("mau_table_hit");
  writer.StartObject();     // mau_table_hit
  writer.Key("next_table");
  writer.StartObject();     // next table
  writer.Key("stage");
  writer.Int(next_table_stage_);
  writer.Key("table");
  writer.Int(next_table_table_);
  writer.EndObject();       // next table
  writer.Key("action_instr_addr");
  writer.String(action_instr_addr_hex.str().c_str());
  writer.Key("stats_addr");
  writer.String(stats_addr_hex.str().c_str());
  writer.Key("stats_addr_consumed");
  writer.Bool(stats_addr_consumed_);
  writer.EndObject();       // mau_table_hit
  writer.EndObject();       // event
}

/* Serialize EventMauStatefulAlu */
template<typename W>
void EventMauStatefulAlu<W>::Serialize(W& writer) const {
  writer.StartObject();
  model_core::Event<W>::Serialize(writer);
  writer.Key("context");
  EventMau<W>::Serialize(writer);
  writer.Key("mau_stateful_alu");
  writer.StartObject();
  writer.Key("meter_alu");
  writer.Int(meter_alu_);
  writer.Key("stateful_instr");
  writer.Int(stateful_instr_);
  writer.EndObject();
  writer.EndObject();
}

} // MODEL_CHIP_NAMESPACE
#endif // _SHARED_EVENT_IMPL_
