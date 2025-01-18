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

#ifndef _SHARED_SWEEP_TIME_INFO_
#define _SHARED_SWEEP_TIME_INFO_

#include <vector>
#include <unordered_map>
#include <rmt-defs.h>
#include <rmt-sweeper.h>
#include <address.h>

namespace MODEL_CHIP_NAMESPACE {

  class SweepConfigEntry {
    static constexpr int      kMeterSubwordWidth = Address::kMeterSubwordWidth;
    static constexpr int      kOneTickShift      = RmtSweeper::kOneTickShift;
    static constexpr uint64_t kAbsFirstT         = UINT64_C(0);
    static constexpr uint64_t kAbsLastT          = UINT64_C(0xFFFFFFFFFFFFFFFF);

    static uint64_t interval_to_ticks(int i)  { return RmtSweeper::interval_to_ticks(i); }
    static uint64_t interval_to_cycles(int i) { return RmtSweeper::interval_to_cycles(i); }

 public:
    SweepConfigEntry(bool on_demand, bool enabled, bool alu_enabled, uint8_t interval,
                     uint8_t subword_shift, uint8_t sweep_op4, uint64_t vpn_mask,
                     uint64_t config_T)
      : on_demand_(on_demand), enabled_(enabled), alu_enabled_(alu_enabled),
        interval_(interval), subword_shift_(subword_shift), sweep_op4_(sweep_op4),
        vpn_mask_(vpn_mask), config_T_(config_T),
        start_T_(kAbsFirstT), end_T_(kAbsLastT) {

      set_calculated_start_t(config_T);
    }
    ~SweepConfigEntry() { }

    // ALL the _t values below and all Ts are in cycles
    //
    bool     on_demand()               const { return on_demand_; }
    bool     enabled()                 const { return enabled_; }
    bool     alu_enabled()             const { return alu_enabled_; }
    uint8_t  interval()                const { return interval_; }
    uint8_t  subword_shift()           const { return subword_shift_; }
    uint8_t  sweep_op4()               const { return sweep_op4_; }
    uint64_t vpn_mask()                const { return vpn_mask_; }
    uint64_t config_t()                const { return config_T_; }
    uint64_t start_t()                 const { return start_T_; }
    uint64_t end_t()                   const { return end_T_; }
    uint8_t  n_subwords()              const { return 1 << subword_shift(); }
    uint8_t  huffman_shift()           const { return kMeterSubwordWidth - subword_shift(); }
    uint64_t interval_t()              const { return interval_to_cycles(interval()); }
    uint64_t interval_ticks()          const { return interval_to_ticks(interval()); }

    void     set_on_demand(bool tf)          { on_demand_ = tf; }
    void     set_enabled(bool tf)            { enabled_ = tf; }
    void     set_alu_enabled(bool tf)        { alu_enabled_ = tf; }
    void     set_interval(uint8_t i)         { interval_ = i; }
    void     set_subword_shift(uint8_t ss)   { subword_shift_ = ss; }
    void     set_sweep_op4(uint8_t op4)      { sweep_op4_ = op4; }
    void     set_vpn_mask(uint64_t vpn_mask) { vpn_mask_ = vpn_mask; }
    //void   set_interval_t(uint64_t T)      { }
    void     set_config_t(uint64_t T)        { config_T_ = T; }
    void     set_start_t(uint64_t T)         { start_T_ = T; }
    void     set_end_t(uint64_t T)           { end_T_ = T; }

    bool     after_start(uint64_t T)   const { return (T >= start_t()); }
    bool     before_end (uint64_t T)   const { return (T <= end_t());   }
    bool     in_start_end(uint64_t T)  const { return (after_start(T) && before_end(T)); }
    uint64_t prev_t(uint64_t T)        const { return (T > interval_t()) ?T - interval_t() :kAbsFirstT; }
    uint64_t next_t(uint64_t T)        const { return T + interval_t(); }
    bool     sweep_on_demand()         const { return enabled() &&  on_demand(); }
    bool     sweep_immediate()         const { return enabled() && !on_demand(); }
    bool     sweep_vpn(int vpn)        const {
      return ((vpn >= 0) && (vpn <= 63)) ?(((vpn_mask() >> vpn) & 1) == 1) :false;
    }
    bool is_same(bool on_demand, bool enabled, bool alu_enabled, uint8_t interval,
                 uint8_t subword_shift, uint8_t sweep_op4, uint64_t vpn_mask) const {
      return ((on_demand_ == on_demand) && (enabled_ == enabled) &&
              (alu_enabled_ == alu_enabled) &&
              (subword_shift_ == subword_shift) && (sweep_op4_ == sweep_op4) &&
              (interval_ == interval) && (vpn_mask_ == vpn_mask));
    }
    uint64_t sweep_before_t(uint64_t T, bool *before) const {
      // Snap to interval boundary before T (which may be before start_t)
      *before = (T < start_t()); if (*before) T = start_t();
      return (((T - start_t()) / interval_t()) * interval_t()) + start_t();
    }

