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
#include <string>
#include <rmt-log.h>
#include <rmt-object-manager.h>
#include <mau-tcam-row.h>


namespace MODEL_CHIP_NAMESPACE {

  MauTcamRow::MauTcamRow(RmtObjectManager *om,
                         int pipeIndex, int mauIndex, int rowIndex,
                         Mau *mau, MauInput *mau_input)
      : MauObject(om, pipeIndex, mauIndex, kType, rowIndex, mau),
        row_index_(rowIndex),
        match_xbars_{ { { chip_index(),pipeIndex,mauIndex,rowIndex/2,0 },
                        { chip_index(),pipeIndex,mauIndex,rowIndex/2,1 } } } {

    static_assert(kTcamSearchBuses == 2, "MauTcamRow can only handle 2 search buses");
    // NB. Only allow MauTcamRow on *even* rows - see explanation in mau.cpp
    RMT_ASSERT((rowIndex % 2) == 0); 
    reset_resources();
    RMT_LOG(RmtDebug::verbose(), "MAU_TCAM_ROW::create\n");
  }
  MauTcamRow::~MauTcamRow() {
    RMT_LOG(RmtDebug::verbose(), "MAU_TCAM_ROW::delete\n");
  }

  void MauTcamRow::get_input(Phv *phv,
                             BitVector<kTotalMatchInputBits> *input_bits,
                             BitVector<kTotalMatchValidBits> *valid_bits) {
    Mau *mauobj = mau();
    RMT_ASSERT (mauobj != NULL);
    mauobj->get_total_match_input(phv, input_bits, valid_bits);
    
  }

  void MauTcamRow::get_match_data(Phv *phv, int search_bus, int which_data,
                                  BitVector<kTcamWidth> *search_data) {
    RMT_ASSERT((search_bus >= 0) && (search_bus < kTcamSearchBuses));
    RMT_ASSERT((which_data == 0) || (which_data == 1));

    TcamRowVhWithReg *match_xbar = &match_xbars_[search_bus];
    BitVector<kTotalMatchInputBits>  input_bits;
    BitVector<kTotalMatchValidBits>  valid_bits;
    BitVector<TcamRowVhWithReg::kVersionDataWidth> ingress_version_bits;
    BitVector<TcamRowVhWithReg::kVersionDataWidth> egress_version_bits;
    BitVector<kTcamWidth> search_data_dummy;
    BitVector<kTcamWidth> *search_even = (which_data==0) ?search_data :&search_data_dummy;
    BitVector<kTcamWidth> *search_odd  = (which_data==1) ?search_data :&search_data_dummy;
    
    get_input(phv, &input_bits, &valid_bits);

    ingress_version_bits.set_byte(phv->ingress_version(), 0);
    egress_version_bits.set_byte(phv->egress_version(), 0);

    match_xbar->CalculateSearchData(input_bits, valid_bits,
                                    ingress_version_bits, egress_version_bits,
                                    search_even, search_odd);
    RMT_LOG(RmtDebug::kRmtDebugTrace, "MAU_TCAM_ROW::get_match_data row=%d even=%" PRIX64 " odd=%" PRIX64 "\n",
            row_index_,search_even->get_word(0),search_odd->get_word(0));

  }
      
  void MauTcamRow::reset_resources() {
  }


}
