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

#include <mau-event-emitter.h>
#include <mau-object.h>
#include <rmt-object-manager.h>
#include <event.h>

namespace MODEL_CHIP_NAMESPACE {

MauEventEmitter::MauEventEmitter(RmtObjectManager *om, MauObject* m_obj)
    : om_(om), m_obj_(m_obj) {}

MauEventEmitter::~MauEventEmitter() {}

void MauEventEmitter::log_gateway(const uint64_t pkt_id, const Gress gress, const int table, const uint32_t instr_full_addr, const int next_table_stage, const int next_table_table) {
  om_->log_mau_gateway(pkt_id, m_obj_->pipe_index(), gress, m_obj_->mau_index(), table, instr_full_addr, next_table_stage, next_table_table);
}

void MauEventEmitter::log_gateway(const uint64_t pkt_id, const Gress gress, const int table, const bool enabled, const bool match, const int next_table_stage, const int next_table_table) {
  om_->log_mau_gateway(pkt_id, m_obj_->pipe_index(), gress, m_obj_->mau_index(), table, enabled, match, next_table_stage, next_table_table);
}

void MauEventEmitter::log_table_hit(const uint64_t pkt_id, const Gress gress, const int table, const int next_table_stage, const int next_table_table, const uint32_t action_instr_addr, const uint32_t stats_addr, const bool stats_addr_consumed) {
  om_->log_mau_table_hit(pkt_id, m_obj_->pipe_index(), gress, m_obj_->mau_index(), table, next_table_stage, next_table_table, action_instr_addr, stats_addr, stats_addr_consumed);
}

void MauEventEmitter::log_stateful_alu(const uint64_t pkt_id, const Gress gress, const int table, const int meter_alu, const int stateful_instr) {
  om_->log_mau_stateful_alu(pkt_id, m_obj_->pipe_index(), gress, m_obj_->mau_index(), table, meter_alu, stateful_instr);
}

}
