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

// MauTeop - Tofino/TofinoB0
// In shared/ because identical across these chips

#ifndef _TOFINOXX_MAU_TEOP_
#define _TOFINOXX_MAU_TEOP_

#include <mau-teop-common.h>

namespace MODEL_CHIP_NAMESPACE {

  class MauTeop : public MauTeopCommon {

 public:
    MauTeop(RmtObjectManager *om, int pipeIndex, int mauIndex, Mau *mau);
    virtual ~MauTeop();

    inline bool  teop_available()        { return false; }
    inline Teop *teop_allocate()         { return NULL; }
    inline void  teop_free(Teop *teop)   { }
    inline bool  teop_being_used(int lt) { return false; }
    inline bool  teop_being_used()       { return false; }
    inline bool  teop_enabled(int s_or_m, int alu) { return false; }
    inline bool  teop_input_meter_addr(const Teop &teop, int alu,
                                       uint32_t *meter_addr, int *lt) { return false; }
    inline bool  teop_input_stats_addr(const Teop &teop, int alu,
                                       uint32_t *stats_addr, int *lt) { return false; }
    inline void  teop_output_meter_addr(Teop *teop, int lt, bool ingress,
                                        int alu, uint32_t meter_addr) { }
    inline void  teop_output_stats_addr(Teop *teop, int lt, bool ingress,
                                        int alu, uint32_t stats_addr) { }

 private:
    Mau  *mau_;

  };

}

#endif // _TOFINOXX_MAU_TEOP_
