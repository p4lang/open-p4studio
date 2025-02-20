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

#ifndef _SHARED_MAU_MOVEREGS_
#define _SHARED_MAU_MOVEREGS_

#include <rmt-defs.h>
#include <mau-defs.h>
#include <mau-object.h>

#include <register_includes/movereg_ad_direct_array.h>
#include <register_includes/movereg_stats_ctl_array.h>
#include <register_includes/movereg_meter_ctl_array.h>
#include <register_includes/movereg_idle_ctl_array.h>
#include <register_includes/movereg_ad_stats_alu_to_logical_xbar_ctl_array.h>
#include <register_includes/movereg_ad_meter_alu_to_logical_xbar_ctl_array.h>
#include <register_includes/movereg_idle_pop_ctl_array.h>
#include <register_includes/mau_cfg_movereg_tcam_only.h>

// These registers removed - leave commented out in case they come back!
//#include <tofino/register_includes/movereg_meter_mc_req_to_stats_xbar_ctl.h>
//#include <tofino/register_includes/movereg_meter_mc_req_to_idle_xbar_ctl_array.h>
//#include <tofino/register_includes/movereg_stats_mc_gnt_to_meter_xbar_ctl.h>
//#include <tofino/register_includes/movereg_idle_mc_gnt_to_meter_xbar_ctl.h>


namespace MODEL_CHIP_NAMESPACE {

  class Mau;
  class MauMoveregsCtl;

  class MauMoveregs {
    static constexpr int kTables = MauDefs::kLogicalTablesPerMau;
    static constexpr int kSramColumns = MauDefs::kSramColumnsPerMau;
    static constexpr int kSramRows = MauDefs::kSramRowsPerMau;
    static constexpr int kMapramColumns = MauDefs::kMapramColumnsPerMau;
    static constexpr int kMapramRows = MauDefs::kMapramRowsPerMau;
    static constexpr int kXmIndexWidth = MauDefs::kSramAddressWidth;
    static constexpr int kTmIndexWidth = MauDefs::kTcamAddressWidth;
    static constexpr int kXmVpnWidth = Address::kXmMatchVpnWidth;
    static constexpr int kTmVpnWidth = Address::kTmMatchVpnWidth;

    // Flags for call to do_update
    static constexpr uint8_t kFlagsUpdDefRam = 0x01;
    static constexpr uint8_t kFlagsUpdEvicts = 0x02;
    static constexpr uint8_t kFlagsUpdColWrs = 0x04;
    static constexpr uint8_t kFlagsUpdMtrAlu = 0x08;
    static constexpr uint8_t kFlagsUpdAll    = 0xFF;

 public:
    static bool kAllowMoveregsCommitOnTcamHit; // Defined in rmt-config.cpp

    MauMoveregs() :
        mau_moveregs_ctl_(nullptr),
        relative_time_(0),
        lt_(-1) {
      reset();
    }
    ~MauMoveregs() { mau_moveregs_ctl_ = NULL; lt_ = -1; }

    inline void init(MauMoveregsCtl *mau_moveregs_ctl, int lt) {
      mau_moveregs_ctl_ = mau_moveregs_ctl;
      relative_time_ = UINT64_C(0);
      lt_ = lt;
      reset();
    }
    inline void reset() {
      S_addr_ = D_addr_ = oldD_addr_ = -1;
      last_commit_from_ = last_commit_to_ = last_upd_from_ = last_upd_to_ = -1;
    }
    inline MauMoveregsCtl *ctl() { return mau_moveregs_ctl_; }

