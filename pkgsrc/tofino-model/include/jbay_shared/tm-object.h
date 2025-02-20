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

#ifndef __JBAY_SHARED_TM_OBJECT__
#define __JBAY_SHARED_TM_OBJECT__

#include <rmt-object-manager.h>
#include <rmt-log.h>

namespace MODEL_CHIP_NAMESPACE {

class TmObject : public DefaultLogger {

  public:
    TmObject(RmtObjectManager *om, uint8_t pipe_index);

    virtual uint8_t get_tm_index() const { return m_pipe_index_;  }

  private:
    uint8_t              m_pipe_index_;
};
}
#endif
