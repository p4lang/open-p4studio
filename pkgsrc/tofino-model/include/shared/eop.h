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

#ifndef _SHARED_EOP_
#define _SHARED_EOP_

#include <rmt-defs.h>
#include <mau-defs.h>
#include <time-info.h>

namespace MODEL_CHIP_NAMESPACE {

  class Eop {
 public:
    Eop()  { }
    ~Eop() { }

    static constexpr uint8_t kFlagIngressErr = 0x1;
    static constexpr uint8_t kFlagEgressErr  = 0x2;
    static constexpr int     kNumEopMax = MauDefs::kNumEopAddrs;
    static_assert( (kNumEopMax < 0xFF), "EOP 0xFF reserved to be invalid");

    static inline bool valid_eopnum(uint8_t eopnum) {
      return (eopnum < kNumEopMax);
    }
    static inline uint8_t make_eopnum(uint8_t src_port, bool resubmit) {
      return ((src_port & 0x7F) << 1) | (resubmit ? 1 : 0);
    }


    inline uint16_t ingress_pktlen()  const { return ingress_pktlen_; }
    inline uint8_t  ingress_eopnum()  const { return ingress_eopnum_; }
    inline bool     ingress_eoperr()  const { return ((flags_ & kFlagIngressErr) != 0); }
    inline bool     ingress_valid()   const { return valid_eopnum(ingress_eopnum_); }
    inline uint16_t egress_pktlen()   const { return egress_pktlen_; }
    inline uint8_t  egress_eopnum()   const { return egress_eopnum_; }
    inline bool     egress_eoperr()   const { return ((flags_ & kFlagEgressErr) != 0); }
    inline bool     egress_valid()    const { return valid_eopnum(egress_eopnum_); }
    inline bool     valid()           const { return ingress_valid() || egress_valid(); }

    inline void set_ingress_eoperr(const bool eoperr) {
      if (!ingress_valid()) return;
      if (eoperr) flags_ |= kFlagIngressErr; else flags_ &= ~kFlagIngressErr;
    }
    inline void set_ingress_eopinfo(const uint16_t pktlen, const uint8_t eopnum,
                                    const bool eoperr=false) {
      if (!valid_eopnum(eopnum)) return;
      ingress_pktlen_ = pktlen;
      ingress_eopnum_ = eopnum;
      set_ingress_eoperr(eoperr);
    }

    inline void set_egress_eoperr(const bool eoperr) {
      if (!egress_valid()) return;
      if (eoperr) flags_ |= kFlagEgressErr; else flags_ &= ~kFlagEgressErr;
    }
    inline void set_egress_eopinfo(const uint16_t pktlen, const uint8_t eopnum,
                                   const bool eoperr=false) {
      if (!valid_eopnum(eopnum)) return;
      egress_pktlen_ = pktlen;
      egress_eopnum_ = eopnum;
      set_egress_eoperr(eoperr);
    }

    inline void set_eopinfo(const uint16_t pktlen, const uint8_t eopnum,
                            bool ingress_eopinfo, const bool eoperr=false) {
      if (ingress_eopinfo)
        set_ingress_eopinfo(pktlen, eopnum, eoperr);
      else
        set_egress_eopinfo(pktlen, eopnum, eoperr);
    }

    // access to time_info
    // use set_mau_tick_time and set_mau_random_value to set
    //  specific values to match the RTL simulation
    void set_meter_tick_time(int mau_index,int meter_index,uint64_t tick_time) {
      time_info_.set_meter_tick_time(mau_index,meter_index,tick_time);
    }
    void set_meter_random_value(int mau_index,int meter_index, uint64_t random_value) {
      time_info_.set_meter_random_value(mau_index,meter_index,random_value);
    }
    // use this to initialise to plausible values, first_tick_time is
    //  used as the tick time for the first mau and the seed for the
    //  random numbers (it doesn't bother to do the random accurately)
    void setup_time_info(uint64_t first_tick_time) {
      time_info_.set_all(first_tick_time);
    }

    uint64_t get_meter_tick_time(int mau_index, int meter_index) {
      return time_info_.get_meter_tick_time(mau_index,meter_index);
    }
    uint64_t get_meter_random_value(int mau_index, int meter_index) {
      return time_info_.get_meter_random_value(mau_index,meter_index);
    }

    void set_time_info_from(const TimeInfoType& other) {
      time_info_.set_from(other);
    }
    const TimeInfoType &get_time_info() const {
      return time_info_;
    }
    bool is_before(TimeInfoType& other) const {
      return time_info_.is_before(other);
    }

    void set_relative_time(uint64_t relative_time) {
      time_info_.set_relative_time(relative_time);
    }
    bool relative_time_valid()   const { return time_info_.relative_time_valid(); }
    uint64_t get_relative_time() const { return time_info_.get_relative_time(); }

 private:
    uint16_t      ingress_pktlen_ = 0;
    uint16_t      egress_pktlen_ = 0;
    uint8_t       ingress_eopnum_ = 0xFF;
    uint8_t       egress_eopnum_ = 0xFF;
    uint8_t       flags_ = 0;
    TimeInfoType  time_info_{};
  };
}

#endif // _SHARED_EOP_
