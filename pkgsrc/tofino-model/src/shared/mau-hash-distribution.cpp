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

#include <mau.h>
#include <string>
#include <rmt-log.h>
#include <rmt-object-manager.h>
#include <mau-sbox.h>

namespace MODEL_CHIP_NAMESPACE {

uint64_t MauHashDistribution::get_48b_hash(Phv *phv, int which)
{
  RMT_ASSERT(which<2);
  if (get_48b_cache_id_[which].IsValid() && get_48b_cache_id_[which].Equals(phv->cache_id())) {
  } else {
    // Recalculate and cache

    bool hash_enable;
    int  hash_sel;
    registers_.hash_group_sel(which,&hash_enable,&hash_sel);
    hash_48b_[which] = 0;
    if (hash_enable) {
      RMT_ASSERT(hash_sel < kHashGroups);
      
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauHashDistrib),
              "Selected hash-group %d for hash %d\n", hash_sel, which);

      BitVector<kHashOutputWidth> hash_output;
      hash_output  = mau()->get_hash_output(phv, hash_sel);
      hash_48b_[which] = hash_output.get_word(0, kHashDistribSelectWidth); // Truncate to 48b

      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauHashDistrib),
              "Selected hash-value %d = 0x%016" PRIx64 "\n", which, hash_48b_[which]);
    }
    get_48b_cache_id_[which].SetFrom(phv->cache_id());
  }
  return hash_48b_[which];
}



uint32_t MauHashDistribution::get_16b_hash(Phv *phv, int which)
{
  RMT_ASSERT( which < kHashDistribGroups );
  if (get_16b_cache_id_[which].IsValid() && get_16b_cache_id_[which].Equals(phv->cache_id())) {
  } else {
    // Recalculate and cache
    uint64_t hash_48b;
    if (which < 3) {
      hash_48b = get_48b_hash(phv,0);
      hash_16b_[which] = (hash_48b >> (which*kHashletWidth)) & ((1<< kHashletWidth)-1);
    } else {
      hash_48b = get_48b_hash(phv,1);
      hash_16b_[which] = (hash_48b >> ((which-3)*kHashletWidth)) & ((1<< kHashletWidth)-1);
    }

    if (registers_.selector_sps_enable(which)) {
      MauSbox sbox;
      // Do SPS scrambling... 
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauHashDistrib),
              "hash bucket %d: 16b hash before 0x%x and after sps15 scrambling = 0x%x\n", which, 
              hash_16b_[which], sbox.sps15(hash_16b_[which]));
      hash_16b_[which] = sbox.sps15(hash_16b_[which]);
    }
    else {
      // do the expansion
      // Figure out if it in slices 0-2 or 3-5
      int which_slices = (which<3) ? 0 : 1;
      // and and which hash_group within the three
      int which_group   = which_slices ? which-3 : which;
      if ( which_group < 2 ) {
        bool expand = registers_.hash_slice_group_expand(which_slices,which_group);
        if (expand) {
          // group 0: {slice[38:32], slice[15:0]}
          // group 1: {slice[45:39], slice[31:16]} 
          int shift = which_group ? 39 : 32;
          hash_16b_[which] |= ((hash_48b >> shift)&0x7f)<<16;
        }
      }
      else { // hash_group 2 uses a shift for expansion
        int shift = registers_.hash_slice_group2_shift(which_slices);
        hash_16b_[which] = hash_16b_[which] >> shift;
      }

      
    }
    get_16b_cache_id_[which].SetFrom(phv->cache_id());
  }
  return hash_16b_[which];
}

uint32_t MauHashDistribution::get_16b_hash_masked_and_shifted(Phv *phv, int which)
{
  RMT_ASSERT( which < kHashDistribGroups );
  uint32_t h = get_16b_hash(phv, which);
  h = h &  registers_.hash_group_mask(which);
  h = h << registers_.hash_group_shift(which);
  h = h & 0x7fffff; // reduce to 23 bits
  return h;
}

uint32_t MauHashDistribution::get_final_hash(Phv *phv, int logical_table, MauHashGroupResultType result_type)
{
  int  word_index;
  bool word_enable;

  registers_.bus_hash_group_xbar_ctl(logical_table,result_type,
                                     &word_enable,&word_index);
  if ( word_enable )
    return get_16b_hash_masked_and_shifted(phv,word_index);
  return 0;
}

