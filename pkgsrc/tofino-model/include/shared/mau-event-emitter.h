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

#ifndef _SHARED_MAU_EVENT_EMITTER_
#define _SHARED_MAU_EVENT_EMITTER_

#include <stdint.h>

namespace MODEL_CHIP_NAMESPACE {

class RmtObjectManager;
class MauObject;
enum class Gress;

class MauEventEmitter {
 public:
  MauEventEmitter(RmtObjectManager *om, MauObject *m_obj);
  virtual ~MauEventEmitter();
  void log_gateway(const uint64_t pkt_id, const Gress gress, const int table, const uint32_t instr_full_addr, const int next_table_stage, const int next_table_table);
  void log_gateway(const uint64_t pkt_id, const Gress gress, const int table, const bool enabled, const bool match, const int next_table_stage, const int next_table_table);
  void log_table_hit(const uint64_t pkt_id, const Gress gress, const int table, const int next_table_stage, const int next_table_table, const uint32_t action_instr_addr, const uint32_t state_addr, const bool stats_addr_consumed);
  void log_stateful_alu(const uint64_t pkt_id, const Gress gress, const int table, const int meter_alu, const int stateful_instr);
 private:
  RmtObjectManager* om_;
  MauObject* m_obj_;
};

}  // MODEL_CHIP_NAMESPACE

#endif  // _SHARED_MAU_EVENT_EMITTER_
