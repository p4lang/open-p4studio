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

#include <string>
#include <memory>
#include <common/rmt-assert.h>
#include <rmt-log.h>
#include <rmt-object-manager.h>
#include <phv-factory.h>


namespace MODEL_CHIP_NAMESPACE {

  PhvFactory::PhvFactory(RmtObjectManager *om) : RmtObject(om) {
  }
  PhvFactory::~PhvFactory() { }

  Phv *PhvFactory::phv_create() {
    RmtObjectManager *om = get_object_manager();
    Phv *phv = new Phv(om, this);
    if (phv == NULL) return phv;
    
    if (kPhvInitTimeRandOnAlloc) {
      // If no sweep yet happened T could still be 0.
      // In that case make it 1
      uint64_t T = om->time_get_cycles();
      if (T == UINT64_C(0)) T = UINT64_C(1);
      phv->setup_time_info(T);
    }
    if (kPhvInitAllValid) {
      // Set all PHV words valid, in which case...
      phv->set_all_valid();
      // ...need to track writes to catch no-multi-write
      phv->start_recording_written();
    }
    return phv;
  }
  void PhvFactory::phv_delete(Phv *phv) {
    phv->destroy();
    delete phv;
  }

}
