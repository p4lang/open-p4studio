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

#ifndef _SHARED_MAU_LOOKUP_RESULT_
#define _SHARED_MAU_LOOKUP_RESULT_

#include <string>
#include <cstdint>
#include <bitvector.h>
#include <nxt-tab.h>
#include <address.h>

/*
 * MauLookupResult is a generic result holding object used all over
 * the Tofino Verification Model to harvest lookup results from
 * Tofino ExactMatch and TernaryMatch lookups.
 *
 * Each MAU can have 16 LogicalTables within it and currently there
 * are also 16 MauLookupResults allocated within each MAU where
 * MauLookupResult N is directly associated with LogicalTable N.  Thus
 * whenever LogicalTable N is looked-up then MauLookupResult N is
 * reused to do the lookup.  Note, however, lower level functions are
 * unaware of this mapping and so it is possible to use a dynamically
 * allocated MauLookupResult object instead.
 *
 * In between each PHV lookup all 16 fixed MauLookupResult within
 * every MAU are invalidated (by a call to invalidate()). Whenever a
 * LogicalTable lookup subsequently begins the corresponding
 * MauLookupResult is reset() to indicate it is in use, but reset()
 * also sets the match field false to indicate no match information is
 * stored within the MauLookupResult. (If a dynamically allocated
 * MauLookupResult object is used it should be manually reset()).
 *
 * During later ExactMatch or TernaryMatch processing the match field
 * may be set true to indicate a match has occurred in which case
 * other relevant match fields may be set to convey more information
 * about the match found.  For instance a TernaryMatch will set the
 * ternarymatch field (and possibly the ternaryindirect field if a
 * TIND was used). An ExactMatch will set the exactmatch field.
 *
 * Finally, a subset of the valid MauLookupResults are marked as active
 * indicating that the corresponding table has been selected by the
 * predication process.
 *
 * The table below indicates what fields are used in what circumstances
 *
 * FIELD         INVALID IN-USE MISS  EXACT-HIT  TERN-HIT  TIND-HIT
 * valid            F      T      T        T         T         T
 * active           F      F     TF**     TF**      TF**      TF**
 * match            -      F      F        T         T         T
 * exactmatch       -      -      -        T        TF*       TF*
 * ternarymatch     -      -      -       TF*        T         T
 * ternaryindirect  -      -      -        -         F         T
 * row              -      -      -     sramRow      -      tindRow
 * col              -      -      -     sramCol      -      tindCol
 * outbus           -      -      -     matchBus     -      tindBus
 * hitindex         -      -      -    sramIndex  TCAMaddr TCAMaddr
 * hitentry         -      -      -     hit1-5       0         0
 * logical_tables   -      -      -    sramLogTabs   -     tindLogTabs
 * payload          -      -      -        0      TCAMpyld TCAMpayld
 * next_table_orig  -      -      -    sramNxtTab    -     tindNxtTab
 * next_table_mask  -      -      -    (as above +mask/default)
 * next_table_form  -      -      -    (as above +map)
 * next_table_pred  -      -      -    (as above +predication fixup)
 * next_table       -      -      -    (most recent of orig,mask,form)
 * instr            -      -      -    instrAddr     -     instrAddr
 *
 * *Note it is possible for there to be simultaneously BOTH an exactmatch
 * hit AND a ternarymatch hit - in this case both the exactmatch and
 * ternarymatch fields will be true. However, the exactmatch operations
 * run AFTER the ternarymatch operations so the field values will
 * correspond to the exactmatch results.
 *
 * **active is only set if the corresponding table is predicated ON
 *
 * A stash hit will show up as an exact match with the row and hitindex
 *  set to the stash word that hit.
 *
 * These data the MauLookupResult also preserves a copy of all values
 * looked up using it (assuming MauLookupResult::extract_addresses is
 * used to perform the lookup). In this case the looked-up values can
 * be accessed via calls to imm_data(), act_inst_addr(), act_data_addr()
 * stats_addr(), meter_addr(), idletime_addr() and selector_len().
 */

