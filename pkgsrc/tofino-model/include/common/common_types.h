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

/* @fixme top matter */

#ifndef _COMMON_TYPES_H_
#define _COMMON_TYPES_H_

#define COMPILER_REFERENCE(ref) (void) (ref)

typedef enum bfm_error_s {
    BFM_E_NONE=0,
    BFM_E_PARAM=-1,
    BFM_E_EXISTS=-2,
    BFM_E_UNKNOWN=-3,
    BFM_E_NOT_SUPPORTED=-4,
    BFM_E_NOT_FOUND=-5
} bfm_error_t;

#endif /* _COMMON_TYPES_H_ */
