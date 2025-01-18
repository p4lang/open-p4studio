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

// MauPredication - Tofino/TofinoB0
// In shared/ because identical across these chips

#ifndef _TOFINOXX_MAU_PREDICATION_
#define _TOFINOXX_MAU_PREDICATION_

#include <mau-predication-common.h>

namespace MODEL_CHIP_NAMESPACE {

  class MauPredication : public MauPredicationCommon {

 private:
    uint16_t get_lookup_mask(bool ing, int start_tab);
    uint16_t get_active_mask(bool ing, int nxt_tab);

 public:
    MauPredication(RmtObjectManager *om, int pipeIndex, int mauIndex, Mau *mau);
    virtual ~MauPredication();

    void start(bool thread_active[]);
    void end();
    int  get_next_table(bool ingress, int curr_lt, bool *do_lookup);
    int  get_first_table(bool ingress, bool *do_lookup);
    void set_next_table(int lt, const MauLookupResult &result);
    uint16_t lt_info(uint16_t pred_sel);
    uint16_t lts_active();

 private:
    Mau       *mau_;
    uint16_t   lt_ingress_;     // Ingress
    uint16_t   lt_egress_;      // Egress
    uint16_t   lt_counters_;    // With counters
    uint16_t   lt_countable_;   // With counters - ON given thread_active
    uint16_t   lt_lookupable_;  // ON given thread active and ing/egr nxt_tab
    uint16_t   lt_runnable_;    // Lookupable OR countable
    uint16_t   lt_active_;      // Initial nxt_tab LT and all subsequent nxt_tab LTs
    uint16_t   lt_warn_;        // LTs to complain about
    std::array< uint16_t, kThreads >  next_table_;  // Output nxt_tab vals

  };

}

#endif // _TOFINOXX_MAU_PREDICATION_