    // Funcs to form meter/stats/idle addresses
    inline int get_meter_addr(int addr, int meter_size, bool mask=true) const {
      if ((meter_size == 1) || (meter_size == 2)) meter_size = 0;
      // Only need to add Huffman bits for shifts 4,5,6,7
      int huff_bits = (meter_size > 3) ?((1<<(meter_size-3))-1) << 2 :0;
      int ret_addr = (addr << meter_size) | huff_bits;
      if (mask) ret_addr &= static_cast<int>(Address::kMeterAddrAddrMask);
      return ret_addr;
    }
    inline int get_stats_addr(int addr, int stats_size,
                              bool from_tcam, bool mask=true) const {
      int indexWidth = kXmIndexWidth, vpnWidth = kXmVpnWidth;
      if (from_tcam) {
        indexWidth = kTmIndexWidth; vpnWidth = kTmVpnWidth;
      }
      int indexOffset = 0, vpnOffset = indexOffset + indexWidth;
      uint32_t indexMask = ((1u<<indexWidth)-1), vpnMask = ((1u<<vpnWidth)-1);
      int index = (static_cast<uint32_t>(addr) >> indexOffset) & indexMask;
      int vpn = (static_cast<uint32_t>(addr) >> vpnOffset) & vpnMask;
      // Extract VPN[5:4] into vpnLo leaving vpnHi - used in 3/6 entry cases,
      // otherwise vpnHi is original vpn and vpnLo is 0.
      int vpnHi = ((vpn >> 6) << 4) | (vpn & 0xF);
      int vpnLo = (vpn >> 4) & 0x3;
      int shift = 0;
      switch (stats_size) {
        case 0: shift = 3; vpnHi = vpn; vpnLo = 0; break; // 1 entry
        case 1: shift = 2; vpnHi = vpn; vpnLo = 0; break; // 2 entries
        case 2: shift = 3;                         break; // 3 entries
        case 3: shift = 1; vpnHi = vpn; vpnLo = 0; break; // 4 entries
        case 4: shift = 2;                         break; // 6 entries
        default: RMT_ASSERT(0); break;
      }
      int ret_addr = (((vpnHi << indexWidth) | index) << shift) | vpnLo;
      if (mask) ret_addr &= static_cast<int>(Address::kStatsAddrAddrMask);
      return ret_addr;
    }
    inline int get_idle_addr(int addr, int idle_size, bool mask=true) const {
      int ret_addr = (addr << (idle_size+1)) | ((1<<idle_size)-1);
      if (mask) ret_addr &= static_cast<int>(Address::kIdletimeAddrAddrMask);
      return ret_addr;
    }
    inline int get_idle_initval(const bool idle_init, int idle_size) {
      int max = 0;
      switch (idle_size) {
        case 0:  max =  1;  break;
        case 1:  max =  3;  break;
        case 2:  max =  7;  break;
        case 3:  max = 63;  break;
        default: RMT_ASSERT(0); break;
      }
      if (idle_init) {
        // PopTableMovAdr(IdleInit==1). Always return max value in this case
        return max;
      } else {
        // PopTableMovAdr(IdleInit==0). Return active val OR idle val
        if (reg_idle_mode() == 1) {
          // active - initval determined by setting idle_2way_en
          return static_cast<int>(reg_idle_2way_en());
        } else {
          // idle - send max-1
          return max-1;
        }
      }
    }

    // Main APIs
    void push_table_move_addr(const int addr,
                              const uint64_t relative_time);
    void pop_table_move_addr(const bool idle_init,
                             const bool stats_init_ones,
                             const uint64_t relative_time);

    // Sub OPs called whilst doing push_table, pop_table
    bool move_ok();
    void shift_and_load(const int addr);
    void initialize(const bool idle_init, const bool stats_init_ones);
    void inhibit();
    void commit();
    void prepare();
    // update_addresses calls other update_X funcs
    void update_addresses();
    void update_deferred_ram();
    void update_queued_color_writes();
    void update_queued_evictions();
    void update_meter();
    // This called out of TCAM code to trigger commit
    void maybe_commit(const int tcam_hit_addr);

    // Funcs to access register info
    Mau    *mau();
    uint8_t reg_meter_size();
    uint8_t reg_stats_size();
    uint8_t reg_idle_size();
    bool    reg_meter_deferred();
    bool    reg_stats_deferred();
    bool    reg_stats_from_tcam();
    bool    reg_meter_color();
    bool    reg_stats_as_mc();
    bool    reg_idle_as_mc();
    bool    reg_direct_meter();
    bool    reg_direct_stats();
    bool    reg_direct_idle();
    uint8_t reg_idle_2way_en();
    uint8_t reg_idle_mode();
    bool    reg_tcam_only();
    int     reg_stats_lt_to_alu();
    int     reg_meter_lt_to_alu();

 private:
    void commit(int fromAddr, int toAddr, uint64_t T);
    void do_commit(int fromAddr, int toAddr, uint64_t T);
    void update_addresses(int fromAddr, int toAddr);
    void do_update_addresses(int fromAddr, int toAddr, uint8_t flags);

 private:
    MauMoveregsCtl  *mau_moveregs_ctl_;
    uint64_t         relative_time_;
    int              lt_;
    int              S_addr_;
    int              D_addr_;
    int              oldD_addr_;
    int              last_commit_from_;
    int              last_commit_to_;
    int              last_upd_from_;
    int              last_upd_to_;

  }; // MauMoveregs




