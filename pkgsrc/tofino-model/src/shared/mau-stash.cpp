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
#include <mau-sram.h> // for match_nibble_s1q0bar_enable_update()
#include <mau-sram-row.h>

namespace MODEL_CHIP_NAMESPACE {

MauStash::MauStash(RmtObjectManager *om, int pipeIndex, int mauIndex,
                   int rowIndex, Mau *mau, MauSramRow *row)
    : MauObject(om, pipeIndex, mauIndex, RmtTypes::kRmtTypeMauStash,rowIndex, mau),
    mau_stash_reg_(om,pipeIndex,mauIndex,rowIndex,this),
    row_(row) {
  inhibit_array_[0] = 0xFFFFFFFF; inhibit_array_[1] = 0xFFFFFFFF;
  RMT_LOG_VERBOSE("MAU_STASH::create\n");
}
MauStash::~MauStash() {
  RMT_LOG_VERBOSE("MAU_STASH::delete\n");
}


void MauStash::lookup(Phv *phv, int which_stash, uint32_t *hit_mask) {
  *hit_mask = 0;

  if ( ! ( mau_stash_reg_.get_enable( which_stash ))) {
    return;
  }

  std::array<BitVector<kWholeHashWidth>,2> all_hash;

  int group      = mau_stash_reg_.get_hash_adr_which_group(which_stash);
  int group_word = mau_stash_reg_.get_hash_adr_which_word(which_stash);
  int bank_word  = mau_stash_reg_.get_hashbank_select(which_stash);

  // get the needed hash words
  for (int hw=0;hw<2;++hw) {
    if ( hw == group_word || hw == bank_word ) {
      row_->get_hash(phv,hw,&(all_hash[hw]));
      RMT_LOG(RmtDebug::verbose(),
              "MauStash::lookup hash[%d]=%" PRIx64 "\n",
              hw, all_hash[hw].get_word(0));
    }
  }

  uint32_t hash_address = all_hash[ group_word ].get_word( kAddressWidth * group,
                                                           kAddressWidth );
  uint32_t hash_bank    = all_hash[ bank_word ].get_word( kMaskedEqualsStart,
                                                          kMaskedEqualsWidth );

  RMT_LOG(RmtDebug::verbose(),
          "MauStash::lookup stash=%d group=%d group_word=%d bank_word=%d hash_address=0x%x hash_bank=0x%x\n",
          which_stash,group,group_word,bank_word, hash_address,hash_bank);

  
  // Go through all the entries finding all the matches (must do all
  //  information is there for match central to sort out wide matches)
  for (int i=0; i < kEntries/2; ++i) {
    int index = i + (which_stash * (kEntries/2));

    // Calculate whether the entry entry is valid for current phv
    int version_valid = mau_stash_reg_.get_thread( which_stash ) ?
        phv->egress_version() : phv->ingress_version();
    bool vv_bit0 = version_valid & 1;
    bool vv_bit1 = version_valid & 2;
    bool valid =
        ((   vv_bit0  && mau_stash_reg_.get_valid_version_bit_0_new( index ) ) ||
         ( (!vv_bit0) && mau_stash_reg_.get_valid_version_bit_0_old( index ) )) &&
        ((   vv_bit1  && mau_stash_reg_.get_valid_version_bit_1_new( index ) ) ||
         ( (!vv_bit1) && mau_stash_reg_.get_valid_version_bit_1_old( index ) ));
    
    if (valid) {
      // the stored hash address must match the incoming hash address
      //  and the bank must match the masked incoming bank
      if ( (hash_address == mau_stash_reg_.get_hashkey_data( index ) ) &&
           ((hash_bank & mau_stash_reg_.get_bank_enable_mask( index )) ==
            mau_stash_reg_.get_bank_enable_id( index ) ) ) {

        // now see if the data matches
        int match_data_bus = mau_stash_reg_.get_match_data_select(which_stash);
        RMT_ASSERT(match_data_bus<2);
        BitVector<kMatchDataWidth> match_data;
        row_->get_match_data(phv,match_data_bus,&match_data);
        
        // note: masked_equals() takes a positive mask (0=ignore), this is
        //  to inverse of the mask in the registers which have 1=ignore. The inversion
        //  in mask_update() below, so mask_array_ stores a positive mask
        if (match_data.masked_equals(entries_[index], mask_array_[which_stash]) &&
            (!hit_inhibited(which_stash, mau_stash_reg_.get_match_address(index))) ) {	
          *hit_mask |= (1 << i);
          RMT_LOG(RmtDebug::verbose(),
                  "MauStash::lookup(entry=%d) 0x%02x HIT "
                  "(matchAddr=0x%08x inhibitAddr=0x%08x)\n",
                  index, *hit_mask,
                  mau_stash_reg_.get_match_address(index),
                  inhibit_array_[which_stash]);
        }
        else {
          RMT_LOG(RmtDebug::verbose(),
                  "MauStash::lookup(entry=%d) data didn't match match_data=%s entry=%s mask=%s hit_inhibited=%d match_address=%x\n",
                  index, match_data.to_string().c_str(), entries_[index].to_string().c_str(), mask_array_[which_stash].to_string().c_str(),
                  hit_inhibited(which_stash, mau_stash_reg_.get_match_address(index)), mau_stash_reg_.get_match_address(index) );
        }
      }
      else {
        RMT_LOG(RmtDebug::verbose(),
                "MauStash::lookup(entry=%d) address didn't match hash_addr=%x hash_key_data=%x hash_bank=%x bank_enable_mask=%x bank_enable_id=%x\n",
                index, hash_address, mau_stash_reg_.get_hashkey_data( index ),
                hash_bank, mau_stash_reg_.get_bank_enable_mask( index ),
                mau_stash_reg_.get_bank_enable_id( index ));
      }
    }
    else {
      RMT_LOG(RmtDebug::verbose(),
              "MauStash::lookup(entry=%d) not valid vv_0=%d vv_1=%d\n",
              index, vv_bit0, vv_bit1);
    }
  }
}

// moved to chip specific
//void MauStash::set_match_output(const int which_bus, const int index)

void MauStash::entry_update(uint32_t a1,uint32_t a0,uint32_t v) {
  RMT_ASSERT(a1<kEntries);
  RMT_ASSERT(a0< MauDefs::kSramWidth / 32);
  entries_[a1].set32( a0, v );
}

void MauStash::mask_update(uint32_t a1,uint32_t a0,uint32_t v) {
  RMT_ASSERT(a1<2);
  RMT_ASSERT(a0< MauDefs::kSramWidth / 32);
  uint32_t positive_mask= ~v;
  mask_array_[a1].set32( a0, positive_mask );
}

}