    uint64_t calculate_start_t(uint64_t T) const {
      // Figure out when notional previous sweep would have occurred
      // then add on the interval in cycles to give us time first sweep
      return enabled() ?calculate_previous_sweep_t(T) + interval_t() :T;
    }
    uint64_t calculate_end_t(uint64_t T) const {
      // Figure out when actual previous sweep occurred and set that
      // as the time of the last sweep
      return enabled() ?calculate_previous_sweep_t(T) :T;
    }
    void set_calculated_start_t(uint64_t T) { set_start_t( calculate_start_t(T) ); }
    void set_calculated_end_t(uint64_t T)   { set_end_t( calculate_end_t(T) ); }


 private:
    uint64_t calculate_previous_sweep_t(uint64_t T) const {
      // We need to figure out when previous sweep would have occurred given
      // passed-in T
      //
      // This is determined by when the interval bit was flipped 0->1 or 1->0
      // interval_t() is just  1 << ( interval() + kOneTickShift )
      // So we're figuring out when bit at interval() + kOneTickShift flipped
      //
      // If shift == interval() + kOneTickShift and X == 2^shift then
      // we're working out T / X * X  OR  (T >> shift) << shift
      //
      int shift = interval() + kOneTickShift;
      return (T >> shift) << shift;
    }

    bool        on_demand_;
    bool        enabled_;
    bool        alu_enabled_;
    uint8_t     interval_;
    uint8_t     subword_shift_;
    uint8_t     sweep_op4_;
    uint64_t    vpn_mask_;
    uint64_t    config_T_;
    uint64_t    start_T_;
    uint64_t    end_T_;
  };



  class SweepTimeInfo {
    static constexpr uint64_t kAbsFirstT = UINT64_C(0);
    static constexpr uint64_t kAbsLastT  = UINT64_C(0xFFFFFFFFFFFFFFFF);

 public:
    SweepTimeInfo()                                                           { reset(); }
    SweepTimeInfo(int chipIndex, int pipeIndex, int stageIndex, int aluIndex) { reset(); }
    ~SweepTimeInfo()                                                          { clear(); }


 private:
    void clear() {
      last_sweep_T_.clear();
      configs_.clear();
      n_configs_ = 0;
      on_demand_enabled_ever_ = false;
      flush_cache();
    }
    void reset() {
      clear();
    }

    int find_enabled_config(uint64_t T, int vpn, int start_pos=0) {
      if (start_pos < 0) start_pos = 0;
      int n_conf = n_configs_; // Take copy
      while (start_pos < n_conf) {
        if ( (configs_[start_pos].enabled()) &&
             (configs_[start_pos].sweep_vpn(vpn)) &&
             (configs_[start_pos].before_end(T)) ) break;
        start_pos++;
      }
      return (start_pos < n_conf) ?start_pos :-1;
    }
    int r_find_enabled_config(uint64_t T, int vpn, int start_pos=-1) {
      int n_conf = n_configs_; // Take copy
      // Start at end by default
      if (start_pos < 0) start_pos = n_conf + start_pos;
      while (start_pos >= 0) {
        if ( (configs_[start_pos].enabled()) &&
             (configs_[start_pos].sweep_vpn(vpn)) &&
             (configs_[start_pos].after_start(T)) ) break;
        start_pos--;
      }
      return (start_pos >= 0) ?start_pos :-1;
    }

    uint64_t update_cache(uint64_t last_T) {
      cached_last_T_ = last_T;
      return last_T;
    }
    uint64_t update_cache(int pos, uint64_t last_T) {
      cached_last_T_ = last_T; cached_pos_ = pos;
      return last_T;
    }
    uint64_t update_cache(uint32_t addr, int pos, uint64_t last_T) {
      cached_last_T_ = last_T; cached_pos_ = pos; cached_addr_ = addr;
      return last_T;
    }
    uint64_t flush_cache() {
      // XXX: use an invalid addr when flushing cache
      return update_cache(0xFFFFFFFFu, -1, kAbsLastT);
    }

    uint64_t get_cached_next_sweep_t(uint32_t addr) {
      if (addr != cached_addr_) return kAbsLastT;
      uint64_t next_t = configs_[cached_pos_].next_t(cached_last_T_);
      if (configs_[cached_pos_].in_start_end(next_t)) {
        // next_t still in curr config interval - return
        return update_cache(next_t);
      } else {
        // Hunt for next valid config interval
        int addr_vpn = Address::meter_addr_get_vpn(addr);
        int tmp_pos = cached_pos_ + 1;
        int n_conf = n_configs_; // Take copy
        while (tmp_pos < n_conf) {
          if ((configs_[tmp_pos].enabled()) &&
              (configs_[tmp_pos].sweep_vpn(addr_vpn))) {
            return update_cache( tmp_pos, configs_[tmp_pos].start_t() );
          }
          tmp_pos++;
        }
      }
      // No joy - stop caching this addr
      return flush_cache();
    }


