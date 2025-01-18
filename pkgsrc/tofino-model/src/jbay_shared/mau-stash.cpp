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

#include <string>
#include <rmt-log.h>
#include <rmt-object-manager.h>
#include <mau-object.h>
#include <mau-stash.h>
#include <mau-sram-row.h>

namespace MODEL_CHIP_NAMESPACE {


void MauStash::set_match_output(Phv* phv, const int which_bus, const int index, const int which_stash) {
  RMT_ASSERT(index<kEntries);


  BitVector<kMatchOutputBusWidth> busVal;

  if ( mau_stash_reg_.stash_bus_overload( which_stash ) ) {
    // some bytes need to be copied from the match data to the bus
    BitVector<kMatchDataWidth> match_data;
    // the match data always comes from stash 0's input
    int match_data_bus = mau_stash_reg_.get_match_data_select(0 /*which_stash*/);
    row_->get_match_data(phv,match_data_bus,&match_data);
    for (int byte=0;byte<8;++byte) {
      int bit = byte*8;
      if ( mau_stash_reg_.stash_bus_overload_bytemask( which_stash, byte ) ) {
        busVal.set_word( match_data.get_word(bit,8), bit,8);
      }
      else {
        busVal.set_word( entries_[index].get_word(bit,8), bit,8);
      }
    }
    RMT_LOG(RmtDebug::verbose(),
            "MauStash Bus Overload [%02x] match_data=%016" PRIx64 " entry[%d]=%016" PRIx64 " busval=%016" PRIx64 "\n",
            mau_stash_reg_.stash_bus_overload( which_stash ), match_data.get_word(0), index, entries_[index].get_word(0),
            busVal.get_word(0));
  }
  else {
    // no overload, just get the data from the stash entry
    busVal.set_word(entries_[index].get_word(0,64),0,64);
  }

  busVal.set32(2, mau_stash_reg_.get_match_address(index));

  row_->set_match_output_bus(which_bus,busVal,888);
}

}
