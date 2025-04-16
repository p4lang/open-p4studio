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

#ifndef _SHARED_MAU_EXECUTE_STEP_
#define _SHARED_MAU_EXECUTE_STEP_

#include <address.h>
#include <eop.h>
#include <teop.h>
#include <evict-info.h>

namespace MODEL_CHIP_NAMESPACE {


enum StateOp {
  kStateOpPhvLookup, kStateOpHandleEop, kStateOpHandleTeop,
  kStateOpPbusRd, kStateOpPbusWr, kStateOpDump, kStateOpSweep,
  kStateOpStatefulClear
};    

class MauExecuteState {
public:
  // Flags used by distrib_pbus_address_early
  static constexpr uint8_t kFlagsDistribMeterAddrOnStatsBus = 0x01;
  static constexpr uint8_t kFlagsDistribMeterAddrOnIdleBus  = 0x02;
  static constexpr uint8_t kFlagsPbusReadWriteRsvd          = 0xF0; // See mau.h

  MauExecuteState(Phv* iphv, Phv* ophv, int ingress_next_table, int egress_next_table)
      : iphv_(iphv), ophv_(ophv),
        next_table_{{ingress_next_table,egress_next_table}},
        eop_(), data_(), evict_info_() {
    }

  Phv* const iphv_;
  Phv* const ophv_;
  std::array<int,2>  next_table_;           // ingress_next_table, egress_next_table
  Phv* match_phv_ = nullptr;
  Phv* action_phv_ = nullptr;
  Phv* next_ophv_ = nullptr;
  bool at_eop_ = false;
  Eop  eop_;
  bool at_teop_ = false;
  Teop teop_;
  BitVector<MauDefs::kDataBusWidth> data_;
  uint64_t  relative_time_ = UINT64_C(0);
  uint64_t  meter_tick_time_ = UINT64_C(0);
  uint64_t  meter_random_value_ = UINT64_C(0);
  uint32_t  addr_ = Address::invalid();
  uint8_t   addrtype_ = AddrType::kNone;
  uint8_t   logical_table_ = 0xFF;
  uint8_t   flags_ = 0;
  uint8_t   rd_color_ = 0;
  uint8_t   color_ = 0;
  int8_t    ret_ = -128;
  StateOp   op_ = kStateOpPhvLookup;
  uint32_t  rw_raddr_ = Address::invalid();
  int       rw_format_ = 0;
  EvictInfo evict_info_;
  std::array<std::list<std::pair<int, uint32_t>>, 8> salu_log_valuelist_;//SALU output details.
                                                // one value list per physical 
                                                // row assuming meteralu
                                                // on each row.
  std::array<bool,MauDefs::kNumStatefulAlus> match_bus_{};
  std::array<bool,MauDefs::kNumStatefulAlus> learn_or_match_bus_{};

  uint64_t get_meter_tick_time(int mau_index, int meter_index, bool ingress) {
    if ( at_teop_ ) {
      return teop_.get_meter_tick_time(mau_index,meter_index);
    }
    else if ( at_eop_ ) {
      return eop_.get_meter_tick_time(mau_index,meter_index);
    }
    else if ( match_phv_ ) {
      return match_phv_->get_meter_tick_time(mau_index,meter_index,ingress);
    }
    else {
      // for sweeps
      return meter_tick_time_;
    }
  }
  uint64_t get_meter_random_value(int mau_index, int meter_index, bool ingress) {
    if ( at_teop_ ) {
      return teop_.get_meter_random_value(mau_index,meter_index);
    }
    else if ( at_eop_ ) {
      return eop_.get_meter_random_value(mau_index,meter_index);
    }
    else if ( match_phv_ ) {
      return match_phv_->get_meter_random_value(mau_index,meter_index,ingress);
    }
    else {
      // TODO: this is reached by indirect writes leading to stateful instructions
      //   need to work out a way of passing the random numbers in on this path
      // make it return value out of structure for now rather than hardcoded 0
      return meter_random_value_;
    }
  }

  bool get_relative_time(uint64_t* time, bool ingress) {
    if ( at_teop_ ) {
      *time  = teop_.get_relative_time();
      return teop_.relative_time_valid();
    }
    else if ( at_eop_ ) {
      *time  = eop_.get_relative_time();
      return eop_.relative_time_valid();
    }
    else if ( match_phv_ ) {
      *time  = match_phv_->get_relative_time(ingress);
      return match_phv_->relative_time_valid(ingress);
    }
    else {
      // for sweep
      *time = relative_time_;
      return true;
    }
  }

