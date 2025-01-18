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

// MauOpHandler - Tofino/TofinoB0
// In shared/ because identical across these chips

#ifndef _TOFINOXX_MAU_OP_HANDLER_
#define _TOFINOXX_MAU_OP_HANDLER_

#include <mau-op-handler-common.h>


namespace MODEL_CHIP_NAMESPACE {

  class MauOpHandler : public MauOpHandlerCommon {

 public:
    MauOpHandler(RmtObjectManager *om, int pipeIndex, int mauIndex, Mau *mau);
    virtual ~MauOpHandler();

    void instr_handle_perchip(int instr, int data_size,
                              uint64_t data0, uint64_t data1, uint64_t T);
  };
}

#endif // _TOFINOXX_MAU_OP_HANDLER_