namespace MODEL_CHIP_NAMESPACE {

  class Mau;
  class MauResultBus;
  class MauLogicalTable;
  class MauSramRow;
  class Phv;

  class MauLookupResult {

    static constexpr int kMatchOutputBuses = MauDefs::kMatchOutputBusesPerMau;
    static constexpr int kTindOutputBuses = MauDefs::kTindOutputBusesPerMau;
    static constexpr int kLogicalTables = MauDefs::kLogicalTablesPerMau;
    static constexpr int kTableResultBusWidth = MauDefs::kTableResultBusWidth;
    static constexpr int kMatchOutputBusWidth = MauDefs::kMatchOutputBusWidth;
    static constexpr int kTindOutputBusWidth = MauDefs::kTindOutputBusWidth;
    static constexpr int kTableResultMatchAddrPos = MauDefs::kTableResultMatchAddrPos;
    static constexpr int kTableResultMatchAddrWidth = MauDefs::kTableResultMatchAddrWidth;
    static constexpr uint32_t kTableResultMatchAddrMask = (1u<<kTableResultMatchAddrWidth)-1;

    static constexpr int kSelImmData = 0;
    static constexpr int kSelInstr   = 1;
    static constexpr int kSelActData = 2;
    static constexpr int kSelStats   = 3;
    static constexpr int kSelMeter   = 4;
    static constexpr int kSelIdle    = 5;
    static constexpr int kSelNxtTab  = 6;
    static constexpr int kSelSelLen  = 7;
    static constexpr int kSelMin     = kSelImmData;
    static constexpr int kSelMax     = kSelSelLen;

 public:
    static bool kRelaxLookupShiftPfePosCheck;     // Defined in rmt-config.cpp
    static bool kRelaxLookupShiftOpPosCheck;      // Defined in rmt-config.cpp
    static bool kRelaxPayloadShifterEnabledCheck; // Defined in rmt-config.cpp

    static const char    *kSelNameTab[];
    static const uint8_t  kAddrMaskWidthTab[];
    static const uint8_t  kAddrWidthTab[];
    static const uint8_t  kAddrPfePosTab[];
    static const uint8_t  kAddrOpPosTab[];
    static const uint8_t  kPhysBusPadTab[];
    static const uint8_t  kPhysPerentryBitsTab[];
    static const uint8_t  kPhysOpBitsTab[];
    static const uint8_t  kPhysVpnshiftPosTab[];
    static const uint8_t  kPhysSwizzleBitsTab[];
    static const uint8_t  kPhysSwizzleToPosTab[];
    static const uint32_t kPhysDfltMaskTab[];
    static const bool     kLogDfltBeforeMaskTab[];
    static const uint32_t kLogDfltBfMaskMaskTab[];

    MauLookupResult()
        : mau_(nullptr),
          ltab_(nullptr),
          cnf_(nullptr),
          phv_(nullptr) {
      reset();
      set_valid(false);
    }
    ~MauLookupResult() { valid_ = false; }

