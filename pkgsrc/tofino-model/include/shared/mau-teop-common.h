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

#ifndef _SHARED_MAU_TEOP_COMMON_
#define _SHARED_MAU_TEOP_COMMON_

#include <rmt-defs.h>
#include <mau-defs.h>
#include <mau-object.h>
#include <teop.h>


namespace MODEL_CHIP_NAMESPACE {

  class MauTeopCommon : public MauObject {

 public:
    //static bool kRelaxTeopCheck; // Defined in rmt-config.cpp

    static constexpr int kType = RmtTypes::kRmtTypeMauTeop;

    MauTeopCommon(RmtObjectManager *om, int pipeIndex, int mauIndex, Mau *mau);
    virtual ~MauTeopCommon();

    // PER-CHIP implementation
    virtual bool  teop_available() = 0;
    virtual Teop *teop_allocate() = 0;
    virtual void  teop_free(Teop *teop) = 0;
    virtual bool  teop_being_used(int lt) = 0;
    virtual bool  teop_being_used() = 0;
    virtual bool  teop_enabled(int s_or_m, int alu) = 0;
    virtual bool  teop_input_meter_addr(const Teop &teop, int alu,
                                        uint32_t *meter_addr, int *lt) = 0;
    virtual bool  teop_input_stats_addr(const Teop &teop, int alu,
                                        uint32_t *stats_addr, int *lt) = 0;
    virtual void  teop_output_meter_addr(Teop *teop, int lt, bool ingress,
                                         int alu, uint32_t meter_addr) = 0;
    virtual void  teop_output_stats_addr(Teop *teop, int lt, bool ingress,
                                         int alu, uint32_t stats_addr) = 0;
  };

}

#endif // _SHARED_MAU_TEOP_COMMON_

