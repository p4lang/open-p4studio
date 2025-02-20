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
#include <mau-logical-table.h>
#include <string>
#include <rmt-log.h>
#include <rmt-object-manager.h>
#include <mau-stateful-counters.h>

namespace MODEL_CHIP_NAMESPACE {

  MauLogicalTable::MauLogicalTable(RmtObjectManager *om,
                                   int pipeIndex, int mauIndex, int tableIndex,
                                   Mau *mau)
      : MauObject(om, pipeIndex, mauIndex, kType, tableIndex, mau),
        table_index_(tableIndex), sram_columns_(0u), logical_tcams_(0u),
        gateway_payloads_{}, evaluate_all_(kEvaluateAllDefault),
        mau_logical_table_reg_(om, pipeIndex, mauIndex, tableIndex, this),
        mau_(mau) {
    // Store ptr to per-MAU result bus and per-MAU address distribution
    mau_result_bus_ = mau->mau_result_bus();
    mau_deps_ = mau->mau_dependencies();
    mau_addr_dist_ = mau->mau_addr_dist();
    mau_hash_dist_ = mau->mau_hash_dist();
    mau_instr_store_ = mau->mau_instr_store();
    mau_stateful_counters_ = mau->mau_stateful_counters();
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugCreate),
            "MauLogicalTable::create\n");
  }
  MauLogicalTable::~MauLogicalTable() {
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugDelete),
            "MauLogicalTable::delete\n");
  }


  static_assert( (MauLogicalTable::kSramColumnsPerMau == 12),
                 "kDescendingPriColIndexTab must change if cols != 12");

  // Process SRAM columns in this order - NB absence of cols 0,1 handled inside lookup_exact_match()
  const int MauLogicalTable::kDescendingPriColIndexTab[MauLogicalTable::kSramColumnsPerMau] = {
    0, 1, 2, 3, 4, 5, 11, 10, 9, 8, 7, 6
  };
  // This table allows us to suppress lookups in other columns when we see hits
  // Potentially useful to limit work in evaluate_all mode
  // NOT currently used - DV want to see all hits always
  const uint32_t MauLogicalTable::kSuppressColLookupOnHitMaskTab[MauLogicalTable::kSramColumnsPerMau] = {
    //  0,     1,     2,     3,     4,     5,    11,    10,     9,     8,     7,     6
    ~0x0u, ~0x0u, ~0x0u, ~0x0u, ~0x0u, ~0x0u, ~0x0u, ~0x0u, ~0x0u, ~0x0u, ~0x0u, ~0x0u
  };


  bool MauLogicalTable::lookup_match(Phv *match_phv, MauLookupResult *result,
                                     bool ingress, bool sram_tcam) {
    bool ternary_hit = false;
    bool exact_hit = false;
    bool stash_hit = false;

    bool gateway_inhibit_associated_table = lookup_gateway(match_phv, result, ingress);
    RmtObjectManager *om = get_object_manager();
    auto table_name = om->get_table_name(pipe_index(), s_index(), table_index());

    // Only run the normal lookups if the gateway inhibit is false.
    // Unless gateway_payload_disabled() is true in which case the gateway table
    //  is not meant to provide the match payload and we need to run the normal lookup
    //  to drive the match payload. In this case the gateway table has already provided
    //  the next table and the gateway_payload_disabled flag in the result prevents
    //  this next table being overwritten by the normal lookup.
    if ( (! gateway_inhibit_associated_table) ||
         result->gateway_payload_disabled() ) {
      if ((sram_tcam) && (logical_tcams_ != 0)) {
        RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTableLookup),
                "%s MauLogicalTable::lookup_ternary_match(%d) 0x%08x START...\n",
                table_name.c_str(), table_index(), logical_tcams_);
        ternary_hit = lookup_ternary_match(match_phv, result, ingress);
        RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTableLookup),
                "%s MauLogicalTable::lookup_ternary_match(%d) END\n",
                table_name.c_str(), table_index());
      }
      if ((sram_tcam) && (sram_columns_ != 0)) {
        RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTableLookup),
                "%s MauLogicalTable::lookup_exact_match(%d) 0x%08x START...\n",
                table_name.c_str(), table_index(), sram_columns_);
        exact_hit = lookup_exact_match(match_phv, result, ingress);
        RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTableLookup),
                "%s MauLogicalTable::lookup_exact_match(%d) END\n",
                table_name.c_str(), table_index());
      }
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTableLookup),
              "%s MauLogicalTable::lookup_stash(%d) START...\n",
              table_name.c_str(), table_index());
      stash_hit = lookup_stash(match_phv, result, ingress);
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTableLookup),
              "%s MauLogicalTable::lookup_stash(%d) END\n",
              table_name.c_str(), table_index());
    }
    else {
      return true; // gateway table provided the result
    }

    RMT_ASSERT( ! ( exact_hit && stash_hit ) );

    if (ternary_hit && (exact_hit || stash_hit)) {
      RMT_LOG_VERBOSE("Found ternary match AND exact match\n");
    }
    return ternary_hit || exact_hit || stash_hit;
  }


  bool MauLogicalTable::lookup_gateway(Phv *match_phv, MauLookupResult *result,
                                           bool ingress) {
    if ((!check_ingress_egress(ingress)) || (match_phv == NULL) || (!enabled())) return false;
    if (!gateway_is_enabled()) return false;

    Mau *mauobj = mau();
    RMT_ASSERT (mauobj != nullptr);
    MauSramRow* row = mauobj->sram_row_lookup( gateway_table_row() );
    RMT_ASSERT (row != nullptr);
    int which = gateway_table_which();

    if (table_index_ != row->get_gateway_table_logical_table(which)) {
      RMT_LOG_WARN("Gateway Logical Table field does not match row=%d which=%d (%d)!\n",
                   gateway_table_row(),gateway_table_which(),
                   row->get_gateway_table_logical_table(which));
    }

    bool hit;
    int hit_index;
    row->get_gateway_table_result(match_phv,which,&hit,&hit_index);

    mauobj->mau_info_incr(MAU_GW_RUNS);
    if (hit) mauobj->mau_info_incr(MAU_GW_HITS);
    result->set_gatewayran(true);
    result->set_gatewaymatch(hit);
    result->set_gatewayhitindex(hit_index);

    bool inhibit_associated_table = gateway_get_inhibit(hit,hit_index);
    RMT_LOG(RmtDebug::verbose(),
            "MauLogicalTable::lookup_gateway inhibit_associated_table=%s\n",
            inhibit_associated_table?"true":"false");
    if ( inhibit_associated_table ) {
      mauobj->mau_info_incr(MAU_GW_INHIBITS);

      uint16_t nxt_tab = gateway_get_next_table(hit,hit_index);
      result->set_next_table(nxt_tab);
      result->set_match(true);
      result->set_gatewayinhibit(true);
      RMT_LOG(RmtDebug::verbose(),
              "MauLogicalTable::lookup_gateway nxt-tab=%d(0x%x)\n",
              nxt_tab,nxt_tab);
      // Gateway table taking over, so fire the payloads
      for (int p=0;p<kGatewayPayloads;++p) {
        for (int b=0;b<2;++b) {
          MauDefs::BusTypeEnum bus = b ? MauDefs::kExactMatchBus : MauDefs::kTindBus;

          if ( active_gateway_payload( bus, p ) ) {
            int row_n = p / kGatewayPayloadsPerRow;
            int which = p % kGatewayPayloadsPerRow;
            RMT_LOG(RmtDebug::verbose(),
                    "MauLogicalTable::lookup_gateway payload row=%d which=%d\n",
                    row_n,which);
            Mau *mauobj = mau();
            RMT_ASSERT (mauobj != nullptr);
            MauSramRow* row = mauobj->sram_row_lookup( row_n );
            RMT_ASSERT (row != nullptr);
            int which_bus = which; // payload can only drive its own bus
            RMT_LOG(RmtDebug::verbose(),
                    "MauLogicalTable::lookup_gateway payload_enabled bus=%d which=%d\n",
                    b,which);
            bool em_bus_enabled,tind_bus_enabled;
            row->get_gateway_payload_result_busses_enabled(which,&em_bus_enabled,&tind_bus_enabled);

            if (( em_bus_enabled   && (bus == MauDefs::kExactMatchBus)) ||
                ( tind_bus_enabled && (bus == MauDefs::kTindBus))) {

              bool em_disable,tind_disable;
              row->get_gateway_payload_disable(which,&em_disable,&tind_disable);

              // check if this gateway payload is disabled (never is in Tofino)
              if (( em_disable   && (bus == MauDefs::kExactMatchBus)) ||
                  ( tind_disable && (bus == MauDefs::kTindBus))) {
                // Tofino will never get here
                // Do not put the payload on here, but set this flag, so later
                //  the normal lookup will run and set the bus.
                // This flag also stops the normal lookup from setting the
                //  next table, as this is meant to come from the gateway table
                //  in this case
                result->set_gateway_payload_disabled(true);
              }
              else {
                // Tofino will always take this path
                // Gateway table not disabled, put payload on bus
                row->put_gateway_payload_on_bus(which);
              }
            }

            result->set_row(row_n);
            result->set_outbus(1<<which_bus);
            if (bus == MauDefs::kTindBus) {
              result->set_ternarymatch(true);
              result->set_ternaryindirect(true);
              // need to get the match address from the payload
              //  in the ternary case as tind bus does not carry this
              uint32_t match_addr = row->get_payload_adr(which);
              // RefactorMLR: now also need to put match_addr onto row!
              row->set_tcam_match_addr(which_bus, match_addr);
              result->set_hitindex( match_addr );
            }
            else {
              result->set_exactmatch(true);
              result->set_hitindex(0);
            }
            // In Tofino always use extractor 0 (so set hit entry to 0), on
            //   In JBay if gateway_payload_exact_shift_ovr is set, then
            //   it uses the hit index (or 4 for a miss) to select the extractor
            if ((bus == MauDefs::kExactMatchBus) &&
                mau_result_bus_->get_gateway_payload_exact_shift_ovr(p)) {
              // Tofino can't get here
              if ( result->gatewaymatch() ) {
                result->set_hitentry_gateway( result->gatewayhitindex() );
              }
              else {
                result->set_hitentry_gateway(4);
              }
            }
            else {
              result->set_hitentry_gateway(0);
            }
          }
        }
      }
    }
    return inhibit_associated_table;
  }


  bool MauLogicalTable::lookup_exact_match(Phv *match_phv, MauLookupResult *result,
                                           bool ingress) {
    if ((!check_ingress_egress(ingress)) || (match_phv == NULL) || (!enabled())) return false;
    Mau *mauobj = mau();
    RMT_ASSERT (mauobj != NULL);
    mauobj->mau_info_incr(MAU_XM_RUNS);
    result->set_exactran(true);

    uint32_t sram_columns_copy = sram_columns_; // In case we ever allow async update
    int hitcnt = 0;
    for (int i = 0; i < kSramColumnsPerMau; i++) {
      // Go through cols in priority order so we can still allow ShortCircuit mode
      int colIndex = kDescendingPriColIndexTab[i];
      if (((sram_columns_copy & (1<<colIndex)) != 0) || evaluate_all()) {
        MauSramColumn *sram_column = mauobj->sram_column_lookup(colIndex);
        if ((sram_column != NULL) &&
            (sram_column->lookup(match_phv, table_index(), result))) {
          hitcnt++;
          if (hitcnt == 1) {
            bool wide = (result->hitindex() < 2);
            mauobj->mau_info_incr(MAU_XM_HITS);
            mauobj->mau_info_incr((wide) ?MAU_XM_WIDE_HITS :MAU_XM_NORMAL_HITS);
          }
          result->set_col(colIndex);
          if (!evaluate_all()) return true;
          sram_columns_copy &= kSuppressColLookupOnHitMaskTab[i];
        }
      }
    }
    return (hitcnt > 0);
  }


  void MauLogicalTable::lookup_paired_ltcam(Phv *match_phv, MauLookupResult *result,
                                            bool ingress, int ltc_index, int paired_ltc_index) {
    RMT_ASSERT((paired_ltc_index >= 0) && (paired_ltc_index < kLogicalTcamsPerMau));
    Mau *mauobj = mau();
    RMT_ASSERT (mauobj != NULL);
    mauobj->mau_info_incr(MAU_LTCAM_PAIRED_RUNS);

    MauLogicalTcam *ltcam2 = mauobj->logical_tcam_lookup(paired_ltc_index);
    RMT_ASSERT(ltcam2 != NULL);
    MauLookupResult result2;
    result2.init(mauobj, mauobj->mau_result_bus());
    result2.setup_lookup(match_phv, table_index());
    ltcam2->lookup_ternary_match(match_phv, table_index(),
                                 &result2, ingress, evaluate_all());
    if ((result->match() != result2.match()) ||
        (result->hitindex() != result2.hitindex())) {
      // Flag up an error in the ingress/egress packet of the match_phv
      mauobj->mau_info_incr(MAU_LTCAM_PAIRED_ERRORS);
      match_phv->set_pkterr(MauDefs::kErrMauTcamErrorDetected, ingress);
      RMT_LOG(RmtDebug::error(RmtDebug::kRmtDebugMauLogicalTableLookup,
                              kRelaxPairedLtcamErrors),
              "MauLogicalTable::lookup_paired_ltcam: "
              "Mismatch in paired ltcams %d %d - err logged in Phv\n",
              ltc_index, paired_ltc_index);

    }
  }

  bool MauLogicalTable::lookup_ternary_match(Phv *match_phv, MauLookupResult *result,
                                             bool ingress) {
    if ((!check_ingress_egress(ingress)) || (match_phv == NULL) || (!enabled())) return false;
    Mau *mauobj = mau();
    RMT_ASSERT (mauobj != NULL);
    mauobj->mau_info_incr(MAU_LTCAM_RUNS);
    result->set_ternaryran(true);

    uint32_t logical_tcams_copy = logical_tcams_;
    int hitcnt = 0;
    for (int ltcIndex = 0; ltcIndex < kLogicalTcamsPerMau; ltcIndex++) {
      if ((logical_tcams_copy & (1<<ltcIndex)) != 0) {
        MauLogicalTcam *logical_tcam = mauobj->logical_tcam_lookup(ltcIndex);
        RMT_ASSERT (logical_tcam != NULL);
        int paired_ltc_index = logical_tcam->paired_ltcam(ingress);

        if (logical_tcam->lookup_ternary_match(match_phv, table_index(),
                                               result, ingress, evaluate_all())) {
          mauobj->mau_info_incr(MAU_LTCAM_HITS);
          hitcnt++;

          // Lookup in paired LTCAM if we have one. This is a way of
          // detecting bit errors in important TCAMs
          if (paired_ltc_index >= 0)
            lookup_paired_ltcam(match_phv, result, ingress, ltcIndex, paired_ltc_index);

          if (!evaluate_all()) return true;
          if (hitcnt > 1) RMT_LOG_VERBOSE("Multiple logical tcam hits detected\n");
        }
        else {
          // Lookup in paired LTCAM also on no match
          if (paired_ltc_index >= 0)
            lookup_paired_ltcam(match_phv, result, ingress, ltcIndex, paired_ltc_index);
        }
      }
    }
    return (hitcnt > 0);
  }

  bool MauLogicalTable::lookup_stash(Phv *match_phv, MauLookupResult *result,
                                     bool ingress) {
    if ((!check_ingress_egress(ingress)) || (match_phv == NULL) || (!enabled())) return false;
    Mau *mauobj = mau();
    RMT_ASSERT( mauobj );
    MauStashColumn* stash_column = mauobj->stash_column_get();
    RMT_ASSERT( stash_column );
    bool hit = stash_column->lookup( match_phv, table_index_, result);
    return hit;
  }

  uint8_t MauLogicalTable::packet_color(Phv *match_phv, MauLookupResult *result) {
    uint8_t packet_color = 0;
    // Upto 4 color busses
    uint8_t color_buses = mau_result_bus_->get_which_color_buses(table_index_);
    int     color_bus = -1;

    if (color_buses != 0) {
      int n_buses = __builtin_popcount(color_buses);
      uint32_t meter_addr = result->extract_meter_addr(mau_,table_index_);
      int meter_index = Address::meter_addr_get_index(meter_addr);
      // For TofinoA0, there is a bug in snapshot that cannot distinguish if a gateway
      // is inhibiting a table. To work around this, the gateway_payload_match_adr is
      // configured to an invalid value with index == 0x3FF (1023).
      // So if we see this index on Tofino we DON'T complain about the address.
      bool maybe_snapA0_workaround = ((is_tofinoXX()) && (meter_index == 1023));

      // Get color from all selected color buses (insist only one provides it)
      // Complain if 1 bus only, it's not driven, and we have a hit (and !SnapA0)
      bool complain = (result->match() && !maybe_snapA0_workaround && (n_buses == 1));

      for (uint8_t bus = 0; bus < MauDefs::kNumColorBuses; bus++) {
        if (((color_buses >> bus) & 1) == 1) {
          uint8_t color = 0;
          mau_->get_color_bus(bus, &color, complain);
          RMT_ASSERT((color == 0) || (packet_color == 0));
          packet_color |= color;
          color_bus = static_cast<int>(bus);
        }
      }
    }

    uint32_t meter_addr = result->extract_meter_addr(mau(),table_index_);
    int op3 = Address::meter_addr_op3( meter_addr );

    if ( op3 == Address::kMeterOp3ColorAware ) {

      // get the precolor from the phv via the hash distribution
      int pre_color = mau_hash_dist_->get_meter_precolor(match_phv, table_index_);

      RMT_LOG(RmtDebug::verbose(),
              "MauLogicalTable::packet_color color aware pre_color=%d\n",pre_color);

      if ( pre_color > packet_color )
        packet_color = pre_color;
    }

    RMT_LOG(RmtDebug::verbose(),
            "MauLogicalTable::packet_color packet_color=%d, color_bus=%d\n",
            packet_color,color_bus);

    return packet_color;
  }

  void MauLogicalTable::output_immediate_data(Phv *match_phv, MauLookupResult *result, bool ingress) {
    Mau *mauobj = mau();
    RMT_ASSERT (mauobj != NULL);
    uint32_t imm_data = result->extract_imm_data(mauobj,table_index());

    // May need to OR in some RNG data
    uint8_t which_rng, byte_mask;
    mau_result_bus_->get_imm_data_rng_ctl(table_index(), &which_rng, &byte_mask);
    if (byte_mask != 0) {
      // Get 64b of immediate random data from Phv (this can be programmed up to match RTL)
      uint64_t imm_rand64 = match_phv->get_immediate_data_random_value(mau_index(),ingress);
      // Pick lo or hi rand 32b
      if (which_rng == 1) imm_rand64 >>= 32;
      uint32_t imm_rand32 = static_cast<uint32_t>(imm_rand64 & UINT64_C(0xFFFFFFFF));
      // Calculate bytemask to mask rand 32b
      uint32_t mask = 0u;
      for (int i = 0; i <= 3; i++) {
        if (((byte_mask >> i) & 1) == 1) mask |= 0xFFu << (i*8);
      }
      // And OR into imm_data
      imm_data |= (imm_rand32 & mask);
    }

    // the byte of mapped color can be or'ed into the MSByte of the immediate data
    uint32_t meter_addr = result->extract_meter_addr(mau(),table_index());
    bool meter_pfe = Address::meter_addr_enabled( meter_addr );
    if ( mau_result_bus_->get_meter_enable(table_index()) && meter_pfe ) {
      uint8_t color = packet_color(match_phv, result);
      imm_data |= (mau_result_bus_->map_color(color,table_index()) << 24);
    }

    // Map 3b OPs to 4b OPs for stateful_counters get_immediate_data, as a header time can use 0 colour for this
    constexpr bool sop_col_aware_becomes_sweep = is_jbay_or_later();
    uint32_t meter_addr_op4 = Address::meter_addr_map_op3en_to_op4(meter_addr, 0 /*color*/, sop_col_aware_becomes_sweep);

    // or in any stateful logging counter and maybe take off the stateful log stage vpn offset
    imm_data = mau_->mau_stateful_counters()->get_immediate_data( table_index(), imm_data, meter_addr_op4 );

    // Push immediate data onto action output bus
    action_hv_bus_output_imm_data(table_index(), imm_data);
  }

  void MauLogicalTable::distrib_action_data_address(Phv *match_phv, MauLookupResult *result,
                                                    bool ingress) {
    // Need line below for now - really should have an extra step to do this
    (void)result->extract_action_instr_addr(mau(),table_index());
    uint32_t action_data_addr = result->extract_action_data_addr(mau(),table_index());
    mau_addr_dist_->pre_pfe_checks_action(table_index(), ingress, action_data_addr);
    mau_addr_dist_->distrib_action_addresses(table_index(), ingress, action_data_addr);

  }
  void MauLogicalTable::distrib_idletime_address(Phv *match_phv, MauLookupResult *result,
                                                 bool ingress) {
    uint32_t idletime_addr = result->extract_idletime_addr(mau(),table_index());
    idletime_addr = Address::idletime_addr_map_en_to_op(idletime_addr); // Nop->MarkActive
    mau_addr_dist_->pre_pfe_checks_idle(table_index(), ingress, idletime_addr);
    mau_addr_dist_->distrib_idletime_addresses(table_index(), ingress, idletime_addr);
  }

  void MauLogicalTable::distrib_stats_address_at_hdr(Phv *match_phv, MauLookupResult *result,
                                                     bool ingress) {
    uint32_t stats_addr = result->extract_stats_addr(mau(),table_index());
    mau_addr_dist_->pre_pfe_checks_stats(table_index(), ingress, stats_addr);

    stats_addr = Address::stats_addr_map_en_to_op(stats_addr); // Nop->Stats
    Teop *teop = match_phv->teop();
    uint8_t eop_num = ingress ?match_phv->ingress_eopnum() :match_phv->egress_eopnum();
    (void)mau_addr_dist_->distrib_stats_addresses(table_index(), ingress, stats_addr, teop);
    mau_addr_dist_->defer_stats_addresses(table_index(), ingress, stats_addr, teop, eop_num);
  }

  void MauLogicalTable::distrib_meter_address_at_hdr(Phv *match_phv, MauLookupResult *result,
                                                     bool ingress) {
    constexpr bool sop_col_aware_becomes_sweep = is_jbay_or_later();

    uint32_t meter_addr_x = result->extract_meter_addr(mau(),table_index());
    bool meter_addr_x_en = Address::meter_addr_enabled(meter_addr_x);
    int meter_addr_x_op3 = Address::meter_addr_op3(meter_addr_x);
    if ((meter_addr_x_en) && (meter_addr_x_op3 == Address::kMeterOp3Nop) &&
        (!kRelaxHdrtimeMeterAddrNopCheck)) {
      // XXX: Check PFE-enabled meter addr is not NOP
      RMT_ASSERT(0 && "Extracted PFE-enabled MeterOP is NOP" );
    }
    mau_addr_dist_->pre_pfe_checks_meter(table_index(), ingress, meter_addr_x);

    // On JBay col_aware addresses are mapped to be Sweep OPs (aka StatefulClear OPs)
    bool col_aware = (meter_addr_x_op3 == Address::kMeterOp3ColorAware);
    if (sop_col_aware_becomes_sweep) col_aware = false;  // Ignore addr was col_aware

    // Only get packet_color() if meter addr is enabled (otherwise see spurious errors printed)
    uint8_t color = (meter_addr_x_en) ?packet_color(match_phv,result) :0;


    // SOP: Map 3b OPs to 4b OPs and if meter/lpf meter/color OR in colour (unless becoming Sweep)
    uint32_t meter_addr_sop = Address::meter_addr_map_op3en_to_op4(meter_addr_x, color,
                                                                   sop_col_aware_becomes_sweep);
    // Possibly increment a stateful_counter and then OR into addr
    // This invocation only active for Tofino - JBay handles stateful_counters within MauAddrDist
    uint32_t meter_addr_sop_before = meter_addr_sop;
    meter_addr_sop = mau_stateful_counters_->maybe_increment_stateful_counter(meter_addr_sop,
                                                                              table_index(),
                                                                              *result);
    // Complain if non-stateful addr modified - can only happen on Tofino
    if (meter_addr_sop != meter_addr_sop_before) {
      bool is_stateful = Address::meter_addr_is_stateful(meter_addr_sop);
      const char *qual = (is_stateful) ?"Stateful" :"Non-stateful";
      RMT_LOG((is_stateful) ?RmtDebug::verbose() :RmtDebug::error(),
              "MauLogicalTable::distrib_meter_address_at_hdr: "
              "%s meter_addr modified with stateful counter (0x%08x->0x%08x)\n",
              qual, meter_addr_sop_before, meter_addr_sop);
    }


    // EOP: Map 3b OPs to 4b OPs and if meter/lpf meter/color OR in colour (not mapped to Sweep)
    uint32_t meter_addr_eop = Address::meter_addr_map_op3en_to_op4(meter_addr_x, color,false);
    Teop *teop = match_phv->teop();
    uint8_t eop_num = ingress ?match_phv->ingress_eopnum() :match_phv->egress_eopnum();



    // Distribute addresses now (hdrtime) and/or defer addresses for later (eoptime)
    bool sop_distrib = mau_addr_dist_->distrib_meter_addresses(table_index(), ingress,
                                                               meter_addr_sop, teop, color);
    mau_addr_dist_->defer_meter_addresses(table_index(), ingress,
                                          meter_addr_eop, teop, eop_num);

    // Color aware meter addresses should NOT be distributed at hdrtime - complain if so
    if (col_aware && sop_distrib && !kRelaxHdrtimeMeterAddrColorCheck) {
      RMT_ASSERT(0 && "ColorAware MeterAddr invalid at HdrTime");
    }
  }


  // First distribution of addresses to backend - primarily used to
  // put meter addresses onto Idle/Stats bus so we can read color mapram
  // Only does anything if state->flags non-zero
  void MauLogicalTable::distrib_pbus_address_early(MauExecuteState *state) {
    int lt = static_cast<int>(state->logical_table_);
    RMT_ASSERT(lt == table_index());
    bool ing = is_ingress();
    uint32_t addr = state->addr_;
    if (state->addrtype_ != AddrType::kMeter) return;
    if (!Address::meter_addr_op_enabled(addr)) return;
    if (state->flags_ == 0) return;
    int meter_shift = Address::color_addr_get_shift(0u, AddrType::kMeter);

    if ((state->flags_ & MauExecuteState::kFlagsDistribMeterAddrOnStatsBus) != 0) {
      // Compensate for different shifts
      int stats_shift = Address::color_addr_get_shift(0u, AddrType::kStats);
      uint32_t s_addr = Address::addrShift(addr, meter_shift - stats_shift);
      mau_addr_dist_->distrib_stats_addresses(lt, ing, s_addr, false);
    }
    if ((state->flags_ & MauExecuteState::kFlagsDistribMeterAddrOnIdleBus) != 0) {
      // Compensate for different shifts
      int idle_shift = Address::color_addr_get_shift(0u, AddrType::kIdle);
      uint32_t i_addr = Address::addrShift(addr, meter_shift - idle_shift);
      mau_addr_dist_->distrib_idletime_addresses(lt, ing, i_addr);
    }
  }

  // Distribute stats/meter/idle address etc to backend
  // Here we insist OP has already been set on address and is not NOP
  void MauLogicalTable::distrib_pbus_address(MauExecuteState *state) {
    int lt = static_cast<int>(state->logical_table_);
    RMT_ASSERT(lt == table_index());
    bool ing = is_ingress();
    uint32_t addr = state->addr_;
    switch (state->addrtype_) {
      case AddrType::kStats:
        if (!Address::stats_addr_op_enabled(addr)) return;
	mau_addr_dist_->distrib_stats_addresses(lt, ing, addr, true);
        break;
      case AddrType::kMeter:
        if (!Address::meter_addr_op_enabled(addr)) return;
	//if (state->flags_ != 0) {
	//  Read color and stash (as OP4) in state if requested
	//  uint8_t color = 0;
	//  int color_bus = mau_result_bus_->get_which_color_bus(lt);
	//  if (color_bus != -1) {
	//     mau_->get_color_bus(color_bus, &color, true);
        //     state->rw_format_ = Address::meter_color_get_color_op4(color);
        //  }
        //}
	mau_addr_dist_->distrib_meter_addresses(lt, ing, addr, true);
        break;
      case AddrType::kSelect: case AddrType::kStateful:
        if (!Address::meter_addr_op_enabled(addr)) return;
	mau_addr_dist_->distrib_meter_addresses(lt, ing, addr, false);
        break;
      case AddrType::kIdle:
        if (!Address::idletime_addr_op_enabled(addr)) return;
	mau_addr_dist_->distrib_idletime_addresses(lt, ing, addr);
        break;
    }
  }


  // Output immediate data to ActionHVBus - first a byte, then a word
  void MauLogicalTable::action_hv_bus_output_imm_data_byte(int tab, int which_byte, uint8_t byte_val) {
    BitVector<kActionHVOutputBusWidth> *action_hv_bus = mau()->action_hv_output_bus();
    int posA = -1, posB = -1, posC = -1; // Each byte could potentially be output at 3 positions
    mau_result_bus_->get_action_hv_bus_imm_bytepos(tab, which_byte, &posA, &posB, &posC);
    if (posA >= 0) action_hv_bus->set_byte( action_hv_bus->get_byte( posA ) | byte_val, posA);
    if (posB >= 0) action_hv_bus->set_byte( action_hv_bus->get_byte( posB ) | byte_val, posB);
    if (posC >= 0) action_hv_bus->set_byte( action_hv_bus->get_byte( posC ) | byte_val, posC);
  }
  void MauLogicalTable::action_hv_bus_output_imm_data(int tab, uint32_t imm_data) {
    action_hv_bus_output_imm_data_byte(tab, 0, static_cast<uint8_t>((imm_data >>  0) & 0xFF));
    action_hv_bus_output_imm_data_byte(tab, 1, static_cast<uint8_t>((imm_data >>  8) & 0xFF));
    action_hv_bus_output_imm_data_byte(tab, 2, static_cast<uint8_t>((imm_data >> 16) & 0xFF));
    action_hv_bus_output_imm_data_byte(tab, 3, static_cast<uint8_t>((imm_data >> 24) & 0xFF));
  }

  // Note this func requests the pre-predication meter_addr (3rd param == false in call to
  // extract_meter_addr). Most 'normal' funcs don't specify a 3rd param and hence get default
  // post-predication meter_addr.
  void MauLogicalTable::extract_addrs(MauLookupResult *result,
                                      const BitVector<kTableResultBusWidth>& data,
                                      uint16_t *next_table,
                                      uint32_t *imm_data,
                                      uint32_t *act_data_addr,
                                      uint8_t  *act_instr_addr,
                                      uint32_t *stats_addr,
                                      uint32_t *meter_addr,
                                      uint32_t *idletime_addr,
                                      uint32_t *selector_len) {

    if (next_table) *next_table = result->extract_next_table(mau(),table_index());
    if (imm_data) *imm_data = result->extract_imm_data(mau(), table_index());
    if (act_data_addr) *act_data_addr = result->extract_action_data_addr(mau(), table_index());
    if (act_instr_addr) *act_instr_addr = result->extract_action_instr_addr(mau(), table_index());
    if (stats_addr) *stats_addr = result->extract_stats_addr(mau(), table_index());
    if (meter_addr) *meter_addr = result->extract_meter_addr(mau(), table_index(), false);
    if (idletime_addr) *idletime_addr = result->extract_idletime_addr(mau(), table_index());
    if (selector_len) *selector_len = result->extract_selector_len(mau(), table_index());

  }

  void MauLogicalTable::reset_resources() {
  }

  void MauLogicalTable::set_evaluate_all(bool tf) {
    Mau *mauobj = mau();
    RMT_ASSERT (mauobj != NULL);
    evaluate_all_ = tf;
    for (int colIndex = 0; colIndex < kSramColumnsPerMau; colIndex++) {
      MauSramColumn *sram_column = mauobj->sram_column_lookup(colIndex);
      if (sram_column != NULL) sram_column->set_evaluate_all(table_index(), tf);
    }
  }

  void MauLogicalTable::get_tind_offset_nbits(uint32_t matchAddr,int tind_bus,
                                              uint8_t *off, uint8_t *nbits) {
    RMT_ASSERT ((off != NULL) && (nbits != NULL));
    // default values
    *nbits = kTindOutputBusWidth;
    *off   = 0;
    // Figure out which subword bits to get from TIND bus
    uint8_t tind_datasel = mau_result_bus_->get_tind_ram_data_size(tind_bus);
    if ((tind_datasel > 0) && (tind_datasel < 6)) {
      int subword = tcam_match_addr_get_subword(matchAddr, tind_datasel-1);
      *nbits = 2<<tind_datasel; // 4,8,16,32,64
      *off = *nbits * subword;
      // the correct 64 bits have already been placed on the tind bus, so
      //  just use the offset within the 64 bits
      if (*off >= 64) {
        *off -= 64;
      }
    }
  }

}