  class MauMoveregsCtl : public MauObject {
    static constexpr int kType = RmtTypes::kRmtTypeMauMoveregs;
    static constexpr int kTables = MauDefs::kLogicalTablesPerMau;
    static constexpr int kNumStatsAlus = MauDefs::kNumStatsAlus;
    static constexpr int kNumMeterAlus = MauDefs::kNumMeterAlus;

 public:
    MauMoveregsCtl(RmtObjectManager *om, int pipeIndex, int mauIndex, Mau *mau);
    virtual ~MauMoveregsCtl();

    // Register accessors
    inline bool reg_direct_stats(int lt) {
      // Stats=0 Meter=1 Idle=2 in movereg_ad_direct
      int alu = reg_stats_lt_to_alu(lt);
      bool direct = ((alu >= 0) &&
                     (movereg_stats_ctl_arr_.movereg_stats_ctl_direct(alu) == 1) &&
                     ((movereg_ad_direct_arr_.movereg_ad_direct(0) & (1<<lt)) != 0u));
      if (direct) {
        // Validate movereg completion CSR - should be enabled and refer to same ALU
        int j = (lt / 8);         // array index 0 for LogicalTables 0-7, index 1 for 8-15
        int shift = (lt % 8) * 3; // 3b control fields for each LogicalTable (MSB enable)
        uint32_t reg = movereg_ad_stats_alu_to_lt_.movereg_ad_stats_alu_to_logical_xbar_ctl(j);
        uint32_t ctl = reg >> shift;
        int alu2 = static_cast<int>(ctl & 0x3);
        RMT_ASSERT((ctl & 0x4) != 0u);
        RMT_ASSERT(alu2 == alu);
      }
      return direct;
    }
    inline bool reg_direct_meter(int lt) {
      // Stats=0 Meter=1 Idle=2 in movereg_ad_direct
      int alu = reg_meter_lt_to_alu(lt);
      bool direct = ((alu >= 0) &&
                     (movereg_meter_ctl_arr_.movereg_meter_ctl_direct(alu) == 1) &&
                     ((movereg_ad_direct_arr_.movereg_ad_direct(1) & (1<<lt)) != 0u));
      if (direct) {
        // Validate movereg completion CSR - should be enabled and refer to same ALU
        int j = (lt / 8);         // array index 0 for LogicalTables 0-7, index 1 for 8-15
        int shift = (lt % 8) * 3; // 3b control fields for each LogicalTable (MSB enable)
        uint32_t reg = movereg_ad_meter_alu_to_lt_.movereg_ad_meter_alu_to_logical_xbar_ctl(j);
        uint32_t ctl = reg >> shift;
        int alu2 = static_cast<int>(ctl & 0x3);
        RMT_ASSERT((ctl & 0x4) != 0u);
        RMT_ASSERT(alu2 == alu);
      }
      return direct;
    }
    inline bool reg_direct_idle(int lt) {
      // Stats=0 Meter=1 Idle=2 in movereg_ad_direct
      return (((movereg_ad_direct_arr_.movereg_ad_direct(2) & (1<<lt)) != 0u) &&
              (movereg_idle_ctl_arr_.movereg_idle_ctl_direct(lt) == 1));
    }
    inline uint8_t reg_stats_size(int lt) {
      int alu = reg_stats_lt_to_alu(lt);
      return (alu >= 0) ?movereg_stats_ctl_arr_.movereg_stats_ctl_size(alu) :0;
    }
    inline uint8_t reg_meter_size(int lt) {
      int alu = reg_meter_lt_to_alu(lt);
      return (alu >= 0) ?movereg_meter_ctl_arr_.movereg_ad_meter_shift(alu) :0;
    }
    inline uint8_t reg_idle_size(int lt) {
      return movereg_idle_ctl_arr_.movereg_idle_ctl_size(lt);
    }
    inline bool reg_stats_deferred(int lt) {
      int alu = reg_stats_lt_to_alu(lt);
      return (alu >= 0) ?(movereg_stats_ctl_arr_.movereg_stats_ctl_deferred(alu) == 1) :false;
    }
    inline bool reg_meter_deferred(int lt) {
      int alu = reg_meter_lt_to_alu(lt);
      return (alu >= 0) ?(movereg_meter_ctl_arr_.movereg_meter_ctl_deferred(alu) == 1) :false;
    }
    inline bool reg_stats_from_tcam(int lt) {
      int alu = reg_stats_lt_to_alu(lt);
      return (alu >= 0) ?(movereg_stats_ctl_arr_.movereg_stats_ctl_tcam(alu) == 1) :false;
    }
    inline bool reg_meter_color(int lt) {
      int meter_alu = reg_meter_lt_to_alu(lt); // Lookup *MeterALU* for LT
      if (meter_alu < 0) return false;
      // Check whether MeterColor enabled for MeterALU
      if (movereg_meter_ctl_arr_.movereg_meter_ctl_color_en(meter_alu) == 0) return false;
      return true;
    }
    inline bool reg_stats_as_mc(int lt) {
      int meter_alu = reg_meter_lt_to_alu(lt); // Lookup *MeterALU* for LT
      if (meter_alu < 0) return false;
      // Check whether MeterColor enabled for MeterALU
      if (movereg_meter_ctl_arr_.movereg_meter_ctl_color_en(meter_alu) == 0) return false;
      // Check that Stats bus configured to send MeterColor
      // Use specific func reg_stats_lt_to_color_alu - checks lt match, direct=0, mc=1
      int stats_alu = reg_stats_lt_to_color_alu(lt, meter_alu);
      if (stats_alu < 0) return false;

      // Which StatsALU is the MeterALU listening to, if any
      //uint32_t gnt_reg = movereg_stats_mc_gnt_to_meter_.movereg_stats_mc_gnt_to_meter_xbar_ctl();
      //uint32_t gnt_ctl = gnt_reg >> (meter_alu * 3);
      // If MeterALU not listening to grants from a StatsALU then no MeterColor
      //if ((gnt_ctl & 0x4) == 0u) return false;
      //int stats_alu = static_cast<int>(gnt_ctl & 0x3);
      // Check this StatsALU configured to send MeterColor
      //if (movereg_stats_ctl_arr_.movereg_stats_ctl_mc(stats_alu) == 0) return false;
      //uint32_t req_reg = movereg_meter_mc_req_to_stats_.movereg_meter_mc_req_to_stats_xbar_ctl();
      //uint32_t req_ctl = req_reg >> (stats_alu * 3);
      // If StatsALU not listening to requests from MeterALU then no MeterColor
      //if ((req_ctl & 0x4) == 0u) return false;
      // If StatsALU not listening to requests from *this* MeterALU then no MeterColor
      //if (static_cast<int>(req_ctl & 0x3) != meter_alu) return false;
      // Everything configured correctly so we have MeterColor
      return true;
    }
    inline bool reg_idle_as_mc(int lt) {
      int meter_alu = reg_meter_lt_to_alu(lt); // Lookup *MeterALU* for LT
      if (meter_alu < 0) return false;
      // Check whether MeterColor enabled for MeterALU
      if (movereg_meter_ctl_arr_.movereg_meter_ctl_color_en(meter_alu) == 0) return false;
      // Check Idle LT configured to send MeterColor
      if (movereg_idle_ctl_arr_.movereg_idle_ctl_mc(lt) == 0) return false;

      // If Idle LT not listening to requests from MeterALU then no MeterColor
      //int i = (lt / 8); // array index 0 for LogicalTables 0-7, index 1 for 8-15
      //uint32_t req_reg = movereg_meter_mc_req_to_idle_.movereg_meter_mc_req_to_idle_xbar_ctl(i);
      //uint32_t req_ctl = req_reg >> ((lt % 8) * 3);
      //if (((req_ctl & 0x4) == 0u) || (static_cast<int>(req_ctl & 0x3) != meter_alu)) return false;
      // If MeterALU not listening to grants from LT then no MeterColor
      //uint32_t gnt_reg = movereg_idle_mc_gnt_to_meter_.movereg_idle_mc_gnt_to_meter_xbar_ctl();
      //uint32_t gnt_ctl = gnt_reg >> (meter_alu * 5);
      //if (((gnt_ctl & 0x10) == 0u) || (static_cast<int>(gnt_ctl & 0xF) != lt)) return false;
      // Everything configured correctly so we have MeterColor
      return true;
    }
    inline uint8_t reg_idle_pfe(int lt) {
      uint32_t reg = movereg_idle_pop_ctl_.movereg_idle_pop_ctl(lt/8);
      uint32_t ctl = reg >> ((lt%8) * 3); // 3b per LT with Bit2=pfe
      return static_cast<uint8_t>((ctl >> 2) & 0x1);
    }
    inline uint8_t reg_idle_2way_en(int lt) {
      uint32_t reg = movereg_idle_pop_ctl_.movereg_idle_pop_ctl(lt/8);
      uint32_t ctl = reg >> ((lt%8) * 3); // 3b per LT with Bit1=2way_en
      return static_cast<uint8_t>((ctl >> 1) & 0x1);
    }
    inline uint8_t reg_idle_mode(int lt) {
      uint32_t reg = movereg_idle_pop_ctl_.movereg_idle_pop_ctl(lt/8);
      uint32_t ctl = reg >> ((lt%8) * 3); // 3b per LT with Bit0=mode
      return static_cast<uint8_t>((ctl >> 0) & 0x1);
    }
    inline bool reg_tcam_only(int lt) {
      return ((mau_cfg_movereg_tcam_only_.mau_cfg_movereg_tcam_only() & (1<<lt)) != 0u);
    }
    inline int reg_stats_lt_to_alu(int lt) {
      RMT_ASSERT((lt >= 0) && (lt < kTables));
      for (int alu = 0; alu < kNumStatsAlus; alu++) {
        if ((movereg_stats_ctl_arr_.movereg_stats_ctl_lt(alu) == lt) &&
            (movereg_stats_ctl_arr_.movereg_stats_ctl_direct(alu) == 1))
          return alu;
      }
      return -1;
    }
    inline int reg_stats_lt_to_color_alu(int lt, int start_alu) {
      RMT_ASSERT((lt >= 0) && (lt < kTables));
      RMT_ASSERT((start_alu >= 0) && (start_alu < kNumStatsAlus));
      for (int i = 0; i < kNumStatsAlus; i++) {
        int alu = (start_alu + i) % kNumStatsAlus;
        if ((movereg_stats_ctl_arr_.movereg_stats_ctl_lt(alu) == lt) &&
            (movereg_stats_ctl_arr_.movereg_stats_ctl_direct(alu) == 0) &&
            (movereg_stats_ctl_arr_.movereg_stats_ctl_mc(alu) == 1))
          return alu;
      }
      return -1;
    }
    inline int reg_meter_lt_to_alu(int lt) {
      RMT_ASSERT((lt >= 0) && (lt < kTables));
      for (int alu = 0; alu < kNumMeterAlus; alu++) {
        if ((movereg_meter_ctl_arr_.movereg_meter_ctl_lt(alu) == lt) &&
            (movereg_meter_ctl_arr_.movereg_meter_ctl_direct(alu) == 1))
            return alu;
      }
      return -1;
    }