    inline bool     valid()           const { return valid_; }
    inline bool     active()          const { return active_; }
    inline bool     inactive()        const { return inactive_; }
    inline bool     match()           const { return match_; }
    inline bool     exactran()        const { return exactran_; }
    inline bool     exactmatch()      const { return exactmatch_; }
    inline bool     gatewayran()      const { return gatewayran_; }
    inline bool     gatewaymatch()    const { return gatewaymatch_; }
    inline int      gatewayhitindex() const { return gatewayhitindex_; }
    inline bool     gatewayinhibit()  const { return gatewayinhibit_; }
    inline bool     gateway_payload_disabled()  const { return gateway_payload_disabled_; }
    // Leave this for backward compatibility
    inline bool     gateway()         const { return gatewayinhibit_; }
    inline bool     stashran()        const { return stashran_; }
    inline bool     stash()           const { return stash_; }
    inline bool     ternaryran()      const { return ternaryran_; }
    inline bool     ternarymatch()    const { return ternarymatch_; }
    inline bool     ternaryindirect() const { return ternaryindirect_; }
    inline int      ternaryhitindex() const { return ternaryhitindex_; }
    inline uint8_t  ternarypayload()  const { return ternarypayload_; }
    inline int      row()             const { return row_; }
    inline int      col()             const { return col_; }
    inline int      outbus()          const { return outbus_; }
    inline int      hitindex()        const { return hitindex_; }
    inline int      hitentry()        const { return hitentry_; }
    inline uint16_t logical_tables()  const { return logical_tables_; }
    inline uint16_t next_table_orig() const { return next_table_orig_; }
    inline uint16_t next_table_mask() const { return next_table_mask_; }
    inline uint16_t next_table_form() const { return next_table_form_; }
    inline uint16_t next_table_pred() const { return next_table_pred_; }
    inline uint16_t next_table()      const { return next_table_; }
    inline uint8_t  payload()         const { return payload_; }
    inline uint8_t  instr()           const { return instr_; }
    inline bool     ingress()         const { return ingress_; }
    inline bool     has_bitmask_ops() const { return bitmask_ops_; }
    inline int      logical_table()   const { return logical_table_; }
    inline uint16_t match_buses()     const { return match_buses_; }
    inline uint16_t tind_buses()      const { return tind_buses_; }
    inline uint8_t  ltcams()          const { return ltcams_; }
    inline uint8_t  tallied()         const { return tallied_; }

    inline uint8_t  result_bus()      const { return (row_ << 1) | ((outbus_>>1) & 0x1); }
    inline bool     invalid()         const { return !valid_; }
    inline bool     miss()            const { return !match_; }
    inline Mau*     mau()             const { return mau_; };

    inline void     set_ingress(bool ingress)       { ingress_ = ingress; }
    inline void     set_valid(bool tf)              { valid_ = tf; }
    inline void     set_active(bool tf)             { active_ = tf; inactive_ = !tf; }
    inline void     set_match(bool tf)              { match_ = tf; }
    inline void     set_exactran(bool tf)           { exactran_ = tf; }
    inline void     set_exactmatch(bool tf)         { exactmatch_ = tf; }
    inline void     set_gatewayran(bool tf)         { gatewayran_ = tf; }
    inline void     set_gatewaymatch(bool tf)       { gatewaymatch_ = tf; }
    inline void     set_gatewayhitindex(int v)      { gatewayhitindex_ = v; }
    inline void     set_gatewayinhibit(bool tf)     { gatewayinhibit_ = tf; }
    inline void     set_gateway_payload_disabled(bool tf) { gateway_payload_disabled_ = tf; }
    inline void     set_stashran(bool tf)           { stashran_ = tf; }
    inline void     set_stash(bool tf)              { stash_ = tf; }
    inline void     set_ternaryran(bool tf)         { ternaryran_ = tf; }
    inline void     set_ternarymatch(bool tf)       { ternarymatch_ = tf; }
    inline void     set_ternaryindirect(bool tf)    { ternaryindirect_ = tf; }
    inline void     set_ternaryhitindex(int v)      { ternaryhitindex_ = v; }
    inline void     set_ternarypayload(uint8_t v)   { ternarypayload_ = v; }
    inline void     set_row(int v)                  { row_ = v; }
    inline void     set_col(int v)                  { col_ = v; }
    inline void     set_outbus(int v)               { outbus_ = v; }
    inline void     set_hitindex(int v)             { hitindex_ = v; }
    // when gateway_payload_disabled_ is true the lookup runs as normal, but the hitentry must
    //   comes from the gateway table, so we must prevent it being set by the normal lookup
    inline void     set_hitentry(int v) {
      if ( ! gateway_payload_disabled_ )  hitentry_ = v;
    }
    // this version is unconditional for use in the gateway lookup itself
    inline void     set_hitentry_gateway(int v) {
      hitentry_ = v;
    }
    inline void     set_logical_tables(uint16_t v)  { logical_tables_ = v; }
    inline void     set_next_table_orig(uint16_t v) { next_table_orig_ = v; next_table_ = v; }
    inline void     set_next_table_mask(uint16_t v) { next_table_mask_ = v; next_table_ = v; }
    inline void     set_next_table_form(uint16_t v) { next_table_form_ = v; next_table_ = v; }
    inline void     set_next_table_pred(uint16_t v) { next_table_pred_ = v; }
    // when gateway_payload_disabled_ is true the lookup runs as normal, but the next table must
    //   comes from the gateway table, so we must prevent it being set by the normal lookup
    inline void     set_next_table(uint16_t v)      {
      if (! gateway_payload_disabled_ )  set_next_table_orig(v);
    }
    inline void     set_payload(uint8_t v)          { payload_ = v; }

