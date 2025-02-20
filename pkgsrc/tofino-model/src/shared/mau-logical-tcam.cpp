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
#include <mau-logical-tcam.h>

#include <string>
#include <algorithm>
#include <rmt-log.h>
#include <rmt-object-manager.h>
#include <mau-logical-table.h>
#include <mau-logical-tcam-col.h>
#include <mau-sram.h>

namespace MODEL_CHIP_NAMESPACE {

  MauLogicalTcam::MauLogicalTcam(RmtObjectManager *om,
                                 int pipeIndex, int mauIndex, int ltcamIndex,
                                 Mau *mau)
      : MauObject(om, pipeIndex, mauIndex, kType, ltcamIndex, mau),
        ltcam_index_(ltcamIndex), curr_seq_(0), pending_seq_(1),
        lookup_cache_id_(), lookup_cache_ingress_(), cached_result_(),
        ingress_(true), has_run_(false),
        logical_table_(-1), hits_(0u), misses_(0u),
        mau_logical_tcam_reg_(om, pipeIndex, mauIndex, ltcamIndex, this) {

    static_assert( ((kTcamRowsPerMau % 2) == 0),
                   "Need to recode calculate_chain if odd num TCAM rows");
    for (int tab = 0; tab < kLogicalTablesPerMau; tab++) {
      tind_srams_used_[tab].fill_all_zeros();
    }
    // Allocate per-col objects
    for (int col = 0; col < kTcamColumnsPerMau; col++) {
      cols_[col] = new MauLogicalTcamCol(chip_index(), pipeIndex, mauIndex,
                                         ltcamIndex, col, this);
    }
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugCreate),
            "MauLogicalTcam::create\n");
  }
  MauLogicalTcam::~MauLogicalTcam() {
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugDelete),
            "MauLogicalTcam::delete\n");
    // Free per-col objects
    for (int col = 0; col < kTcamColumnsPerMau; col++) {
      delete cols_[col];
      cols_[col] = NULL;
    }
    tcams_.clear();
  }



  bool MauLogicalTcam::lookup_ternary_match(Phv *match_phv, int logicalTableIndex,
                                            MauLookupResult *result,
                                            bool ingress, bool evalAll) {

    // NB. Function maybe called with logicalTableIndex = -1 (in DV_MODE)
    if ((match_phv == NULL) || (result == NULL) || (!enabled()))
      return false;

    int tcams_examined = 0;
    int tcams_lookedup = 0;
    Mau *mauobj = mau();
    RMT_ASSERT (mauobj != NULL);
    has_run_ = true;

    // It's possible multiple logical tables use same logical tcam
    // so see if we've already done this lookup and if so use cache
    // (BUT..... not sure if hardware actually supports this!)
    //
    if ((lookup_cache_ingress_ == ingress) &&
        (lookup_cache_id_.IsValid()) &&
        (lookup_cache_id_.Equals(match_phv->cache_id()))) {
      result->copy(cached_result_);
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTcamLookup),
              "MauLogicalTcam::lookup_ternary_match<%d,T=%d>=%d  Using cached result\n",
              ltcam_index_, logicalTableIndex, result->ternarymatch());
      if (result->ternarymatch()) {
        RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTcamLookup),
                "MauLogicalTcam::lookup_ternary_match<%d,T=%d>=%d  matchAddr=%d[0x%08x]\n",
                ltcam_index_, logicalTableIndex, result->ternarymatch(),
                result->hitindex(), result->hitindex());
      }
      return result->ternarymatch();
    }

    // WIP: we only run a physical TCAM if it is driving *some* powered LTCAM
    uint8_t powered_ltcams = mau()->get_powered_ltcams();

    // Go through our vector of TCAMs (which should be sorted by priority),
    // and lookup PHV in each. The TCAM lookup will return the highest pri
    // entry that matches
    auto it = tcams_.begin();
    while (it != tcams_.end()) {

      tcams_examined++;
      MauTcam *tcam = *it;
      RMT_ASSERT (tcam != NULL);
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTcamLookup),
              "MauLogicalTcam::lookup_ternary_match<%d,T=%d> "
              "Try TCAM(%d,%d) vpn=%d[0x%04x] poweredLTCAMs=0x%02x %s\n",
              ltcam_index_, logicalTableIndex, tcam->row_index(), tcam->col_index(),
              tcam->get_vpn(), tcam->get_vpn(), powered_ltcams,
              tcam->wide_match()?"WIDE":"");
      if (tcam->check_ingress_egress(ingress) &&
          tcam->drives_ltcam(ltcam_index_, powered_ltcams)) {
        bool bitmap;
        int start, entries;
        (void)tcam->get_ltcam_result_info(ltcam_index_, &start, &entries, &bitmap);

        tcams_lookedup++;

        int      hitIndex = -1;
        uint32_t hitAddr;
        if (bitmap) {
          // Bitmap (and may also be wide)
          // Do lookup_wide_match - which will afterwards compute bitmap result
          hitIndex = lookup_wide_match(match_phv, tcam, result, ingress, evalAll);
          if (hitIndex > 0) mauobj->mau_info_incr(MAU_LTCAM_BITMAP_HITS);
          if (hitIndex == 0) hitIndex = -1; // Map misses to -1
        }
        else if (tcam->wide_match()) {
          // Wide TCAM lookup - does pri lookups using consistent fixed HEAD
          hitIndex = lookup_wide_match(match_phv, tcam, result, ingress, evalAll);
          if (hitIndex >= 0) mauobj->mau_info_incr(MAU_LTCAM_WIDE_HITS);
          RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTcamLookup),
                  "MauLogicalTcam::lookup_ternary_match<%d,T=%d>=%d  {wide match}\n",
                  ltcam_index_, logicalTableIndex, hitIndex);

        } else if (entries < kTcamEntries) {
          // Not bitmap, not wide - optimise to do single TCAM lookup in [start,start+entries)
          int hitPri = tcam->lookup(match_phv, start+entries-1, start);
          if (hitPri >= 0) {
            hitIndex = tcam->get_index(hitPri);
            RMT_ASSERT((hitIndex >= start) && (hitIndex < start + entries));
            hitIndex -= start;
            mauobj->mau_info_incr(MAU_LTCAM_NORMAL_HITS);
          }

        } else {
          // Simple TCAM lookup - explicit index lookup using current val HEAD
          hitIndex = tcam->lookup_index(match_phv);
          if (hitIndex >= 0) mauobj->mau_info_incr(MAU_LTCAM_NORMAL_HITS);
          RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTcamLookup),
                  "MauLogicalTcam::lookup_ternary_match<%d,T=%d>=%d {normal match}\n",
                  ltcam_index_, logicalTableIndex, hitIndex);
        }

        if (hitIndex >= 0) {
          // Splice in VPN to get hitAddr (but no VPN in bitmap mode)
          hitAddr = tcam->get_hit_address(ltcam_index_, hitIndex);
          if (!match_phv->match_only() && (logicalTableIndex >= 0)) {
            // Pass hitAddr (unshifted VPN+hitIndex) to movereg code
            // We might need to advance a pending commit
            mauobj->mau_moveregs(logicalTableIndex)->maybe_commit(hitAddr);
          }

          // Now shift hitIndex to find TIND address
          int matchAddrShift = tcam_match_addr_shift();
          uint32_t matchAddr = tcam->make_match_address(hitAddr, matchAddrShift);
          RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTcamLookup),
                  "MauLogicalTcam::lookup_ternary_match<%d,T=%d>=%d "
                  "(Shift=%d hitAddr=%d[0x%08x] matchAddr=%d[0x%08x])\n",
                  ltcam_index_, logicalTableIndex, hitIndex,
                  matchAddrShift, hitAddr, hitAddr, matchAddr, matchAddr);

          // Push matchAddr onto appropriate rows/buses
          uint16_t tind_buses = get_tind_buses();
          for (int b = 0; b < kTindOutputBusesPerMau; b++) {
            if ((tind_buses & (1<<b)) != 0) {
              MauSramRow *row = mauobj->sram_row_lookup(b >> 1);
              if (row != NULL) row->set_tcam_match_addr(b & 0x1, matchAddr);
            }
          }



          if (find_tind_sram(matchAddr, logicalTableIndex, result, evalAll)) {
            mauobj->mau_info_incr(MAU_LTCAM_TIND_HITS);
            result->set_ternaryindirect(true);
          } else {
            mauobj->mau_info_incr(MAU_LTCAM_NO_TIND_HITS);
            result->set_ternaryindirect(false); // No TIND
            // No TIND - fill in result info using physbus associated with LTCAM
            if (tind_buses == 0) {
              RMT_LOG(RmtDebug::info(RmtDebug::kRmtDebugMauLogicalTcamLookup),
                      "MauLogicalTcam::lookup_ternary_match<%d,T=%d> No TIND and no PHYS_BUS\n",
                      ltcam_index_, logicalTableIndex);
            }
          }
          // Maintain LTCAM specific result
          uint8_t payload = RmtObject::is_chip1() ?0 :tcam->get_payload_pri(hitIndex) & 1;
          result->set_ternaryhitindex(matchAddr);
          result->set_ternarypayload(payload);
          result->set_ternarymatch(true);
          // Maintain generic results - may get overwritten by gateways etc
          result->set_payload(result->payload() | result->ternarypayload()); // Should OR
          result->set_hitindex(matchAddr);
          result->set_hitentry(0);
          result->set_match(true);

          // Maybe also squirrel away result in PhvPipeData (if configured to do so)
          if (PhvDataCtrl::do_store(match_phv->get_pipe_data_ctrl(mau_index(), PhvData::kTcamMatchAddr))) {
            match_phv->set_pipe_data_tcam_match_addr(mau_index(), ltcam_index_,
                                                     matchAddr, 1, result->ternarypayload());
          }
          break;
        }
      }
      ++it;
    }

    if (!result->ternarymatch()) {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTcamLookup),
              "MauLogicalTcam::lookup_ternary_match<%d,T=%d> TCAMs examined=%d lookedup=%d NO HITS FOUND\n",
            ltcam_index_, logicalTableIndex, tcams_examined, tcams_lookedup);
      misses_++;
    }
    else hits_++;

    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTcamLookup),
            "MauLogicalTcam::lookup_ternary_match<%d,T=%d> HitMiss=%s (tot_hits=%d tot_misses=%d)\n",
            ltcam_index_, logicalTableIndex, (result->ternarymatch()) ?"HIT" :"MISS", hits_, misses_);

    if (logicalTableIndex >= 0) {
      // Update cache with results of lookup in case we lookup this PHV again
      // but only if we were called with a valid LT
      //
      // NB. We also cache NEGATIVE results for a PHV. This is to ensure we give
      // back a consistent answer in the case where multiple logical tables use
      // the same logical TCAM, as the hardware would not perform separate lookups.
      //
      lookup_cache_ingress_ = ingress;
      lookup_cache_id_.SetFrom(match_phv->cache_id());
      cached_result_.copy(*result);
    }
    return result->ternarymatch();
  }

  int MauLogicalTcam::lookup_wide_match(Phv *match_phv, MauTcam *tcam0,
                                        MauLookupResult *result,
                                        bool ingress, bool evalAll) {
    Mau *mauobj = mau();
    RMT_ASSERT (mauobj != NULL);

    BitVector<kTcamEntries> hits(UINT64_C(0));
    int start, entries;
    bool bitmap;
    // Setup initial hits BV to reflect portion of tcam0 used by this LTCAM
    // This is a new WIP feature allowing a TCAM (or TCAM chain) to
    // be used by upto 8 LTCAMs - also allows bitmap mode lookup
    (void)tcam0->get_ltcam_result_info(ltcam_index(), &start, &entries, &bitmap);
    hits.set_ones(start, entries);

    // Find bitmask describing chain containing tcam0
    uint32_t chain = tcam_find_chain(tcam0->row_index(), tcam0->col_index());
    // Get a consistent headptr value for all upcoming calls
    int head = tcam0->get_tcam_start();

    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTcamLookup),
            "MauLogicalTcam::lookup_wide_match<%d> TCAM0=(%d,%d) "
            "Head=%d Chain=0x%04x %s_Range=[%d,%d].....\n",
            ltcam_index_, tcam0->row_index(), tcam0->col_index(),
            head, chain, bitmap?"BMP":"PRI", start, start+entries-1);

    int hit_pri = lookup_chain_simple(mauobj, match_phv, tcam0, &hits);
    if (hit_pri < 0) {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTcamLookup),
              "MauLogicalTcam::lookup_wide_match<%d> TCAM0=(%d,%d) NO CHAIN HIT\n",
              ltcam_index_, tcam0->row_index(), tcam0->col_index());
      return -1;
    }

    uint32_t bitmap_addr = 0u;
    int      index = 0;
    if (bitmap) {
      // May need to call logic to do bitmap result calc on output hits
      bitmap_addr = tcam0->compute_bitmap_result(start, entries, &hits);
    } else {
      // Make hit index relative to the TCAM portion we used
      index = tcam0->get_index(hit_pri, head);
      RMT_ASSERT((index >= start) && (index < start + entries));
      index -= start;
    }
    // Log hit info
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTcamLookup),
            "MauLogicalTcam::lookup_wide_match<%d> TCAM0=(%d,%d) "
            "Head=%d Chain=0x%04x %s_Range=[%d,%d] "
            "ChainHitPri=%d Index=%d BitmapAddr=0x%04x DONE\n",
            ltcam_index_, tcam0->row_index(), tcam0->col_index(),
            head, chain, bitmap?"BMP":"PRI", start, start+entries-1,
            hit_pri, index, bitmap_addr);
    return (bitmap) ?bitmap_addr :index;
  }

  bool MauLogicalTcam::find_tind_sram(uint32_t matchAddr, int logicalTableIndex,
                                      MauLookupResult *result, bool evalAll) {
    if (!enabled() || (logicalTableIndex < 0)) return false;

    int type = MauDefs::kSramTypeTind;
    int vpn = Address::tcam_match_addr_get_vpn(matchAddr, tcam_match_addr_shift());
    int index = Address::tcam_match_addr_get_index(matchAddr);
    int hitcnt = 0;
    Mau *mauobj = mau();
    RMT_ASSERT (mauobj != NULL);

    // Take a copy to insulate ourself from async update
    BitVector<kSramsPerMau> tind_sram_bitmap_copy;
    tind_sram_bitmap_copy.copy_from(tind_srams_used_[logicalTableIndex]);

    // Iterate through SRAMs (kept in colmajor order in bitmap) driving tind outputs
    int c_nextIndex = tind_sram_bitmap_copy.get_first_bit_set(-1);
    if (c_nextIndex < 0) {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTcamFindTind),
              "MauLogicalTcam::find_tind_sram(type=%d,vpn=%d) NO TINDS EXAMINED!!\n",
              type, vpn);
    }
    while (c_nextIndex >= 0) {
      // Remap colmajor back to rowmajor
      int nextIndex = Mau::sram_array_index_remap_colmajor_to_rowmajor(c_nextIndex);
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTcamFindTind),
              "MauLogicalTcam::find_tind_sram(type=%d,vpn=%d in TIND=%d)\n",
              type, vpn, nextIndex);
      MauSram *sram = mauobj->sram_lookup(nextIndex);
      if ((sram != NULL) &&
          (sram->matches_type_vpn_table(type, vpn, -1))) {
        uint8_t which_bus, next_tab;
        hitcnt++;

        // Get TIND sram to push result onto tind_out_bus
        int which_word = Address::tcam_match_addr_get_which(matchAddr);
        sram->set_tind_output(logicalTableIndex, index, which_word, &which_bus, &next_tab);

        // Verify that nxtab bus is linked up to correct logical_table and warn if not.
        // NOT NECESSARY - verbal communication from JayP - MAU SyncUp meet - 9/3/2016
        //
        // if (mauobj->get_logical_table_for_nxtab_bus(1, sram->get_nxtab_bus()) !=
        //    logicalTableIndex) {
        //  RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTcamFindTind),
        //          "MauLogicalTcam::find_tind_sram: "
        //          "Outputting next_tab %d from TIND but nxtab_bus %d on row %d not linked "
        //          "to LT %d (linked to %d instead) so *NOT* updating next_tab result\n",
        //          next_tab, sram->get_nxtab_bus(), sram->row_index(), logicalTableIndex,
        //          mauobj->get_logical_table_for_nxtab_bus(1, sram->get_nxtab_bus()));
        //}

        // Fill in rest of result - also set_next_table as
        // Predication expects next_table_orig() to be set
        result->set_next_table(next_tab);
        result->set_row(sram->row_index());
        result->set_col(sram->col_index());
        result->set_outbus(which_bus);
        result->set_logical_tables(sram->get_logical_tables());
        RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTcamFindTind),
                "MauLogicalTcam::find_tind_sram(type=%d,vpn=%d *FOUND* in TIND=%d)\n",
                type, vpn, nextIndex);
        RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTcamFindTind),
                "MauLogicalTcam::find_tind_sram(matchAddr=%d(0x%08x),index=%d,"
               "row=%d,outbus=0x%1x)\n",
                matchAddr, matchAddr, index, result->row(), result->outbus());
        if (!evalAll) return true;
        if (hitcnt > 1) {
          RMT_LOG(RmtDebug::error(RmtDebug::kRmtDebugMauLogicalTcamFindTind),
                  "MauLogicalTcam::find_tind_sram - Multiple TIND matches detected\n");
        }

      } else if (sram != NULL) {
          RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTcamFindTind),
                "MauLogicalTcam::find_tind_sram(type=%d,vpn=%d) MISMATCH in TIND %d "
                "[found type=%d,vpn=%d,%d]\n", type, vpn, nextIndex,
                sram->get_type(), sram->get_vpn(0), sram->get_vpn(1));
      }
      c_nextIndex = tind_sram_bitmap_copy.get_first_bit_set(c_nextIndex);
    }
    return (hitcnt > 0);
  }


  uint32_t MauLogicalTcam::tcam_find_chain(int endrow, int col) {
    uint32_t my_chain = 0u;
    Mau *mauobj = mau();
    RMT_ASSERT(mauobj != NULL);

    int top = kTcamRowsPerMau-1;
    int mid_hi = kTcamRowsPerHalf;
    int mid_lo = kTcamRowsPerHalf-1;
    int bot = 0;
    MauTcam *mid_lo_tcam = mauobj->tcam_lookup(mid_lo, col);
    bool bot_chain_out = ((mid_lo_tcam != NULL) && (mid_lo_tcam->get_chain_out()));

    if (endrow <= mid_lo) {
      // Bottom half - calc chain around endrow in 0-5
      my_chain = calculate_chain(mauobj, col, bot, mid_lo, endrow);
      // If my_chain includes mid_lo(5) and bottom half chains out then splice in top_chain
      // (NB. bot_chain_out ALWAYS determines whether we include other half)
      if (((my_chain & (1<<mid_lo)) != 0) && (bot_chain_out)) {
        my_chain |= calculate_chain(mauobj, col, top, mid_hi, mid_hi);
        // TODO: TCAM 6 needn't specify CHAIN OUT - handle here?
      }
    } else {
      // Top half - calc chain around endrow in 11-6
      my_chain = calculate_chain(mauobj, col, top, mid_hi, endrow);
      // If my_chain includes mid_hi(6) and bottom half chains out then include bot_chain
      // (NB. bot_chain_out ALWAYS determines whether we include other half)
      if (((my_chain & (1<<mid_hi)) != 0) && (bot_chain_out))
        my_chain |= calculate_chain(mauobj, col, bot, mid_lo, mid_lo);
    }
    // Return zero chain if all it contains is our row
    if (my_chain == (1u<<endrow)) my_chain = 0u;
    // Sanity check
    (void)sanity_check_chain_internal(mauobj, col, endrow, my_chain);
    return my_chain;
  }
  uint32_t MauLogicalTcam::tcam_find_chain_rest(int endrow, int col) {
    return (tcam_find_chain(endrow, col) & ~(1<<endrow));
  }


  // Handle change in bitmap that maps LogicalTCAM -> physicalTCAMs
  // Attach ourself to TCAM so it can update us when its config changes
  void MauLogicalTcam::tcam_table_map_updated(int col,
                                              uint32_t new_table_map,
                                              uint32_t old_table_map) {
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTcamUpdateTableMap),
            "MauLogicalTcam::tcam_table_map_updated<%d> New=0x%08x Old=0x%08x\n",
            ltcam_index_, new_table_map, old_table_map);
    Mau *mauobj = mau();
    RMT_ASSERT(mauobj != NULL);
    for (int row = 0; row < kTcamRowsPerMau; row++) {
      uint32_t mask = 1<<row;
      if ((new_table_map & mask) != (old_table_map & mask)) {
        MauTcam *tcam = mauobj->tcam_lookup(row, col);
        if (tcam != NULL) {
          // Tick pending so we update our tcams_ vector
          // before processing next PHV
          tcam_config_changed(row, col);

          // Attach/detach ourself from TCAM as
          // appropriate. If we are attached we
          // will be notified when TCAM config changes
          if (((new_table_map & mask) == 0) &&
              ((old_table_map & mask) != 0)) {
            RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTcamDetachFromTcam),
                    "MauLogicalTcam::tcam_table_map_updated<%d> Detaching TCAM<%d,%d>\n",
                    ltcam_index_, row, col);
            tcam->detach_ltcam(ltcam_index());
          }
          if (((new_table_map & mask) != 0) &&
              ((old_table_map & mask) == 0)) {
            RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTcamAttachToTcam),
                    "MauLogicalTcam::tcam_table_map_updated<%d> Attaching TCAM<%d,%d>\n",
                    ltcam_index_, row, col);
            tcam->attach_ltcam(ltcam_index());
          }
        }
      }
    }
  }


  // This is the fundamental logical table for the LTCAM
  // derived from the tcam_to_logical_table_ixbar

  void MauLogicalTcam::update_logical_table(int new_logtab) {
    int old_logtab = get_logical_table();
    if (new_logtab != old_logtab) {
      Mau *mauobj = mau();
      RMT_ASSERT(mauobj != NULL);
      MauLogicalTable *old_table = NULL;
      MauLogicalTable *new_table = NULL;
      if ((old_logtab >= 0) && (old_logtab < kLogicalTablesPerMau)) {
        old_table = mauobj->logical_table_lookup(old_logtab);
      }
      if ((new_logtab >= 0) && (new_logtab < kLogicalTablesPerMau)) {
        new_table = mauobj->logical_table_lookup(new_logtab);
      }
      if (old_table != NULL) {
        old_table->remove_logical_tcam(ltcam_index());
      }
      if (new_table != NULL) {
        new_table->add_logical_tcam(ltcam_index());
        ingress_ = new_table->is_ingress();
        pending_seq_++; // Schedule TCAM recheck
      }
      logical_table_ = new_logtab;
    }
  }


  // All this stuff simply to build association between
  // logical tables and TIND SRAMs
  //
  // 060315:
  // Actually as it stands just builds association between
  // Logical TCAMs and TIND SRAMs. The association to Logical Tables
  // is purely determined by the tcam_hit_to_logical_table_ixbar
  // (changing that causes the func above update_logical_table()
  // to be called).
  // Are there scenarios where TINDs drive physical buses mapped
  // to tables but the tcam_hit_to_logical_table_ixbar is NOT set??
  // Doesn't seem to have occurred so far! If possible would need
  // to add calls to update_logical_table() into the funcs below.

  void MauLogicalTcam::remove_tind_logical_table(int sram, int logtab) {
    Mau *mauobj = mau();
    RMT_ASSERT(mauobj != NULL);
    if ((sram >= 0) && (sram < kSramsPerMau) &&
        (logtab >= 0) && (logtab < kLogicalTablesPerMau)) {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTcamUpdateDelTind),
              "MauLogicalTcam::remove_tind_logical_table<%d>: Clearing TIND=%d from LogTable=%d\n",
              ltcam_index(), sram, logtab);
      int c_sram = Mau::sram_array_index_remap_rowmajor_to_colmajor(sram);
      tind_srams_used_[logtab].clear_bit(c_sram);
    }
  }
  void MauLogicalTcam::add_tind_logical_table(int sram, int logtab) {
    Mau *mauobj = mau();
    RMT_ASSERT(mauobj != NULL);
    if ((sram >= 0) && (sram < kSramsPerMau) &&
        (logtab >= 0) && (logtab < kLogicalTablesPerMau)) {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTcamUpdateAddTind),
              "MauLogicalTcam::add_tind_logical_table<%d>: Setting TIND=%d in LogTable=%d\n",
              ltcam_index(), sram, logtab);
      // Remap to colmajor order as we want to iterate across colN before colN+1
      // (because colN has higher priority)
      int c_sram = Mau::sram_array_index_remap_rowmajor_to_colmajor(sram);
      tind_srams_used_[logtab].set_bit(c_sram);
    }
  }
  void MauLogicalTcam::update_tind_logical_tables(int sram,
                                                  uint16_t new_logtabs,
                                                  uint16_t old_logtabs) {
    for (int i = 0; i < kLogicalTablesPerMau; i++) {
      bool in_new = (((new_logtabs >> i) & 1) == 1);
      bool in_old = (((old_logtabs >> i) & 1) == 1);
      if      ( in_new && !in_old) {    add_tind_logical_table(sram, i); }
      else if (!in_new &&  in_old) { remove_tind_logical_table(sram, i); }
    }
  }
  void MauLogicalTcam::update_tind_sram(MauSram *sram,
                                        uint16_t new_logtabs, uint16_t old_logtabs) {
    RMT_ASSERT(sram != NULL);
    update_tind_logical_tables(sram->sram_index(), new_logtabs, old_logtabs);
  }
  void MauLogicalTcam::add_tind_sram(MauSram *sram, uint16_t new_logtabs) {
    update_tind_sram(sram, new_logtabs, 0);
  }
  void MauLogicalTcam::remove_tind_sram(MauSram *sram, uint16_t old_logtabs) {
    update_tind_sram(sram, 0, old_logtabs);
  }


  // Check gresses of TCAMs match gress of LT
  // Check mode/size of mini-TCAMs used by this LTCAM are consistent
  bool MauLogicalTcam::tcam_check_gress_mode(int lt) {
    // Check the gress of all TCAMs vs LT gress
    bool ok = true;
    Mau *mauobj = mau();
    RMT_ASSERT(mauobj != NULL);

    MauTcam *tcam0 = nullptr;
    int start0, entries0;
    bool bitmap0 = false, multi0 = false;

    MauDependencies *deps = mauobj->mau_dependencies();
    uint16_t lts_ing = deps->lt_ingress();
    uint16_t lts_egr = deps->lt_egress();
    uint16_t lts_ght = deps->lt_ghost();
    // MauDeps reports ghost LTs as ingress LTs also
    lts_ing &= ~lts_ght;
    bool ing0 = (((lts_ing >> lt) & 1) == 1);
    bool egr0 = (((lts_egr >> lt) & 1) == 1);
    bool ght0 = (((lts_ght >> lt) & 1) == 1);

    auto it = tcams_.begin();
    while (it != tcams_.end()) {
      MauTcam *tcam = *it;
      RMT_ASSERT (tcam != NULL);
      if ((tcam->is_ingress() == ing0) && (tcam->is_ghost() == ght0) &&
          (tcam->is_egress() == egr0)) {
        // TCAM ingress/ghost/egress flags match LT flags - OK
      } else {
        const char *iegstr[8] = { "undefined!", "ingress", "egress",
                                  "ING+EGR!", "ghost",
                                  "ING+GHT!", "EGR+GHT!", "ING+EGR+GHT!" };
        int lt_ieg = (ing0?1:0) | (egr0?2:0) | (ght0?4:0);
        int tcam_ieg = (tcam->is_ingress()?1:0) | (tcam->is_egress()?2:0) |
            (tcam->is_ghost()?4:0);
        RMT_LOG(RmtDebug::error(),
                "TCAMs associated with LT %d (LTCAM=%d) must ALL be %s. "
                "TCAM(%d,%d) is %s!\n", lt, ltcam_index(), iegstr[lt_ieg],
                tcam->row_index(), tcam->col_index(), iegstr[tcam_ieg]);
        ok = false;
      }
      if (tcam0 == nullptr) {
        tcam0 = tcam; // Get start_pos, n_entries, bitmap_mode for this LTCAM in tcam0
        multi0 = tcam0->get_ltcam_result_info(ltcam_index(), &start0, &entries0, &bitmap0);
      }
      if (tcam != tcam0) {
        int start, entries;
        bool bitmap, multi; // Get start_pos, n_entries, bitmap_mode for this LTCAM in tcam
        multi = tcam->get_ltcam_result_info(ltcam_index(), &start, &entries, &bitmap);
        if ((bitmap != bitmap0) || (entries != entries0) || (multi != multi0)) {
          const char *outstr[2] = { "and does NOT output to LTCAM!", "and outputs to LTCAM" };
          int ltc = ltcam_index();
          RMT_LOG(RmtDebug::error(kRelaxTcamCheck),
                  "mini-TCAMs associated with LTCAM %d (LT=%d) have inconsistent config. "
                  "TCAM(%d,%d) has mode %s and size %d %s "
                  "but TCAM(%d,%d) has mode %s and size %d %s\n", ltc, lt,
                  tcam0->row_index(), tcam0->col_index(),
                  bitmap0?"BMP":"PRI", entries0, (multi0 == multi)?"":outstr[multi0?1:0],
                  tcam->row_index(), tcam->col_index(),
                  bitmap?"BMP":"PRI", entries, (multi0 == multi)?"":outstr[multi?1:0]);
        }
      }
      ++it;
    }
    return ok;
  }


  // Config of some attached TCAM has changed
  void MauLogicalTcam::tcam_config_changed(int row, int col) {
    // Some TCAM has had a config change
    // Could be a change in priority
    pending_seq_++;
  }
  void MauLogicalTcam::update_tcam_config_internal() {
    int n_logtab_mismatches = 0;
    int curr_tcam_logtab = -1;
    Mau *mauobj = mau();
    RMT_ASSERT(mauobj != NULL);

    while (curr_seq_ < pending_seq_) {
      curr_seq_ = pending_seq_;
      // Find all TCAMs belonging to us adding to tcams_ vector
      // This vector spans *all* columns.
      tcams_.clear();
      for (int col = kTcamColumnsPerMau-1; col >= 0; col--) {
        uint32_t tcam_map = table_map(col);
        uint32_t chain = 0u;
        for (int row = kTcamRowsPerMau-1; row >= 0; row--) {
          if ((tcam_map & (1<<row)) != 0) {
            // Only examine TCAMs that appear in our table_map
            MauTcam *tcam = mauobj->tcam_lookup(row, col);
            if (tcam != NULL) {
              // Check we have a consensus wrt logical_table
              // (Could be just one of these informative fields)
              int per_tcam_logtab = tcam->get_logical_table();
              if (per_tcam_logtab >= 0) {
                if (curr_tcam_logtab < 0) {
                  curr_tcam_logtab = per_tcam_logtab;
                } else if (curr_tcam_logtab != per_tcam_logtab) {
                  RMT_LOG(RmtDebug::warn(),
                          "MauLogicalTcam::update_tcam_config "
                          "Logical table mismatch in LTCAM::upd_tcam_cnf\n");
                  n_logtab_mismatches++;
                }
              }

              if (tcam->get_match_output_enable()) {
                // If TCAM in table_map and it has output
                // enabled then add it to our tcams_ vec
                tcams_.push_back(tcam);
                // See if this TCAM is part of a chain
                // by calling func that returns chain mask
                // (but which excludes this TCAM).
                chain = tcam_find_chain_rest(row, col);
                RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTcamUpdate),
                        "MauLogicalTcam::update_tcam_config "
                        "LTCAM=%d Adding TCAM(%d,%d) chain=0x%04x\n",
                        ltcam_index(), row, col, chain);

                tcam->set_wide_match((chain != 0u));
                // Complain if any other TCAMs in chain
                // also appear in tcam_map
                if ((tcam_map & chain) != 0) {
                  RMT_LOG(RmtDebug::warn(),
                          "MauLogicalTcam::update_tcam_config "
                          "Only a single TCAM in chain should appear in tcam_table_map "
                          "[Row=%d Map=0x%04x Chain=0x%04x]\n", row, tcam_map, chain);
                }
              } else {
                RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTcamUpdate),
                        "MauLogicalTcam::update_tcam_config "
                        "LTCAM=%d NOT adding TCAM(%d,%d) Chain=0x%04x - "
                        "match_output_enable NOT set\n",
                        ltcam_index(), row, col, chain);
              }
            }
          }
        }
      }
    }

    if (n_logtab_mismatches == 0) {
      // All TCAMs agree on logical_table
      // Check it matches ours!
      if ((logical_table_ >= 0) && (curr_tcam_logtab >= 0) &&
          (logical_table_ != curr_tcam_logtab)) {
        RMT_LOG(RmtDebug::warn(),
                "MauLogicalTcam: Logical table mismatch in LTCAM: LTCAM/TCAM disagree\n");
      }
    }
    // Then sort the tcams_ vector into priority order
    std::sort(tcams_.begin(), tcams_.end(), tcam_sort_func);

    // Check all gresses match up
    if (logical_table_ >= 0) (void)tcam_check_gress_mode(logical_table_);

    // And maybe dump out our sorted list
    if (rmt_log_check(RmtDebug::kRmtDebugMauLogicalTcamUpdate)) {
      int cnt = 0;
      auto it = tcams_.begin();
      while (it != tcams_.end()) {
        MauTcam *tcam = *it;
        RMT_ASSERT (tcam != NULL);
        RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTcamUpdate),
                "MauLogicalTcam::update_tcam_config<%d> %d: TCAM(%d,%d) vpn=%d[0x%04x] W=%d\n",
                ltcam_index_, cnt, tcam->row_index(), tcam->col_index(),
                tcam->get_vpn(), tcam->get_vpn(), tcam->wide_match()? 1:0);
        ++cnt;
        ++it;
      }
    }
  }
  void MauLogicalTcam::reset_resources() {
    // reset_resources is called at very start of PHV lookup.
    // Should try and keep mappings consistent during entirety
    // of PHV lookup so only update tcam_configs in between PHVs
    if (curr_seq_ < pending_seq_) update_tcam_config_internal();
    has_run_ = false;
  }


  // Return paired MauLogicalTcam if there is one
  MauLogicalTcam *MauLogicalTcam::paired_ltcam_obj(bool ingress) {
    Mau *mauobj = mau();
    int pair = paired_ltcam(ingress);
    if ((mauobj == NULL) || (pair < 0)) return NULL;
    return mauobj->logical_tcam_get(pair);
  }

  // Find logical table possibly looking in paired MauLogicalTcam
  int MauLogicalTcam::find_logical_table() {
    int lt = get_logical_table();
    if (lt >= 0) return lt;
    // See if we have a paired LTCAM - it may have a LT
    MauLogicalTcam *pair = paired_ltcam_obj(ingress_);
    return (pair != NULL) ?pair->get_logical_table() :-1;
  }



  // More internal funcs

  uint32_t MauLogicalTcam::calculate_chain(Mau *mauobj, int col,
                                           int start_row, int end_row, int my_row) {
    RMT_ASSERT(mauobj != NULL);

    int delta;
    if (start_row <= end_row) {
      RMT_ASSERT((start_row <= my_row) && (my_row <= end_row));
      delta = 1;
    } else {
      RMT_ASSERT((start_row >= my_row) && (my_row >= end_row));
      delta = -1;
    }

    MauTcam *tcam = NULL;
    uint32_t chain = 0u;
    bool seen_my_row = false;
    int row = start_row;

    while (true) {
      if (row == my_row) seen_my_row = true;

      tcam = mauobj->tcam_lookup(row, col);
      if ((tcam != NULL) && (tcam->get_chain_out())) {
        // Accumulate chain
        chain |= 1<<row;
      } else if (seen_my_row) {
        // Past my row and no more chain - done - break
        break;
      } else {
        // Not yet seen my row - reset chain - continue
        chain = 0u;
      }
      // Got to end row - break
      if (row == end_row) break;
      row += delta;
    }

    chain |= 1<<my_row; // Chain always contains my_row
    //printf("CHAINCALC: col=%d start=%d end=%d me=%d chain=0x%04x\n",
    //       col, start_row, end_row, my_row, chain);
    return chain;
  }


  // Naive, but hopefully correct wide TCAM lookup
  int MauLogicalTcam::lookup_chain_simple(Mau *mauobj, Phv *match_phv, int col,
                                          int start_row, int end_row,
                                          uint32_t chain, int head,
                                          BitVector<kTcamEntries> *hits) {
    int delta = (start_row <= end_row) ?1 :-1;
    BitVector<kTcamEntries> hits_in(UINT64_C(0));
    BitVector<kTcamEntries> hits_out(UINT64_C(0));
    bool first_time = true;

    int row = start_row;
    while (true) {
      if ((chain & (1<<row)) != 0u) {
        MauTcam *tcam = mauobj->tcam_lookup(row, col);
        RMT_ASSERT(tcam != NULL); // All TCAMs of chain must exist

        if (first_time) hits_in.copy_from(*hits);
        hits_out.fill_all_zeros();
        // Find all hits
        int n_hits = tcam->lookup(match_phv, hits_in, &hits_out, head);
        hits_in.copy_from(hits_out);
        if (n_hits == 0) break;
        first_time = false;

        hits_out.fill_all_zeros();
        // Expand hits using MRD
        tcam->find_range(hits_in, &hits_out, head);
        hits_in.copy_from(hits_out);
      }
      if (row == end_row) break;
      row += delta;
    }
    hits->copy_from(hits_in);
    return hits_in.get_last_bit_set();
  }
  // Calls lookup_chain_simple twice (top & bottom) and splices results together
  int MauLogicalTcam::lookup_chain_simple(Mau *mauobj, Phv *match_phv, int col,
                                          uint32_t chain, int head,
                                          BitVector<kTcamEntries> *hits) {

    uint32_t tcam_bot_mask = (1 << kTcamRowsPerHalf) - 1;
    uint32_t tcam_top_mask = tcam_bot_mask << kTcamRowsPerHalf;
    uint32_t chain_bot = chain & tcam_bot_mask;
    uint32_t chain_top = chain & tcam_top_mask;
    if ((chain_bot == 0u) && (chain_top == 0u)) return -1;

    int top = kTcamRowsPerMau-1;
    int mid_hi = kTcamRowsPerHalf;
    int mid_lo = kTcamRowsPerHalf-1;
    int bot = 0;
    int hit_pri_bot = -1;
    int hit_pri_top = -1;
    BitVector<kTcamEntries> bot_hits(UINT64_C(0));
    BitVector<kTcamEntries> top_hits(UINT64_C(0));

    if (chain_bot != 0u) {
      bot_hits.copy_from(*hits);
      hit_pri_bot = lookup_chain_simple(mauobj, match_phv, col, bot, mid_lo,
                                        chain_bot, head, &bot_hits);
      if (chain_top == 0u) {
        hits->copy_from(bot_hits);
        return hit_pri_bot; // DONE - no chain in top half
      }
    }
    if (chain_top != 0u) {
      top_hits.copy_from(*hits);
      hit_pri_top = lookup_chain_simple(mauobj, match_phv, col, top, mid_hi,
                                        chain_top, head, &top_hits);
      if (chain_bot == 0u) {
        hits->copy_from(top_hits);
        return hit_pri_top; // DONE - no chain in bot half
      }
    }
    // Getting here means the chain spreads across central (TCAM5/6) boundary - check
    RMT_ASSERT(((chain & (1<<kTcamRowsPerHalf)) != 0) && ((chain & (1<<(kTcamRowsPerHalf-1))) != 0));

#ifdef WIDE_TCAM_DEBUG
    printf("BotHits=");
    for (int i=511; i>=0; i--) {
      if (bot_hits.bit_set(i)) printf("%d ", i);
    }
    printf("\n");
    printf("TopHits=");
    for (int i=511; i>=0; i--) {
      if (top_hits.bit_set(i)) printf("%d ", i);
    }
    printf("\n");
#endif
    // AND top_hits and bot_hits together
    top_hits.mask(bot_hits);
    hits->copy_from(top_hits);

    int eff_pri = top_hits.get_last_bit_set();

    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTcamLookup),
            "MauLogicalTcam::lookup_chain_simple<%d> Chain across TCAM central boundary "
            "HitPriTop=%d  HitPriBot=%d EffectiveHitPri=%d\n", ltcam_index_,
            hit_pri_top, hit_pri_bot, eff_pri);
    // Return eff_pri - could still be -1 if no overlap in hit ranges
    return eff_pri;
  }
  // Shim func that allows us to just pass TCAM obj - head/chain etc all inferred
  int MauLogicalTcam::lookup_chain_simple(Mau *mauobj, Phv *match_phv, MauTcam *tcam,
                                          BitVector<kTcamEntries> *hits) {
    RMT_ASSERT(hits != nullptr);
    int head = tcam->get_tcam_start(), col = tcam->col_index(), row = tcam->row_index();
    uint32_t chain = tcam_find_chain(row, col);
    if (chain == 0u) {
      chain = (1u<<row); // Solo TCAM, no chain - add self to make a chain of 1
      return lookup_chain_simple(mauobj, match_phv, col, row, row, chain, head, hits);
    } else {
      (void)sanity_check_chain_internal(mauobj, col, row, chain);
      return lookup_chain_simple(mauobj, match_phv, col, chain, head, hits);
    }
  }



  bool MauLogicalTcam::sanity_check_chain_internal(Mau *mauobj, int col,
                                                   int start_row, uint32_t chain) {
    uint32_t oblig_match_out_enable_rows = 0u;
    if (GLOBAL_FALSE) {
      // XXX: keep Klocwork happy by pretending this code might be
      // executed, when in fact it isn't.
      // From MAU uArchs Figure 'Wide TCAM Constraints'
      uint32_t rows_5_and_6 = (1<<5) | (1<<6);
      oblig_match_out_enable_rows = (1<<2) | (1<<9);
      if ((chain & rows_5_and_6) == rows_5_and_6)
        oblig_match_out_enable_rows |= rows_5_and_6;
    }
    // Find first tcam in chain that has match_out_enable set
    // We do all subsequent checks against this TCAM
    MauTcam *tcam0 = NULL;
    for (int row = 0; row < kTcamRowsPerMau; row++) {
      if ((chain & (1<<row)) != 0) {
        MauTcam *tcam = mauobj->tcam_lookup(row, col);
        if ((tcam != NULL) && (tcam->get_match_output_enable())) {
          tcam0 = tcam;
          break;
        }
      }
    }
    if (tcam0 == NULL) return false;

    int head0 = tcam0->get_tcam_start();
    uint8_t vpn0 = tcam0->get_vpn();
    bool ing0 = tcam0->is_ingress();
    bool ght0 = tcam0->is_ghost();
    bool egr0 = tcam0->is_egress();
    int  start0, entries0;
    bool bmp0;
    bool phys0 = tcam0->get_ltcam_result_info(ltcam_index(), &start0, &entries0, &bmp0);
    bool ret = true;

    for (int row = 0; row < kTcamRowsPerMau; row++) {
      int start, entries;
      bool bmp;

      if ((chain & (1<<row)) != 0) {
        MauTcam *tcam = mauobj->tcam_lookup(row, col);
        if (tcam == NULL) return false;
        if (tcam->get_tcam_start() != head0) {
          RMT_LOG(RmtDebug::warn(),
                  "TCAMs in chain must all use same HEAD_PTR  "
                  "TcamZero=(%d,%d) has HEAD=%d  TcamThis=(%d,%d) has HEAD=%d\n",
                  start_row, col, head0, row, col, tcam->get_tcam_start());
          ret = false;
        }
        if (tcam->get_vpn() != vpn0) {
          RMT_LOG(RmtDebug::warn(),
                  "TCAMs in chain must all use same VPN  "
                  "TcamZero=(%d,%d) has VPN=%d  TcamThis=(%d,%d) has VPN=%d\n",
                  start_row, col, vpn0, row, col, tcam->get_vpn());
          ret = false;
        }
        if ((tcam->is_ingress() == ing0) &&
            (tcam->is_ghost() == ght0) &&
            (tcam->is_egress() == egr0)) {
          // ingress/ghost/egress flag matches - ok
        } else {
          RMT_LOG(RmtDebug::warn(),
                  "TCAMs in chain must all be for same gress  "
                  "TcamZero=(%d,%d) has ING/GHT/EGR=%d/%d/%d  "
                  "TcamThis=(%d,%d) has ING/GHT/EGR=%d/%d/%d\n",
                  start_row, col, ing0, ght0, egr0, row, col,
                  tcam->is_ingress(), tcam->is_ghost(), tcam->is_egress());
          ret = false;
        }
        if (((oblig_match_out_enable_rows & (1<<row)) != 0) &&
            (!tcam->get_match_output_enable())) {
          RMT_LOG(RmtDebug::warn(),
                  "TCAM %d in chain 0x%04x must set tcam_match_out_enable\n",
                  row, chain);
          ret = false;
        }
        bool phys = tcam0->get_ltcam_result_info(ltcam_index(), &start, &entries, &bmp);
        if ((phys == phys0) && (start == start0) && (entries == entries0) && (bmp == bmp0)) {
          // Phys result config matches for this particular LTCAM - ok
        } else {
          RMT_LOG(RmtDebug::warn(),
                  "TCAMs in chain must all use same PRI/BMP config for LTCAM %d "
                  "TcamZero=(%d,%d) has Start/Entries/Bmp=%d/%d/%c "
                  "TcamThis=(%d,%d) has Start/Entries/Bmp=%d/%d/%c\n",
                  ltcam_index(), 
                  start_row, col, start0,entries0,bmp0?'T':'F', 
                  row, col, start,entries,bmp?'T':'F');
          ret = false;
        }
      }
    }
    return ret;
  }


}
