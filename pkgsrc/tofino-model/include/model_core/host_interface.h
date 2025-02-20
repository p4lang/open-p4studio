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

#ifndef _MODEL_CORE_HOST_INTERFACE__
#define _MODEL_CORE_HOST_INTERFACE__

#ifdef USE_DEFUNCT_HOST_INTERFACE_H
/* This file now defunct - use model.h directly instead */
#include <model.h>

#ifdef __REF_MODEL_WRAPPER_HPP__
/* TODO:WRAP: REMOVE: remove this file and refs to it.
 * Some hacks to allow ref_model_wrapper.cpp to compile
 * Can remove this file entirely once wrapper code updated
 * Also will be able to remove extra tokens and defines
 * from rmt-defs-shared.h
 */
#define GLOBAL_HOST_INTERFACE GLOBAL_MODEL

namespace tofino { }
namespace tofino_rmt = tofino;
namespace model_core {
  typedef Model HostInterface;
}
using model_core::HostInterface;

#endif

#endif

#endif
