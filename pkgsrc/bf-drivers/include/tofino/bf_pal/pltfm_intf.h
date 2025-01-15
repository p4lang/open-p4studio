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


#ifndef _TOFINO_BF_PAL_PLATFORM_INTF_H
#define _TOFINO_BF_PAL_PLATFORM_INTF_H

#include <bf_types/bf_types.h>

/**
 * @brief Get the type of the device (model or asic)
 * @param[in] dev_id Device id
 * @param[out] is_sw_model Pointer to bool flag to return true for model and
                      false for asic devices
 * @return Status of the API call
 */
bf_status_t bf_pal_pltfm_type_get(bf_dev_id_t dev_id, bool *is_sw_model);

#endif
