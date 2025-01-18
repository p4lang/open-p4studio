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

#ifndef _SHARED_RMT_REG_VERSION_H_
#define _SHARED_RMT_REG_VERSION_H_

#include <register_includes/generated-versions.h>
#ifndef BFNREGS_TAG
#define BFNREGS_TAG "000"
#endif

#include <model_core/rmt-version.h>
#ifndef RMT_VERSION
#define RMT_VERSION "VER"
#endif
#ifndef RMT_VERSION_LONG
#define RMT_VERSION_LONG "VERSION"
#endif

#include <common/rmt-features.h>


#define RMT_REG_VERSION        BFNREGS_TAG

#ifdef BFN_INTERNAL
#define RMT_CHIP_VERSION       RMT_VERSION ":" RMT_REG_VERSION
#define RMT_CHIP_VERSION_LONG  RMT_VERSION_LONG ":" RMT_REG_VERSION

#else
#define RMT_CHIP_VERSION       RMT_VERSION
#define RMT_CHIP_VERSION_LONG  RMT_VERSION_LONG

#endif


#endif // _SHARED_RMT_REG_VERSION_
