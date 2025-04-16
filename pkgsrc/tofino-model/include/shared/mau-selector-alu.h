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

#ifndef _SHARED_MAU_SELECTOR_ALU_H_
#define _SHARED_MAU_SELECTOR_ALU_H_

#include <cstdint>
#include <string>
#include <cstdint>
#include <rmt-defs.h>
#include <mau-defs.h>
#include <mau-object.h>
#include <pipe.h>
#include <cache-id.h>
#include <phv.h>
#include <mau-lookup-result.h>
#include <mau-input.h>
#include <mau-dependencies.h>
#include <mau-result-bus.h>
#include <mau-addr-dist.h>
#include <mau-instr-store.h>
#include <mau-op-handler.h>
#include <mau-memory.h>
#include <mau-logical-row.h>
#include <mau-sram-column.h>
#include <mau-sram-row.h>
#include <mau-sram.h>
#include <mau-logical-tcam.h>
#include <mau-tcam-row.h>
#include <mau-tcam.h>

#include <register_includes/selector_alu_ctl.h>
#include <register_includes/stateful_ctl.h>
#include <register_includes/exactmatch_row_hashadr_xbar_ctl_array.h>
#include <register_includes/mau_map_and_alu_row_addrmap.h>
#include <register_includes/meter_alu_group_data_delay_ctl.h>
#include <register_includes/meter_alu_group_phv_hash_mask_array2.h>
#include <register_includes/meter_alu_group_phv_hash_shift.h>

namespace MODEL_CHIP_NAMESPACE {

  class MauExecuteState;

  class MauSelectorAlu : public MauObject {

    public:

      static constexpr int kType                = RmtTypes::kRmtTypeSelectorAlu;
      static constexpr int kHashOutputWidth     = MauDefs::kHashOutputWidth;
      static constexpr int kSramWidth           = MauDefs::kSramWidth;
      static constexpr int kTableResultBusWidth = MauDefs::kTableResultBusWidth;
      static constexpr int kHashGroups          = MauDefs::kHashGroups;
      static constexpr uint32_t kSelectorAluInvalOutput = MauDefs::kSelectorAluInvalOutput;
      static constexpr uint32_t kSelectorAluLogicalRows = MauDefs::kSelectorAluLogicalRows;

      static inline bool is_inval_output(uint32_t o)  { return (o == kSelectorAluInvalOutput); }
      static inline int  get_selector_alu_regs_index(int logrow) {
        int idx = __builtin_popcountl(kSelectorAluLogicalRows & ((1u<<(logrow+1))-1)) - 1;
        RMT_ASSERT(idx >= 0);
        return idx;
      }


      MauSelectorAlu(RmtObjectManager   *om,
                     int                 pipeIndex,
                     int                 mauIndex,
                     int                 log_row,
                     Mau                *mau,
                     MauLogicalRow      *logicalrow,
                     int                 sram_row,
                     int                 sram_which);

      ~MauSelectorAlu() {}

      void run_alu_with_state(MauExecuteState *state);
      void get_input_data(BitVector<kSramWidth> *data);
      void reset_resources() { }

      void phv_hash_mask_write_callback(int a1, int a0);
      uint64_t get_alu_data(Phv *phv);


    private:
      void run_alu_with_phv(Phv *phv,  BitVector<kSramWidth> *selector_word);


      int bitcount(uint64_t   num) {
        uint8_t  val = 0;
        while (num) {
          val++;
          num = num >>  1;
        }
        return (val);
      }

      MauLogicalRow        *logical_row_;
      Mau                  *mau_;
      int                   alu_index_;

      // Registers...
      register_classes::MeterAluGroupDataDelayCtl         meter_alu_group_ctl_;
      register_classes::SelectorAluCtl                    selector_alu_ctl_;
      register_classes::StatefulCtl                       stateful_ctl_;
      // meter_alu_group_phv_hash_mask_ is in the meter alu group (where the selector alu is
      //  also) which is mostly implmented in mau-meter-alu.[h|cpp], but as the selector alu is
      //  not in mau-meter-alu for historical reasons it is duplicated here also.  This register
      // does not exist on Tofino, but is handled using dummy registers which default to all ones
      // (ie no masking)
      register_classes::MeterAluGroupPhvHashMaskArray2   meter_alu_group_phv_hash_mask_;
      register_classes::MeterAluGroupPhvHashShift        meter_alu_group_phv_hash_shift_;

      uint8_t               s_word_msb_{0};             // Port vector len of corresponding s_word
                                                        // obtained from Selector RAM
      uint16_t              hash_bucket_array_[3];
      uint8_t               hash_mod_array_[3];         // hash-bucket_array[i] MOD s_word_msb

      BitVector<kSramWidth> s_word_;                    // entire selector word (upto 128b);
                                                        // Used for plan-b resilient hash calculation
                                                        // and by Fair hash

      std::array<BitVector<kHashOutputWidth>, MauHashGenerator::kGroups>  all_hash_group_result;

      uint64_t phv_hash_mask_{};

      bool is_resilient_hash_mode();
      uint8_t get_selector_alu_hash_select();
      void get_hash_buckets(Phv *phv); // break hash[41:0] into 3 buckets
      void set_s_word_from_selector_ram(BitVector<kSramWidth>  *selector_word);
      uint8_t get_resilient_selection_index(Phv *phv, BitVector<kSramWidth>  *selector_word);
      uint8_t get_planB_resilient_selection_index(Phv *phv, BitVector<kSramWidth>  *selector_word);
      uint8_t get_fair_hash_selection_index(Phv *phv, BitVector<kSramWidth>  *selector_word);

      uint32_t last_address_=0;
      uint64_t last_relative_time_=0;
      BitVector<kSramWidth> data_used_{ UINT64_C(0) };
      BitVector<kSramWidth> last_data_in_{ UINT64_C(0) };
  };


}


#endif
