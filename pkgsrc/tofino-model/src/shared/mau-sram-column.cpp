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
#include <mau-sram-column.h>
#include <string>
#include <rmt-log.h>
#include <rmt-object-manager.h>
#include <mau-logical-table.h>


namespace MODEL_CHIP_NAMESPACE {

  MauSramColumn::MauSramColumn(RmtObjectManager *om,
                               int pipeIndex, int mauIndex, int columnIndex, Mau *mau)
      : MauObject(om, pipeIndex, mauIndex, kType, 0x3F, columnIndex, mau),
        column_index_(columnIndex), srams_(),
        curr_map_seq_(0), pending_map_seq_(1),
        lookup_cache_id_(), cached_result_(),
        sram_rows_used_(), reverse_hitmap_(),
        mau_sram_column_reg_(om, pipeIndex, mauIndex, columnIndex, this) {

    static_assert( MauDefs::kSramRowsPerMau <= 16, "Row bitset must fit in uint16_t" );
    static_assert( kHitmapInputs <= 32, "Input bitset must fit in uint32_t" );
    // kHitmapOutputs must fit in uint8_t, and be < 255 (value 0xFF reserved)
    static_assert( kHitmapOutputs < 255, "Output must fit in uint8_t" );

    for (int t = 0; t < MauDefs::kLogicalTablesPerMau; t++)
      evaluate_all_[t] = kEvaluateAllDefault;
    for (int h = 0; h < kHitmapInputs; h++)
      hitmap_xbar_[h] = 0xFF;
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugCreate),
            "MAU_SRAM_COLUMN::create\n");
  }
  MauSramColumn::~MauSramColumn() {
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugDelete),
            "MAU_SRAM_COLUMN::delete\n");
    for (int i = 0; i < kSramRows; i++) srams_[i] = NULL;
  }


  // Funcs to bind SRAMs within a column to a logical table

  void MauSramColumn::remove_logical_table(int row, int logtab) {
    Mau *mauobj = mau();
    RMT_ASSERT (mauobj != NULL);
    if ((logtab >= 0) && (logtab < kLogicalTablesPerMau)) {
      uint16_t old_rows_used = sram_rows_used_[logtab];
      sram_rows_used_[logtab] &= ~(1<<row);
      if ((sram_rows_used_[logtab] == 0) && (old_rows_used != 0)) {
        MauLogicalTable *table = mauobj->logical_table_lookup(logtab);
        if (table != NULL) table->remove_sram_column(column_index());
      }
    }
  }
  void MauSramColumn::add_logical_table(int row, int logtab) {
    Mau *mauobj = mau();
    RMT_ASSERT (mauobj != NULL);
    if ((logtab >= 0) && (logtab < kLogicalTablesPerMau)) {
      uint16_t old_rows_used = sram_rows_used_[logtab];
      sram_rows_used_[logtab] |= (1<<row);
      if (old_rows_used == 0) {
        MauLogicalTable *table = mauobj->logical_table_lookup(logtab);
        if (table != NULL) table->add_sram_column(column_index());
      }
    }
  }
  void MauSramColumn::update_logical_tables(int row,
                                            uint16_t new_logtabs, uint16_t old_logtabs) {
    for (int i = 0; i < kLogicalTablesPerMau; i++) {
      bool in_new = (((new_logtabs >> i) & 1) == 1);
      bool in_old = (((old_logtabs >> i) & 1) == 1);
      if      ( in_new && !in_old) {    add_logical_table(row, i); }
      else if (!in_new &&  in_old) { remove_logical_table(row, i); }
    }
  }

  // These may be invoked by CALLBACK from mau_sram code in response
  // to a call to sram->update_logical_table from the func lower down.
  // They may also be called in response to an SRAM config register
  // changing, which could happen *during* PHV processing

  void MauSramColumn::update_match_sram(MauSram *sram,
                                        uint16_t new_logtabs, uint16_t old_logtabs) {
    RMT_ASSERT(sram != NULL);
    RMT_ASSERT(sram->col_index() == column_index());
    update_logical_tables(sram->row_index(), new_logtabs, old_logtabs);
  }
  void MauSramColumn::add_match_sram(MauSram *sram, uint16_t new_logtabs) {
    update_match_sram(sram, new_logtabs, 0);
  }
  void MauSramColumn::remove_match_sram(MauSram *sram, uint16_t old_logtabs) {
    update_match_sram(sram, 0, old_logtabs);
  }

  int MauSramColumn::get_logical_table_for_row(uint8_t row, int result_buses, int which) {
    return mau_sram_column_reg_.get_logical_table_for_row(row, result_buses, which);
  }
  uint16_t MauSramColumn::get_all_logical_tables_for_row(uint8_t row, int result_buses, int which) {
    return mau_sram_column_reg_.get_all_logical_tables_for_row(row, result_buses, which);
  }
  uint16_t MauSramColumn::get_all_logical_tables_for_row_nxtab(uint8_t row, int result_buses, int which) {
    return get_all_logical_tables_for_row(row, result_buses, which+2);
  }
  uint16_t MauSramColumn::get_all_xm_logical_tables_for_row(uint8_t row) {
    return get_all_logical_tables_for_row(row, 0x3, 0);
  }
  uint16_t MauSramColumn::get_all_tm_logical_tables_for_row(uint8_t row) {
    return get_all_logical_tables_for_row(row, 0x3, 1);
  }

  void MauSramColumn::update_sram_logical_table_mappings() {
    for (uint8_t row = 0; row < kSramRows; row++) {
      MauSram *sram = srams_[row];
      if (sram != NULL) {
        int old_logtab = sram->get_logical_table();
        uint32_t old_logtabs = sram->get_logical_tables();
        // Select whether we lookup ExactMatch (0) or TernaryMatch (1) mapping
        int xm_tm = -1;
        int result_buses = 0;
        if (sram->is_match_sram()) {
          xm_tm = 0;
          result_buses = sram->get_match_result_buses();
        } else if (sram->is_tind_sram()) {
          xm_tm = 1;
          result_buses = sram->get_tind_result_buses();
        }
        if ((xm_tm >= 0) && (result_buses != 0)) {
          int new_logtab = get_logical_table_for_row(row, result_buses, xm_tm);
          uint16_t new_logtabs = get_all_logical_tables_for_row(row, result_buses, xm_tm);
          if ((new_logtab != old_logtab) || (new_logtabs != old_logtabs)) {
            // This call may result in a CALLBACK to one of the functions above
            sram->update_logical_tables(new_logtabs, old_logtabs);
          }
        }
      }
    }
  }

  void MauSramColumn::update_hitmap() {
    for (int in = 0; in < kHitmapInputs; in++) {
      uint8_t old_out = hitmap_xbar_[in];
      uint8_t new_out = mau_sram_column_reg_.get_hitmap_output_map(in);
      if (new_out >= kHitmapOutputs) new_out = 0xFF;
      if (new_out != old_out) {
        hitmap_xbar_[in] = new_out;
        if (old_out < kHitmapOutputs)
          reverse_hitmap_[old_out] &= ~(1<<in);
        if (new_out < kHitmapOutputs)
          reverse_hitmap_[new_out] |= (1<<in);
      }
    }
    for (int out = 0; out < kHitmapOutputs; out++) {
      if (reverse_hitmap_[out] != 0) {
        RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauSramColumnUpdateHitmap),
                "MauSramColumn::update_hitmap: col=%d rev_hitmap[%d]=0x%04x\n",
                column_index(), out, reverse_hitmap_[out]);
      }
    }
  }


  void MauSramColumn::handle_xtra_hits(int hit_row, int hit_index, int which_hit,
                                       MauLookupResult *result) {
    // This func only deals with *extra* hits
    // Normal/Wide first hits counted in MauLogicalTable::lookup_exact_match
    if (!result->exactmatch()) return;

    mau()->mau_info_incr(MAU_XM_XTRA_HITS);
    if (hit_row == result->row()) {
      // Another hit on same row - could be fine if doing AlgoTCAM say
      if (column_index() == result->col()) {
        const char *iostr[] = { "inner", "(out/in)", "(in/out)", "outer" };
        int iosel = ((result->hitentry() < 2) ?0x1 :0x0) | ((which_hit < 2) ?0x2 :0x0);
        mau()->mau_info_incr(MAU_XM_MULTIRAM_HITS);
        RMT_LOG(RmtDebug::error(kRelaxMultiColumnHitCheck),
                "MauSramColumn::handle_xtra_hits: Multiple %s hits detected in SRAM "
                "Row=%d Col=%d (LT=%d)\n",
                iostr[iosel], result->row(), result->col(), result->logical_table());
      } else {
        mau()->mau_info_incr(MAU_XM_MULTICOL_HITS);
        RMT_LOG(RmtDebug::error(kRelaxMultiColumnHitCheck),
                "MauSramColumn::handle_xtra_hits: Multiple column hits detected "
                "Row=%d Col1=%d Col2=%d (LT=%d)\n",
                result->row(), result->col(), column_index(), result->logical_table());
      }
    } else {
      // Hits on different rows - BAD!
      mau()->mau_info_incr(MAU_XM_MULTIPLE_HITS);
      RMT_LOG(RmtDebug::error(kRelaxMultiHitCheck),
              "MauSramColumn::handle_xtra_hits: HITS in different rows! "
              "Col=%d Row1=%d Row2=%d (LT=%d)\n",
              result->col(), result->row(), hit_row, result->logical_table());
      if (!kRelaxMultiHitCheck) { THROW_ERROR(-2); }
    }
  }


  bool MauSramColumn::handle_inner_hit(int lt, int sram_row, int hit_index, int which_hit,
                                       std::array<uint32_t,kHitmapOutputs>& local_reverse_hitmap,
                                       MauLookupResult *result) {
    uint32_t tmp_next_table = 0u;
    MauSram *outSram = srams_[sram_row];
    RMT_ASSERT(outSram != NULL);

    // Output this hit - in MauSramRow it will be either:
    // 1. get put onto the relevant result bus (NORMAL path)
    // 2. be ignored if it's a lower pri than an existing hit
    // 3. cause an error if it's a higher pri than an existing hit
    //    (this indicates a bug as cols should always be executed
    //    in descending priority order)
    // 4. cause an error if this col is in a different priority
    //    group to the existing hit
    outSram->set_match_output(lt, hit_index, which_hit);

    if (result->exactmatch()) {
      handle_xtra_hits(sram_row, hit_index, which_hit, result);
      // We process columns/hits in descending pri order
      // so if we've already found a match we're done
      return true;
    }

    result->set_row(sram_row);
    result->set_col(column_index());
    result->set_outbus(outSram->get_match_result_bus());
    result->set_hitindex(hit_index);
    result->set_hitentry(which_hit);
    result->set_logical_tables(outSram->get_logical_tables());
    result->set_payload(0);

    if (outSram->get_next_table(hit_index, which_hit, &tmp_next_table)) {
      result->set_next_table(tmp_next_table);
      result->set_exactmatch(true);
      return true;
    } else {
      RMT_LOG(RmtDebug::warn(),
              "MauSramColumn::handle_inner_hit Unable to get next_table!\n");
      return false;
    }
  }
  bool MauSramColumn::handle_outer_hit(int lt, int sram_row, int hit_index, int which_hit,
                                       std::array<uint32_t,kHitmapOutputs>& local_reverse_hitmap,
                                       MauLookupResult *result) {
    uint8_t input = hit_make(sram_row, which_hit);
    uint8_t output = hitmap_xbar_[input];
    if (output >= kHitmapOutputs) {
      RMT_LOG(RmtDebug::warn(),
	      "MauSramColumn::handle_outer_hit: hit on unenabled input %d ignored\n",input);
      return false;
    }
    RMT_ASSERT (output < kHitmapOutputs);

    local_reverse_hitmap[output] |= (1 << input);
    if (local_reverse_hitmap[output] == reverse_hitmap_[output]) {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauSramColumnOuterHit),
              "MauSramColumn::handle_outer_hit: col=%d rev_hitmap[%d]=0x%04x\n",
              column_index(), output, reverse_hitmap_[output]);

      uint32_t tmp_next_table = 0u;
      int rowToOutput = out_get_row(output);
      int hitToOutput = out_get_which(output);
      MauSram *outSram = srams_[rowToOutput];
      RMT_ASSERT(outSram != NULL);

      // Output this hit - in MauSramRow it will be either:
      // 1. get put onto the relevant result bus (NORMAL path)
      // 2. be ignored if it's a lower pri than an existing hit
      // 3. cause an error if it's a higher pri than an existing hit
      //    (this indicates a bug as cols should always be executed
      //    in descending priority order)
      // 4. cause an error if this col is in a different priority
      //    group to the existing hit
      outSram->set_match_output(lt, hit_index, hitToOutput);

      if (result->exactmatch()) {
        handle_xtra_hits(rowToOutput, hit_index, which_hit, result);
        // We process columns/hits in descending pri order
        // so if we've already found a match we're done
        return true;
      }

      result->set_row(rowToOutput);
      result->set_col(column_index());
      result->set_outbus(outSram->get_match_result_bus());
      result->set_hitindex(hit_index);
      result->set_hitentry(hitToOutput);
      result->set_logical_tables(outSram->get_logical_tables());
      result->set_payload(0);

      if (outSram->get_next_table(hit_index, hitToOutput, &tmp_next_table)) {
        result->set_next_table(tmp_next_table);
        result->set_exactmatch(true);
        return true;
      } else {
        RMT_LOG(RmtDebug::warn(),
                "MauSramColumn::handle_outer_hit Unable to get next_table!\n");
        return false;
      }
    } else {
      return false;
    }
  }
  bool MauSramColumn::lookup_internal(Phv *phv, int logicalTableIndex, MauLookupResult *result) {

    // Lookup logicalTableIndex to get srams_used (should only include match_srams)
    // For each sram with bit set get MauSram* using mau()
    // Double-check type of sram is still match_sram
    // Call lookup - if we get inner match we're done
    // If we get outer match maintain own copy reverse_hitmap_
    // If local reverse_hitmap == configured per-column reverse_hitmap DONE again

    // Setup a temporary reverse_hitmap to check for multi-SRAM matches
    std::array<uint32_t,kHitmapOutputs> local_reverse_hitmap;
    for (int j = 0; j < kHitmapOutputs; j++) local_reverse_hitmap[j] = 0u;
    int hitcnt = 0;

    std::array<int,kSramRows>     sram_hit_indexes;
    std::array<uint8_t,kSramRows> sram_match_masks;
    for (int rowIndex = 0; rowIndex < kSramRows; rowIndex++) {
      sram_hit_indexes[rowIndex] = -1;
      sram_match_masks[rowIndex] = 0;
    }

    // Take a copy to insulate ourself from async update
    uint16_t sram_rows_used_copy = sram_rows_used_[logicalTableIndex];

    // Find all matches in all SRAMs used
    for (int rowIndex = 0; rowIndex < kSramRows; rowIndex++) {
      if (((sram_rows_used_copy >> rowIndex) & 1) == 1) {
        MauSram *mauSram = srams_[rowIndex];
        if ((mauSram != NULL) && (mauSram->is_match_sram())) {

          uint8_t match_mask = 0;
          sram_hit_indexes[rowIndex] = mauSram->lookup(phv, &match_mask);
          sram_match_masks[rowIndex] = match_mask;

          if (sram_hit_indexes[rowIndex] >= 0) {
            RMT_ASSERT(match_mask != 0);
            RMT_LOG(RmtDebug::verbose(),
                    "MauSramColumn::lookup in row %d (rows_used=0x%02x) HitIndex=%d Matches=0x%x\n",
                    rowIndex, sram_rows_used_copy, sram_hit_indexes[rowIndex], match_mask);
          } else {
            RMT_ASSERT(match_mask == 0);
            RMT_LOG(RmtDebug::verbose(),
                    "MauSramColumn::lookup in row %d (rows_used=0x%02x) No hits\n",
                    rowIndex, sram_rows_used_copy);
          }
        }
      }
    }
    // Now go through in HIT priority looking for inner/outer hits
    for (int i = 4; i >= 2; i--) {
      for (int rowIndex = 0; rowIndex < kSramRows; rowIndex++) {
        if ((sram_hit_indexes[rowIndex] >= 0) &&
            (hit_check(sram_match_masks[rowIndex], i))) {

          if (handle_inner_hit(logicalTableIndex, rowIndex,
                               sram_hit_indexes[rowIndex], i,
                               local_reverse_hitmap, result)) {
            hitcnt++;
            if (!evaluate_all(logicalTableIndex)) return true;
          }
        }
      }
    }
    for (int j = 1; j >= 0; j--) {
      // XXX:  Look for enabled *output* pri j in hitmap_output_xbar
      // and from that figure out what rows/hits we need to examine in
      // sram_hit_indexes/sram_match_masks
      //
      for (uint8_t in = 0; in < kHitmapInputs; in++) {
        uint8_t out = mau_sram_column_reg_.get_hitmap_output_map(in);
        if ((out < kHitmapOutputs) && (out_get_which(out) == j)) {
          int inRow = hit_get_row(in);
          int inHit = hit_get_which(in);

          if ((sram_hit_indexes[inRow] >= 0) &&
              (hit_check(sram_match_masks[inRow], inHit))) {

            if (handle_outer_hit(logicalTableIndex, inRow,
                                 sram_hit_indexes[inRow], inHit,
                                 local_reverse_hitmap, result)) {
              hitcnt++;
              if (!evaluate_all(logicalTableIndex)) return true;
            }
          }
        }
      }
    }
    return (hitcnt > 0);
  }



  bool MauSramColumn::lookup(Phv *phv, int logicalTableIndex, MauLookupResult *result) {
    // Lookup PHV using the srams in this column that belong to logicalTableIndex
    RMT_ASSERT((logicalTableIndex >= 0) && (logicalTableIndex < kLogicalTablesPerMau));
    // Remember if we already had an XM match in an earlier column
    bool previous_match = result->exactmatch();

    bool hit = lookup_internal(phv, logicalTableIndex, result);
    if (hit) {
      MauSram *outSram = srams_[result->row()];
      RMT_ASSERT(outSram != NULL);
      RMT_ASSERT(result->exactmatch());
      // Figure out what LTs are hooked up to NxtTab bus output (0=>XM)
      int buses = outSram->get_nxtab_result_buses(); // Upto 2 buses
      uint16_t lts = get_all_logical_tables_for_row_nxtab(result->row(), buses, 0);
      // Verify that nxtab buses are linked up to our logical_table else suppress hit
      if (((lts >> logicalTableIndex) & 1) == 0) {
        RMT_LOG(RmtDebug::error(), "MauSramColumn::lookup: "
                "Found hit in col %d but nxtab_buses 0x%x on row %d not linked to LT %d "
                "(linked to LTs 0x%04x instead) so suppressing hit\n", column_index(),
                buses, result->row(), logicalTableIndex, lts);
        // If this column was only/first match clear exactmatch result
        if (!previous_match) result->set_exactmatch(false);
        hit = false;
      } else {
        result->set_match(true); // All OK
      }
    }
    return hit;
  }

  void MauSramColumn::reset_resources() {
    // reset_resources is called at very start of PHV lookup.
    // Should try and keep mappings consistent during entirety
    // of PHV lookup so only update maps in between PHVs
    if (curr_map_seq_ < pending_map_seq_) update_maps();
  }
  void MauSramColumn::update_maps() {
    while (curr_map_seq_ < pending_map_seq_) {
      curr_map_seq_ = pending_map_seq_;
      update_sram_logical_table_mappings();
      update_hitmap();
    }
  }
  void MauSramColumn::maps_changed() {
    pending_map_seq_++;
  }

}
