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
#include <rmt-log.h>
#include <mau-hash-generator-with-reg.h>

namespace MODEL_CHIP_NAMESPACE {

void MauHashGeneratorWithReg::CalculateOutput(const MauHashGenerator::InputT& input_bytes,
                       const MauHashGenerator::ValidT& valid_bits,
                       const int group,
                       const CacheId& cache_id,
                       OutputT* output) {
    if ( current_cache_id_[group].IsValid() && current_cache_id_[group].Equals(cache_id) ) {
      output->set_from( 0, cache_[group] );
    }
    else {
      for (int output_bit=0; output_bit< MauHashGenerator::kOutputWidth; ++output_bit) {
        bool b = mau_hash_generator_.CalculateOutputBit(input_bytes,valid_bits,
                                                        galois_field_matrix_bv_,
                                                        galois_field_valid_matrix_bv_,
                                                        hashgroup_config_bv_,
                                                        hash_seed_bv_,
                                                        group,output_bit);
        output->set_bit( b, output_bit );
      }
      current_cache_id_[group].SetFrom(cache_id);
      cache_[group].set_from( 0, *output );
    }
    RMT_LOG_OBJ(mau_, RmtDebug::verbose(), "MauHashGeneratorWithReg::CalculateOutput group=%d out=%s\n",
                group,output->to_string().c_str());
  }

}
