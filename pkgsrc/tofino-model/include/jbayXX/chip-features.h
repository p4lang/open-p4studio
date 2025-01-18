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

#ifndef _JBAYXX_CHIP_FEATURES_H_
#define _JBAYXX_CHIP_FEATURES_H_

#define MAU_FEATURES_0_SPECIAL              0x74u
#ifdef DV_MODE
#define MAU_FEATURES_0                      0x00u
#else
#define MAU_FEATURES_0                      0x74u
#endif
#define MAU_FEATURES_6                      0x00u
#define HAS_TAGALONG_PHV                    false
#define INSTR_COND_MOVE_IS_UNCONDITIONAL    true
#define INSTR_COND_MUX_IS_COND_MOVE         true
#define INSTR_INVALIDATE_IS_NOP             false
#define INSTR_INVALIDATE_IS_ERR             true
#define INSTR_PAIRDPF_IS_ERR                true
#define INSTR_DATADEP_SHIFT_SUPPORTED       true
#define INSTR_REVERSE_SUBTRACT_SUPPORTED    false
#define PHV_INIT_ALL_VALID                  true
#define SNAPSHOT_COMPARE_VALID_BIT          false
#define SNAPSHOT_CAPTURE_VALID_BIT          false
#define MEMORY_CORE_SPLIT                   true
#define CHIP_USE_MUTEX                      true
#define LOOKUP_UNUSED_LTCAMS                false
#define RELAX_DEPARSER_CLOT_LEN_CHECKS      true

#define BUG_ACTION_DATA_BUS_MASK_BEFORE_DUP false

#endif // _JBAYXX_CHIP_FEATURES_H_
