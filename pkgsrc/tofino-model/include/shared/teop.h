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

#ifndef _SHARED_TEOP_
#define _SHARED_TEOP_

#include <common/rmt-assert.h>
#include <rmt-defs.h>
#include <mau-defs.h>
#include <time-info.h>
#include <address.h>

namespace MODEL_CHIP_NAMESPACE {

  class TeopEntry {
 private:
    static constexpr int     kLogicalTables = MauDefs::kLogicalTablesPerMau;
    static constexpr int     kStages        = RmtDefs::kStagesMax;    
    static_assert( (kStages < 0xFF),        "Too many MAUs");
    static_assert( (kLogicalTables < 0xFF), "Too many tables");
    static constexpr uint8_t kFlagStatsEn  = 0x01;
    static constexpr uint8_t kFlagMeterEn  = 0x02;
    static constexpr uint8_t kFlagStatsAdr = 0x10;
    static constexpr uint8_t kFlagMeterAdr = 0x20;
    
    static uint8_t addrtype_get_flag(uint8_t addrtype) {
      switch (addrtype) {
        case AddrType::kStats: return kFlagStatsAdr;
        case AddrType::kMeter: return kFlagMeterAdr;
        default:               return 0;
      }
    }
    static void check_addrtype(uint8_t flag) {
      RMT_ASSERT(((flag & kFlagStatsAdr) == 0) || ((flag & kFlagMeterAdr) == 0));
    }      
    static uint8_t flag_get_addrtype(uint8_t flag) {
      check_addrtype(flag);
      if      ((flag & kFlagStatsAdr) != 0) return AddrType::kStats;
      else if ((flag & kFlagMeterAdr) != 0) return AddrType::kMeter;
      else                                  return AddrType::kNone;
    }

 public:
    TeopEntry()   { reset(); }
    ~TeopEntry()  { reset(); }
    
    inline void reset() {
      addr_ = Address::invalid();
      color_ = 0;
      flags_ = 0;
      stage_ = 0xFF; lt_ = 0xFF; // Just for debug
    }
    // Getters
    inline uint32_t addr()      const { return addr_; };
    inline uint8_t  color()     const { return color_; };
    inline bool     stats_en()  const { return ((flags_ & kFlagStatsEn) != 0); }
    inline bool     meter_en()  const { return ((flags_ & kFlagMeterEn) != 0); }
    inline bool     in_use()    const { return stats_en() || meter_en(); }
    inline int      stage()     const { return in_use() ?static_cast<int>(stage_) :-1; }
    inline int      lt()        const { return in_use() ?static_cast<int>(lt_) :-1; }
    inline uint8_t  addrtype()  const { return flag_get_addrtype(flags_); }
    inline bool     ok_to_use(int st, int lt, uint8_t at) const {
      if (!in_use() || (stage() == 0xFF)) return true;
      if ((stage() == st) && (addrtype() == at)) return true;
      return false;
    }
    // Setters
    inline void set_stats_addr(uint32_t stats_addr, bool meter_too) {
      RMT_ASSERT(!in_use()); // Forbid re-use
      addr_ = stats_addr; color_ = 0; flags_ = kFlagStatsEn|kFlagStatsAdr;
      if (meter_too) flags_ |= kFlagMeterEn;
    }
    inline void set_meter_addr(uint32_t meter_addr, bool stats_too, uint8_t color=0) {
      RMT_ASSERT(!in_use()); // Forbid re-use
      addr_ = meter_addr; color_ = color; flags_ = kFlagMeterEn|kFlagMeterAdr;
      if (stats_too) flags_ |= kFlagStatsEn;
    }
    inline void set_addr(uint32_t addr, bool stats_en, bool meter_en, uint8_t color=0) {
      addr_ |= addr; color_ |= color; // But allow re-use here
      if (stats_en) flags_ |= kFlagStatsEn;
      if (meter_en) flags_ |= kFlagMeterEn;
    }
    inline void set_addrtype(uint8_t addrtype) {
      flags_ |= addrtype_get_flag(addrtype);
      check_addrtype(flags_);
    }
    inline void set_stage_lt(int stage, int lt) {
      RMT_ASSERT(in_use());
      RMT_ASSERT((stage >= 0) && (stage < kStages));
      RMT_ASSERT((lt >= 0) && (lt < kLogicalTables));
      stage_ = static_cast<uint8_t>(stage);
      lt_ = static_cast<uint8_t>(lt);
    }
    inline void set_stage_lt(int stage, int lt, uint8_t addrtype) {
      RMT_ASSERT(ok_to_use(stage, lt, addrtype));
      set_stage_lt(stage, lt);
      set_addrtype(addrtype);
    }