    inline void set_match_buses(uint16_t buses) {
      match_buses_ = buses;
    }
    inline void set_tind_buses(uint16_t buses) {
      tind_buses_ = buses;
    }
    inline void set_ltcams(uint8_t ltcams) {
      ltcams_ = ltcams;
    }
    inline void set_logical_table(int v) {
      RMT_ASSERT((v >= 0) && (v < kLogicalTables));
      RMT_ASSERT((logical_table_ < 0) || (logical_table_ == v));
      logical_table_ = v;
    }
    inline void set_result_bus_from_physbus_index(int physbus) {
      set_row(physbus >> 1);
      set_outbus(1 <<(physbus & 0x1)); // So either 0x1 or 0x2
    }

    inline void copy(const MauLookupResult& r) {
      valid_ = r.valid();
      active_ = r.active();
      inactive_ = r.inactive();
      match_ = r.match();
      exactran_ = r.exactran();
      exactmatch_ = r.exactmatch();
      gatewayran_ = r.gatewayran();
      gatewaymatch_ = r.gatewaymatch();
      gatewayhitindex_ = r.gatewayhitindex();
      gatewayinhibit_ = r.gatewayinhibit();
      gateway_payload_disabled_ = r.gateway_payload_disabled();
      stashran_ = r.stashran();
      stash_ = r.stash();
      ternaryran_ = r.ternaryran();
      ternarymatch_ = r.ternarymatch();
      ternaryindirect_ = r.ternaryindirect();
      ternaryhitindex_ = r.ternaryhitindex();
      ternarypayload_ = r.ternarypayload();
      row_ = r.row();
      col_ = r.col();
      outbus_ = r.outbus();
      hitindex_ = r.hitindex();
      hitentry_ = r.hitentry();
      logical_tables_ = r.logical_tables();
      next_table_orig_ = r.next_table_orig();
      next_table_mask_ = r.next_table_mask();
      next_table_form_ = r.next_table_form();
      next_table_pred_ = r.next_table_pred();
      next_table_ = r.next_table();
      payload_ = r.payload();
      instr_ = r.instr();
      logical_table_ = r.logical_table();
      match_buses_ = r.match_buses();
      tind_buses_ = r.tind_buses();
      ltcams_ = r.ltcams();
      tallied_ = r.tallied();
    }
    inline void reset() {
      valid_ = true;
      active_ = false;
      inactive_ = false;
      match_ = false;
      exactran_ = false;
      exactmatch_ = false;
      gatewayran_ = false;
      gatewaymatch_ = false;
      gatewayhitindex_ = -1;
      gatewayinhibit_ = false;
      gateway_payload_disabled_ = false;
      stashran_ = false;
      stash_ = false;
      ternaryran_ = false;
      ternarymatch_ = false;
      ternaryindirect_ = false;
      ternaryhitindex_ = -1;
      ternarypayload_ = 0;
      row_ = 0;
      col_ = 0;
      outbus_ = 0;
      hitindex_ = 0;
      hitentry_ = 0;
      logical_tables_ = 0;
      next_table_orig_ = 0;
      next_table_mask_ = NxtTab::inval_next_table();
      next_table_form_ = NxtTab::inval_next_table();
      next_table_pred_ = 0;
      next_table_ = 0;
      payload_ = 0;
      instr_ = 0;
      ltab_ = nullptr;
      logical_table_ = -1;
      ingress_ = true;
      bitmask_ops_ = false;
      match_buses_ = 0;
      tind_buses_ = 0;
      ltcams_ = 0;
      tallied_ = 0;
      reset_addresses();
    }
    inline void invalidate() {
      reset();
      valid_ = false;
    }

