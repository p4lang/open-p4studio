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

#include <model_core/model.h>
#include <rmt-defs.h>
#include <rmt-object.h>
#include <rmt-object-manager.h>

namespace MODEL_CHIP_NAMESPACE {

  RmtObject::RmtObject(RmtObjectManager *om) : om_(om) {
  }
  RmtObject::~RmtObject() {
  }
  RmtObjectManager *RmtObject::get_object_manager() const {
    return om_;
  }
  int RmtObject::chip_index() const {
    return get_object_manager()->chip_index();
  }

}
