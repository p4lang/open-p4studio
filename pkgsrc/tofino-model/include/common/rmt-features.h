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

#ifndef _RMT_FEATURES_H_
#define _RMT_FEATURES_H_


/* Check only one mode defined */
#ifdef DV_MODE
#ifdef RTL_MODE
#define ERROR_MULTIPLE_MODES_DEFINED
#endif
#endif
#ifdef DV_MODE
#ifdef SW_MODE
#define ERROR_MULTIPLE_MODES_DEFINED
#endif
#endif
#ifdef RTL_MODE
#ifdef SW_MODE
#define ERROR_MULTIPLE_MODES_DEFINED
#endif
#endif
#ifdef ERROR_MULTIPE_MODES_DEFINED
#error Only one of DV_MODE, RTL_MODE, SW_MODE can be defined
#endif



/* Pick a default mode (SW_MODE) if none defined */
#ifndef NO_MODE
#ifndef DV_MODE
#ifndef RTL_MODE
#ifndef SW_MODE
#define SW_MODE
#endif
#endif
#endif
#endif



/* Treat DV_MODE and RTL_MODE as BFN_INTERNAL by default      */
/* (but we also allow SW_MODE builds to include BFN_INTERNAL) */
#ifndef BFN_INTERNAL
#ifdef DV_MODE
#define BFN_INTERNAL
#endif
#ifdef RTL_MODE
#define BFN_INTERNAL
#endif
#endif


/* For each mode define what features to enable */
/* These control globals defined in rmt-global.cpp or rmt-config.cpp */
/* So features can be dynamically controlled by tweaking global vars */



#ifdef DV_MODE
/* DV_MODE used for RTL verification */

#define FEATURE_ABORT_ON_ERROR       1
#define FEATURE_THROW_ON_ERROR       1
#ifdef NDEBUG
#define FEATURE_THROW_ON_ASSERT      1
#else
#define FEATURE_THROW_ON_ASSERT      1
#endif
#define DEFAULT_EN_RETURN_0BAD       0
#define INITIAL_LOG_FLAGS            0x07u
#define FIXED_LOG_FLAGS              0x07u
#define PKT_INIT_TIMERAND_ON_ALLOC   false
#define PHV_INIT_TIMERAND_ON_ALLOC   false
#define ZEROISE_PUSH_POP_DATA        false
#define MAPRAM_WRITE_DATA1_HAS_TIME  true
#define KEEP_FULL_RES_STATS          true
#define ACCESS_FULL_RES_STATS        true
#define EXTRACT_CHECK_RELAX          true
#define OFLO_ROW_CHECK_RELAX         true
#define ADDRS_CONSUMED_CHECK_RELAX   true
#define DELAY_CHECK_RELAX            true
#define PAIRED_LTCAM_CHECK_RELAX     true
#define INSTR_FORMAT_CHECK_RELAX     true
#define INSTR_CONFIG_CHECK_RELAX     false
#define SNAPSHOT_USE_PHV_TIME        true
#define SNAPSHOT_MASK_THREAD_FIELDS  false
#define PRE_PFE_ADDR_CHECK_RELAX     true
#define SALU_PRED_RSI_CHECK_RELAX    false
#define SYNTH2PORT_CHECK_RELAX       true
#define DEPENDENCY_CHECK_RELAX       true
#define PARSE_CHECK_RELAX            true
#define SYNC_SALU_CNTR_CLEAR         false
#define SALU_CNTR_TICK_CHECK_TIME    true
#define RESET_UNUSED_LOOKUP_RESULTS  true
#define SET_NEXT_TABLE_PRED          true
#define METER_EXPONENT_CHECK_RELAX   true
#define METER_SWEEP_ON_DEMAND        false
#define CHIP_USE_GLOBAL_TIME_IF_ZERO false
#define CLOT_OVERLAP_CHECK_RELAX     false
#define CLOT_ALLOW_ADJACENT          true
#define CLOT_ALLOW_DUPLICATE         true
#define CLOT_ALLOW_REPEAT_EMIT       true

#endif /* DV_MODE */



#ifdef SW_MODE
/* SW_MODE used by software builds eg p4factory */

