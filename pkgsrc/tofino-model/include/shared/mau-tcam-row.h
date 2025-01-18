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

#ifndef _SHARED_MAU_TCAM_ROW_
#define _SHARED_MAU_TCAM_ROW_

#include <string>
#include <cstdint>
#include <rmt-defs.h>
#include <mau-defs.h>
#include <mau-object.h>
#include <mau-tcam.h>
#include <tcam-row-vh-xbar-with-reg.h>
#include <phv.h>


namespace MODEL_CHIP_NAMESPACE {

  class MauInput;

  class MauTcamRow : public MauObject {
    static constexpr int kType = RmtTypes::kRmtTypeMauTcamRow;
    static constexpr int kTotalMatchInputBits = MauDefs::kTotalMatchInputBits;
    static constexpr int kTotalMatchValidBits = MauDefs::kTotalMatchValidBits;

    static constexpr int kTcamWidth = MauDefs::kTcamWidth;
    static constexpr int kTcamAddressWidth  = MauDefs::kTcamAddressWidth;
    static constexpr int kTcamSearchBuses  = MauDefs::kTcamSearchBusesPerRow;
    
 public:
    MauTcamRow(RmtObjectManager *om, int pipeIndex, int mauIndex, int rowIndex,
               Mau *mau, MauInput *mau_input);
    virtual ~MauTcamRow();

    inline int row_index() const { return row_index_; }

    void get_input(Phv *phv,
                   BitVector<kTotalMatchInputBits> *input_bits,
                   BitVector<kTotalMatchValidBits> *valid_bits);
    void get_match_data(Phv *phv, int search_bus, int which_data,
                        BitVector<kTcamWidth> *search_data);
    void reset_resources();

    
 private:
    int                                              row_index_;
    std::array< TcamRowVhWithReg, kTcamSearchBuses>  match_xbars_;

  };
}
#endif // _SHARED_MAU_TCAM_ROW_
