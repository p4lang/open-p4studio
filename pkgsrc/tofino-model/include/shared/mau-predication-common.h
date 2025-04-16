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

#ifndef _SHARED_MAU_PREDICATION_COMMON_
#define _SHARED_MAU_PREDICATION_COMMON_

#include <string>
#include <cstdint>
#include <rmt-defs.h>
#include <mau-defs.h>
#include <mau-object.h>
#include <mau-io.h>
#include <nxt-tab.h>


namespace MODEL_CHIP_NAMESPACE {

  class Mau;
  class MauLookupResult;


  // Flags for call to lt_info()
  struct Pred {
    static constexpr uint16_t  kNone       = 0x0000; 
    static constexpr uint16_t  kIngress    = 0x0001; // Fundamental configured vals
    static constexpr uint16_t  kEgress     = 0x0002;
    static constexpr uint16_t  kGhost      = 0x0004;
    static constexpr uint16_t  kCounters   = 0x0008; 
    static constexpr uint16_t  kCountable  = 0x0010; // Calculated on start & stored
    static constexpr uint16_t  kLookupable = 0x0020; // (depend on ing/egr active
    static constexpr uint16_t  kActive     = 0x0040; //  and ing/egr nxt_tab)
    static constexpr uint16_t  kRunnable   = 0x0080;
    static constexpr uint16_t  kWarn       = 0x0100; 
    static constexpr uint16_t  kUsed       = 0x0200; // LTs in ing/egr/ght
    static constexpr uint16_t  kUnused     = 0x0400; // LTs *not* in ing/egr/ght
    static constexpr uint16_t  kUnpowered  = 0x0800; // LTs *not* lookupable
    static constexpr uint16_t  kIngThread  = 0x1000; // LTs in ing (or ing+ght on JBay)
    static constexpr uint16_t  kEgrThread  = 0x2000; // LTs in egr
    static constexpr uint16_t  kMpr        = 0x4000; // LTs from MPR IOs + mpr_always_active
    static constexpr uint16_t  kMprMask    = 0x8000; // LTs in *active* ing/egr/ght
    
    // Aliases
    static constexpr uint16_t  kIngActive  = kIngThread|kActive;
    static constexpr uint16_t  kEgrActive  = kEgrThread|kActive;
    static constexpr uint16_t  kPowered    = kUsed|kLookupable;
    static constexpr uint16_t  kActiveMask = kMprMask;
  };

  
  class MauPredicationCommon : public MauObject {

 public:
    static bool kRelaxPredicationCheck; // Defined in rmt-config.cpp
    static bool kAllowUnpoweredTablesToBecomeActive;
    static bool kPowerTablesForInactiveThreads;

    static constexpr int      kType = RmtTypes::kRmtTypeMauPredication;
    static constexpr int      kThreadIngress = 0;
    static constexpr int      kThreadEgress = 1;
    static constexpr int      kThreadGhost = 2; // JBay only
    static constexpr int      kThreads = 3;
    static constexpr int      kTables = MauDefs::kLogicalTablesPerMau;
    static constexpr uint16_t kLtAll = static_cast<uint16_t>(0xFFFFFFFFu >> (32-kTables));
    static constexpr uint8_t  kModeNone         = 0;
    static constexpr uint8_t  kModeEvaluateAll  = 1;
    static constexpr uint8_t  kModeShortCircuit = 2;
    static_assert( (kTables <= 16), "LTMask must fit in uint16_t");

    // Convenience funcs to get thread names
    static const char *thrd_str(int thrd) {
      const char *iegstr[4] = { "ingress", "egress", "ghost", "ALL" };
      return iegstr[thrd & 3];
    }
    static const char *thrdmask_str(uint8_t thrdmask) {
      const char *iegstr[8] = { "", "I", "E", "IE", "G", "IG", "EG", "IEG" };
      return iegstr[thrdmask & 7];
    }

    // Some bit-twiddling funcs
    static inline bool is_bit_set(uint16_t lts, int lt) {
      return ((lt >= 0) && (lt < kTables) && (((lts >> lt) & 1) == 1));
    }
    static inline int find_first(uint16_t lts) {
      return __builtin_ffs(lts) - 1;
    }
    static inline uint16_t mask_first(uint16_t lts)  {
      int lt = find_first(lts);
      return (lt >= 0) ?1<<lt :0;
    }
    static inline uint16_t mask_equal_or_above(int lt)  {
      return static_cast<uint16_t>( (kLtAll << lt) & kLtAll );
    }
    static inline uint16_t mask_above(int lt)  {
      return mask_equal_or_above(lt+1);
    }
    
    MauPredicationCommon(RmtObjectManager *om, int pipeIndex, int mauIndex, Mau *mau);
    virtual ~MauPredicationCommon();

    inline uint8_t get_run_mode()       const { return mode_; }
    inline void    set_run_mode(uint8_t mode) { mode_ = mode; }
    inline void    set_evaluate_all(bool tf) {
      set_run_mode(tf ?kModeEvaluateAll :kModeShortCircuit);
    }

    // PER-CHIP implementation
    virtual void start(bool thread_active[]) = 0;
    virtual void end() = 0;
    virtual int  get_next_table(bool ingress, int curr_lt, bool *do_lookup) = 0;
    virtual int  get_first_table(bool ingress, bool *do_lookup) = 0;
    virtual void set_next_table(int lt, const MauLookupResult &result) = 0;
    virtual uint16_t lt_info(uint16_t pred_sel) = 0;
    virtual uint16_t lts_active() = 0;
    
 private:
    uint8_t mode_;
  };
  
}

#endif // _SHARED_MAU_PREDICATION_COMMON_

