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

#ifndef _SHARED_MAU_HASH_GENERATOR_H_
#define _SHARED_MAU_HASH_GENERATOR_H_

#include <array>
#include <cstdint>
#include <rmt-log.h>
#include <rmt-object.h>
#include <mau-defs.h>
#include <bitvector.h>

namespace MODEL_CHIP_NAMESPACE {

class MauHashGenerator {
public:
  static constexpr int kInputBytes  = MauDefs::kHashInputBytes;
  static constexpr int kInputBits   = kInputBytes * 8;
  static constexpr int kOutputWidth = MauDefs::kHashOutputWidth;
  static constexpr int kGroups      = MauDefs::kHashGroups;
  static constexpr int kFirstStages = MauDefs::kHashFirstStages;
  static constexpr int kFirstStageBytes = kInputBytes / kFirstStages;
  static constexpr int kFirstStageBits  = kFirstStageBytes * 8;

  using InputT                  = BitVector<kInputBits>;
  using ValidT                  = BitVector<kInputBytes>;
  using GaloisFieldMatrixT      = std::array< BitVector<kInputBits>, kOutputWidth >;
  using GaloisFieldValidMatrixT = std::array< BitVector<kInputBytes>, kOutputWidth >;
  using Hashgroup_ConfigT       = std::array< BitVector<kFirstStages>, kGroups >;
  using HashSeedT               = std::array< BitVector<kOutputWidth>, kGroups >;

  MauHashGenerator() {}
  ~MauHashGenerator() {}

  inline bool CalculateOutputBit(const InputT& inputBytes,
                                 const ValidT& validBits,
                                 const GaloisFieldMatrixT& galois_field_matrix_bv,
                                 const GaloisFieldValidMatrixT& galois_field_valid_matrix_bv,
                                 const Hashgroup_ConfigT& hashgroup_config_bv,
                                 const HashSeedT& hash_seed_bv,
                                 const int group, const int output_bit) const {
    return ( hash_seed_bv[group].get_bit( output_bit ) ^
             CalculateOutputBit<kFirstStages-1>(inputBytes,validBits,
                                                galois_field_matrix_bv,
                                                galois_field_valid_matrix_bv,
                                                hashgroup_config_bv,group,output_bit) );
  }

  // templated function so that masked_parity can be called templated with constexpr values and get all the compiler goodness
  //  that comes from that.
  // start it off by calling with one minus the number of first stages
  template <int FIRST_STAGE> inline bool CalculateOutputBit(const InputT& inputBytes,
                                                            const ValidT& validBits,
                                                            const GaloisFieldMatrixT& galois_field_matrix_bv,
                                                            const GaloisFieldValidMatrixT& galois_field_valid_matrix_bv,
                                                            const Hashgroup_ConfigT& hashgroup_config_bv,
                                                            const int group, const int output_bit) const {
    bool p = false;
    // only do the hash for the first stage if the bit is set in the hashgroup_config register
    if ( hashgroup_config_bv[group].get_bit(FIRST_STAGE) ) {
      p = static_cast<bool>(inputBytes.masked_parity<FIRST_STAGE*kFirstStageBytes*8, kFirstStageBytes * 8>(galois_field_matrix_bv[output_bit]));
      p = (p != static_cast<bool>(validBits.masked_parity<FIRST_STAGE*kFirstStageBytes, kFirstStageBytes>(galois_field_valid_matrix_bv[output_bit])));
      //LOG_INFO("%d -> %d\n",FIRST_STAGE,p);
    }
    if (FIRST_STAGE > 0) {
      // need " ? : " in template arg to stop recursion in g++
      bool next = CalculateOutputBit<(FIRST_STAGE > 0) ? FIRST_STAGE - 1 : 0>(
          inputBytes,
          validBits,
          galois_field_matrix_bv,
          galois_field_valid_matrix_bv,
          hashgroup_config_bv,
          group,
          output_bit);
      p = (p != next);
    }
    return p;
  }

};

}

#endif
