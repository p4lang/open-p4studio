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

#ifndef _SHARED_MAU_STASH_REG_
#define _SHARED_MAU_STASH_REG_

#include <cstdint>
#include <rmt-defs.h>
#include <rmt-log.h>
#include <mau-defs.h>
#include <mau-object.h>

// Reg defs auto-generated from Semifore
#include <register_includes/stash_bank_enable_array.h>
#include <register_includes/stash_data_array2.h>
#include <register_includes/stash_hashkey_data_array.h>
#include <register_includes/stash_match_address_array.h>
#include <register_includes/stash_version_valid_array.h>
#include <register_includes/stash_match_input_data_ctl_array.h>
#include <register_includes/stash_match_mask_array2.h>
#include <register_includes/stash_match_result_bus_select_array.h>
#include <register_includes/stash_bus_overload_bytemask_array.h>

namespace MODEL_CHIP_NAMESPACE {

class MauStash;

class MauStashReg : public MauObject {

  static constexpr int    kEntries = MauDefs::kStashEntries;

public:
  MauStashReg(RmtObjectManager *om, int pipeIndex, int mauIndex,
              int rowIndex, MauStash *mauStash);
  virtual ~MauStashReg();
  
  uint32_t get_bank_enable_id(int which_entry) {
    RMT_ASSERT( which_entry < kEntries );
    return stash_bank_enable_array_.stash_bank_enable_bank_id(which_entry);
  }
  uint32_t get_bank_enable_mask(int which_entry) {
    RMT_ASSERT( which_entry < kEntries );
    return stash_bank_enable_array_.stash_bank_enable_bank_mask(which_entry);
  }

  uint32_t get_hashkey_data(int which_entry) {
    RMT_ASSERT( which_entry < kEntries );
    return stash_hashkey_data_array_.stash_hashkey_data(which_entry);
  }
  uint32_t get_match_address(int which_entry) {
    RMT_ASSERT( which_entry < kEntries );
    return stash_match_address_array_.stash_match_address(which_entry);
  }

  bool get_valid_version_bit_1_old(int which_entry) {
    RMT_ASSERT( which_entry < kEntries );
    return 0x8 & stash_version_valid_array_.stash_version_valid(which_entry);
  }
  bool get_valid_version_bit_0_old(int which_entry) {
    RMT_ASSERT( which_entry < kEntries );
    return 0x4 & stash_version_valid_array_.stash_version_valid(which_entry);
  }
  bool get_valid_version_bit_1_new(int which_entry) {
    RMT_ASSERT( which_entry < kEntries );
    return 0x2 & stash_version_valid_array_.stash_version_valid(which_entry);
  }
  bool get_valid_version_bit_0_new(int which_entry) {
    RMT_ASSERT( which_entry < kEntries );
    return 0x1 & stash_version_valid_array_.stash_version_valid(which_entry);
  }

  uint8_t get_match_data_select(int which_stash) {
    return stash_match_input_data_ctl_array_.
        stash_match_data_select(which_stash);
  }
  // Codes 0-4 are 10b groups 0-4 of hashword0.
  // Codes 5-9 are 10b groups 0-4 of hashword1.
  uint8_t get_hash_adr_which_group(int which_stash) {
    uint8_t v = stash_match_input_data_ctl_array_.stash_hash_adr_select(which_stash);
    RMT_ASSERT( v <= 9 );
    if ( v <= 4 ) {
      return v;
    }
    else {
      return v-5;
    }
  }
  uint8_t get_hash_adr_which_word(int which_stash) {
    uint8_t v = stash_match_input_data_ctl_array_.stash_hash_adr_select(which_stash);
    RMT_ASSERT( v <= 9 );
    if ( v <= 4 ) {
      return 0;
    }
    else {
      return 1;
    }
  }
  uint8_t get_hashbank_select(int which_stash) {
    return stash_match_input_data_ctl_array_.
        stash_hashbank_select(which_stash);
  }
  uint8_t get_enable(int which_stash) {
    return stash_match_input_data_ctl_array_.
        stash_enable(which_stash);
  }
  uint8_t get_logical_table(int which_stash) {
    return stash_match_input_data_ctl_array_.
        stash_logical_table(which_stash);
  }
  uint8_t get_thread(int which_stash) {
    return stash_match_input_data_ctl_array_.
        stash_thread(which_stash);
  }

  bool get_match_result_bus_select(int which_stash,int which_bus) {
    RMT_ASSERT(which_bus<2);
    RMT_ASSERT(which_stash<2);
    uint8_t v=stash_match_result_bus_select_array_.stash_match_result_bus_select(which_stash);
    return ( 1 == ( (v>>which_bus) & 1 ) );
  }

  bool stash_bus_overload_bytemask(int which_stash, int which_byte) {
    RMT_ASSERT(which_stash<2);
    RMT_ASSERT(which_byte<8);
    uint8_t v = stash_bus_overload_bytemask_array_.stash_bytemask(which_stash);
    return ( 1 == ( (v>>which_byte) & 1 ));
  } 
  bool stash_bus_overload(int which_stash) {
    RMT_ASSERT(which_stash<2);
    return ( 0 != stash_bus_overload_bytemask_array_.stash_bytemask(which_stash) );
  }
 
private:
  void data_callback(uint32_t a1,uint32_t a0);
  void mask_callback(uint32_t a1,uint32_t a0);

  register_classes::StashBankEnableArray stash_bank_enable_array_;
  register_classes::StashDataArray2 stash_data_array2_;
  register_classes::StashHashkeyDataArray stash_hashkey_data_array_;
  register_classes::StashMatchAddressArray stash_match_address_array_;
  register_classes::StashVersionValidArray stash_version_valid_array_;
  register_classes::StashMatchInputDataCtlArray stash_match_input_data_ctl_array_;
  register_classes::StashMatchMaskArray2 stash_match_mask_array2_;
  register_classes::StashMatchResultBusSelectArray stash_match_result_bus_select_array_;
  register_classes::StashBusOverloadBytemaskArray stash_bus_overload_bytemask_array_;
  
  MauStash                       *mau_stash_;

  };
}
#endif // _SHARED_MAU_STASH_REG_