int MauHashDistribution::selector_numword(uint32_t selector_len) {
  return (selector_len >> 0) & 0x1F;
}
int MauHashDistribution::selector_shift(uint32_t selector_len, bool selector_mod_is_vpn) {
  int shift = (selector_len >> 5) & 0x7;
  if ((selector_mod_is_vpn) && (selector_len != 0u)) {
    RMT_LOG(RmtDebug::error(kRelaxHashSelectorLenCheck),
            "MauHashDistribution::selector_len(%d) Non-zero selector_len INVALID "
            "as selector mod is VPN!)\n", selector_len);
    if (!kRelaxHashSelectorLenCheck) { THROW_ERROR(-2); }
    shift = 0; // Fix
  }
  if (shift > 5) {
    RMT_LOG(RmtDebug::error(kRelaxHashSelectorShiftCheck),
            "MauHashDistribution::selector_len(%d) (shift=%d => INVALID)\n", selector_len, shift);
    if (!kRelaxHashSelectorShiftCheck) { THROW_ERROR(-2); }
  }
  if (shift > 0) {
    int numword = selector_numword(selector_len);
    if (numword == 0) {
      // uArch doc 6.2.10.5.5 says a value of 0 in selector_numword
      // field should always generate a selector offset of 0
      // Given shift not zero check and complain if numword is 0
      RMT_LOG(RmtDebug::warn(),
              "MauHashDistribution:: selector_numword is 0 but selector_shift is %d "
              "[sel_len=%d(0x%x)]\n", shift, selector_len, selector_len);
    }    
  }
  return shift;
}

uint32_t MauHashDistribution::get_immediate_data_ls_hash(Phv *phv, int logical_table) {
  return 0xffff & get_final_hash(phv,logical_table,MauHashGroupResultType::ImmediateActionDataLo);
}
uint32_t MauHashDistribution::get_immediate_data_ms_hash(Phv *phv, int logical_table) {
  return 0xffff & get_final_hash(phv,logical_table,MauHashGroupResultType::ImmediateActionDataHi);
}

uint32_t MauHashDistribution::get_immediate_data_hash(Phv *phv, int logical_table)
{
  return (get_immediate_data_ms_hash(phv,logical_table) << 16) |
      get_immediate_data_ls_hash(phv,logical_table);
}

uint32_t MauHashDistribution::get_meter_address(Phv *phv, int logical_table)
{
  return 0x7fffff & get_final_hash(phv,logical_table,MauHashGroupResultType::MeterAddr);
}
uint32_t MauHashDistribution::get_stats_address(Phv *phv, int logical_table)
{
  return 0x07ffff & get_final_hash(phv,logical_table,MauHashGroupResultType::StatsAddr);
}
uint32_t MauHashDistribution::get_action_address(Phv *phv, int logical_table)
{
  return 0x3fffff & get_final_hash(phv,logical_table,MauHashGroupResultType::ActionDataAddr);
}