    void init(Mau *mau, MauResultBus* cnf);  // you have to call this!

    void setup_lookup(Phv *phv,int logtab);
    uint16_t extract_next_table(Mau *mau, int logtab);

    // These mostly assume the lt is active! (ie predicated ON)
    // hence 4th param = true in the calls to get_log_data
    //
    // However in the case of extract_meter_addr you can specify
    // getting the pre-predication address if you want.
    // Note all 'normal' funcs just want the post-predication
    // address which is the default.
    // The only func that requests the pre-predication address
    // is MauLogicalTable::extract_addrs which is only used by DV.
    //
    uint32_t extract_action_instr_raw_addr(Mau *mau, int logtab) {
      return get_log_data(cnf_, kSelInstr, logtab, true);
    }
    uint32_t extract_action_instr_addr(Mau *mau, int logtab) {
      uint32_t data = extract_action_instr_raw_addr(mau, logtab);
      uint8_t  instr_width = get_addr_width(cnf_, kSelInstr, 99, 99, 99);
      uint32_t instr_mask = (1u<<instr_width)-1;
      // Preserve masked val in instr_ var (DV wrapper uses it)
      instr_ = static_cast<uint8_t>(data & instr_mask);
      if (!has_bitmask_ops()) {
        // If NOT bitmask op, overwrite 1b above PFE with ingress/egress bit
        uint8_t ie_bit = get_addr_pfe_pos(cnf_, kSelInstr, 99, 99, 99) + 1;
        instr_ &= ~(1 << ie_bit);
        instr_ |= ((ingress() ?0 :1) << ie_bit);
      }
      return static_cast<uint32_t>(instr_);
    }
    uint32_t extract_imm_data(Mau *mau, int logtab) {
      return get_log_data(cnf_, kSelImmData, logtab, true);
    }
    uint32_t extract_action_data_addr(Mau *mau, int logtab) {
      return get_log_data(cnf_, kSelActData, logtab, true);
    }
    uint32_t extract_stats_addr(Mau *mau, int logtab) {
      return get_log_data(cnf_, kSelStats, logtab, true);
    }
    uint32_t extract_meter_addr(Mau *mau, int logtab, bool post_predication=true) {
      return get_log_data(cnf_, kSelMeter, logtab, post_predication);
    }
    uint32_t extract_idletime_addr(Mau *mau, int logtab) {
      return get_log_data(cnf_, kSelIdle, logtab, true);
    }
    uint32_t extract_selector_len(Mau *mau, int logtab) {
      return get_log_data(cnf_, kSelSelLen, logtab, true);
    }

    // Needs to be public as called by MauHashDistribution logic (via Mau)
    uint32_t get_selector_action_address(Phv *phv, int logtab);
    uint32_t get_selector_address(Phv *phv, int logtab);

    // This is for DV to get the selector length to pass to the hash distribution functions
    // Note, get_selector_address and get_selector_action_address BOTH now need to be
    // passed the ALU corresponding to the logical_table NOT the logical_table.
    uint32_t get_selector_length(int logtab);

