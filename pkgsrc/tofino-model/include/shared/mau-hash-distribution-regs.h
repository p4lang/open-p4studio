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

#ifndef _SHARED_MAU_HASH_DISTRIBUTION_REGS_H_
#define _SHARED_MAU_HASH_DISTRIBUTION_REGS_H_

#include <cstdint>
#include <string>
#include <cstdint>
#include <rmt-defs.h>
#include <mau-defs.h>
#include <register_adapters.h>
#include <mau-hash-distribution-chip-regs.h>

#include <register_includes/mau_hash_group_xbar_ctl_array2.h>
#include <register_includes/mau_selector_hash_sps_enable.h>
#include <register_includes/mau_hash_group_config.h>
#include <register_includes/mau_selector_action_entry_size_array.h>
#include <register_includes/mau_hash_group_mask_array.h>
#include <register_includes/mau_hash_group_shiftcount.h>
#include <register_includes/mau_meter_precolor_hash_map_to_logical_ctl_array.h>
#include <register_includes/mau_meter_precolor_hash_sel.h>
#include <register_includes/mau_hash_group_expand_array.h>

namespace MODEL_CHIP_NAMESPACE {

enum class MauHashGroupResultType { 
    ImmediateActionDataHi = 0,
    ImmediateActionDataLo = 1,
    MeterAddr             = 2,
    StatsAddr             = 3,
    ActionDataAddr        = 4,
    HashModDividend       = 5 };

class MauHashDistributionRegs : public MauObject {

    public:
    MauHashDistributionRegs(RmtObjectManager   *om,
                            int chipIndex, int pipeIndex, int mauIndex, Mau *mau) :
          MauObject(om, pipeIndex, mauIndex, mau),
          mau_hash_distribution_chip_regs_(om, pipeIndex, mauIndex, mau),
          hash_group_xbar_ctl_(default_adapter(hash_group_xbar_ctl_,chipIndex, pipeIndex, mauIndex)),
          hash_group_config_(default_adapter(hash_group_config_,chipIndex, pipeIndex, mauIndex)),
          sel_sps_enable_(default_adapter(sel_sps_enable_,chipIndex, pipeIndex, mauIndex)),
          hash_group_mask_(default_adapter(hash_group_mask_,chipIndex, pipeIndex, mauIndex)),
          hash_group_shiftcount_(default_adapter(hash_group_shiftcount_,chipIndex, pipeIndex, mauIndex)),
          hash_group_expand_(default_adapter(hash_group_expand_,chipIndex, pipeIndex, mauIndex)),
          meter_precolor_hash_map_to_logical_ctl_(default_adapter(meter_precolor_hash_map_to_logical_ctl_,chipIndex, pipeIndex, mauIndex)),
          meter_precolor_hash_sel_(default_adapter(meter_precolor_hash_sel_,chipIndex, pipeIndex, mauIndex)),
          selector_action_entry_size_(default_adapter(selector_action_entry_size_,chipIndex, pipeIndex, mauIndex))
    {
        hash_group_xbar_ctl_.reset();
        hash_group_config_.reset(); 
        sel_sps_enable_.reset();
        hash_group_mask_.reset();
        hash_group_shiftcount_.reset();
        hash_group_expand_.reset();
        meter_precolor_hash_map_to_logical_ctl_.reset();
        meter_precolor_hash_sel_.reset();
        selector_action_entry_size_.reset();
      }

      ~MauHashDistributionRegs() {}

      static bool kRelaxGroupEnableChecks; // defined in rmt-config.cpp
    
