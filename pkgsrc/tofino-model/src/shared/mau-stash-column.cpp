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
#include <mau-stash-column.h>

#include <string>
#include <rmt-log.h>
#include <rmt-object-manager.h>
#include <mau-logical-table.h>


namespace MODEL_CHIP_NAMESPACE {

MauStashColumn::MauStashColumn(RmtObjectManager *om, int pipeIndex, int mauIndex, Mau *mau)
    : MauObject(om, pipeIndex, mauIndex, RmtTypes::kRmtTypeMauStashColumn, 0x3F, mau),
      mau_stash_column_reg_(om, pipeIndex, mauIndex, this) {

  RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugCreate), "MAU_STASH_COLUMN::create\n");
  // find all the stashes
  for (int i=0;i<kStashRows;++i) {
    MauSramRow* row = mau->sram_row_get(i);
    RMT_ASSERT(row);
    stashes_[i] = row->get_stash();
  }
}
MauStashColumn::~MauStashColumn() {
  RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugDelete), "MAU_STASH_COLUMN::delete\n");
}

bool MauStashColumn::lookup(Phv *phv, int logicalTableIndex, MauLookupResult *result) {
  RMT_ASSERT((logicalTableIndex >= 0) && (logicalTableIndex < kLogicalTablesPerMau));
  
  bool hit=false;
  for (int row=0;row<kStashRows;++row) {
    for (int bus=0;bus<2;++bus) {
      if ( BusIsInLogicalTable(row,bus,logicalTableIndex) ) {
        // find which stash(es) on the row drive this bus
        bool stash_drives_bus[2];
        FindWhichStashesDriveBus(row,bus,&stash_drives_bus[0],&stash_drives_bus[1]);
        
        if ( stash_drives_bus[0] || stash_drives_bus[1] ) {
          mau()->mau_info_incr(MAU_STASH_RUNS);
          result->set_stashran(true);
          
          bool stash_enabled[2];
          int destination_row[2];
          for (int w=0;w<2;++w) {
            mau_stash_column_reg_.get_hitmap_destination_row(row,w,&stash_enabled[w],&destination_row[w]);
          }
          
          // if a stash drives a bus then it should be enabled in the hitmap
          RMT_ASSERT(  (stash_drives_bus[0] && ! stash_enabled[0] ) == 0 );
          RMT_ASSERT(  (stash_drives_bus[1] && ! stash_enabled[1] ) == 0 );
          
          // if both stashes drive the same bus, I think the hitmap row must be the same
          //   (ie must not be different)
          RMT_ASSERT ( ! ((stash_drives_bus[0] && stash_drives_bus[1]) &&
                      (destination_row[0] != destination_row[1])));
          
          // which stash to use for the destination_row
          int use = stash_drives_bus[0] ? 0 : 1;
          
          // the destination row for the stash that drives the bus must
          //  be the same row as the bus is on (ie this one)
          RMT_ASSERT ( destination_row[use] == row );
          
          // now find all stash halves that have the same row (and so
          //   all contribute to this hit)
          uint8_t overall_hit[2]{};

          // for stashes that are contributing set the initial hit to all ones
          for (int w=0;w<2;++w) {
            if ( stash_drives_bus[w] )
              overall_hit[w] = ~ 0;
          }
          
          for (int r=0;r<kStashRows;++r) {
            for (int w=0;w<2;++w) {
              bool en;
              int dst_row;
              mau_stash_column_reg_.get_hitmap_destination_row(r,w,&en,&dst_row);
              if (en && dst_row == destination_row[use]) {
                uint32_t hit_mask;
                stashes_[r]->lookup(phv, w, &hit_mask);
                overall_hit[w] &= hit_mask;
              }
            }
          }
          
          for (int which_stash=0;which_stash<2;++which_stash) {
            if ( overall_hit[which_stash] ) {
              if (HasOnlyOneBitSet( overall_hit[which_stash] ) ) {
                RMT_ASSERT( ! hit ); // there should only be one hit overall
                mau()->mau_info_incr(MAU_STASH_HITS);
                hit = true;
                int index = 0;
                for (int b=0;b<kStashEntries/2;++b) {
                  if ( (overall_hit[which_stash] >> b) & 1 ) break;
                  index++;
                }
                int full_index = index + (which_stash ? MauDefs::kStashEntries/2 : 0);
                result->set_row( row );
                result->set_col( -1 );
                result->set_outbus( 1 << bus );
                result->set_hitindex( full_index );
                result->set_hitentry( 0 ); // stashes only use entry 0
                result->set_logical_tables( 1 << logicalTableIndex );
                result->set_payload(0); // tcam action bit
                uint8_t next_table = mau_stash_column_reg_.get_next_table(row,which_stash,index);
                result->set_next_table( next_table );
                result->set_exactmatch(true);
                result->set_match(true);
                result->set_stash(true);
                // drive the results onto the match bus
                stashes_[row]->set_match_output(phv,bus,full_index,which_stash);
              }
              else {
                RMT_ASSERT(0); // there should only be one hit overall
              }
              
            }
            
          }
        }
      }
    }
  
  }
  return hit;
}

void MauStashColumn::FindWhichStashesDriveBus(int row,int bus,bool* stash_0_drives_bus,bool* stash_1_drives_bus) {

    *stash_0_drives_bus = *stash_1_drives_bus = false;


  // The stash_row_nxtable_bus_drive register was used, 
  //   this works for Tofino because this register is always programmed the same as
  //   stash_match_result_bus_select, but it is not correct. So now we just check
  //   that which_bus is correct, and use stash_match_result_bus_select to do the work
    
    for (int stash_which=0;stash_which<2;++stash_which) {
      bool nt_bus0,nt_bus1;
      mau_stash_column_reg_.get_next_table_bus_drive(row,stash_which,&nt_bus0,&nt_bus1);
      bool drives_bus = stashes_[row]->get_match_result_bus_select(stash_which,bus);
      // TODO: check nt_bus0 , nt_bus1

      if ( drives_bus != (bus ? nt_bus1 : nt_bus0) ) {
        RMT_LOG_WARN("Stash match_result_bus_select does not match next_table_bus_drive "
                     "row=%d which=%d bus=%d  %s and %s!\n",
                     row, stash_which, bus, drives_bus?"true":"false",
                     (bus?nt_bus1:nt_bus0) ?"true":"false");
      }
      
      if (drives_bus) {
        if (stash_which==0) {
          *stash_0_drives_bus = true;
        }
        else {
          *stash_1_drives_bus = true;
        }
      }
    }

}

}
