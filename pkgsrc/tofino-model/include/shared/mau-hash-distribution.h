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

#ifndef _SHARED_MAU_HASH_DISTRIBUTION_H_
#define _SHARED_MAU_HASH_DISTRIBUTION_H_

#include <cstdint>
#include <string>
#include <cstdint>
#include <rmt-defs.h>
#include <mau-defs.h>
#include <mau-object.h>
#include <pipe.h>
#include <phv.h>
#include <cache-id.h>
#include <mau-hash-distribution-regs.h>

namespace MODEL_CHIP_NAMESPACE {
  class Mau;

  class MauHashDistribution : public MauObject {

 public:
    static bool kRelaxHashSelectorLenCheck; // Defined in rmt-config.cpp
    static bool kRelaxHashSelectorShiftCheck; 

    static constexpr int kHashOutputWidth        = MauDefs::kHashOutputWidth;
    static constexpr int kHashDistribSelectWidth = MauDefs::kHashDistribSelectWidth;
    static constexpr int kHashDistribGroups      = MauDefs::kHashDistribGroups;
    static constexpr int kHashletWidth           = MauDefs::kHashletWidth;
    static constexpr int kTableResultBusWidth    = MauDefs::kTableResultBusWidth;
    static constexpr int kHashGroups             = MauDefs::kHashGroups;
  
    MauHashDistribution(RmtObjectManager   *om,
                        int                chipIndex, 
                        int                pipeIndex, 
                        int                mauIndex, 
                        Mau                *mau) :
        MauObject(om, pipeIndex, mauIndex, mau),
        get_48b_cache_id_{},
        get_16b_cache_id_{},
        registers_(om,chipIndex,pipeIndex,mauIndex,mau)
    {
    }

    ~MauHashDistribution() {}

    /** get the hash needed for the immediate data
     */
    uint32_t get_immediate_data_hash(Phv *phv, int logical_table);

    /** get the hash needed for the meter address
     */
    uint32_t get_meter_address(Phv *phv, int logical_table);

    /** get the hash needed for the stats address
     */
    uint32_t get_stats_address(Phv *phv, int logical_table);

    /** get the hash needed for the meter address
     */
    uint32_t get_action_address(Phv *phv, int logical_table);

    /** get the hash needed for the selector address
     */
    uint32_t get_selector_address(Phv *phv, int logical_table, int alu, uint32_t sel_len);
    /** get the hash needed for the selector action address
     */
    uint32_t get_selector_action_address(Phv *phv, int logical_table, int alu, uint32_t sel_len);

    /** get the meter precolor via the hash generator
     */
    uint8_t get_meter_precolor(Phv *phv, int logical_table);

    // get the just the least significant or most significant portion of the immediate data hash
    //   used by DV to check against the RTL
    uint32_t get_immediate_data_ls_hash(Phv *phv, int logical_table);
    uint32_t get_immediate_data_ms_hash(Phv *phv, int logical_table);

    // only used by DV, who don't know the sel_len
    uint32_t get_selector_address(Phv *phv, int logtab);
    uint32_t get_selector_action_address(Phv *phv, int logtab);

   private:

    // get one of the two 48 bit hashes
    uint64_t  get_48b_hash(Phv *phv, int which);
    // get one of the six 16bit hashes - can now be 23b wide because of expansion
    uint32_t  get_16b_hash(Phv *phv, int which);
    // get one of the six 16b hashes after masking and shifting (can be 23 bits wide)
    uint32_t get_16b_hash_masked_and_shifted(Phv *phv, int which);

    // get the final hashes just before they get masked to be the outputs
    uint32_t get_final_hash(Phv *phv, int logical_table, MauHashGroupResultType result_type);

    // selector_shift = selector_len[7:5] and selector_numword = selector_len[4:0]
    int selector_numword(uint32_t selector_len);
    int selector_shift(uint32_t selector_len, bool selector_mod_is_vpn=false);
    
    uint32_t selector_h1_hash(Phv *phv, int alu) {
      return 0x7fff & get_final_hash(phv,alu,MauHashGroupResultType::HashModDividend);
    }
    uint32_t selector_addresses_enabled(int alu) {
      int  index;
      bool enable;
      registers_.bus_hash_group_xbar_ctl(alu,MauHashGroupResultType::HashModDividend,
                                         &enable,&index);
      return enable;
    }
    bool selector_mod_is_vpn(int alu, uint8_t addr_type) { 
      return registers_.selector_mod_is_vpn(alu, addr_type);
    }
    uint32_t selector_dividend(int alu, uint8_t adrtype, uint32_t hash_input) {
      return registers_.selector_dividend(alu, adrtype, hash_input);
    }
    uint8_t selector_divisor(int alu, uint8_t adrtype, uint32_t selector_len) {
      return registers_.selector_divisor(alu, adrtype, selector_len);
    }
    uint32_t selector_hash_mod_len(Phv *phv, int alu, uint8_t adrtype, uint32_t selector_len) {
      uint32_t dividend = selector_dividend(alu, adrtype, selector_h1_hash(phv, alu));
      uint8_t  divisor = selector_divisor(alu, adrtype, selector_len);
      return (divisor == 0) ?0u :dividend % divisor;
    }

   private:
    std::array<CacheId,2>                  get_48b_cache_id_;
    std::array<CacheId,kHashDistribGroups> get_16b_cache_id_;

    // cached values of the internal state
    uint64_t  hash_48b_[2]; 
    uint32_t  hash_16b_[kHashDistribGroups];

    MauHashDistributionRegs registers_;
  };

}


#endif
