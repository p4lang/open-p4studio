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

#include <mau.h>
#include <common/rmt-assert.h>
#include <rmt-log.h>
#include <rmt-object-manager.h>


namespace MODEL_CHIP_NAMESPACE {

// Map string->int
int MauInfo::lookup_name(const char *name) {
  try {
    return static_string_map_.at(std::string(name));
  } catch (const std::exception&) {
    RMT_LOG_OBJ(mau_, RmtDebug::verbose(), "Unable to map string \'%s\'\n",
                name);
    RMT_ASSERT(0);
  }
}

// String versions read/write
void MauInfo::write_name(const char *name, uint32_t val) {
  write(lookup_name(name), val);
}
// Allow reset on read
uint32_t MauInfo::read_name(const char *name, bool rst) {
  int index = lookup_name(name);
  uint32_t val = read(index);
  if (rst) reset(index);
  return val;
}

// Dump all values in order
void MauInfo::dump() {
  for (int i = 0; i < size_; i++) {
    uint32_t val = read(i);
    const char *key = lookup_index(i);
    if ((key != NULL) && (val != 0u)) printf("%s = %d\n", key, val);
  }
}


// Maintain strings in a vector too - create once
int MauInfo::static_init() {
  if (static_string_vec_ == NULL) {
    int max_val = 0;
    for (auto kv : static_string_map_) {
      RMT_ASSERT(kv.second >= 0);
      if (kv.second > max_val) max_val = kv.second;
    }
    std::vector<const char*> *string_vec = new std::vector<const char*>(max_val+1);
    for (auto kv : static_string_map_) {
      string_vec->at(kv.second) = strdup(kv.first.c_str());
    }
    static_string_vec_ = string_vec;
  }
  return static_cast<int>(static_string_vec_->size());
}

std::vector<const char*> *MauInfo::static_string_vec_ = NULL;

const std::unordered_map<std::string,int> MauInfo::static_string_map_ = {
    { std::string("MAU_ERRORS"), MAU_ERRORS },
    { std::string("MAU_ERRORS_2"), MAU_ERRORS_2 },
    { std::string("MAU_WARNINGS"), MAU_WARNINGS },
    { std::string("MAU_N_PHVS"), MAU_N_PHVS },
    { std::string("MAU_N_MATCH_PHVS"), MAU_N_MATCH_PHVS },
    { std::string("MAU_N_EOPS"), MAU_N_EOPS },
    { std::string("MAU_SRAM_CFG_READS"), MAU_SRAM_CFG_READS },
    { std::string("MAU_SRAM_CFG_WRITES"), MAU_SRAM_CFG_WRITES },
    { std::string("MAU_TCAM_CFG_READS"), MAU_TCAM_CFG_READS },
    { std::string("MAU_TCAM_CFG_WRITES"), MAU_TCAM_CFG_WRITES },
    { std::string("MAU_MAPRAM_CFG_READS"), MAU_MAPRAM_CFG_READS },
    { std::string("MAU_MAPRAM_CFG_WRITES"), MAU_MAPRAM_CFG_WRITES },
    { std::string("MAU_DEFRAM_CFG_READS"), MAU_DEFRAM_CFG_READS },
    { std::string("MAU_DEFRAM_CFG_WRITES"), MAU_DEFRAM_CFG_WRITES },
    { std::string("MAU_BAD_CFG_READS"), MAU_BAD_CFG_READS },
    { std::string("MAU_BAD_CFG_WRITES"), MAU_BAD_CFG_WRITES },
    { std::string("MAU_STATS_VIRT_READS"), MAU_STATS_VIRT_READS },
    { std::string("MAU_STATS_VIRT_WRITES"), MAU_STATS_VIRT_WRITES },
    { std::string("MAU_METER_VIRT_READS"), MAU_METER_VIRT_READS },
    { std::string("MAU_METER_VIRT_WRITES"), MAU_METER_VIRT_WRITES },
    { std::string("MAU_SELECTOR_STATEFUL_VIRT_READS"), MAU_SELECTOR_STATEFUL_VIRT_READS },
    { std::string("MAU_SELECTOR_STATEFUL_VIRT_WRITES"), MAU_SELECTOR_STATEFUL_VIRT_WRITES },
    { std::string("MAU_IDLETIME_VIRT_READS"), MAU_IDLETIME_VIRT_READS },
    { std::string("MAU_IDLETIME_VIRT_WRITES"), MAU_IDLETIME_VIRT_WRITES },
    { std::string("MAU_BAD_VIRT_READS"), MAU_BAD_VIRT_READS },
    { std::string("MAU_BAD_VIRT_WRITES"), MAU_BAD_VIRT_WRITES },
    { std::string("MAU_INSTR_WRITES"), MAU_INSTR_WRITES },
    { std::string("MAU_METER_SWEEPS"), MAU_METER_SWEEPS },
    { std::string("MAU_METER_SINGLE_SWEEPS"), MAU_METER_SINGLE_SWEEPS },
    { std::string("MAU_IDLETIME_SWEEPS"), MAU_IDLETIME_SWEEPS },
    { std::string("MAU_IDLETIME_SINGLE_SWEEPS"), MAU_IDLETIME_SINGLE_SWEEPS },
    { std::string("MAU_STATS_DUMPS"), MAU_STATS_DUMPS },
    { std::string("MAU_STATS_DUMP_WORDS"), MAU_STATS_DUMP_WORDS },
    { std::string("MAU_STATS_LOCKS"), MAU_STATS_LOCKS },
    { std::string("MAU_STATS_UNLOCKS"), MAU_STATS_UNLOCKS },
    { std::string("MAU_IDLETIME_DUMPS"), MAU_IDLETIME_DUMPS },
    { std::string("MAU_IDLETIME_DUMP_WORDS"), MAU_IDLETIME_DUMP_WORDS },
    { std::string("MAU_IDLETIME_LOCKS"), MAU_IDLETIME_LOCKS },
    { std::string("MAU_IDLETIME_UNLOCKS"), MAU_IDLETIME_UNLOCKS },
    { std::string("MAU_BARRIER_OPS"), MAU_BARRIER_OPS },
    { std::string("MAU_MOVEREGS_PUSH"), MAU_MOVEREGS_PUSH },
    { std::string("MAU_MOVEREGS_POP"), MAU_MOVEREGS_POP },
    { std::string("MAU_HITS"), MAU_HITS },
    { std::string("MAU_XM_RUNS"), MAU_XM_RUNS },
    { std::string("MAU_XM_HITS"), MAU_XM_HITS },
    { std::string("MAU_XM_NORMAL_HITS"), MAU_XM_NORMAL_HITS },
    { std::string("MAU_XM_WIDE_HITS"), MAU_XM_WIDE_HITS },
    { std::string("MAU_XM_MULTIPLE_HITS"), MAU_XM_MULTIPLE_HITS },
    { std::string("MAU_XM_MULTICOL_HITS"), MAU_XM_MULTICOL_HITS },
    { std::string("MAU_XM_MULTIRAM_HITS"), MAU_XM_MULTIRAM_HITS },
    { std::string("MAU_XM_XTRA_HITS"), MAU_XM_XTRA_HITS },
    { std::string("MAU_LTCAM_RUNS"), MAU_LTCAM_RUNS },
    { std::string("MAU_LTCAM_HITS"), MAU_LTCAM_HITS },
    { std::string("MAU_LTCAM_NORMAL_HITS"), MAU_LTCAM_NORMAL_HITS },
    { std::string("MAU_LTCAM_WIDE_HITS"), MAU_LTCAM_WIDE_HITS },
    { std::string("MAU_LTCAM_TIND_HITS"), MAU_LTCAM_TIND_HITS },
    { std::string("MAU_LTCAM_NO_TIND_HITS"), MAU_LTCAM_NO_TIND_HITS },
    { std::string("MAU_LTCAM_PAIRED_RUNS"), MAU_LTCAM_PAIRED_RUNS },
    { std::string("MAU_LTCAM_PAIRED_ERRORS"), MAU_LTCAM_PAIRED_ERRORS },
    { std::string("MAU_GW_RUNS"), MAU_GW_RUNS },
    { std::string("MAU_GW_HITS"), MAU_GW_HITS },
    { std::string("MAU_GW_INHIBITS"), MAU_GW_INHIBITS },
    { std::string("MAU_STASH_RUNS"), MAU_STASH_RUNS },
    { std::string("MAU_STASH_HITS"), MAU_STASH_HITS },
    { std::string("MAU_TABLE_COUNTERS_TICKED"), MAU_TABLE_COUNTERS_TICKED },
    { std::string("MAU_STATEFUL_LOG_COUNTERS_TICKED"), MAU_STATEFUL_LOG_COUNTERS_TICKED },
    { std::string("MAU_SNAPSHOTS_TAKEN"), MAU_SNAPSHOTS_TAKEN },
    { std::string("MAU_IMM_DATA_ADDRS_DISTRIBUTED"), MAU_IMM_DATA_ADDRS_DISTRIBUTED },
    { std::string("MAU_IMM_DATA_MISS_ADDRS_DISTRIBUTED"), MAU_IMM_DATA_MISS_ADDRS_DISTRIBUTED },
    { std::string("MAU_INSTR_ADDRS_DISTRIBUTED"), MAU_INSTR_ADDRS_DISTRIBUTED },
    { std::string("MAU_INSTR_MISS_ADDRS_DISTRIBUTED"), MAU_INSTR_MISS_ADDRS_DISTRIBUTED },
    { std::string("MAU_ACT_DATA_ADDRS_DISTRIBUTED"), MAU_ACT_DATA_ADDRS_DISTRIBUTED },
    { std::string("MAU_ACT_DATA_MISS_ADDRS_DISTRIBUTED"), MAU_ACT_DATA_MISS_ADDRS_DISTRIBUTED },
    { std::string("MAU_ACT_DATA_ADDRS_UNCONSUMED"), MAU_ACT_DATA_ADDRS_UNCONSUMED },
    { std::string("MAU_STATS_ADDRS_DISTRIBUTED"), MAU_STATS_ADDRS_DISTRIBUTED },
    { std::string("MAU_STATS_MISS_ADDRS_DISTRIBUTED"), MAU_STATS_MISS_ADDRS_DISTRIBUTED },
    { std::string("MAU_STATS_ADDRS_UNCONSUMED"), MAU_STATS_ADDRS_UNCONSUMED },
    { std::string("MAU_METER_ADDRS_DISTRIBUTED"), MAU_METER_ADDRS_DISTRIBUTED },
    { std::string("MAU_METER_MISS_ADDRS_DISTRIBUTED"), MAU_METER_MISS_ADDRS_DISTRIBUTED },
    { std::string("MAU_METER_ADDRS_UNCONSUMED"), MAU_METER_ADDRS_UNCONSUMED },
    { std::string("MAU_IDLETIME_ADDRS_DISTRIBUTED"), MAU_IDLETIME_ADDRS_DISTRIBUTED },
    { std::string("MAU_IDLETIME_MISS_ADDRS_DISTRIBUTED"), MAU_IDLETIME_MISS_ADDRS_DISTRIBUTED },
    { std::string("MAU_IDLETIME_ADDRS_UNCONSUMED"), MAU_IDLETIME_ADDRS_UNCONSUMED },
    { std::string("MAU_NXT_TAB_ADDRS_DISTRIBUTED"), MAU_NXT_TAB_ADDRS_DISTRIBUTED },
    { std::string("MAU_NXT_TAB_MISS_ADDRS_DISTRIBUTED"), MAU_NXT_TAB_MISS_ADDRS_DISTRIBUTED },
    { std::string("MAU_SEL_LEN_ADDRS_DISTRIBUTED"), MAU_SEL_LEN_ADDRS_DISTRIBUTED },
    { std::string("MAU_SEL_LEN_MISS_ADDRS_DISTRIBUTED"), MAU_SEL_LEN_MISS_ADDRS_DISTRIBUTED },
    { std::string("MAU_INSTR_MAP_INDIRECTIONS_USED"), MAU_INSTR_MAP_INDIRECTIONS_USED },
    { std::string("MAU_NXT_TAB_MAP_INDIRECTIONS_USED"), MAU_NXT_TAB_MAP_INDIRECTIONS_USED },
    { std::string("MAU_OFLO_ADDRS_DISTRIBUTED"), MAU_OFLO_ADDRS_DISTRIBUTED },
    { std::string("MAU_ACTION_SRAMS_READ"), MAU_ACTION_SRAMS_READ },
    { std::string("MAU_STATS_SRAMS_READ"), MAU_STATS_SRAMS_READ },
    { std::string("MAU_STATS_SRAMS_WRITTEN"), MAU_STATS_SRAMS_WRITTEN },
    { std::string("MAU_METER_SRAMS_READ"), MAU_METER_SRAMS_READ },
    { std::string("MAU_METER_SRAMS_WRITTEN"), MAU_METER_SRAMS_WRITTEN },
    { std::string("MAU_SELECTOR_SRAMS_READ"), MAU_SELECTOR_SRAMS_READ },
    { std::string("MAU_SELECTOR_SRAMS_WRITTEN"), MAU_SELECTOR_SRAMS_WRITTEN },
    { std::string("MAU_STATEFUL_SRAMS_READ"), MAU_STATEFUL_SRAMS_READ },
    { std::string("MAU_STATEFUL_SRAMS_WRITTEN"), MAU_STATEFUL_SRAMS_WRITTEN },
    { std::string("MAU_IDLETIME_MAPRAMS_READ"), MAU_IDLETIME_MAPRAMS_READ },
    { std::string("MAU_IDLETIME_MAPRAMS_WRITTEN"), MAU_IDLETIME_MAPRAMS_WRITTEN },
    { std::string("MAU_COLOR_MAPRAMS_READ"), MAU_COLOR_MAPRAMS_READ },
    { std::string("MAU_COLOR_MAPRAMS_WRITTEN"), MAU_COLOR_MAPRAMS_WRITTEN },
    { std::string("MAU_STATS_ALU_INVOCATIONS"), MAU_STATS_ALU_INVOCATIONS },
    { std::string("MAU_STATS_ALU_EVICTIONS"), MAU_STATS_ALU_EVICTIONS },
    { std::string("MAU_STATS_ALU_DUMPS"), MAU_STATS_ALU_DUMPS },
    { std::string("MAU_METER_ALU_TOTAL_INVOCATIONS"), MAU_METER_ALU_TOTAL_INVOCATIONS },
    { std::string("MAU_METER_ALU_NORMAL_INVOCATIONS"), MAU_METER_ALU_NORMAL_INVOCATIONS },
    { std::string("MAU_METER_ALU_LPF_INVOCATIONS"), MAU_METER_ALU_LPF_INVOCATIONS },
    { std::string("MAU_METER_ALU_LPF_RED_INVOCATIONS"), MAU_METER_ALU_LPF_RED_INVOCATIONS },
    { std::string("MAU_METER_ALU_SALU_INVOCATIONS"), MAU_METER_ALU_SALU_INVOCATIONS },
    { std::string("MAU_SELECTOR_ALU_INVOCATIONS"), MAU_SELECTOR_ALU_INVOCATIONS },
    { std::string("MAU_ACTION_HV_UPDATES"), MAU_ACTION_HV_UPDATES },
    { std::string("MAU_VLIW_ALU_EXECUTIONS"), MAU_VLIW_ALU_EXECUTIONS },
    { std::string("MAU_N_TEOPS"), MAU_N_TEOPS }
  };


}
