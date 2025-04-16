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

#ifndef _UTESTS_LPF_UTIL_
#define _UTESTS_LPF_UTIL_

#include <model_core/model.h>
#include <ipb.h>
#include <bitvector.h>
#include <rmt-object-manager.h>
#include <mau.h>
#include <packet.h>
#include "meter_util.h"

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;

namespace MODEL_CHIP_TEST_NAMESPACE {

struct LpfEntry {
  uint64_t timestamp=0;
  uint64_t rate_enable=0; // otherwise sampled
  uint64_t v_old=0;
  uint64_t lpf_action_scale=0;
  uint64_t time_constant_mantissa=0;
  uint64_t rise_time_constant_exponent=0;
  uint64_t fall_time_constant_exponent=0;
  uint64_t red_probablility_scale=0;
  uint64_t red_level_exponent=0;
  uint64_t red_level_max=0;
  uint64_t red_dlevel_100=0;
  uint64_t red_level_0=0;

  LpfEntry() {};
  LpfEntry(uint64_t data0, uint64_t data1) { set_from(data0,data1); }

  uint64_t get_data1() {
    return (((timestamp & UINT64_C(0xfffffff)) << (100-64) ) |
            ((rate_enable & 1 ) << (99-64) ) |
            ((v_old & UINT64_C(0xffffffff)) << (64-64)) );
  }
  uint64_t get_data0() {
    return
        ((lpf_action_scale & 0x1f) << 57  ) |
        ((time_constant_mantissa & 0x1ff) << 43  ) |
        ((rise_time_constant_exponent & 0x1f) << 38  ) |
        ((fall_time_constant_exponent & 0x1f) << 33  ) |
        ((red_probablility_scale & 0x7) << 29  ) |
        ((red_level_exponent & 0x1f) << 24  ) |
        ((red_level_max & 0xff) << 16  ) |
        ((red_dlevel_100 & 0xff) << 8  ) |
        ((red_level_0 & 0xff) << 0  );
  }
  void set_from(uint64_t data0, uint64_t data1) {
    timestamp = (data1>> (100-64)  ) & UINT64_C(0xfffffff);
    rate_enable = (data1 >> (99-64)) & 1;
    v_old = (data1 >> ( 64-64)) & UINT64_C(0xffffffff);
    lpf_action_scale = (data0 >> 57) & 0x1f;
    time_constant_mantissa = (data0 >> 43) & 0x1ff;
    rise_time_constant_exponent = (data0 >> 38) & 0x1f;
    fall_time_constant_exponent = (data0 >> 33) & 0x1f;
    red_probablility_scale = (data0 >> 29) & 0x7;
    red_level_exponent = (data0 >> 24) & 0x1f;
    red_level_max = (data0 >> 16) & 0xff;
    red_dlevel_100 = (data0 >> 8 ) & 0xff;
    red_level_0 = (data0 >> 0) & 0xff;

  }

  // TODO: what parameters to set from?
  //void set_from_parameters( ) {}
};

struct LpfEvent {
  double time;
  double input;
  double output;
};

void lpf_time_constant_to_mantissa_exponent( double tc, int *mantissa, int *exponent );

double mantissa_exponent_to_lpf_time_constant( int mantissa, int exponent);

bool result_within_tolerance_limit(uint64_t result, uint64_t expected_result,
                                   int tolerance_limit_pcent);

#define LPF_TOLERANCE_LIMIT 10  /* 10 % */

void run_lpf_test( TestUtil& tu, int pipe,int stage,int row, int entry,
                   std::vector<LpfEvent> const& events, bool logging_on);

}

#endif
