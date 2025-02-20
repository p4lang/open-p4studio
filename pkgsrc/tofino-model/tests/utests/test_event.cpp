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

#include <utests/test_util.h>

#include "gtest.h"

#include <event.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <rmt-types.h>
#include <packet.h>
#include <phv.h>
#include <memory>

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;

namespace MODEL_CHIP_TEST_NAMESPACE {

using TestStream = rapidjson::StringBuffer;
using TestWriter = rapidjson::Writer<TestStream>;

using model_core::Severity;

using MODEL_CHIP_NAMESPACE::RmtObjectManager;
using MODEL_CHIP_NAMESPACE::Packet;
using MODEL_CHIP_NAMESPACE::PacketBuffer;
using MODEL_CHIP_NAMESPACE::Phv;
using MODEL_CHIP_NAMESPACE::Gress;
using MODEL_CHIP_NAMESPACE::RmtTypes;


using EventMessage = MODEL_CHIP_NAMESPACE::EventMessage<TestWriter>;
using EventPacket = MODEL_CHIP_NAMESPACE::EventPacket<TestWriter>;
using EventPhv = MODEL_CHIP_NAMESPACE::EventPhv<TestWriter>;
using EventMauTableHit = MODEL_CHIP_NAMESPACE::EventMauTableHit<TestWriter>;

constexpr int kExpectBufSize = 2048;
constexpr uint64_t kTime = 333;
constexpr uint32_t kPktId = 444;
constexpr uint32_t kPktFlags = 0;
constexpr int kChip = 555;
constexpr int kPipe = 666;
constexpr Gress kGress = Gress::ingress;
constexpr char kGressString[] = "ingress";
constexpr int kRmtType = RmtTypes::kRmtTypeParser;
constexpr char kRmtTypeString[] = "parser";
constexpr char kMessage[] = "Please pass the potato parser.";

/* Test basic event serialization */
TEST(BFN_TEST_NAME(EventTest),EventMessage) {
  constexpr char expectTemplate[kExpectBufSize] =
  R"#({"real_time":"%s","sim_time":%)#" PRId64 R"#(,"context":{)#"
  R"#("pkt":{"id":%d,"flags":%d},"chip":%d,"pipe":%d,"gress":"%s",)#"
  R"#("component":"%s"},"severity":"WARN","message":"%s"})#";
  char expect[kExpectBufSize];

  TestStream stream;
  TestWriter writer(stream);
  EventMessage event(kTime, kPktId, kChip, kPipe,
      kGress, kRmtType, Severity::kWarn, kMessage);

  if (snprintf(expect, kExpectBufSize, expectTemplate,
      event.real_time().c_str(), kTime, kPktId, kPktFlags, kChip, kPipe,
      kGressString, kRmtTypeString, kMessage) >= kExpectBufSize) {
    // fail because we couldn't fit our string in 'expect'
    ADD_FAILURE();
  }

  event.Serialize(writer);

  EXPECT_STREQ(expect, stream.GetString());
}

class MyEvent: public model_core::Event<TestWriter> {
  // helper class to allow access to the private Event(sim_time, real_time)
  // constructor; for testing, forces the Event real_time_ to be represented in
  // UTC by setting use_local_time false
 public:
  MyEvent(const uint64_t sim_time, std::chrono::system_clock::time_point real_time) :
    model_core::Event<TestWriter>(sim_time, real_time, false) {
  }
  void Serialize(TestWriter& writer) const {
    writer.StartObject();
    model_core::Event<TestWriter>::Serialize(writer);
    writer.EndObject();
  }
};

TEST(BFN_TEST_NAME(EventTest),EventRealTime) {
  TestStream stream;
  TestWriter writer(stream);
  std::chrono::system_clock::time_point time_pt(std::chrono::milliseconds(123456789));
  MyEvent event(kTime, time_pt);
  event.Serialize(writer);
  // note: expect time to be in UTC because MyEvent::use_local_time_ is false
  std::string expected = "{\"real_time\":\"1970-01-02T10:17:36.789000Z\",\"sim_time\":333}";
  ASSERT_TRUE(expected == stream.GetString()) << stream.GetString();
}


/* Test Packet data serialization */
TEST(BFN_TEST_NAME(EventTest),EventPacket) {
  constexpr uint8_t kPacketSize = 6;
  constexpr char expectTemplate[kExpectBufSize] =
  R"#({"real_time":"%s","sim_time":%)#" PRId64 R"#(,"context":{)#"
  R"#("pkt":{"id":%d,"flags":%d},"chip":%d,"pipe":%d,"gress":"%s",)#"
  R"#("component":"%s"},"packet":{"port":%d,"length":%u,"data":)#"
  R"#(["68","65","6c","6c","6f","0"]}})#";
  char expect[kExpectBufSize];

  GLOBAL_MODEL->Reset();
  TestUtil tu(GLOBAL_MODEL.get());
  RmtObjectManager *om = tu.get_objmgr();

  // Create a packet
  uint8_t buf[kPacketSize] = {'h','e','l','l','o','\0'};
  PacketBuffer *pb = om->pktbuf_create(buf, kPacketSize);
  Phv *phv = om->phv_create();
  Packet *packet = om->pkt_create();
  packet->set_phv(phv);
  packet->pkt_id(kPktId);
  packet->append(pb);

  TestStream stream;
  TestWriter writer(stream);
  EventPacket event(kTime, kChip, kPipe, kRmtType, *packet);

  if (snprintf(expect, kExpectBufSize, expectTemplate,
      event.real_time().c_str(), kTime, kPktId, kPktFlags, kChip, kPipe,
      kGressString, kRmtTypeString, -1,
      kPacketSize) >= kExpectBufSize) {
    // fail because we couldn't fit our string in 'expect'
    ADD_FAILURE();
  }

  event.Serialize(writer);
  EXPECT_STREQ(expect, stream.GetString());

  stream.Flush();
  writer.Reset(stream);
  // pipe index cannot be inferred for a packet without a port
  // so expect pipe to be -1
  event = EventPacket(kTime, kChip, kRmtType, *packet);
  event.Serialize(writer);
  if (snprintf(expect, kExpectBufSize, expectTemplate,
      event.real_time().c_str(), kTime, kPktId, kPktFlags, kChip, -1,
      kGressString, kRmtTypeString, -1,
      kPacketSize) >= kExpectBufSize) {
    // fail because we couldn't fit our string in 'expect'
    ADD_FAILURE();
  }


  // delete packet (and packetbuffer)
  om->pkt_delete(packet);
}

/* Test Phv data serialization */
TEST(BFN_TEST_NAME(EventTest),EventPhv) {
  constexpr char expectTemplate[kExpectBufSize] =
  R"#({"real_time":"%s","sim_time":%)#" PRId64 R"#(,"context":{)#"
  R"#("pkt":{"id":%d,"flags":%d},"chip":%d,"pipe":%d,"gress":"%s",)#"
  R"#("component":"%s"},"phv":{"words":)#"
  R"#(["12345678","0","0","0","0","0","0","0","0","0","0","0","0","0","0",)#"
  R"#("0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0",)#"
  R"#("0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0",)#"
  R"#("0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0",)#"
  R"#("0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0",)#"
  R"#("0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0",)#"
  R"#("0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0",)#"
  R"#("0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0",)#"
  R"#("0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0",)#"
  R"#("0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0",)#"
  R"#("0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0",)#"
  R"#("0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0",)#"
  R"#("0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0",)#"
  R"#("0","0","0","0","0"]}})#";
  char expect[kExpectBufSize];

  GLOBAL_MODEL->Reset();
  TestUtil tu(GLOBAL_MODEL.get());
  RmtObjectManager *om = tu.get_objmgr();

  Phv *phv = om->phv_create();
  phv->set_pipe(kPipe);
  phv->set(Phv::make_word(0,0), 0x12345678u);

  TestStream stream;
  TestWriter writer(stream);
  EventPhv event(kTime, kPktId, kChip, kGress, kRmtType, *phv);

  if (snprintf(expect, kExpectBufSize, expectTemplate,
      event.real_time().c_str(), kTime, kPktId, kPktFlags, kChip, kPipe,
      kGressString, kRmtTypeString) >= kExpectBufSize) {
    // fail because we couldn't fit our string in 'expect'
    ADD_FAILURE();
  }

  event.Serialize(writer);
  EXPECT_STREQ(expect, stream.GetString());
  EXPECT_THROW(event = EventPhv(kTime, kChip, kRmtType, *phv), std::runtime_error);
}

/* Test basic EventMau serialization */
TEST(BFN_TEST_NAME(EventTest),EventMauTableHit) {
  constexpr int kMauStage = 3;
  constexpr int kMauTable = 4;
  constexpr int kMauNextStage = 5;
  constexpr int kMauNextTable = 6;
  constexpr uint32_t kActionInstrAddr = 7;
  constexpr uint32_t kStatsAddr = 8;
  constexpr bool kStatsAddrConsumed = true;
  constexpr char kStatsAddrConsumedString[] = "true";

  constexpr char expectTemplate[kExpectBufSize] =
  R"#({"real_time":"%s","sim_time":%)#" PRId64 R"#(,"context":{)#"
  R"#("pkt":{"id":%d,"flags":%d},"chip":%d,"pipe":%d,"gress":"%s",)#"
  R"#("component":"%s","stage":%d,"table":%d},"mau_table_hit":{)#"
  R"#("next_table":{"stage":%d,"table":%d},"action_instr_addr":"%x",)#"
  R"#("stats_addr":"%x","stats_addr_consumed":%s}})#";
  char expect[kExpectBufSize];

  TestStream stream;
  TestWriter writer(stream);
  EventMauTableHit event(kTime, kPktId, kChip, kPipe,
      kGress, kMauStage, kMauTable, kMauNextStage, kMauNextTable,
      kActionInstrAddr, kStatsAddr, kStatsAddrConsumed);

  if (snprintf(expect, kExpectBufSize, expectTemplate,
      event.real_time().c_str(), kTime, kPktId, kPktFlags, kChip, kPipe,
      kGressString, "mau", kMauStage, kMauTable,
      kMauNextStage, kMauNextTable, kActionInstrAddr,
      kStatsAddr, kStatsAddrConsumedString) >= kExpectBufSize) {
    // fail because we couldn't fit our string in 'expect'
    ADD_FAILURE();
  }


  event.Serialize(writer);
  EXPECT_STREQ(expect, stream.GetString());
}

}  // namespace MODEL_CHIP_TEST_NAMESPACE
