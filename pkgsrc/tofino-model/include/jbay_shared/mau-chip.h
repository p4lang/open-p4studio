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

#ifndef _JBAY_SHARED_MAU_CHIP_
#define _JBAY_SHARED_MAU_CHIP_

// Chip specific mau.h code

#include <rmt-defs.h>
#include <mau-defs.h>

// Reg defs auto-generated from Semifore
#include <register_includes/stage_dump_ctl.h>


namespace MODEL_CHIP_NAMESPACE {

class MauChip {

public:
  MauChip(int chipIndex, int pipeIndex, int mauIndex, Mau *mau)
      : stage_dump_ctl_(default_adapter(stage_dump_ctl_, chipIndex, pipeIndex, mauIndex))
  {
    stage_dump_ctl_.reset();
  }
  ~MauChip() { }

  int  pipe_dump_index()  { return stage_dump_ctl_.stage_dump_ctl_pipe(); }
  int  mau_dump_index()   { return stage_dump_ctl_.stage_dump_ctl_stage(); }

 private:
  register_classes::StageDumpCtl  stage_dump_ctl_;
};


}

#endif // _JBAY_SHARED_MAU_CHIP_