    // This is for DV to get access to the physical buses
    void get_bus(int busIndex, int xm_tm, BitVector<kTableResultBusWidth> *bus);


    // Used by Snapshot to get match addresses from physical buses
    uint32_t get_match_addr(int busIndex, int xm_tm);



 private:
    const char *get_sel_name(int sel);
    void get_tind_offset_nbits(MauResultBus *cnf,
                               int sel, int busIndex, int xm_tm, int match,
                               uint32_t matchAddr, uint8_t *off, uint8_t *nbits);
    bool get_tind_bus(MauResultBus *cnf,
                      int sel, int busIndex, int xm_tm, int match,
                      BitVector<kTableResultBusWidth> *bus);
    bool get_match_bus(MauResultBus *cnf,
                       int sel, int busIndex, int xm_tm, int match,
                       BitVector<kTableResultBusWidth> *bus);
    bool payload_shifter_enabled(MauResultBus *cnf,
                                 int sel, int busIndex, int xm_tm, int match);
    bool get_bus(MauResultBus *cnf,
                 int sel, int busIndex, int xm_tm, int match,
                 BitVector<kTableResultBusWidth> *bus);
    uint8_t get_phy_bus_padding(MauResultBus *cnf,
                                int sel, int busIndex, int xm_tm, int match);
    uint32_t get_phy_dflt_mask(MauResultBus *cnf,
                                int sel, int busIndex, int xm_tm, int match);
    uint64_t get_bus_data(MauResultBus *cnf,
                          int sel, int busIndex, int xm_tm, int match,
                          BitVector<kTableResultBusWidth> *bus,
                          uint8_t padding, uint8_t shift);
    uint8_t get_addr_mask_width(MauResultBus *cnf,
                                int sel, int busIndex, int xm_tm, int match);
    uint32_t get_addr_mask(MauResultBus *cnf,
                           int sel, int busIndex, int xm_tm, int match);
    uint8_t get_addr_width(MauResultBus *cnf,
                           int sel, int busIndex, int xm_tm, int match);
    uint8_t get_addr_pfe_pos(MauResultBus *cnf,
                             int sel, int busIndex, int xm_tm, int match);
    uint8_t get_addr_op_pos(MauResultBus *cnf,
                             int sel, int busIndex, int xm_tm, int match);
    uint8_t get_phy_shift(MauResultBus *cnf,
                          int sel, int busIndex, int xm_tm, int match);
    uint32_t get_phy_mask(MauResultBus *cnf,
                          int sel, int busIndex, int xm_tm, int match);
    uint32_t get_phy_dflt(MauResultBus *cnf,
                          int sel, int busIndex, int xm_tm, int match);
    uint8_t get_phy_vpnshift_pos(MauResultBus *cnf,
                                 int sel, int busIndex, int xm_tm, int match);
    uint8_t get_phy_vpnshift(MauResultBus *cnf,
                             int sel, int busIndex, int xm_tm, int match);
    bool get_phy_vpnshift_ins_zeros(MauResultBus *cnf,
                                    int sel, int busIndex, int xm_tm, int match,
                                    uint8_t *pos, uint8_t *n_bits);
    bool get_phy_ins_zeros(MauResultBus *cnf,
                           int sel, int busIndex, int xm_tm, int match,
                           uint8_t *pos, uint8_t *n_bits);

    uint8_t get_phy_perentry_bits(MauResultBus *cnf,
                                  int sel, int busIndex, int xm_tm, int match);
    uint8_t get_phy_perentry_pos(MauResultBus *cnf,
                                 int sel, int busIndex, int xm_tm, int match);
    uint32_t get_pfe(MauResultBus *cnf,
                     int sel, int busIndex, int xm_tm, int match,
                     uint8_t padding, uint8_t shift, uint32_t dflt);

