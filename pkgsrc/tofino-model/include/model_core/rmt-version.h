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

#ifndef _MODEL_CORE_RMT_VERSION_H_
#define _MODEL_CORE_RMT_VERSION_H_

#define RMT_REV "1.1.0"

#include <model_core/build-version.h>
#ifndef BUILD_NUM
#define BUILD_NUM "00000"
#endif

#include <model_core/model-version.h>
#ifndef MODEL_TAG
#define MODEL_TAG "000"
#endif


/* Versioning */
#define RMT_CODE_VERSION  RMT_REV
#define RMT_BUILD_VERSION BUILD_NUM
#define RMT_MODEL_TAG     MODEL_TAG
#define RMT_VERSION       RMT_MODEL_TAG
#define RMT_VERSION_LONG  "Tofino Verification Model - Version " RMT_VERSION


#endif // _MODEL_CORE_RMT_VERSION_H_
