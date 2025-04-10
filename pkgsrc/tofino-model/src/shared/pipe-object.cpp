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
#include <rmt-log.h>
#include <rmt-object-manager.h>
#include <pipe-object.h>
#include <port.h>


namespace MODEL_CHIP_NAMESPACE {

  PipeObject::PipeObject(RmtObjectManager *om, int pipeIndex)
      : RmtObject(om), RmtLogger(om,0x3F), RmtEventEmitter(om, 0),
        spinlock_(), pipe_index_(pipeIndex), stage_index_(0x3F),
        row_tab_index_(0x3F), col_index_(0x3F), version_(0), enabled_(true),
        pipe_(NULL) {
  }
  PipeObject::PipeObject(RmtObjectManager *om, int pipeIndex, Pipe *pipe)
      : RmtObject(om), RmtLogger(om,0x3F), RmtEventEmitter(om, 0),
        spinlock_(), pipe_index_(pipeIndex), stage_index_(0x3F),
        row_tab_index_(0x3F), col_index_(0x3F), version_(0), enabled_(true),
        pipe_(pipe) {
  }
  PipeObject::PipeObject(RmtObjectManager *om, int pipeIndex, int stageIndex,
                         int typeIndex)
      : RmtObject(om), RmtLogger(om,typeIndex), RmtEventEmitter(om, typeIndex),
        spinlock_(), pipe_index_(pipeIndex), stage_index_(stageIndex),
        row_tab_index_(0x3F), col_index_(0x3F), version_(0), enabled_(true),
        pipe_(NULL) {
  }
  PipeObject::PipeObject(RmtObjectManager *om, int pipeIndex, int stageIndex,
                         int typeIndex, int rowtabIndex, int colIndex)
      : RmtObject(om), RmtLogger(om,typeIndex), RmtEventEmitter(om, typeIndex),
        spinlock_(), pipe_index_(pipeIndex), stage_index_(stageIndex),
        row_tab_index_(rowtabIndex), col_index_(colIndex), version_(0),
        enabled_(true), pipe_(NULL) {
  }
  PipeObject::~PipeObject() {
    enabled_ = false;
  }

  /**
   * Checks that the given port index maps to this object's pipe and returns
   * the port index relative to the pipe.
   * @param portIndex A port index
   * @return The pipe relative port index
   */
  int PipeObject::get_local_port_index(int portIndex) {
    RMT_ASSERT(Port::get_pipe_num(portIndex) == pipe_index());
    return Port::get_pipe_local_port_index(portIndex);
  }
}