    inline void clear_stats_en() {
      flags_ &= ~kFlagStatsEn;
    }
    inline void clear_meter_en() {
      flags_ &= ~kFlagMeterEn;
    }
    
 private:
    uint32_t  addr_;
    uint8_t   color_;
    uint8_t   flags_;
    uint8_t   stage_; // Just for debug
    uint8_t   lt_;    // Just for debug
  };

  

  class Teop {
 public:
    static constexpr int     kNumTeopBuses   = MauDefs::kNumTeopBuses;
    static constexpr int     kTeopBusArraySz = (kNumTeopBuses > 0) ?kNumTeopBuses :1;
    static constexpr uint8_t kFlagHdrTime  = 0x01;
    static constexpr uint8_t kFlagTeopTime = 0x02;
    static constexpr uint8_t kErrFCS       = 0x01;
    static constexpr uint8_t kErrTrunc     = 0x02;

    Teop()  { reset(); }
    ~Teop() { reset(); }

    inline void reset() {
      flags_ = 0; errors_ = 0; byte_len_ = 0;
      for (int i = 0; i < kTeopBusArraySz; i++) teop_buses_[i].reset();
      flags_ = kFlagHdrTime;
    }
    // Getters
    inline bool     hdr_time()       const { return ((flags_ & kFlagHdrTime) != 0); }
    inline bool     teop_time()      const { return ((flags_ & kFlagTeopTime) != 0); }
    inline uint8_t  errors()         const { return errors_; }
    inline bool     error_fcs()      const { return ((errors_ & kErrFCS) != 0); }
    inline bool     error_trunc()    const { return ((errors_ & kErrTrunc) != 0); }
    inline uint16_t byte_len()       const { return byte_len_; }
    inline uint32_t addr(int i)      const { return teop_buses_[i].addr(); };
    inline uint8_t  color(int i)     const { return teop_buses_[i].color(); };
    inline bool     stats_en(int i)  const { return teop_buses_[i].stats_en(); }
    inline bool     meter_en(int i)  const { return teop_buses_[i].meter_en(); }
    inline bool     in_use(int i)    const { return teop_buses_[i].in_use(); }
    inline int      stage(int i)     const { return teop_buses_[i].stage(); }
    inline int      lt(int i)        const { return teop_buses_[i].lt(); }
    inline int      addrtype(int i)  const { return teop_buses_[i].addrtype(); }
    inline bool     ok_to_use(int i, int stage, int lt, uint8_t addrtype) const {
      return teop_buses_[i].ok_to_use(stage, lt, addrtype);
    }
    inline bool     in_use()         const {
      for (int i = 0; i < kNumTeopBuses; i++) {
        if (in_use(i)) return true;
      }
      return false;
    }
    // Setters
    inline void set_stats_addr(int bus, uint32_t stats_addr, bool meter_too) {
      RMT_ASSERT(hdr_time());
      RMT_ASSERT((bus >= 0) && (bus < kNumTeopBuses));
      teop_buses_[bus].set_stats_addr(stats_addr, meter_too);
    }
    inline void set_meter_addr(int bus, uint32_t meter_addr, bool stats_too, uint8_t color=0) {
      RMT_ASSERT(hdr_time());
      RMT_ASSERT((bus >= 0) && (bus < kNumTeopBuses));
      teop_buses_[bus].set_meter_addr(meter_addr, stats_too, color);
    }
    inline void set_addr(int bus, uint32_t addr, bool stats_en, bool meter_en,
                         uint8_t color=0) {
      RMT_ASSERT(hdr_time());
      RMT_ASSERT((bus >= 0) && (bus < kNumTeopBuses));
      teop_buses_[bus].set_addr(addr, stats_en, meter_en, color);
    }
    inline void set_raw_addr(int bus, uint32_t raw_addr) {
      uint32_t addr  = ((raw_addr >> 4) & 0x3FFFF);
      uint8_t  color = static_cast<uint8_t>((raw_addr >> 2) & 0x3);
      bool  stats_en = (((raw_addr >> 1) & 1) == 1);
      bool  meter_en = (((raw_addr >> 0) & 1) == 1);
      set_addr(bus, addr, stats_en, meter_en, color);
    }
    inline void set_stage_lt(int bus, int stage, int lt) {
      RMT_ASSERT(hdr_time());
      RMT_ASSERT((bus >= 0) && (bus < kNumTeopBuses));
      teop_buses_[bus].set_stage_lt(stage, lt);
    }
    inline void set_stage_lt(int bus, int stage, int lt, uint8_t addrtype) {
      RMT_ASSERT(hdr_time());
      RMT_ASSERT((bus >= 0) && (bus < kNumTeopBuses));
      teop_buses_[bus].set_stage_lt(stage, lt, addrtype);
    }
    inline void set_error(uint8_t error) {
      flags_ &= ~kFlagHdrTime; flags_ |= kFlagTeopTime;
      errors_ |= error;
    }
    inline void set_error_fcs()   {
      set_error(kErrFCS);
    }
    inline void set_error_trunc() {
      set_error(kErrTrunc);
    }
    inline void set_byte_len(uint16_t byte_len) {
      flags_ &= ~kFlagHdrTime; flags_ |= kFlagTeopTime;
      byte_len_ = byte_len;
    }

    
    // Access to time_info just like in Eop
    //
    // use set_mau_tick_time and set_mau_random_value to set
    //  specific values to match the RTL simulation
    inline void set_meter_tick_time(int mau_index,int meter_index,uint64_t tick_time) {
      time_info_.set_meter_tick_time(mau_index,meter_index,tick_time);
    }
    inline void set_meter_random_value(int mau_index,int meter_index, uint64_t random_value) {
      time_info_.set_meter_random_value(mau_index,meter_index,random_value);
    }
    // use this to initialise to plausible values, first_tick_time is
    //  used as the tick time for the first mau and the seed for the
    //  random numbers (it doesn't bother to do the random accurately)
    inline void setup_time_info(uint64_t first_tick_time) {
      time_info_.set_all(first_tick_time);
    }
    inline uint64_t get_meter_tick_time(int mau_index, int meter_index) {
      return time_info_.get_meter_tick_time(mau_index,meter_index);
    }
    inline uint64_t get_meter_random_value(int mau_index, int meter_index) {
      return time_info_.get_meter_random_value(mau_index,meter_index);
    }
    inline void set_time_info_from(const TimeInfoType& other) {
      time_info_.set_from(other);
    }
    inline const TimeInfoType &get_time_info() const {
      return time_info_;
    }    
    inline bool is_before(TimeInfoType& other) const {
      return time_info_.is_before(other);
    }
    inline void set_relative_time(uint64_t relative_time) {
      time_info_.set_relative_time(relative_time);
    }
    inline bool relative_time_valid()   const {
      return time_info_.relative_time_valid();
    }
    inline uint64_t get_relative_time() const {
      return time_info_.get_relative_time();
    }

    inline void clear_stats_en() {
      for (int i = 0; i < kTeopBusArraySz; i++) teop_buses_[i].clear_stats_en();
    }
    inline void clear_meter_en() {
      for (int i = 0; i < kTeopBusArraySz; i++) teop_buses_[i].clear_meter_en();
    }
    
 private:
    uint8_t                                  flags_;
    uint8_t                                  errors_;
    uint16_t                                 byte_len_;     
    std::array< TeopEntry, kTeopBusArraySz > teop_buses_;
    TimeInfoType                             time_info_{};
    
  };
}

#endif // _SHARED_TEOP_
