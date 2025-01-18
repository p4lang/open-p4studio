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

#ifndef _PARSER_ARBITER_COMMON_
#define _PARSER_ARBITER_COMMON_

#include <functional>
#include <common/rmt-util.h>
#include <pipe-object.h>
#include <eop.h>

namespace MODEL_CHIP_NAMESPACE {

using namespace model_common;

template<typename REGISTER_CLASS>
class ParserArbiterCommon : public PipeObject {
 public:
  // chip-agnostic enumeration of counter types; each chip type uses different
  // enums for counter types, so this generic enum is used by the constructor
  // to specify the counter type when calling the chip specific create_counter
  // function
  enum CtrEnum {
    EEopCount,
    EPhvCount,
    IEopCount,
    INormEopCount,
    INormPhvCount,
    IPhvCount,
    IResubEopCount,
    IResubPhvCount,
    CtrEnumUnknown
  };

  // subclasses must implement a function of this type to pass to the
  // constructor for creating instances of phv counters
  typedef std::function<REGISTER_CLASS(int,      // chipIndex
                                       int,      // pipeIndex
                                       CtrEnum)> // chip-agnostic counter enum
      create_counter_fn_t;

  // the common constructor calls subclass create_counter_fn to get chip
  // specific register instances of type REGISTER_CLASS
  ParserArbiterCommon(RmtObjectManager *om,
             int pipeIndex,
             create_counter_fn_t create_counter_fn) :
      PipeObject(om, pipeIndex),
      e_eop_count_(
          create_counter_fn(chip_index(), pipeIndex, CtrEnum::EEopCount)),
      e_phv_count_(
          create_counter_fn(chip_index(), pipeIndex, CtrEnum::EPhvCount)),
      i_eop_count_(
          create_counter_fn(chip_index(), pipeIndex, CtrEnum::IEopCount)),
      i_norm_eop_count_(
          create_counter_fn(chip_index(), pipeIndex, CtrEnum::INormEopCount)),
      i_norm_phv_count_(
          create_counter_fn(chip_index(), pipeIndex, CtrEnum::INormPhvCount)),
      i_phv_count_(
          create_counter_fn(chip_index(), pipeIndex, CtrEnum::IPhvCount)),
      i_resub_eop_count_(
          create_counter_fn(chip_index(), pipeIndex, CtrEnum::IResubEopCount)),
      i_resub_phv_count_(
          create_counter_fn(chip_index(), pipeIndex, CtrEnum::IResubPhvCount)) {
    reset();
  }

  ~ParserArbiterCommon() {}

  void increment_e_eop_count(uint16_t n_bytes) {
    inc_count(e_eop_count_, n_bytes);
  }
  void increment_e_phv_count() {
    inc_count(e_phv_count_, UINT64_C(1));
  }
  void increment_i_eop_count(uint16_t n_bytes) {
    inc_count(i_eop_count_, n_bytes);
  }
  void increment_i_norm_eop_count(uint16_t n_bytes) {
    inc_count(i_norm_eop_count_, n_bytes);
  }
  void increment_i_norm_phv_count() {
    inc_count(i_norm_phv_count_, UINT64_C(1));
  }
  void increment_i_phv_count() {
    inc_count(i_phv_count_, UINT64_C(1));
  }
  void increment_i_resub_eop_count(uint16_t n_bytes) {
    inc_count(i_resub_eop_count_, n_bytes);
  }
  void increment_i_resub_phv_count() {
    inc_count(i_resub_phv_count_, UINT64_C(1));
  }

  void handle_phv(Phv *phv, bool egress, bool is_resubmit=false) {
    if (egress) {
      increment_e_phv_count();
    } else {
      increment_i_phv_count();
      is_resubmit ? increment_i_resub_phv_count() :
                    increment_i_norm_phv_count();
    }
  }

  void handle_eop(Eop *eop, bool is_resubmit=false) {
    if (eop->ingress_valid()) {
      uint16_t pktlen = eop->ingress_pktlen();
      increment_i_eop_count(pktlen);
      is_resubmit ? increment_i_resub_eop_count(pktlen) :
                    increment_i_norm_eop_count(pktlen);
    }
    if (eop->egress_valid()) {
      increment_e_eop_count(eop->egress_pktlen());
    }
  }

  void reset() {
    e_eop_count_.reset();
    e_phv_count_.reset();
    i_eop_count_.reset();
    i_norm_eop_count_.reset();
    i_norm_phv_count_.reset();
    i_phv_count_.reset();
    i_resub_eop_count_.reset();
    i_resub_phv_count_.reset();
  }

 private:
  void inc_count(REGISTER_CLASS &counter, uint64_t n) {
    counter.phv_cnt(
      Util::increment_and_wrap(counter.phv_cnt(), wrap_counter_size_, n));
  }

  int wrap_counter_size_ = 64;
  REGISTER_CLASS e_eop_count_;
  REGISTER_CLASS e_phv_count_;
  REGISTER_CLASS i_eop_count_;
  REGISTER_CLASS i_norm_eop_count_;
  REGISTER_CLASS i_norm_phv_count_;
  REGISTER_CLASS i_phv_count_;
  REGISTER_CLASS i_resub_eop_count_;
  REGISTER_CLASS i_resub_phv_count_;

};

}
#endif // _PARSER_ARBITER_COMMON_
