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

#include <mau-defs.h>
#include <mau-delay.h>
#include <rmt-object.h>

namespace MODEL_CHIP_NAMESPACE {

int MauDelay::base() {
  static_assert( (MauDefs::kMauBaseDelay > MauDefs::kMauBasePredicationDelay),
                 "Invalid BaseDelay or PredicationDelay");
  return MauDefs::kMauBaseDelay;
}
int MauDelay::base_predication() {
  return MauDefs::kMauBasePredicationDelay;
}

int MauDelay::tcam_raw(uint32_t ftrs) {
  if (MauDefs::tcam_present(ftrs)) return MauDefs::kMauTcamExtraDelay;
  return 0;
}
int MauDelay::selector_raw(uint32_t ftrs) {
  if (MauDefs::selector_present(ftrs) || MauDefs::stateful_qlag(ftrs))
    return MauDefs::kMauMeterAluSelectorExtraDelay;
  return 0;
}
int MauDelay::meter_lpf_raw(uint32_t ftrs) {
  if (MauDefs::meter_lpf_present(ftrs)) return MauDefs::kMauMeterAluMeterLpfExtraDelay;
  return 0;
}
int MauDelay::stateful_raw(uint32_t ftrs) {
  int delay = 0;
  if (MauDefs::stateful_present(ftrs)) {
    delay = MauDefs::kMauMeterAluStatefulExtraDelay;
    if (MauDefs::divide_used(ftrs))
      delay += MauDefs::kMauMeterAluStatefulDivideExtraDelay;
  }
  return delay;
}
void MauDelay::apply_precedence(int *d_tcam, int *d_sel, int *d_lpf, int *d_stfl) {
  // From uArch doc Section 3.2
  if (*d_sel > 0) {
    // if selector tables are present, then stateful/meter LPF tables
    // do not add additional latency
    *d_lpf = *d_stfl = 0;
  } else {
    if (RmtObject::is_jbay_or_later()) {
      // if stateful tables are present, then meter LPF tables
      // do not add additional latency
      if (*d_stfl > 0) *d_lpf = 0;
    } else {
      // if meter LPF tables are present, then stateful tables
      // do not add additional latency
      if (*d_lpf > 0) *d_stfl = 0;
    }
  }
}
void MauDelay::all_raw(uint32_t ftrs, int *d_tcam, int *d_sel, int *d_mlpf, int *d_stfl) {
  *d_tcam = tcam_raw(ftrs);
  *d_sel = selector_raw(ftrs);
  *d_mlpf = meter_lpf_raw(ftrs);
  *d_stfl = stateful_raw(ftrs);
  apply_precedence(d_tcam, d_sel, d_mlpf, d_stfl);
}

int MauDelay::tcam(uint32_t ftrs) {
  int d_tcam = 0, d_sel = 0, d_mlpf = 0, d_stfl = 0;
  all_raw(ftrs, &d_tcam, &d_sel, &d_mlpf, &d_stfl);
  apply_precedence(&d_tcam, &d_sel, &d_mlpf, &d_stfl);
  return d_tcam;
}
int MauDelay::selector(uint32_t ftrs) {
  int d_tcam = 0, d_sel = 0, d_mlpf = 0, d_stfl = 0;
  all_raw(ftrs, &d_tcam, &d_sel, &d_mlpf, &d_stfl);
  apply_precedence(&d_tcam, &d_sel, &d_mlpf, &d_stfl);
  return d_sel;
}
int MauDelay::meter_lpf(uint32_t ftrs) {
  int d_tcam = 0, d_sel = 0, d_mlpf = 0, d_stfl = 0;
  all_raw(ftrs, &d_tcam, &d_sel, &d_mlpf, &d_stfl);
  apply_precedence(&d_tcam, &d_sel, &d_mlpf, &d_stfl);
  return d_mlpf;
}
int MauDelay::stateful(uint32_t ftrs) {
  int d_tcam = 0, d_sel = 0, d_mlpf = 0, d_stfl = 0;
  all_raw(ftrs, &d_tcam, &d_sel, &d_mlpf, &d_stfl);
  apply_precedence(&d_tcam, &d_sel, &d_mlpf, &d_stfl);
  return d_stfl;
}
int MauDelay::alu(uint32_t ftrs) {
  int d_tcam = 0, d_sel = 0, d_mlpf = 0, d_stfl = 0;
  all_raw(ftrs, &d_tcam, &d_sel, &d_mlpf, &d_stfl);
  apply_precedence(&d_tcam, &d_sel, &d_mlpf, &d_stfl);
  return d_sel + d_mlpf + d_stfl;
}

int MauDelay::predication(uint32_t ftrs) {
  return base_predication() + tcam(ftrs);
}
int MauDelay::total_added(uint32_t ftrs) {
  return tcam(ftrs) + alu(ftrs);
}
int MauDelay::pipe_latency(uint32_t ftrs) {
  return base() + tcam(ftrs) + alu(ftrs);
}
int MauDelay::post_predication(uint32_t ftrs) {
  return base() - base_predication() + alu(ftrs);
}


int MauDelay::get_delay(int which_delay, uint32_t series_ftrs, uint32_t stage_ftrs) {
  switch (which_delay) {
      // Fixed delays (but chip-specific)
    case kBase:              return base();
    case kBasePredication:   return base_predication();
      // Stage-specific raw delays
    case kTcamRaw:           return tcam_raw(stage_ftrs);
    case kSelectorRaw:       return selector_raw(stage_ftrs);
    case kMeterLpfRaw:       return meter_lpf_raw(stage_ftrs);
    case kStatefulRaw:       return stateful_raw(stage_ftrs);
      // Series-specific raw delays
    case kSeriesTcamRaw:     return tcam_raw(series_ftrs);
    case kSeriesSelectorRaw: return selector_raw(series_ftrs);
    case kSeriesMeterLpfRaw: return meter_lpf_raw(series_ftrs);
    case kSeriesStatefulRaw: return stateful_raw(series_ftrs);
      // Stage-specific delays post precedence application
    case kTcam:              return tcam(stage_ftrs);
    case kSelector:          return selector(stage_ftrs);
    case kMeterLpf:          return meter_lpf(stage_ftrs);
    case kStateful:          return stateful(stage_ftrs);
    case kAlu:               return alu(stage_ftrs);
      // Series delays (also post precedence)
    case kSeriesTcam:        return tcam(series_ftrs);
    case kSeriesSelector:    return selector(series_ftrs);
    case kSeriesMeterLpf:    return meter_lpf(series_ftrs);
    case kSeriesStateful:    return stateful(series_ftrs);
    case kSeriesAlu:         return alu(series_ftrs);
      // Calculated composite delays (always series delays)
    case kPredication:       return predication(series_ftrs);
    case kTotalAdded:        return total_added(series_ftrs);
    case kPipeLatency:       return pipe_latency(series_ftrs);
    case kPostPredication:   return post_predication(series_ftrs);
      // Anything else return invalid value
    default:                 return 0xFF;
  }
}


// SPECIFIC CSR DELAYS - stage CSRs

int MauDelay::stage_deferred_eop_bus_internal(uint32_t series_ftrs, uint32_t stage_ftrs) {
  return predication(series_ftrs) + MauDefs::kDeferredEopBusInternalExtraDelay;
}
int MauDelay::stage_deferred_eop_bus_output(uint32_t series_ftrs, uint32_t stage_ftrs) {
  if (MauDefs::next_stage_match_dep(stage_ftrs)) {
    return pipe_latency(series_ftrs) + MauDefs::kDeferredEopBusOutputMatchDepExtraDelay;
  } else if (MauDefs::next_stage_action_dep(stage_ftrs)) {
    return MauDefs::kDeferredEopBusOutputActionDepDelay;
  } else {
    return 0;
  }
}

int MauDelay::stage_start_table_fifo0(uint32_t series_ftrs, uint32_t stage_ftrs,
                                      int prev_post_pred_delay) {
  if (MauDefs::curr_stage_match_dep(stage_ftrs)) {
    int pred = predication(series_ftrs);
    if (MauDefs::curr_stage_is_mau0(stage_ftrs)) {
      return pred + MauDefs::kMauMatchDepStartTableFifo0ExtraDelayMau0;
    } else {
      return prev_post_pred_delay + pred + MauDefs::kMauMatchDepStartTableFifo0ExtraDelayMauN;
    }
  } else if (MauDefs::curr_stage_action_dep(stage_ftrs)) {
    return MauDefs::kMauActionDepStartTableFifo0Delay;
  } else {
    return 0;
  }
}

int MauDelay::stage_action_output(uint32_t series_ftrs) {
  return pipe_latency(series_ftrs) - 3;
}



// SPECIFIC CSR DELAYS - MeterALU CSRs

int MauDelay::meter_alu_group_action_common(int alu, uint32_t series_ftrs, uint32_t alu_ftrs) {
  int exp = 0;
  // Delays here depend on features of individual ALU as well as series features
  if (MauDefs::stateful_divide(series_ftrs)) {
    if (MauDefs::selector_present(series_ftrs)) {
      exp = MauDefs::kMeterAluGroupActDelay_Sel_Div;
      if (MauDefs::divide_used(alu_ftrs)) exp = MauDefs::kMeterAluGroupActDelay_Sel_DivEn;
    } else {
      exp = MauDefs::kMeterAluGroupActDelay_Div;
      if (MauDefs::divide_used(alu_ftrs)) exp = MauDefs::kMeterAluGroupActDelay_DivEn;
    }
  } else {
    exp = MauDefs::kMeterAluGroupActDelay;
    if (MauDefs::selector_present(series_ftrs)) exp = MauDefs::kMeterAluGroupActDelay_Sel;
  }
  return exp;
}
int MauDelay::meter_alu_group_action_right(int alu, uint32_t series_ftrs, uint32_t alu_ftrs) {
  int exp = 0;
  if (MauDefs::right_action_override_used(alu_ftrs)) {
    exp = MauDefs::kMeterAluGroupActDelay_Adb;
    if (MauDefs::selector_present(series_ftrs)) exp = MauDefs::kMeterAluGroupActDelay_Sel_Adb;
  } else {
    exp = meter_alu_group_action_common(alu, series_ftrs, alu_ftrs);
  }
  return exp;
}
int MauDelay::meter_alu_group_action_left(int alu, uint32_t series_ftrs, uint32_t alu_ftrs) {
  return meter_alu_group_action_common(alu, series_ftrs, alu_ftrs);
}

int MauDelay::meter_alu_group_data(int alu, uint32_t series_ftrs, uint32_t alu_ftrs) {
  int bot_half_subtract = (alu < static_cast<int>(MauDefs::kNumMeterAlus/2)) ?1 :0;
  return MauDefs::kMeterAluGroupDataDelayBase + tcam_raw(series_ftrs) - bot_half_subtract;
}


}
