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

#ifndef _SHARED_MAU_DELAY_
#define _SHARED_MAU_DELAY_

namespace MODEL_CHIP_NAMESPACE {

class MauDelay {
 public:
  static constexpr int kBase              =  1;
  static constexpr int kBasePredication   =  2;
  static constexpr int kPredication       =  3;
  static constexpr int kTotalAdded        =  4;
  static constexpr int kPipeLatency       =  5;
  static constexpr int kPostPredication   =  6;
  static constexpr int kTcam              =  7;
  static constexpr int kSelector          =  8;
  static constexpr int kMeterLpf          =  9;
  static constexpr int kStateful          = 10;
  static constexpr int kAlu               = 11;
  static constexpr int kSeriesTcam        = 12;
  static constexpr int kSeriesSelector    = 13;
  static constexpr int kSeriesMeterLpf    = 14;
  static constexpr int kSeriesStateful    = 15;
  static constexpr int kSeriesAlu         = 16;
  // Raw values before precedence applied
  static constexpr int kTcamRaw           = 17;
  static constexpr int kSelectorRaw       = 18;
  static constexpr int kMeterLpfRaw       = 19;
  static constexpr int kStatefulRaw       = 20;
  static constexpr int kSeriesTcamRaw     = 21;
  static constexpr int kSeriesSelectorRaw = 22;
  static constexpr int kSeriesMeterLpfRaw = 23;
  static constexpr int kSeriesStatefulRaw = 24;

  static int  base();
  static int  base_predication();
  // These can be called with stage features or series features
  static int  tcam_raw(uint32_t ftrs);
  static int  selector_raw(uint32_t ftrs);
  static int  meter_lpf_raw(uint32_t ftrs);
  static int  stateful_raw(uint32_t ftrs);
  static void apply_precedence(int *d_tcam, int *d_sel, int *d_lpf, int *d_stfl);
  static void all_raw(uint32_t ftrs, int *d_tcam, int *d_sel, int *d_mlpf, int *d_stfl);
  static int  tcam(uint32_t ftrs);
  static int  selector(uint32_t ftrs);
  static int  meter_lpf(uint32_t ftrs);
  static int  stateful(uint32_t ftrs);
  static int  alu(uint32_t ftrs);
  static int  predication(uint32_t ftrs);
  static int  total_added(uint32_t ftrs);
  static int  pipe_latency(uint32_t ftrs);
  static int  post_predication(uint32_t ftrs);
  // These must be called with series features and stage features
  static int  get_delay(int which_delay, uint32_t series_ftrs, uint32_t stage_ftrs);
  static int  stage_deferred_eop_bus_internal(uint32_t series_ftrs, uint32_t stage_ftrs);
  static int  stage_deferred_eop_bus_output(uint32_t series_ftrs, uint32_t stage_ftrs);
  static int  stage_start_table_fifo0(uint32_t series_ftrs, uint32_t stage_ftrs,
                                      int prev_post_pred_delay);
  static int  stage_action_output(uint32_t series_ftrs);
  // These must be called with ALU, series features and MeterALU features
  static int  meter_alu_group_action_common(int alu, uint32_t series_ftrs, uint32_t alu_ftrs);
  static int  meter_alu_group_action_right(int alu, uint32_t series_ftrs, uint32_t alu_ftrs);
  static int  meter_alu_group_action_left(int alu, uint32_t series_ftrs, uint32_t alu_ftrs);
  static int  meter_alu_group_data(int alu, uint32_t series_ftrs, uint32_t alu_ftrs);

  MauDelay()  { }
  ~MauDelay() { }
};

}
#endif // _SHARED_MAU_DELAY_
