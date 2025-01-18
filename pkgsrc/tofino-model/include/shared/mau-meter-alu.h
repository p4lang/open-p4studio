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

#ifndef _SHARED_MAU_METER_ALU_
#define _SHARED_MAU_METER_ALU_

#include <string>
#include <cstdint>
#include <rmt-defs.h>
#include <mau-defs.h>
#include <mau-object.h>
#include <phv.h>
#include <bitvector.h>
#include <mau-memory.h>
#include <mau-meter.h>
#include <mau-lpf-meter.h>
#include <mau-stateful-alu.h>
#include <mau-chip-meter-alu.h>
#include <sweep-time-info.h>

#include <register_includes/meter_ctl.h>
#include <register_includes/red_value_ctl.h>
#include <register_includes/stateful_ctl.h>
#include <register_includes/selector_alu_ctl.h>
#include <register_includes/meter_alu_thread_array.h>
#include <register_includes/meter_alu_group_action_ctl.h>
#include <register_includes/meter_alu_group_phv_hash_mask_array2.h>
#include <register_includes/meter_alu_group_phv_hash_shift.h>

namespace MODEL_CHIP_NAMESPACE {

class Mau;
class MauExecuteState;
class MauLogicalRow;

class MauMeterAlu : public MauObject {

    static constexpr int  kType = RmtTypes::kRmtTypeMauMeterAlu;
    static constexpr int  kLogicalRows = MauDefs::kLogicalRowsPerMau;
    static constexpr int  kDataBusWidth = MauDefs::kDataBusWidth;
    static constexpr int  kSramWidth = MauDefs::kSramWidth;
    static constexpr int  kActionOutputBusWidth = MauDefs::kActionOutputBusWidth;
    static constexpr int  kMeterBytecountAdjustWidth = 14; // needed to do the sign extension properly
    static constexpr uint32_t kMeterAluLogicalRows = MauDefs::kMeterAluLogicalRows;
    static constexpr int kStatefulMeterAluDataBits = MauDefs::kStatefulMeterAluDataBits;

 public:
    static bool kRelaxThreadCheck; // Defined in rmt-config.cpp
    static bool kRelaxOpCheck;

    static const char *kOp4Strings[];
    static const char *get_opstr(int op4) { return kOp4Strings[op4 & 0xF]; }

    static inline int get_meter_alu_regs_index(int logrow) {
      int idx = __builtin_popcountl(kMeterAluLogicalRows & ((1u<<(logrow+1))-1)) - 1;
      RMT_ASSERT(idx >= 0);
      return idx;
    }
    static inline int get_meter_alu_logrow_index(int regs_index) {
      int n_logrows_seen_with_meter_alu = 0;
      for (int logrow = 0; logrow < kLogicalRows; logrow++) {
        if ((kMeterAluLogicalRows & (1u<<logrow)) != 0u) {
          if (n_logrows_seen_with_meter_alu == regs_index) return logrow;
          n_logrows_seen_with_meter_alu++;
        }
      }
      return -1;
    }
    static inline uint32_t map_meter_alus_to_rows(uint32_t alus, int *regs_index=NULL) {
      uint32_t rows = 0u;
      int n_logrows_seen_with_meter_alu = 0;
      for (int logrow = 0; logrow < kLogicalRows; logrow++) {
        if ((kMeterAluLogicalRows & (1u<<logrow)) != 0u) {
          // Found an row with a MeterALU - check if alu present in passed-in param
          if ((alus & (1<<n_logrows_seen_with_meter_alu)) != 0u) {
            if (regs_index != NULL) *regs_index = n_logrows_seen_with_meter_alu;
            rows |= (1<<logrow);
          }
          n_logrows_seen_with_meter_alu++;
        }
      }
      return rows;
    }


    MauMeterAlu(RmtObjectManager *om, int pipeIndex, int mauIndex, int logicalRowIndex,
                Mau *mau, MauLogicalRow *mau_log_row,
                int physicalRowIndex, int physicalRowWhich);
    ~MauMeterAlu();


    void ctl_write_callback();
    void phv_hash_mask_write_callback(int a1, int a0);

    bool is_egress_alu();
    bool is_ingress_alu();
    bool check_hdrtime_op4_ok(MauExecuteState *state, uint32_t valid_ops,
                              uint32_t addr, int op, const char *alu_type);
    bool run_alu_with_state(MauExecuteState *state);
    bool run_cmp_alu_with_state(MauExecuteState *state);
    void reset_resources();

    // used by run_alu, but public so DV can probe ALU inputs
    void get_input(BitVector<kDataBusWidth> *data, uint32_t *addr);
    // Next func gets final input post any wacky forwarding behaviour
    void get_input_data(BitVector<kDataBusWidth> *data);

    // not used by model - provided for DV to probe ALU output
    void get_output(BitVector<kDataBusWidth> *data, uint32_t *addr,BitVector<kActionOutputBusWidth> *action=0);
    // old version for Tofino
    void get_output(BitVector<kDataBusWidth> *data, uint32_t *addr,uint32_t *action) {
      BitVector<kActionOutputBusWidth> action_bv{};
      get_output(data,addr,&action_bv);
      if (action) *action = action_bv.get_word(0,32);
    }

