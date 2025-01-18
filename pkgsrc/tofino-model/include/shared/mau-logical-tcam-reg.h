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

#ifndef _SHARED_MAU_LOGICAL_TCAM_REG_
#define _SHARED_MAU_LOGICAL_TCAM_REG_

#include <cstdint>
#include <rmt-defs.h>
#include <mau-defs.h>
#include <mau-object.h>

// Reg defs auto-generated from Semifore
#include <register_includes/match_to_logical_table_ixbar_outputmap_array2.h>
#include <register_includes/tcam_hit_to_logical_table_ixbar_outputmap.h>
#include <register_includes/tcam_match_adr_to_physical_oxbar_outputmap_array.h>
#include <register_includes/tcam_error_detect_enable.h>
#include <register_includes/tcam_match_adr_shift.h>


namespace MODEL_CHIP_NAMESPACE {

  class MauLogicalTcam;

  class MauLogicalTcamReg : public MauObject {
    
 public:
    static constexpr int  kType = RmtTypes::kRmtTypeMauLogicalTcamReg;
    static constexpr int  kSramRowsPerMau = MauDefs::kSramRowsPerMau; 
    static constexpr int  kLogicalTcamsPerMau = MauDefs::kLogicalTcamsPerMau;   
    static constexpr int  kTindOutputBusesPerMau = MauDefs::kTindOutputBusesPerMau;
    static constexpr int  kTindOutputBusesPerRow = MauDefs::kTindOutputBusesPerRow;

    
    MauLogicalTcamReg(RmtObjectManager *om, int pipeIndex, int mauIndex,
                      int ltcamIndex, MauLogicalTcam *mauLogicalTcam);
    virtual ~MauLogicalTcamReg();

    void tcam_ixbar_write_callback();
    int  get_logical_table(); 
    uint16_t get_tind_buses();
    int  paired_ltcam(bool ingress);
    int  tcam_match_adr_shift();


    
 private:
    MauLogicalTcam                                          *mau_logical_tcam_;
    register_classes::MatchToLogicalTableIxbarOutputmapArray2    match_to_logical_table_ixbar_outputmap_;
    register_classes::TcamErrorDetectEnable                      tcam_error_detect_enable_;
    register_classes::TcamMatchAdrShift                          tcam_match_adr_shift_;
    register_classes::TcamMatchAdrToPhysicalOxbarOutputmapArray  tcam_match_addr_to_physical_oxbar_outputmap_;
    register_classes::TcamHitToLogicalTableIxbarOutputmap        tcam_hit_to_logical_table_ixbar_outputmap_;
  };
}
#endif // _SHARED_MAU_LOGICAL_TCAM_REG_
