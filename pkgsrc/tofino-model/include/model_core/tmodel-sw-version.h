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

#ifndef TMODEL_SW_VERSION_H_
#define TMODEL_SW_VERSION_H_

#include <model_core/tmodel-sw-bld-version.h>
#include <model_core/rmt-version.h>

extern "C" {

/* Versioning for SW and customers */
#define TMODEL_SW_REV "9.13.4"
#define TMODEL_SW_VER TMODEL_SW_REV "-" TMODEL_BLD_VER
#define TMODEL_SW_INTERNAL_VER TMODEL_SW_VER "(" RMT_REV "-" TMODEL_GIT_VER ")"

}

#endif // TMODEL_SW_VERSION_H_