      void hash_group_sel(int which,bool* enable,int* select) {
        RMT_ASSERT( which==0 || which==1 );
        uint8_t v = hash_group_config_.hash_group_sel();
        // pick the correct 4 bit entry and decode it
        if (which) v = v>>4;
        *enable = v & 0x8;
        *select = v & 0x7;
      }
      void precolor_hash_map( int logical_table, bool* enable, int* which ) {
        int w = meter_precolor_hash_map_to_logical_ctl_.mau_meter_precolor_hash_map_to_logical_ctl( logical_table / 4 );
        int v = w >> ((logical_table%4)*5);
        *enable = 0x10 & v;
        *which  = 0x0F & v;
      }
      void precolor_hash_sel( int which, bool* enable, int* select ) {
        int v = meter_precolor_hash_sel_.mau_meter_precolor_hash_sel();
        if (which) v = v>>4;
        *enable = v & 0x8;
        *select = v & 0x7;
        if (*enable) {
          if ( ! hash_group_enable(*select) ) {
            RMT_LOG(RmtDebug::error(kRelaxGroupEnableChecks),
                    "MauHashDistributionRegs: precolor %d selects hash %d which is not enabled\n",
                    which,*select);
          }
        }
      }

    bool selector_sps_enable(int which) {
      RMT_ASSERT( which>=0 && which < 6);
      return 1 & (sel_sps_enable_.mau_selector_hash_sps_enable() >> which);
    }

    uint32_t hash_group_mask(int which) {
      RMT_ASSERT(which>=0 && which < 6);
      return hash_group_mask_.mau_hash_group_mask(which);
    }

    int hash_group_shift(int which) {
      RMT_ASSERT(which>=0 && which < 6);
      int v = hash_group_shiftcount_.mau_hash_group_shiftcount();
      return (v >> (3*which)) & 0x7;
    }

    // which slices chooses between slices 0-2 and 3-5, which_group is 0 or 1 (group 2 uses a
    //   shift in expansion so has its own function)
    bool hash_slice_group_expand(int which_slices,int which_group ) {
      RMT_ASSERT( which_slices==0 || which_slices==1 );
      RMT_ASSERT( which_group==0 || which_group==1 );
      return which_group ?
          hash_group_expand_.hash_slice_group1_expand(which_slices) :
          hash_group_expand_.hash_slice_group0_expand(which_slices);
    }
    // which slices chooses slices 0-2 or 3-5, so group2 is either slice 2 or slice 5
    int hash_slice_group2_shift(int which_slices) {
      RMT_ASSERT( which_slices==0 || which_slices==1 );
      return hash_group_expand_.hash_slice_group2_expand(which_slices);
    }
    bool hash_group_enable(int group) {
      RMT_ASSERT(group>=0 && group<6);
      return (hash_group_config_.hash_group_enable()>>group) & 1;
    }
    bool hash_group_ctl(int group) {
      RMT_ASSERT(group>=0 && group<6);
      return (hash_group_config_.hash_group_ctl()>>(group*2)) & 3;
    }
    
    void bus_hash_group_xbar_ctl(int lt_or_alu,MauHashGroupResultType result_type,
                                 bool* enable, int* select) {

      // NOTE: In the case of HashModDividend this func is 
      // called with an ALU (0-3) rather than an LT (0-15)
      // as only bottom 4 outputs of hashmod dividend xbar used
      RMT_ASSERT(lt_or_alu >= 0 && lt_or_alu < 16);

      int index = static_cast<int>(result_type);
      int val = 0xf & (hash_group_xbar_ctl_.mau_hash_group_xbar_ctl(index, lt_or_alu/8 )
                        >> ((lt_or_alu%8)*4)) ;
      *enable = 0x8 & val;
      *select = 0x7 & val;
      if (*enable) {
        if ( ! hash_group_enable(*select) ) {
          RMT_LOG(RmtDebug::error(kRelaxGroupEnableChecks),
                  "MauHashDistributionRegs: type=%d selects hash %d which is not enabled\n",
                  result_type,*select);
        }
        if ( result_type == MauHashGroupResultType::ImmediateActionDataHi ||
             result_type == MauHashGroupResultType::ImmediateActionDataLo ||
             result_type == MauHashGroupResultType::MeterAddr ||
             result_type == MauHashGroupResultType::StatsAddr ||
             result_type == MauHashGroupResultType::ActionDataAddr ) {
          if ( hash_group_ctl( *select ) != 1 ) { // 1 is immediate / meter /stats / actiondata / precolor
            RMT_LOG(RmtDebug::error(kRelaxGroupEnableChecks),
                    "MauHashDistributionRegs: type=%d selects hash %d so hash_group_ctl[%d] should be 1, but is %d\n",
                    result_type,*select,*select,hash_group_ctl( *select ));
          }
        }
        else if ( result_type == MauHashGroupResultType::HashModDividend ) { 
          RMT_ASSERT(lt_or_alu >= 0 && lt_or_alu < 4);
          if ( hash_group_ctl( *select ) != 0 ) { // 0 is selector hash mod
            RMT_LOG(RmtDebug::error(kRelaxGroupEnableChecks),
                    "MauHashDistributionRegs: type=%d selects hash %d so hash_group_ctl[%d] should be 0, but is %d\n",
                    result_type,*select,*select,hash_group_ctl( *select ));
          }

        }
      }
    }

