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
#include <common/rmt-assert.h>
#include <packet-pipe-data.h>


namespace MODEL_CHIP_NAMESPACE {

PacketDeparserData::PacketDeparserData(bool ingress, int deparser)
    : ingress_(ingress), deparser_(deparser), ctrl_() {
}
PacketDeparserData::~PacketDeparserData() {
}


PacketParserData::PacketParserData(bool ingress, int parser)
    : ingress_(ingress), parser_(parser), ctrl_(), counter_data_() {
  (void)ingress_;
  (void)parser_;
}
PacketParserData::~PacketParserData() {
}

void PacketParserData::set_counter_data(int bit_offset, uint64_t data, int width) {
  RMT_ASSERT(bit_offset >= 0);
  //printf("set_counter_data(%d,%d,0x%" PRIx64 ")\n", bit_offset, width, data);
  if (bit_offset < kCounterDataSize) counter_data_.set_word(data, bit_offset, width);
}
uint64_t PacketParserData::get_counter_data(int bit_offset, int width) {
  RMT_ASSERT(bit_offset >= 0);
  uint64_t data = UINT64_C(0);
  if (bit_offset < kCounterDataSize) data = counter_data_.get_word(bit_offset, width);
  //printf("get_counter_data(%d,%d) = 0x%" PRIx64 "\n", bit_offset, width, data);
  return data;
}

void PacketParserData::set_parser_data(int what_data, int bit_offset,
                                       uint64_t data, int width) {
  switch (what_data) {
    case PacketData::kPrsrCntr: set_counter_data(bit_offset, data, width); break;
    default: RMT_ASSERT(0 && "set_parser_data bad argument");
  }
}
uint64_t PacketParserData::get_parser_data(int what_data, int bit_offset, int width) {
  switch (what_data) {
    case PacketData::kPrsrCntr: return get_counter_data(bit_offset, width);
    default:                 return UINT64_C(0);
  }
}

void PacketParserData::set_parser_data_ctrl(int what_data, uint8_t ctrl) {
  RMT_ASSERT((what_data >= PacketData::kMin) && (what_data <= PacketData::kMax));
  RMT_ASSERT((ctrl >= PacketDataCtrl::kCalcOnly) && (ctrl <= PacketDataCtrl::kLoadFromPacket));
  ctrl_[what_data] = ctrl;
}
uint8_t PacketParserData::get_parser_data_ctrl(int what_data) {
  RMT_ASSERT((what_data >= PacketData::kMin) && (what_data <= PacketData::kMax));
  return ctrl_[what_data];
}




PacketPipeData::PacketPipeData() : pipe_(-1) {
  for (int ie = 0; ie <= 1; ie++) {
    for (int i = 0; i < kParsers; i++) parsers_[ie][i] = nullptr;
    for (int i = 0; i < kDeparsers; i++) deparsers_[ie][i] = nullptr;
  }
}
PacketPipeData::~PacketPipeData() {
  for (int ie = 0; ie <= 1; ie++) {
    for (int i = 0; i < kParsers; i++) {
      if (parsers_[ie][i] != nullptr) delete parsers_[ie][i];
      parsers_[ie][i] = nullptr;
    }
    for (int i = 0; i < kDeparsers; i++) {
      if (deparsers_[ie][i] != nullptr) delete deparsers_[ie][i];
      deparsers_[ie][i] = nullptr;
    }
  }
}

PacketParserData *PacketPipeData::packet_parser_data_create(bool ing, int parser) {
  RMT_ASSERT((parser >= 0) && (parser < kParsers));
  return new PacketParserData(ing, parser);
}
PacketParserData *PacketPipeData::packet_parser_data_lookup(bool ing, int parser) {
  RMT_ASSERT((parser >= 0) && (parser < kParsers));
  return parsers_[ing?0:1][parser];
}
PacketParserData *PacketPipeData::packet_parser_data_set(bool ing, int parser,
                                                         PacketParserData *parser_data) {
  RMT_ASSERT((parser >= 0) && (parser < kParsers) && (parser_data != nullptr));
  parsers_[ing?0:1][parser] = parser_data;
  return parser_data;
}
PacketParserData *PacketPipeData::packet_parser_data_get(bool ing, int parser) {
  RMT_ASSERT((parser >= 0) && (parser < kParsers));
  PacketParserData *parser_data = packet_parser_data_lookup(ing, parser);
  if (parser_data != nullptr) return parser_data;
  return packet_parser_data_set(ing, parser, packet_parser_data_create(ing, parser));
}


void PacketPipeData::set_pipe_data_pipe(int pipe) {
  pipe_ = pipe;
}

void PacketPipeData::set_pipe_data_p(bool ing, int parser, int what_data,
                                     int bit_offset, uint64_t data, int width) {
  RMT_ASSERT((parser >= 0) && (parser < kParsers));
  RMT_ASSERT((what_data >= PacketData::kMin) && (what_data <= PacketData::kMax));
  PacketParserData *parser_data = packet_parser_data_get(ing, parser);
  RMT_ASSERT(parser_data != nullptr);
  parser_data->set_parser_data(what_data, bit_offset, data, width);
}
uint64_t PacketPipeData::get_pipe_data_p(bool ing, int parser, int what_data,
                                         int bit_offset, int width) {
  RMT_ASSERT((what_data >= PacketData::kMin) && (what_data <= PacketData::kMax));
  PacketParserData *parser_data = packet_parser_data_lookup(ing, parser);
  RMT_ASSERT(parser_data != nullptr);
  return parser_data->get_parser_data(what_data, bit_offset, width);
}



void PacketPipeData::set_pipe_data_ctrl_p(bool ing, int parser, int what_data, uint8_t ctrl) {
  RMT_ASSERT((what_data >= PacketData::kMin) && (what_data <= PacketData::kMax));
  RMT_ASSERT((ctrl >= PacketDataCtrl::kCalcOnly) && (ctrl <= PacketDataCtrl::kLoadFromPacket));
  // Ignore set of Calc if no PacketParserData object yet exists as Calc is default
  if ((ctrl == PacketDataCtrl::kCalcOnly) && (packet_parser_data_lookup(ing, parser) == nullptr))
    return;
  PacketParserData *parser_data = packet_parser_data_get(ing, parser);
  RMT_ASSERT(parser_data != nullptr);
  parser_data->set_parser_data_ctrl(what_data, ctrl);
}
uint8_t PacketPipeData::get_pipe_data_ctrl_p(bool ing, int parser, int what_data) {
  RMT_ASSERT((what_data >= PacketData::kMin) && (what_data <= PacketData::kMax));
  PacketParserData *parser_data = packet_parser_data_lookup(ing, parser);
  // If no PacketParserData object created yet, just say we want to CalcOnly
  if (parser_data == nullptr) return PacketDataCtrl::kCalcOnly;
  return parser_data->get_parser_data_ctrl(what_data);
}



}