    inline MauStatefulAlu *get_salu() { return &salu_; }
    inline bool salu_uses_divide()    { return stateful_ctl_.salu_enable() && salu_.uses_divide(); }
    bool salu_uses_qlag();

    void update_addresses(int old_addr, int new_addr);
    // public so DV can use
    BitVector<kStatefulMeterAluDataBits> get_alu_data_wide(Phv* phv);

    bool catch_up_sweeps(MauExecuteState *state);
    void update_sweep_time(MauExecuteState *state);


 private:
  bool                              ctor_finished_=false;
  MauLogicalRow                    *mau_log_row_;
  MauMemory                        *mau_memory_;
  MauMeter                          meter_;
  MauLpfMeter                       lpf_meter_;
  MauStatefulAlu                    salu_;
  int                               logical_row_;
  int                               physical_row_which_;
  int                               mau_index_;
  int                               alu_index_;
  register_classes::MeterCtl            meter_ctl_;
  register_classes::RedValueCtl         red_value_ctl_;
  register_classes::StatefulCtl         stateful_ctl_;
  register_classes::SelectorAluCtl      selector_ctl_;
  register_classes::MeterAluThreadArray meter_alu_thread_;
  register_classes::MeterAluGroupActionCtl meter_alu_group_action_ctl_;
  // The register does not exist on Tofino, but is handled using
  //   dummy registers which default to all ones (ie no masking)
  // (it is also duplicated in mau-selector-alu.h)
  register_classes::MeterAluGroupPhvHashMaskArray2 meter_alu_group_phv_hash_mask_;
  register_classes::MeterAluGroupPhvHashShift      meter_alu_group_phv_hash_shift_;
  BitVector<kActionOutputBusWidth>  last_action_{};
  bool                              r_action_override_=false;
  bool                              salu_uses_divide_=false;
  bool                              salu_uses_qlag_=false;
  bool                              has_run_=false;
  BitVector<kStatefulMeterAluDataBits> phv_hash_mask_{};
  // Special data-bus used during catch_up_sweeps - inactive by default
  std::array< BitVector<kDataBusWidth>, 2 >  sweep_data_{};
  int                                        sweep_data_index_ = -1;

  bool run_alu_internal(MauExecuteState *state, bool meter_sweep);
  int  get_packet_len(MauExecuteState *state);
  void set_output_data(BitVector<kDataBusWidth> *data);
  void set_output_addr(uint32_t *addr);
  uint32_t get_alu_data(Phv* phv);
  void set_output_action(BitVector<kActionOutputBusWidth>& action);
  void set_output_action_nocheck(BitVector<kActionOutputBusWidth>& action);

  // JBay only
  bool red_only_mode() {
    if (is_jbay_or_later()) {
      return (!meter_ctl_.lpf_enable()) && meter_ctl_.red_enable();
    }
    else {
      return false;
    }
  }
  bool lpf_enabled_or_red_only_mode() {
    return meter_ctl_.lpf_enable() || red_only_mode();
  }


  bool sweeping() {
    return (sweep_data_index_ >= 0);
  }
  void sweep_start(const BitVector<kDataBusWidth> &data) {
    sweep_data_[0].copy_from(data);
    sweep_data_index_ = 2;
    // All accesses to sweep_data_ are %2 so only [0][1] accessed ever
  }
  void sweep_get_input(BitVector<kDataBusWidth> *data) {
    RMT_ASSERT(sweeping() && "sweep_get_input() called but *not* sweeping");
    data->copy_from(sweep_data_[sweep_data_index_ % 2]);
    sweep_data_index_--; // Move index to other buffer ready for set_output
  }
  void sweep_set_output(BitVector<kDataBusWidth> *data) {
    RMT_ASSERT(sweeping() && "sweep_set_output() called but *not* sweeping");
    sweep_data_[sweep_data_index_ % 2].copy_from(*data);
  }
  void sweep_update_output(uint64_t word, int pos, int width) {
    RMT_ASSERT(sweeping() && "sweep_update_output() called but *not* sweeping");
    sweep_data_[sweep_data_index_ % 2].set_word(word, pos, width);
  }
  bool sweep_data_unchanged(const BitVector<kDataBusWidth> &mask) {
    RMT_ASSERT(sweeping() && "sweep_data_unchanged() called but *not* sweeping");
    int out = sweep_data_index_ % 2;
    return sweep_data_[1-out].masked_equals(sweep_data_[out], mask);
  }
  bool sweep_data_changed(const BitVector<kDataBusWidth> &mask) {
    RMT_ASSERT(sweeping() && "sweep_data_changed() called but *not* sweeping");
    return !sweep_data_unchanged(mask);
  }
  void sweep_continue() {
    RMT_ASSERT(sweeping() && "sweep_continue() called but *not* sweeping");
    // Keeps index >=0; preserves in/out relationship
    if (sweep_data_index_ == 0) sweep_data_index_ = 2;
  }
  void sweep_stop() {
    RMT_ASSERT(sweeping() && "sweep_stop() called but *not* sweeping");
    if ((sweep_data_index_ % 2) == 1) sweep_data_[0].copy_from(sweep_data_[1]);
    // Allow exactly one more sweep_get_input
    sweep_data_index_ = 0;
  }


};
}
#endif // _SHARED_MAU_METER_ALU_