 public:
    // Func to determine sweep time preceding a given time T
    uint64_t get_prev_sweep_t(uint32_t addr, uint64_t T) {
      // Addr should just be VPN/Index/Subword
      int addr_vpn = Address::meter_addr_get_vpn(addr);

      // Find *last* active SweepConfigEntry containing or before T
      int pos = r_find_enabled_config(T, addr_vpn);
      if (pos < 0) return kAbsLastT;

      // Force T into range if not contained (ie range all before)
      if (T > configs_[pos].end_t()) T = configs_[pos].end_t();
      // Get last sweep value before T
      bool dummy;
      return configs_[pos].sweep_before_t(T, &dummy);
    }

    // Addr should just be VPN/Index/Subword
    uint64_t get_next_sweep_t(uint32_t addr) {
      if (addr == cached_addr_) return get_cached_next_sweep_t(addr);

      uint64_t last_t = last_sweep_t(addr); // Else lookup in MAP
      int addr_vpn = Address::meter_addr_get_vpn(addr);

      // Look for relevant SweepConfigEntry.
      // First active one containing or after last_t
      int pos = find_enabled_config(last_t, addr_vpn);
      if (pos < 0) return kAbsLastT;

      // Snap to T value before - it might have been written
      // by a CfgWr so may be unaligned. Then update cache
      // NB. If was aligned shouldn't change!
      bool before_start;
      last_t = configs_[pos].sweep_before_t(last_t, &before_start);
      update_cache(addr, pos, last_t);

      // Then maybe use get_cached_next_sweep_t to advance time
      //  unless last_t was before start of interval and so has
      //  already been advanced
      // Subsequent calls will go direct via get_cached_next_sweep_t
      return (before_start) ?last_t :get_cached_next_sweep_t(addr);
    }

    uint64_t last_sweep_t(uint32_t addr) const {
      try {
        return last_sweep_T_.at(addr);
      } catch (const std::exception&) {
        return kAbsFirstT;
      }
    }
    void set_last_sweep_t(uint32_t addr, uint64_t sweep_T)  {
      if (addr == cached_addr_) flush_cache();
      if ((sweep_T == kAbsFirstT) || (sweep_T == kAbsLastT)) return;
      try {
        last_sweep_T_.at(addr) = sweep_T;
      } catch (const std::exception&) {
        // Probably means no value yet written for addr - use emplace
        try {
          last_sweep_T_.emplace(addr, sweep_T);
        } catch (const std::exception&) { }
      }
    }

    void update_addresses(int old_addr, int new_addr) {
      if ((old_addr < 0) || (new_addr < 0)) return;
      uint64_t T = last_sweep_t( static_cast<uint32_t>(old_addr) );
      set_last_sweep_t( static_cast<uint32_t>(new_addr), T);
    }

    void config(bool on_demand, bool en, bool alu_en, uint8_t interval,
                uint8_t subword_shift, uint8_t sweep_op4, uint64_t vpn_mask,
                uint64_t config_T) {
      int n_conf = n_configs_; // Take copy
      // Suppress identical configs
      if ((n_conf > 0) &&
          (configs_[n_conf-1].is_same(on_demand, en, alu_en, interval, subword_shift,
                                      sweep_op4, vpn_mask))) return;
      // New distinct config
      configs_.push_back(SweepConfigEntry(on_demand, en, alu_en, interval, subword_shift,
                                          sweep_op4, vpn_mask, config_T));
      // Fixup end_T on preceding config
      if (n_conf > 0)
        configs_[n_conf-1].set_calculated_end_t(config_T);

      // And update count in member var
      n_configs_ = static_cast<int>(configs_.size());

      if (on_demand && en) on_demand_enabled_ever_ = true;
    }

    SweepConfigEntry *get_config(uint32_t addr) {
      int pos = cached_pos_;
      int n_conf = n_configs_; // Take copy
      if ((addr != cached_addr_) || (pos < 0) || (pos >= n_conf)) return nullptr;
      return &configs_[pos];
    }

    bool on_demand_used() const { return on_demand_enabled_ever_; }

 private:
    std::unordered_map< uint32_t, uint64_t >  last_sweep_T_;
    std::vector< SweepConfigEntry >           configs_;
    int                                       n_configs_;
    bool                                      on_demand_enabled_ever_;
    uint32_t                                  cached_addr_;
    int                                       cached_pos_;
    uint64_t                                  cached_last_T_;
  };

}

#endif // _SHARED_SWEEP_TIME_INFO_
