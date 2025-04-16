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

#ifndef _SHARED_HASH_ADDRESS_VH_XBAR_WITH_REG_H__
#define _SHARED_HASH_ADDRESS_VH_XBAR_WITH_REG_H__

#include <array>
#include <cstdint>
#include <rmt-log.h>
#include <rmt-defs.h>
#include <rmt-object.h>
#include <bitvector.h>
#include <mau-hash-generator-with-reg.h>
#include <hash-address-vh-xbar.h>
#include <cache-id.h>
#include <common/rmt-util.h>
#include <register_includes/exactmatch_bank_enable_array.h>
#include <register_includes/exactmatch_row_hashadr_xbar_ctl_array.h>
#include <register_includes/exactmatch_mem_hashadr_xbar_ctl_array.h>
#include <register_includes/alu_hashdata_bytemask.h>

namespace MODEL_CHIP_NAMESPACE {

class HashAddressVhXbarWithReg {
public:
  static constexpr int kSramAddressWidth  = MauDefs::kSramAddressWidth;
  static constexpr int kSramRowsPerMau    = MauDefs::kSramRowsPerMau;
  static constexpr int kSramColumnsPerMau = MauDefs::kSramColumnsPerMau;
  static constexpr int kHashOutputWidth   = MauDefs::kHashOutputWidth;

  HashAddressVhXbarWithReg(int chipNumber, int pipe, int mau,int row,
                                      MauHashGeneratorWithReg* mau_hash_generator );

  ~HashAddressVhXbarWithReg() {};

  void CalculateHashAddress(
      int column,
      const MauHashGenerator::InputT& input,
      const MauHashGenerator::ValidT& input_valid,
      const CacheId& cache_id,
      bool* address_valid,
      BitVector<kSramAddressWidth> *address ) const
  {
    hash_address_vh_xbar.CalculateHashAddress(
        input,
        input_valid,
        ram_hash_address_xbar_control_[column],
        ram_hash_address_xbar_enable_[column],
        row_hash_address_xbar_control_,
        row_hash_address_xbar_enable_,
        masked_equals_mux_control_[column],
        masked_equals_mux_enable_[column],
        masked_equals_value_[column],
        masked_equals_mask_[column],
        mau_hash_generator_,
        cache_id,
        address_valid,
        address );
  }

  // this version is used by the gateway tables as they need the
  //  raw hash result
  void CalculateHash(
      const MauHashGenerator::InputT& input,
      const MauHashGenerator::ValidT& input_valid,
      const int which,
      const CacheId& cache_id,
      BitVector<kHashOutputWidth> *hash ) const
  {
    hash_address_vh_xbar.CalculateHash(
        input,
        input_valid,
        row_hash_address_xbar_control_,
        row_hash_address_xbar_enable_,
        which,
        mau_hash_generator_,
        cache_id,
        hash );
    if (which==2 || which==3) {
      hash->mask(alu_hashdata_masks_[which-2]);
    }
  }

private:
  MauHashGeneratorWithReg* mau_hash_generator_;
  const HashAddressVhXbar hash_address_vh_xbar;

  register_classes::ExactmatchMemHashadrXbarCtlArray mem_xbar_ctl_array_;
  register_classes::ExactmatchRowHashadrXbarCtlArray row_xbar_ctl_array_;
  register_classes::ExactmatchBankEnableArray bank_enable_array_;
  register_classes::AluHashdataBytemask alu_hashdata_bytemask_;

  template< class T > using ColumnArray = std::array<T,kSramColumnsPerMau>;
  std::array<int,4> row_hash_address_xbar_control_;
  std::array<bool,4> row_hash_address_xbar_enable_;
  ColumnArray<int> ram_hash_address_xbar_control_;
  ColumnArray<bool> ram_hash_address_xbar_enable_;
  ColumnArray<bool> masked_equals_mux_control_;
  ColumnArray<bool> masked_equals_mux_enable_;
  ColumnArray<BitVector<HashAddressVhXbar::kMaskedEqualsWidth>> masked_equals_value_;
  ColumnArray<BitVector<HashAddressVhXbar::kMaskedEqualsWidth>> masked_equals_mask_;

  std::array<BitVector<kHashOutputWidth>,2> alu_hashdata_masks_;
 // register callbacks

  void MemXbarCtrlArrayCallback(uint32_t a0) {
    RMT_ASSERT( a0 < kSramColumnsPerMau );
    ram_hash_address_xbar_control_[a0] = mem_xbar_ctl_array_.enabled_4bit_muxctl_select( a0 );
    ram_hash_address_xbar_enable_[a0]  = mem_xbar_ctl_array_.enabled_4bit_muxctl_enable( a0 );
  }
  void RowXbarCtrlArrayCallback(uint32_t a0) {
    //In bfnregs 20150107_182406_7982_mau_dev this can take vals up to 4
    //where 2 and 3 indicate Selector ALU hash buses 0 and 1
    RMT_ASSERT( a0 < 4);
    if (a0 < 4) {
      row_hash_address_xbar_control_[a0] = row_xbar_ctl_array_.enabled_3bit_muxctl_select( a0 );
      row_hash_address_xbar_enable_[a0]  = row_xbar_ctl_array_.enabled_3bit_muxctl_enable( a0 );
    }
  }
  void BankEnableArrayCallback( uint32_t a0 ) {
    masked_equals_value_[a0].set32( 0, bank_enable_array_.exactmatch_bank_enable_bank_id(a0) );
    masked_equals_mask_[a0].set32(0, bank_enable_array_.exactmatch_bank_enable_bank_mask(a0));
    // TODO: what is the ecoding of the two bit inp_sel? Guess one hot for now
    int v = bank_enable_array_.exactmatch_bank_enable_inp_sel(a0);
    masked_equals_mux_control_[a0] = v & 2;
    masked_equals_mux_enable_[a0] = v != 0;
  }

  void AluHashdataBytemaskCallback( ) {
    uint8_t byte_mask[2];
    byte_mask[0] = alu_hashdata_bytemask_.alu_hashdata_bytemask_left();
    byte_mask[1] = alu_hashdata_bytemask_.alu_hashdata_bytemask_right();
    for (int i=0;i<2;++i) {
      for (int b=0;(b*8)<kHashOutputWidth;++b) {
        uint8_t val = (byte_mask[i] & (1<<b)) ? 0xff : 0;
        alu_hashdata_masks_[i].set_byte(val,b);
      }

    }
  }

private:
  DISALLOW_COPY_AND_ASSIGN(HashAddressVhXbarWithReg);
};

}

#endif // _SHARED_HASH_ADDRESS_VH_XBAR_WITH_REG_H__