uint32_t MauHashDistribution::get_selector_action_address(Phv *phv, int logical_table, int alu, uint32_t sel_len)
{
  uint32_t action_address = 0u;
  uint8_t  addr_type = AddrType::kAction;
  
  if ( selector_addresses_enabled(alu) ) {

    if (selector_divisor(alu, addr_type, sel_len) != 0) {

      bool     mod_is_vpn = selector_mod_is_vpn(alu, addr_type);
      uint32_t hash_mod_len = selector_hash_mod_len(phv, alu, addr_type, sel_len);
      uint32_t hash_lsb = 0u;
      uint32_t addr = hash_mod_len;
      uint8_t  sel_shift = selector_shift(sel_len, mod_is_vpn); // shift is 0 if VPN
      int      straddle = registers_.selector_action_entry_straddle(alu);
      uint8_t  addr_shift = 0; 

      /* Find out # of Index bits depending on port-vector length */
      /* Don't know this here.... Only after Selector RAM lookup we know this. 
       * Will be done in selector ALU work 
       */
      
      // Note, sel_shift always 0 if VPN mod so no hash_lsbs added if VPN mod
      if (sel_shift != 0) {
        uint32_t h1_hash = selector_h1_hash(phv,alu);
        uint32_t sel_mask = 0xFFu >> (8 - sel_shift);
        
        hash_lsb = (h1_hash >> 10) & sel_mask;
        addr = (hash_mod_len << sel_shift) | hash_lsb;
      }
      
      if (mod_is_vpn) {
        // VPN mod - shift is 15 + 0/1/2/3 hole straddle
        addr_shift = 5 + Address::kIndexWidth + straddle;
      } else {
        // Not VPN mod - shift determined by action_entry_size
        addr_shift = registers_.selector_action_entry_shift(alu);
        
        // But hole straddle may be within 10b hash_mod
        if (straddle > 0) {
          // Skip positions for huffman-bits between hash-lsb bits
          uint32_t addr_l = addr & 0x7;
          uint32_t addr_h = addr & 0x3f8;
          // 05Sep2017:FIX 0 below was a 3
          addr = (addr_h << (0 + straddle)) | addr_l;
        }
      }
      
      action_address = addr << addr_shift;
      
      RMT_LOG(RmtDebug::verbose(),
              "Mod(%s), hash-lsb = 0x%x, sel_shift = %d, straddle = %d, "
              "in_addr = 0x%x, addr_shift = %d, out_addr = 0x%x\n",
              (mod_is_vpn) ?"max_vpn" :"sel_len",
              hash_lsb, sel_shift, straddle, addr, addr_shift, action_address);
      RMT_LOG(RmtDebug::verbose(),
              "Action-ram addr after including selected word offset = 0x%x \n",
              action_address);
    }
  }
  return action_address;
}


uint32_t MauHashDistribution::get_selector_address(Phv *phv, int logical_table, int alu, uint32_t sel_len)
{
  uint32_t selector_address = 0u;
  uint8_t  addr_type = AddrType::kMeter;

  if ( selector_addresses_enabled(alu) ) {
    
    if (selector_divisor(alu, addr_type, sel_len) != 0) {

      bool     mod_is_vpn = selector_mod_is_vpn(alu, addr_type);
      uint32_t hash_mod_len = selector_hash_mod_len(phv, alu, addr_type, sel_len);
      uint32_t addr = hash_mod_len;
      uint8_t  sel_shift = selector_shift(sel_len, mod_is_vpn); // shift is 0 if VPN
      uint8_t  addr_shift = Address::kMeterSubwordWidth;  //  7
      if (mod_is_vpn) addr_shift += Address::kIndexWidth; // 17
      
      // Insert word offset with a block
      // Note, sel_shift always 0 if VPN mod so no hash_lsbs added if VPN mod
      if (sel_shift != 0) {
        uint32_t h1_hash = selector_h1_hash(phv,alu);
        uint32_t sel_mask = 0xFFu >> (8 - sel_shift);
        uint32_t hash_lsb = (h1_hash >> 10) & sel_mask;
        
        // Insert hash LSBs or block selection
        addr  = (hash_mod_len << sel_shift) | hash_lsb;
      }

      // 8 huffman bits changed to 7bits (post CRB changes)
      selector_address = (addr << addr_shift); 
    }
  }
  return selector_address;
}

uint8_t MauHashDistribution::get_meter_precolor(Phv *phv, int logical_table) {

  bool hash_map_enable;
  int  which_chunk;
  registers_.precolor_hash_map( logical_table, &hash_map_enable, &which_chunk );
  uint8_t color=0;
  if (hash_map_enable) {
    RMT_ASSERT( which_chunk < 16 );
    int which_of_two_16b = (which_chunk<8) ? 0 : 1;
    bool enable;
    int  select;
    registers_.precolor_hash_sel( which_of_two_16b, &enable, &select );
    if (enable) {
      RMT_ASSERT( select < kHashDistribGroups );
      // note: this hash is taken before the masking and shifting
      uint16_t hash = get_16b_hash(phv, select);
      color = 3 & ( hash >> ((which_chunk%8)*2));
    }
  }
  return color;
}

// These only used by DV
// Functions work out alu & sel_len internally and call back into MauHashDistribution
uint32_t MauHashDistribution::get_selector_address(Phv *phv, int logtab) {
  return mau()->get_selector_address(phv, logtab);
}
uint32_t MauHashDistribution::get_selector_action_address(Phv *phv, int logtab) {
  return mau()->get_selector_action_address(phv, logtab);
}

}