    // These 2 funcs moved from Log->Phys (ie from LT index to ALU index)
    // as part of regs_25957_mau_dev
    int selector_action_entry_shift(int alu) {
      RMT_ASSERT(alu >= 0 && alu < 4);
      uint32_t entry_width = selector_action_entry_size_.mau_selector_action_entry_size(alu);
      RMT_ASSERT( entry_width <= 7 );
      int shift = 0;
      switch (entry_width) {
        case 0:  shift =  8; break;
        case 1:  shift =  9; break;
        case 2:  shift = 10; break;
        case 3:  shift = 11; break;
        case 4:  shift = 12; break;
        default: shift = 12; break;
      }
      // 0 fixup unless JBay (and doing qLAG when may be -3 or -4)
      int fixup = selector_mod_shift_fixup(alu); 
      return shift + fixup;
    }
    int selector_action_entry_straddle(int alu) {
      RMT_ASSERT(alu >= 0 && alu < 4);
      uint32_t entry_width = selector_action_entry_size_.mau_selector_action_entry_size(alu);
      RMT_ASSERT( entry_width <= 7 );
      switch (entry_width) {
        case 5:  return 1;
        case 6:  return 2;
        case 7:  return 3;
        default: return 0;
      }
    }

    // These vary PER-CHIP
    int selector_mod_shift_fixup(int alu) {
      return mau_hash_distribution_chip_regs_.selector_mod_shift_fixup(alu);
    }
    bool selector_mod_is_vpn(int alu, uint8_t addr_type) {
      return mau_hash_distribution_chip_regs_.selector_mod_is_vpn(alu, addr_type);
    }
    uint32_t selector_dividend(int alu, uint8_t addr_type, uint32_t hash_input) {
      return mau_hash_distribution_chip_regs_.selector_dividend(alu, addr_type, hash_input);
    }
    uint8_t selector_divisor(int alu, uint8_t addr_type, uint32_t selector_len) {
      return mau_hash_distribution_chip_regs_.selector_divisor(alu, addr_type, selector_len);
    }


   private:
    MauHashDistributionChipRegs                                mau_hash_distribution_chip_regs_;
    register_classes::MauHashGroupXbarCtlArray2                hash_group_xbar_ctl_;
    register_classes::MauHashGroupConfig                       hash_group_config_; 
    register_classes::MauSelectorHashSpsEnable                 sel_sps_enable_;
    register_classes::MauHashGroupMaskArray                    hash_group_mask_;
    register_classes::MauHashGroupShiftcount                   hash_group_shiftcount_;
    register_classes::MauHashGroupExpandArray                  hash_group_expand_;
    register_classes::MauMeterPrecolorHashMapToLogicalCtlArray meter_precolor_hash_map_to_logical_ctl_;
    register_classes::MauMeterPrecolorHashSel                  meter_precolor_hash_sel_;
    register_classes::MauSelectorActionEntrySizeArray          selector_action_entry_size_;
  };

}


#endif
