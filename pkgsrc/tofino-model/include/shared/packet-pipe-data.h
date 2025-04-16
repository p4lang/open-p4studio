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

#ifndef _SHARED_PACKET_PIPE_DATA_
#define _SHARED_PACKET_PIPE_DATA_

#include <rmt-defs.h>
#include <bitvector.h>


namespace MODEL_CHIP_NAMESPACE {


class PacketDataCtrl {
 public:
  static constexpr uint8_t kCalcOnly          = 0;
  static constexpr uint8_t kCalcAndStore      = 1;
  static constexpr uint8_t kLoadFromPacket    = 2;
  static constexpr uint8_t kCalcComparePacket = 3; // TODO: Add this?
  static bool do_calc(uint8_t ctl)  { return (ctl == kCalcOnly); }
  static bool do_store(uint8_t ctl) { return (ctl == kCalcAndStore); }
  static bool do_load(uint8_t ctl)  { return (ctl == kLoadFromPacket);  }
};

class PacketData {
 public:
  static constexpr int kMin      =  1;
  static constexpr int kPrsrCntr =  2;
  static constexpr int kMax      =  3;
};



class PacketDeparserData {
  // Deparser stuff - NOT currently used
 public:
  PacketDeparserData(bool ingress, int deparser);
  ~PacketDeparserData();
  void     set_deparser_data(int what_data, int bit_offset, uint64_t data, int width);
  uint64_t get_deparser_data(int what_data, int bit_offset, int width);
  void     set_deparser_data_ctrl(int what_data, uint8_t ctrl);
  uint8_t  get_deparser_data_ctrl(int what_data);

 private:
  bool                                     ingress_;
  int                                      deparser_;
  std::array< uint8_t, PacketData::kMax >  ctrl_;
};



class PacketParserData {
  // Parser stuff

 public:
  static constexpr int kCounterUnitSize        =  64; // of 4 counters in CounterStack
  static constexpr int kCounterCyclesToCapture = 256; // #cycles to capture
  // In BitVector each kCounterUnitSize bits capture a cycles worth of counters
  // Cycle0 starts at bit 0 in BitVector, Cycle1 at but kCounterUnitSize * 1 etc
  static constexpr int kCounterDataSize        = kCounterUnitSize * kCounterCyclesToCapture;

  PacketParserData(bool ingress, int parser);
  ~PacketParserData();

  void     set_counter_data(int bit_offset, uint64_t data, int width);
  uint64_t get_counter_data(int bit_offset, int width);

  void     set_parser_data(int what_data, int bit_offset, uint64_t data, int width);
  uint64_t get_parser_data(int what_data, int bit_offset, int width);

  void     set_parser_data_ctrl(int what_data, uint8_t ctrl);
  uint8_t  get_parser_data_ctrl(int what_data);

 private:
  bool                                     ingress_;
  int                                      parser_;
  std::array< uint8_t, PacketData::kMax >  ctrl_;
  BitVector<kCounterDataSize>              counter_data_;
};



class PacketPipeData {
  static constexpr int kParsers = RmtDefs::kParsers;
  static constexpr int kDeparsers = 4;

 public:
  PacketPipeData();
  ~PacketPipeData();

 private:
  PacketParserData *packet_parser_data_create(bool ing, int parser);
  PacketParserData *packet_parser_data_lookup(bool ing, int parser);
  PacketParserData *packet_parser_data_set(bool ing, int parser,
                                           PacketParserData *parser_data);
  PacketParserData   *packet_parser_data_get(bool ing, int parser);

  PacketDeparserData *packet_deparser_data_create(bool ing, int deparser);
  PacketDeparserData *packet_deparser_data_lookup(bool ing, int deparser);
  PacketDeparserData *packet_deparser_data_set(bool ing, int deparser,
                                               PacketDeparserData *deparser_data);
  PacketDeparserData *packet_deparser_data_get(bool ing, int deparser);

 public:
  void     set_pipe_data_pipe(int pipe);

  void     set_pipe_data_p(bool ing, int parser, int what_data, int bit_offset, uint64_t data, int width);
  uint64_t get_pipe_data_p(bool ing, int parser, int what_data, int bit_offset, int width);

  void     set_pipe_data_ctrl_p(bool ing, int parser, int what_data, uint8_t ctrl);
  uint8_t  get_pipe_data_ctrl_p(bool ing, int parser, int what_data);

  void     set_pipe_data_d(bool ing, int deparser, int what_data, int bit_offset, uint64_t data, int width);
  uint64_t get_pipe_data_d(bool ing, int deparser, int what_data, int bit_offset, int width);

  void     set_pipe_data_ctrl_d(bool ing, int deparser, int what_data, uint8_t ctrl);
  uint8_t  get_pipe_data_ctrl_d(bool ing, int deparser, int what_data);


 private:
  int                                                                 pipe_;
  std::array<  std::array< PacketParserData*,   kParsers >,    2 >    parsers_;  // 0=ingress, 1=egress
  std::array<  std::array< PacketDeparserData*, kDeparsers >,  2 >    deparsers_;
};


}

#endif // _SHARED_PACKET_PIPE_DATA_