#define FEATURE_ABORT_ON_ERROR       0
#define FEATURE_THROW_ON_ERROR       1
#ifdef NDEBUG
#define FEATURE_THROW_ON_ASSERT      1
#else
#define FEATURE_THROW_ON_ASSERT      0
#endif
#define DEFAULT_EN_RETURN_0BAD       1
#define INITIAL_LOG_FLAGS            0x0Fu
#define FIXED_LOG_FLAGS              0x0Fu
#define PKT_INIT_TIMERAND_ON_ALLOC   true
#define PHV_INIT_TIMERAND_ON_ALLOC   true
#define ZEROISE_PUSH_POP_DATA        true
#define MAPRAM_WRITE_DATA1_HAS_TIME  false
#define KEEP_FULL_RES_STATS          true
#define ACCESS_FULL_RES_STATS        true
#define EXTRACT_CHECK_RELAX          false
#define OFLO_ROW_CHECK_RELAX         false
#define ADDRS_CONSUMED_CHECK_RELAX   false
#define DELAY_CHECK_RELAX            false
#define PAIRED_LTCAM_CHECK_RELAX     false
#define INSTR_FORMAT_CHECK_RELAX     false
#define INSTR_CONFIG_CHECK_RELAX     false
#define SNAPSHOT_USE_PHV_TIME        false
#define SNAPSHOT_MASK_THREAD_FIELDS  false
#define PRE_PFE_ADDR_CHECK_RELAX     false
#define SALU_PRED_RSI_CHECK_RELAX    true
#define SYNTH2PORT_CHECK_RELAX       false
#define DEPENDENCY_CHECK_RELAX       false
#define PARSE_CHECK_RELAX            false
#define SYNC_SALU_CNTR_CLEAR         true
#define SALU_CNTR_TICK_CHECK_TIME    false
#define RESET_UNUSED_LOOKUP_RESULTS  false
#define SET_NEXT_TABLE_PRED          false
#define METER_EXPONENT_CHECK_RELAX   false
#define METER_SWEEP_ON_DEMAND        true
#define CHIP_USE_GLOBAL_TIME_IF_ZERO true
#define CLOT_OVERLAP_CHECK_RELAX     false
#define CLOT_ALLOW_ADJACENT          true
#define CLOT_ALLOW_DUPLICATE         false
#define CLOT_ALLOW_REPEAT_EMIT       false

#endif /* SW_MODE */



#ifdef RTL_MODE
/* RTL_MODE mostly like SW_MODE but without FULL_RES_STATS */
/* Disabling FULL_RES_STATS allows PBUS address sanity checks to conform to RTL */

#define FEATURE_ABORT_ON_ERROR       0
#define FEATURE_THROW_ON_ERROR       0
#ifdef NDEBUG
#define FEATURE_THROW_ON_ASSERT      1
#else
#define FEATURE_THROW_ON_ASSERT      0
#endif
#define DEFAULT_EN_RETURN_0BAD       1
#define INITIAL_LOG_FLAGS            0x0Fu
#define FIXED_LOG_FLAGS              0x0Fu
#define PKT_INIT_TIMERAND_ON_ALLOC   true
#define PHV_INIT_TIMERAND_ON_ALLOC   true
#define ZEROISE_PUSH_POP_DATA        true
#define MAPRAM_WRITE_DATA1_HAS_TIME  false
#define KEEP_FULL_RES_STATS          false
#define ACCESS_FULL_RES_STATS        false
#define EXTRACT_CHECK_RELAX          false
#define OFLO_ROW_CHECK_RELAX         false
#define ADDRS_CONSUMED_CHECK_RELAX   false
#define DELAY_CHECK_RELAX            false
#define PAIRED_LTCAM_CHECK_RELAX     false
#define INSTR_FORMAT_CHECK_RELAX     false
#define INSTR_CONFIG_CHECK_RELAX     false
#define SNAPSHOT_USE_PHV_TIME        false
#define SNAPSHOT_MASK_THREAD_FIELDS  false
#define PRE_PFE_ADDR_CHECK_RELAX     false
#define SALU_PRED_RSI_CHECK_RELAX    true
#define SYNTH2PORT_CHECK_RELAX       false
#define DEPENDENCY_CHECK_RELAX       false
#define PARSE_CHECK_RELAX            false
#define SYNC_SALU_CNTR_CLEAR         true
#define SALU_CNTR_TICK_CHECK_TIME    false
#define RESET_UNUSED_LOOKUP_RESULTS  false
#define SET_NEXT_TABLE_PRED          false
#define METER_EXPONENT_CHECK_RELAX   false
#define METER_SWEEP_ON_DEMAND        true
#define CHIP_USE_GLOBAL_TIME_IF_ZERO true
#define CLOT_OVERLAP_CHECK_RELAX     true
#define CLOT_ALLOW_ADJACENT          true
#define CLOT_ALLOW_DUPLICATE         false
#define CLOT_ALLOW_REPEAT_EMIT       false

#endif /* RTL_MODE */




#endif // _RMT_FEATURES_
