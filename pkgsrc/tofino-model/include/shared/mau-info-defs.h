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

#ifndef _SHARED_MAU_INFO_DEFS_H_
#define _SHARED_MAU_INFO_DEFS_H_

#define MAU_ERRORS                           0
#define MAU_ERRORS_2                         1
#define MAU_WARNINGS                         2
#define MAU_N_PHVS                           3
#define MAU_N_MATCH_PHVS                     4
#define MAU_N_EOPS                           5
#define MAU_SRAM_CFG_READS                   6
#define MAU_SRAM_CFG_WRITES                  7
#define MAU_TCAM_CFG_READS                   8
#define MAU_TCAM_CFG_WRITES                  9
#define MAU_MAPRAM_CFG_READS                10
#define MAU_MAPRAM_CFG_WRITES               11
#define MAU_DEFRAM_CFG_READS                12
#define MAU_DEFRAM_CFG_WRITES               13
#define MAU_BAD_CFG_READS                   14
#define MAU_BAD_CFG_WRITES                  15
#define MAU_STATS_VIRT_READS                16
#define MAU_STATS_VIRT_WRITES               17
#define MAU_METER_VIRT_READS                18
#define MAU_METER_VIRT_WRITES               19
#define MAU_SELECTOR_STATEFUL_VIRT_READS    20
#define MAU_SELECTOR_STATEFUL_VIRT_WRITES   21
#define MAU_IDLETIME_VIRT_READS             22
#define MAU_IDLETIME_VIRT_WRITES            23
#define MAU_BAD_VIRT_READS                  24
#define MAU_BAD_VIRT_WRITES                 25
#define MAU_INSTR_WRITES                    26
#define MAU_METER_SWEEPS                    27
#define MAU_METER_SINGLE_SWEEPS             28
#define MAU_IDLETIME_SWEEPS                 29
#define MAU_IDLETIME_SINGLE_SWEEPS          30
#define MAU_STATS_DUMPS                     31
#define MAU_STATS_DUMP_WORDS                32
#define MAU_STATS_LOCKS                     33
#define MAU_STATS_UNLOCKS                   34
#define MAU_IDLETIME_DUMPS                  35
#define MAU_IDLETIME_DUMP_WORDS             36
#define MAU_IDLETIME_LOCKS                  37
#define MAU_IDLETIME_UNLOCKS                38
#define MAU_BARRIER_OPS                     39
#define MAU_MOVEREGS_PUSH                   40
#define MAU_MOVEREGS_POP                    41
#define MAU_HITS                            42
#define MAU_XM_RUNS                         43
#define MAU_XM_HITS                         44
#define MAU_XM_NORMAL_HITS                  45
#define MAU_XM_WIDE_HITS                    46
#define MAU_XM_MULTIPLE_HITS                47
#define MAU_XM_MULTICOL_HITS                48
#define MAU_XM_MULTIRAM_HITS                49
#define MAU_XM_XTRA_HITS                    50
#define MAU_LTCAM_RUNS                      51
#define MAU_LTCAM_HITS                      52
#define MAU_LTCAM_NORMAL_HITS               53
#define MAU_LTCAM_WIDE_HITS                 54
#define MAU_LTCAM_BITMAP_HITS               55
#define MAU_LTCAM_TIND_HITS                 56
#define MAU_LTCAM_NO_TIND_HITS              57
#define MAU_LTCAM_PAIRED_RUNS               58
#define MAU_LTCAM_PAIRED_ERRORS             59
#define MAU_GW_RUNS                         60
#define MAU_GW_HITS                         61
#define MAU_GW_INHIBITS                     62
#define MAU_STASH_RUNS                      63
#define MAU_STASH_HITS                      64
#define MAU_TABLE_COUNTERS_TICKED           65
#define MAU_STATEFUL_LOG_COUNTERS_TICKED    66
#define MAU_SNAPSHOTS_TAKEN                 67
#define MAU_IMM_DATA_ADDRS_DISTRIBUTED      68
#define MAU_IMM_DATA_MISS_ADDRS_DISTRIBUTED 69
#define MAU_INSTR_ADDRS_DISTRIBUTED         70
#define MAU_INSTR_MISS_ADDRS_DISTRIBUTED    71
#define MAU_ACT_DATA_ADDRS_DISTRIBUTED      72
#define MAU_ACT_DATA_MISS_ADDRS_DISTRIBUTED 73
#define MAU_ACT_DATA_ADDRS_UNCONSUMED       74
#define MAU_STATS_ADDRS_DISTRIBUTED         75
#define MAU_STATS_MISS_ADDRS_DISTRIBUTED    76
#define MAU_STATS_ADDRS_UNCONSUMED          77
#define MAU_METER_ADDRS_DISTRIBUTED         78
#define MAU_METER_MISS_ADDRS_DISTRIBUTED    79
#define MAU_METER_ADDRS_UNCONSUMED          80
#define MAU_IDLETIME_ADDRS_DISTRIBUTED      81
#define MAU_IDLETIME_MISS_ADDRS_DISTRIBUTED 82
#define MAU_IDLETIME_ADDRS_UNCONSUMED       83
#define MAU_NXT_TAB_ADDRS_DISTRIBUTED       84
#define MAU_NXT_TAB_MISS_ADDRS_DISTRIBUTED  85
#define MAU_SEL_LEN_ADDRS_DISTRIBUTED       86
#define MAU_SEL_LEN_MISS_ADDRS_DISTRIBUTED  87
#define MAU_INSTR_MAP_INDIRECTIONS_USED     88
#define MAU_NXT_TAB_MAP_INDIRECTIONS_USED   89
#define MAU_OFLO_ADDRS_DISTRIBUTED          90
#define MAU_ACTION_SRAMS_READ               91
#define MAU_STATS_SRAMS_READ                92
#define MAU_STATS_SRAMS_WRITTEN             93
#define MAU_METER_SRAMS_READ                94
#define MAU_METER_SRAMS_WRITTEN             95
#define MAU_SELECTOR_SRAMS_READ             96
#define MAU_SELECTOR_SRAMS_WRITTEN          97
#define MAU_STATEFUL_SRAMS_READ             98
#define MAU_STATEFUL_SRAMS_WRITTEN          99
#define MAU_IDLETIME_MAPRAMS_READ          100
#define MAU_IDLETIME_MAPRAMS_WRITTEN       101
#define MAU_COLOR_MAPRAMS_READ             102
#define MAU_COLOR_MAPRAMS_WRITTEN          103
#define MAU_STATS_ALU_INVOCATIONS          104
#define MAU_STATS_ALU_EVICTIONS            105
#define MAU_STATS_ALU_DUMPS                106
#define MAU_METER_ALU_TOTAL_INVOCATIONS    107
#define MAU_METER_ALU_NORMAL_INVOCATIONS   108
#define MAU_METER_ALU_LPF_INVOCATIONS      109
#define MAU_METER_ALU_LPF_RED_INVOCATIONS  110
#define MAU_METER_ALU_SALU_INVOCATIONS     111
#define MAU_SELECTOR_ALU_INVOCATIONS       112
#define MAU_ACTION_HV_UPDATES              113
#define MAU_VLIW_ALU_EXECUTIONS            114
#define MAU_N_TEOPS                        115

// MUST be big enough to accommodate all values
#define MAU_INFO_DEFS_ARRAY_SIZE           116

#endif // _SHARED_MAU_INFO_DEFS_
