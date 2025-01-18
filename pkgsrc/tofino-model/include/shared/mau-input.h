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

#ifndef _SHARED_MAU_INPUT_
#define _SHARED_MAU_INPUT_

#include <string>
#include <cstdint>
#include <rmt-defs.h>
#include <mau-defs.h>
#include <cache-id.h>
#include <phv.h>
#include <phv-pipe-data.h>
#include <mau-hash-generator-with-reg.h>
#include <mau-input-xbar-with-reg.h>


namespace MODEL_CHIP_NAMESPACE {

  class Mau;

  class MauInput {
 public:
    static constexpr int  kExactMatchInputBits = MauDefs::kExactMatchInputBits;
    static constexpr int  kExactMatchValidBits = MauDefs::kExactMatchValidBits;
    static constexpr int  kTernaryMatchInputBits = MauDefs::kTernaryMatchInputBits;
    static constexpr int  kTernaryMatchValidBits = MauDefs::kTernaryMatchValidBits;
    static constexpr int  kTotalMatchInputBits = MauDefs::kTotalMatchInputBits;
    static constexpr int  kTotalMatchValidBits = MauDefs::kTotalMatchValidBits;

    static constexpr int  kTotalInputBits = kExactMatchInputBits + kTernaryMatchInputBits;
    static constexpr int  kTotalValidBits = kExactMatchValidBits + kTernaryMatchValidBits;

    MauInput(int chipIndex, int pipeIndex, int mauIndex, Mau *mau)
        : mau_index_(mauIndex),
          hash_generator_(chipIndex, pipeIndex, mauIndex, mau),
          input_xbar_(chipIndex, pipeIndex, mauIndex, mau),
          get_input_cache_id_() {
    }
    ~MauInput() {}

    inline MauHashGeneratorWithReg *get_hash_generator() {
      return &hash_generator_;
    }

   private:
    inline void store_data_in_phv(Phv *phv, int what, int base, int data_bits) {
      int off = 0;
      while (off < data_bits) { // Copy upto 64b from cache->PHV
        phv->set_pipe_data(mau_index_, what, off, cached_total_input_.get_word(base + off), 64);
        off += 64;
      }
    }
    inline void load_data_from_phv(Phv *phv, int what, int base, int data_bits) {
      int off = 0;
      while (off < data_bits) { // Copy upto 64b from PHV->cache
        cached_total_input_.set_word(phv->get_pipe_data(mau_index_, what, off, 64), base + off, 64);
        off += 64;
      }
    }
    inline void cache_load(Phv *phv) {
      // Refresh entire cache unconditionally (unless phv cache_id hit)
      // Later we may overwrite XM/TM sections of cached input if they are marked LoadFromPhv
      //
      if (get_input_cache_id_.IsValid() && get_input_cache_id_.Equals(phv->cache_id())) {
        // PHV cache id matches cache cache id - do nothing
      } else {
        // Recalculate output and cache - record PHV cache id
        input_xbar_.CalculateOutput(*phv, cached_total_input_, cached_total_valid_);
        get_input_cache_id_.SetFrom(phv->cache_id());
      }
    }
    inline void maybe_phv_load_store(Phv *phv) {
      // Examine Ixbar XM/TM Ctrl flags in Phv for *this* MAU
      uint8_t xm_dat_ctl = phv->get_pipe_data_ctrl(mau_index_, PhvData::kIxbarXmData);
      uint8_t tm_dat_ctl = phv->get_pipe_data_ctrl(mau_index_, PhvData::kIxbarTmData);
      // Bail out early if we're just calculating data as normal
      if (PhvDataCtrl::do_calc(xm_dat_ctl) && PhvDataCtrl::do_calc(tm_dat_ctl)) return;

      // Separately for XM/TM sections of cached MAU Input Data:
      // 1. If CalcAndStore write relevant BV portion from Cache to PhvPipeData
      if (PhvDataCtrl::do_store(xm_dat_ctl))
        store_data_in_phv(phv, PhvData::kIxbarXmData, 0, kExactMatchInputBits);
      if (PhvDataCtrl::do_store(tm_dat_ctl))
        store_data_in_phv(phv, PhvData::kIxbarTmData,
                          kExactMatchInputBits, kTernaryMatchInputBits);

      // 2. If LoadFromPhv read relevant BV portion into Cache from PhvPipeData
      if (PhvDataCtrl::do_load(xm_dat_ctl))
        load_data_from_phv(phv, PhvData::kIxbarXmData, 0, kExactMatchInputBits);
      if (PhvDataCtrl::do_load(tm_dat_ctl))
        load_data_from_phv(phv, PhvData::kIxbarTmData,
                           kExactMatchInputBits, kTernaryMatchInputBits);

      // If loading both XM and TM sections of cached MAU Input Data and our PHV
      // has a cache_id then record PHV cache_id else invalidate cache
      if (PhvDataCtrl::do_load(xm_dat_ctl) && PhvDataCtrl::do_load(tm_dat_ctl) &&
          phv->cache_id().IsValid()) {
        get_input_cache_id_.SetFrom(phv->cache_id());
      } else {
        get_input_cache_id_.Invalidate();
      }
    }

   public:
    inline void get_exact_match_input(Phv *phv,
                                      BitVector<kExactMatchInputBits> *input_bits,
                                      BitVector<kExactMatchValidBits> *valid_bits) {
      cache_load(phv);
      maybe_phv_load_store(phv);
      // Extract XM bits
      cached_total_input_.extract_into(0, input_bits);
      cached_total_valid_.extract_into(0, valid_bits);
    }
    inline void get_ternary_match_input(Phv *phv,
                                        BitVector<kTernaryMatchInputBits> *input_bits,
                                        BitVector<kTernaryMatchValidBits> *valid_bits) {
      cache_load(phv);
      maybe_phv_load_store(phv);
      // Extract TM bits (they start at offset kExactMatchInputBits)
      cached_total_input_.extract_into(kExactMatchInputBits, input_bits);
      cached_total_valid_.extract_into(kExactMatchValidBits, valid_bits);
    }
    inline void get_total_match_input(Phv *phv,
                                      BitVector<kTotalMatchInputBits> *input_bits,
                                      BitVector<kTotalMatchValidBits> *valid_bits) {
      cache_load(phv);
      maybe_phv_load_store(phv);
      // Extract ALL bits
      cached_total_input_.extract_into(0, input_bits);
      cached_total_valid_.extract_into(0, valid_bits);
    }

 private:
    int                                mau_index_;
    MauHashGeneratorWithReg            hash_generator_;
    MauInputXbarWithReg                input_xbar_;
    CacheId                            get_input_cache_id_;
    BitVector<kTotalMatchInputBits>    cached_total_input_;
    BitVector<kTotalMatchValidBits>    cached_total_valid_;
  };
}
#endif // _SHARED_MAU_INPUT_
