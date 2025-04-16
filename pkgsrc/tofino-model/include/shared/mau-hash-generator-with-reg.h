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

#ifndef _SHARED_MAU_HASH_GENERATOR_WITH_REG_H_
#define _SHARED_MAU_HASH_GENERATOR_WITH_REG_H_
#include <array>
#include <cstdint>
#include <rmt-log.h>
#include <rmt-defs.h>
#include <rmt-object.h>
#include <bitvector.h>
#include <cache-id.h>
#include <mau-hash-generator.h>
#include <register_adapters.h>
#include <mcn_test.h>
#include <register_includes/parity_group_mask_array2.h>
#include <register_includes/galois_field_matrix_array2.h>
#include <register_includes/hash_seed_array.h>

namespace MODEL_CHIP_NAMESPACE {

class MauHashGeneratorWithReg {
public:
  using OutputT                 = BitVector<MauHashGenerator::kOutputWidth>;

MauHashGeneratorWithReg( int chipNumber, int pipe, int mauIndex, Mau *mau) :
    mau_(mau),
    galois_field_matrix_(default_adapter(galois_field_matrix_,chipNumber, pipe, mauIndex,[this](uint32_t a1,uint32_t a0){this->GaloisFieldMatrixCallback(a1,a0);})),
    hashgroup_config_(default_adapter(hashgroup_config_,chipNumber, pipe, mauIndex, [this](uint32_t a1,uint32_t a0){this->HashGroupConfigCallback(a1,a0);})),
    hash_seed_(default_adapter(hash_seed_,chipNumber, pipe, mauIndex,[this](uint32_t a0){this->HashSeedCallback(a0);}))
    {
      galois_field_matrix_.reset();
      hashgroup_config_.reset();
      hash_seed_.reset();
    }

    ~MauHashGeneratorWithReg() {}

  /** Calculate the output of a hash group, outputs are cached using cache_id
   *
   */
  void CalculateOutput(const MauHashGenerator::InputT& input_bytes,
                       const MauHashGenerator::ValidT& valid_bits,
                       const int group,
                       const CacheId& cache_id,
                       OutputT* output);

  /** Calculate one output bit for a hash group, use this if you only want some bits
   *
   */
  bool CalculateOutputBit(const MauHashGenerator::InputT& input_bytes,
                          const MauHashGenerator::ValidT& valid_bits,
                          const int group, const int output_bit) const {
    return mau_hash_generator_.CalculateOutputBit(input_bytes,valid_bits,
                                                  galois_field_matrix_bv_,
                                                  galois_field_valid_matrix_bv_,
                                                  hashgroup_config_bv_,
                                                  hash_seed_bv_,
                                                  group,output_bit);
  }

  // register callbacks
  void GaloisFieldMatrixCallback(uint32_t a1,uint32_t a0);

  void HashGroupConfigCallback(uint32_t a1,uint32_t a0) {
    // hashgroup_config_bv_ used to mirror the register format, but the register
    //  was changed from 8 x 16bit to 8 x 2 x 8 bit, convert to the old format here
    constexpr int reg_width = MauHashGenerator::kFirstStages / 2;
    hashgroup_config_bv_[a1].set_word( hashgroup_config_.parity_group_mask(a1,a0),
                                       a0 ? reg_width:0, // start offset
                                       reg_width );
  }
  void HashSeedCallback(uint32_t a0) {
    // this used to be 8 x 2 x 26 bits, now 52 x 8 so do the conversion here
    //
    for (int i=0;i<8;++i) {
      hash_seed_bv_[i].set_word( (hash_seed_.hash_seed(a0)>>i) &  1, a0, 1);
    }
  }


private:
  MauHashGenerator mau_hash_generator_;
  Mau* mau_;
  std::array<CacheId,MauHashGenerator::kGroups> current_cache_id_ {};
  std::array<OutputT,MauHashGenerator::kGroups> cache_ {};

  // registers
  register_classes::GaloisFieldMatrixArray2 galois_field_matrix_;
  register_classes::ParityGroupMaskArray2   hashgroup_config_;
  register_classes::HashSeedArray           hash_seed_;

  MauHashGenerator::GaloisFieldMatrixT galois_field_matrix_bv_;
  MauHashGenerator::GaloisFieldValidMatrixT galois_field_valid_matrix_bv_;
  MauHashGenerator::Hashgroup_ConfigT  hashgroup_config_bv_;
  MauHashGenerator::HashSeedT          hash_seed_bv_;

private:
  DISALLOW_COPY_AND_ASSIGN(MauHashGeneratorWithReg);
};
}
#endif