    MauMoveregs *get_moveregs(const int lt);
    void push_table_move_addr(const int lt,
                              const int addr,
                              const uint64_t relative_time);
    void pop_table_move_addr(const int lt,
                             const bool idle_init,
                             const bool stats_init_ones,
                             const uint64_t relative_time);


 private:
    std::array< MauMoveregs, kTables >                    moveregs_tab_;
    register_classes::MoveregAdDirectArray                    movereg_ad_direct_arr_;
    register_classes::MoveregStatsCtlArray                    movereg_stats_ctl_arr_;
    register_classes::MoveregMeterCtlArray                    movereg_meter_ctl_arr_;
    register_classes::MoveregIdleCtlArray                     movereg_idle_ctl_arr_;
    register_classes::MoveregAdStatsAluToLogicalXbarCtlArray  movereg_ad_stats_alu_to_lt_;
    register_classes::MoveregAdMeterAluToLogicalXbarCtlArray  movereg_ad_meter_alu_to_lt_;
    register_classes::MoveregIdlePopCtlArray                  movereg_idle_pop_ctl_;
    register_classes::MauCfgMoveregTcamOnly                   mau_cfg_movereg_tcam_only_;
    // These registers removed - leave commented out in case they come back!
    //register_classes::MoveregMeterMcReqToStatsXbarCtl         movereg_meter_mc_req_to_stats_;
    //register_classes::MoveregMeterMcReqToIdleXbarCtlArray     movereg_meter_mc_req_to_idle_;
    //register_classes::MoveregStatsMcGntToMeterXbarCtl         movereg_stats_mc_gnt_to_meter_;
    //register_classes::MoveregIdleMcGntToMeterXbarCtl          movereg_idle_mc_gnt_to_meter_;


  }; // MauMoveregsCtl

}

#endif // _SHARED_MAU_MOVEREGS_