  uint64_t get_phv_time(uint64_t *T_ing, uint64_t *T_egr) {
    if ((T_ing == NULL) || (T_egr == NULL) || (match_phv_ == NULL))
      return UINT64_C(0);
    *T_ing = *T_egr = UINT64_C(0);
    if (match_phv_->ingress_ghost() && match_phv_->relative_time_valid(true))
      *T_ing = match_phv_->get_relative_time(true);
    if (match_phv_->egress() && match_phv_->relative_time_valid(false))
      *T_egr = match_phv_->get_relative_time(false);
    uint64_t T = (*T_ing > *T_egr) ?*T_ing :*T_egr;
    return T;
  }


};
enum WhenToExecute {
  kOnlyAtHeaderTime,
  kOnlyAtEopTime,
  kOnlyAtTeopTime,
  kAtHeaderAndEopTime
};


class MauExecuteStep {
 public:
  MauExecuteStep(std::string name, WhenToExecute when) : name_(name), when_(when) {}
  virtual ~MauExecuteStep() {}
  virtual void execute(MauExecuteState* s) = 0;
  bool skip(MauExecuteState* s) {
    if ( ((when_ == kOnlyAtHeaderTime) && (s->at_eop_ || s->at_teop_)) ||
         ((when_ == kOnlyAtTeopTime) && !s->at_teop_) ||
         ((when_ == kOnlyAtEopTime) && !s->at_eop_) ) {
      return true;
    }
    return false;
  }
 protected:
  std::string name_;
  WhenToExecute when_;
};

class MauExecuteStepMauFunc : public MauExecuteStep {
  using FunctionPtr = void (Mau::*)(MauExecuteState*);
 public:
  MauExecuteStepMauFunc(std::string name,Mau* mau,FunctionPtr func,WhenToExecute when) :
      MauExecuteStep(name,when),
      mau_(mau),func_(func) {}
  void execute(MauExecuteState* s);
 protected:
  Mau* mau_;
  FunctionPtr func_;
};
class MauExecuteStepMauInEgFunc : public MauExecuteStep {
  using FunctionPtr = void (Mau::*)(bool,MauExecuteState*);
 public:
  MauExecuteStepMauInEgFunc(std::string name,Mau* mau,FunctionPtr func,WhenToExecute when) :
      MauExecuteStep(name,when),
      mau_(mau),func_(func) {}
  void execute(MauExecuteState* s);
 protected:
  Mau* mau_;
  FunctionPtr func_;
};


class MauExecuteStepMauAddrDistFunc : public MauExecuteStep {
  using FunctionPtr = void (MauAddrDist::*)(MauExecuteState*);
 public:
  MauExecuteStepMauAddrDistFunc(std::string name,Mau* mau,FunctionPtr func,WhenToExecute when) :
      MauExecuteStep(name,when),
      mau_(mau),func_(func) {}
  void execute(MauExecuteState* s);
 protected:
  Mau* mau_;
  FunctionPtr func_;
};


class MauExecuteStepRunRows : public MauExecuteStep {
  using FunctionPtr = void (MauSramRow::*)();
 public:
  MauExecuteStepRunRows(std::string name,Mau* mau,FunctionPtr func,WhenToExecute when) :
      MauExecuteStep(name,when),
      mau_(mau),func_(func) {}
  void execute(MauExecuteState* s);
 protected:
  Mau* mau_;
  FunctionPtr func_;
};
class MauExecuteStepRunRowsWithState : public MauExecuteStep {
  using FunctionPtr = void (MauSramRow::*)(MauExecuteState*);
 public:
  MauExecuteStepRunRowsWithState(std::string name,Mau* mau,FunctionPtr func,WhenToExecute when) :
      MauExecuteStep(name,when),
      mau_(mau),func_(func) {}
  void execute(MauExecuteState* s);
 protected:
  Mau* mau_;
  FunctionPtr func_;
};


// This runs predicated tables
class MauExecuteStepRunTables : public MauExecuteStep {
  using FunctionPtr = void (MauLogicalTable::*)(Phv*,MauLookupResult*,bool);
 public:
  MauExecuteStepRunTables(std::string name,Mau* mau,FunctionPtr func,WhenToExecute when) :
      MauExecuteStep(name,when),
      mau_(mau),func_(func) {}
  void execute(MauExecuteState* s);
 protected:
  Mau* mau_;
  FunctionPtr func_;
};
// This runs all ingress and egress tables for use at EOP
class MauExecuteStepRunTablesEop : public MauExecuteStep {
  using FunctionPtr = void (MauLogicalTable::*)(const Eop &);
 public:
  MauExecuteStepRunTablesEop(std::string name,Mau* mau,FunctionPtr func,WhenToExecute when) :
      MauExecuteStep(name,when),
      mau_(mau),func_(func) {
    RMT_ASSERT(when == kOnlyAtEopTime); // should only be run at EOP
  }
  void execute(MauExecuteState* s);
 protected:
  Mau* mau_;
  FunctionPtr func_;
};
class MauExecuteStepTableFunc : public MauExecuteStep {
  using FunctionPtr = void (MauLogicalTable::*)(MauExecuteState*);
 public:
  MauExecuteStepTableFunc(std::string name,Mau* mau,FunctionPtr func,WhenToExecute when) :
      MauExecuteStep(name,when),
      mau_(mau),func_(func) {}
  void execute(MauExecuteState* s);
 protected:
  Mau* mau_;
  FunctionPtr func_;
};


}
#endif // _SHARED_MAU_EXECUTE_STEP_
