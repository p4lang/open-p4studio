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


#ifndef _TOFINO_BF_PAL_PLATFORM_FUNC_MGR_H
#define _TOFINO_BF_PAL_PLATFORM_FUNC_MGR_H

#include <bf_types/bf_types.h>

typedef bf_status_t (*bf_pal_pltfm_type_get_fn)(bf_dev_id_t dev_id,
                                                bool *is_sw_model);

typedef struct pltfm_func_s {
  bf_pal_pltfm_type_get_fn pltfm_type_get; /* Device type (model or asic) */
} pltfm_func_s;

/**
 * @brief Get pointer to the function to get the device type (model or asic)
 * @param pltfm_type_get_fn Pointer to the function to get the device type
 * @return Status of the API call
 */
bf_status_t bf_pal_pltfm_type_get_fn_get(
    bf_pal_pltfm_type_get_fn *pltfm_type_get_fn);

/**
 * @brief Register all appropriate platform related functions with the
 * corresponding function pointers encapsulated in a structure
 * @param all_func Pointer to the structure containing all the function pointers
 * @return Status of the API call
 */
bf_status_t bf_pal_pltfm_fn_reg(pltfm_func_s *all_func);

/**
 * @brief Get all the platform function pointers in a structure populated with
 * the registered function pointers
 * @param all_func Pointer to the structure containing all the function pointers
 * @return Status of the API call
 */
bf_status_t bf_pal_pltfm_fn_get(pltfm_func_s *all_func);

#endif
