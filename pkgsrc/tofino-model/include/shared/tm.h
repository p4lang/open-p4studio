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

#ifndef _SHARED_TM_H_
#define _SHARED_TM_H_

#include <rmt-defs.h>
#include <rmt-object-manager.h>
#include <rmt-object.h>
#include <common/rmt-util.h>

namespace MODEL_CHIP_NAMESPACE {

class TMWac;

class TM : public RmtObject {
 public:
  TM(RmtObjectManager *om);
  virtual ~TM();
  void Reset();
  TMWac* get_wac() const;

 private:
  DISALLOW_COPY_AND_ASSIGN(TM);
  TMWac *wac_;
};


}
#endif //_SHARED_TM_H_