    bool get_phy_perentry_copy_bits(MauResultBus *cnf,
                                    int sel, int busIndex, int xm_tm, int match,
                                    uint8_t *from_pos, uint8_t *to_pos, uint8_t *n_bits,
                                    bool *use_pre_ins_data);

    uint8_t get_phy_op_bits(MauResultBus *cnf,
                            int sel, int busIndex, int xm_tm, int match);
    uint8_t get_phy_op_pos(MauResultBus *cnf,
                           int sel, int busIndex, int xm_tm, int match);
    uint32_t get_op(MauResultBus *cnf,
                    int sel, int busIndex, int xm_tm, int match,
                    uint8_t padding, uint8_t shift, uint32_t dflt);

    bool get_phy_pre_mask_copy_bits(MauResultBus *cnf,
                                    int sel, int busIndex, int xm_tm, int match,
                                    uint8_t *from_pos, uint8_t *to_pos, uint8_t *n_bits);
    bool get_phy_post_mask_copy_bits(MauResultBus *cnf,
                                     int sel, int busIndex, int xm_tm, int match,
                                     uint8_t *from_pos, uint8_t *to_pos, uint8_t *n_bits,
                                     bool *use_pre_ins_data);
    uint8_t get_phy_swizzlemode(MauResultBus *cnf,
                                int sel, int busIndex, int xm_tm, int match);
    uint8_t get_phy_swizzle_bits(MauResultBus *cnf,
                                 int sel, int busIndex, int xm_tm, int match);
    uint8_t get_phy_swizzle_to_pos(MauResultBus *cnf,
                                   int sel, int busIndex, int xm_tm, int match);
    uint8_t get_phy_swizzle_from_pos(MauResultBus *cnf,
                                     int sel, int busIndex, int xm_tm, int match);
    bool get_phy_swizzle_move_bits(MauResultBus *cnf,
                                   int sel, int busIndex, int xm_tm, int match,
                                   uint8_t *from_pos, uint8_t *to_pos, uint8_t *n_bits);
    bool get_phy_move_bits(MauResultBus *cnf,
                           int sel, int busIndex, int xm_tm, int match,
                           uint8_t *from_pos, uint8_t *to_pos, uint8_t *n_bits);
    //uint32_t get_phy_hash_distribution_result(MauResultBus *cnf,int sel, int busIndex, int xm_tm, int match);
    uint32_t get_hash_distribution_result(MauResultBus *cnf, int sel, int logtab, int match,
                                          bool post_predication);

    uint32_t get_phy_data(MauResultBus *cnf,
                          int sel, int busIndex, int xm_tm, int match);
    uint32_t get_phy_data(MauResultBus *cnf,
                          int sel, int xm_tm, int match);

    // per MeterALU accessors
    uint32_t get_phy_alu_data(MauResultBus *cnf, int sel, int xm_tm, int alu, int match);
    uint32_t get_phy_alu_data(MauResultBus *cnf, int sel, int alu, int match);
    uint32_t get_phy_alu_data(MauResultBus *cnf, int sel, int alu);


