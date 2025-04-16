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

#ifndef _TOFINOB0_CHIP_FEATURES_H_
#define _TOFINOB0_CHIP_FEATURES_H_

#define MAU_FEATURES_0_SPECIAL              0x0Fu
#ifdef DV_MODE
#define MAU_FEATURES_0                      0x00u
#else
#define MAU_FEATURES_0                      0x0Fu
#endif
#define MAU_FEATURES_6                      0x04u
#define HAS_TAGALONG_PHV                    true
#define INSTR_COND_MOVE_IS_UNCONDITIONAL    false
#define INSTR_COND_MUX_IS_COND_MOVE         false
#define INSTR_INVALIDATE_IS_NOP             false
#define INSTR_INVALIDATE_IS_ERR             false
#define INSTR_PAIRDPF_IS_ERR                false
#define INSTR_DATADEP_SHIFT_SUPPORTED       false
#define INSTR_REVERSE_SUBTRACT_SUPPORTED    false
#define PHV_INIT_ALL_VALID                  false
#define SNAPSHOT_COMPARE_VALID_BIT          true
#define SNAPSHOT_CAPTURE_VALID_BIT          true
#define MEMORY_CORE_SPLIT                   false
#define CHIP_USE_MUTEX                      false
#define LOOKUP_UNUSED_LTCAMS                false
#define RELAX_DEPARSER_CLOT_LEN_CHECKS      false

#define BUG_ACTION_DATA_BUS_MASK_BEFORE_DUP false

#endif // _TOFINOB0_CHIP_FEATURES_H_