    bool flow_enabled(MauResultBus *cnf,
                      int sel, int logtab, int xm_tm, int match,
                      uint32_t data);
    bool flow_enabled(MauResultBus *cnf, int sel, int logtab,
                      uint32_t data);
    bool flow_enabled_for_map(MauResultBus *cnf,
                              int sel, int logtab, int xm_tm, int match,
                              uint32_t data);
    bool flow_enabled_for_actionbit_map(MauResultBus *cnf,
                                        int sel, int logtab,
                                        int xm_tm, int match, uint32_t data);
    uint32_t get_log_mask(MauResultBus *cnf,
                          int sel, int logtab, int xm_tm, int match);
    uint32_t get_log_mask_map(MauResultBus *cnf,
                              int sel, int logtab, int xm_tm, int match);
    uint32_t get_log_dflt(MauResultBus *cnf,
                          int sel, int logtab, int xm_tm, int match);
    bool get_log_dflt_before_mask(MauResultBus *cnf,
                                  int sel, int logtab, int xm_tm, int match);
    uint32_t get_log_dflt_before_mask_mask(MauResultBus *cnf,
                                           int sel, int logtab, int xm_tm, int match);
    bool map_enabled_xm(MauResultBus *cnf,
                        int sel, int logtab, int xm_tm, int match);
    bool map_enabled_tm(MauResultBus *cnf,
                        int sel, int logtab, int xm_tm, int match);
    bool actionbit_map_enabled(MauResultBus *cnf,
                               int sel, int logtab, int xm_tm, int match);
    bool map_enabled_log(MauResultBus *cnf,
                         int sel, int logtab, int xm_tm, int match);
    uint32_t apply_log_dflt_mask(MauResultBus *cnf,
                                 int sel, int logtab, int xm_tm, int match,
                                 uint32_t data);
    uint32_t no_log_dflt_mask(MauResultBus *cnf,
                              int sel, int logtab, int xm_tm, int match,
                              uint32_t data);

    uint32_t no_map_data(MauResultBus *cnf,
                         int sel, int logtab, int xm_tm, int match,
                         uint32_t data);
    uint32_t map_data(MauResultBus *cnf,
                      int sel, int logtab, int xm_tm, int match,
                      uint32_t data);
    uint32_t actionbit_map_data(MauResultBus *cnf,
                                int sel, int logtab, int xm_tm, int match,
                                uint32_t data);
    uint32_t get_log_data_xm(MauResultBus *cnf,
                             int sel, int logtab, int match);
    uint32_t get_log_data_tm(MauResultBus *cnf,
                             int sel, int logtab, int match);
    uint32_t get_log_data_hit(MauResultBus *cnf,
                              int sel, int logtab, int match);
    uint32_t get_log_data_miss(MauResultBus *cnf, int sel, int logtab);
    void     tally_hits(MauResultBus *cnf, int sel, int logtab);
    void     tally_misses(MauResultBus *cnf, int sel, int logtab);
    uint32_t final_futz(MauResultBus *cnf, int sel, int logtab, uint32_t data);
    uint32_t get_log_data(MauResultBus *cnf, int sel, int logtab,
                          bool post_predication);

    void reset_addresses();
    void accumulate_addresses(MauLogicalTable *ltab,
                              const BitVector<kTableResultBusWidth>& bus);

 private:
    bool      valid_;
    bool      active_;
    bool      inactive_;
    bool      match_;
    bool      exactran_;
    bool      exactmatch_;
    bool      gatewayran_;
    bool      gatewaymatch_;
    int       gatewayhitindex_;
    bool      gatewayinhibit_;
    bool      gateway_payload_disabled_;
    bool      stashran_;
    bool      stash_;
    bool      ternaryran_;
    bool      ternarymatch_;
    bool      ternaryindirect_;
    int       ternaryhitindex_;
    uint8_t   ternarypayload_;
    int       row_;
    int       col_;
    int       outbus_;
    int       hitindex_;
    int       hitentry_;
    uint16_t  logical_tables_;
    uint16_t  next_table_orig_;
    uint16_t  next_table_mask_;
    uint16_t  next_table_form_;
    uint16_t  next_table_pred_;
    uint16_t  next_table_;
    uint8_t   payload_;
    uint8_t   instr_;
    Mau             *mau_;
    MauLogicalTable *ltab_;
    MauResultBus    *cnf_;
    Phv             *phv_;
    bool      ingress_;
    bool      bitmask_ops_;
    int       logical_table_;
    uint16_t  match_buses_;
    uint16_t  tind_buses_;
    uint8_t   ltcams_;
    uint8_t   tallied_;
  };
}
#endif // _SHARED_MAU_LOOKUP_RESULT_
