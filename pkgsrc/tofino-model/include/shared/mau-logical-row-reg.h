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

#ifndef _SHARED_MAU_LOGICAL_ROW_REG_
#define _SHARED_MAU_LOGICAL_ROW_REG_

#include <cstdint>
#include <rmt-defs.h>
#include <mau-defs.h>
#include <mau-object.h>

// Reg defs auto-generated from Semifore
#include <register_includes/selector_action_adr_fallback.h>
#include <register_includes/adr_dist_oflo_adr_xbar_ctl.h>
#include <register_includes/adr_dist_oflo2_adr_xbar_ctl.h>
#include <register_includes/ram_address_mux_ctl_array2.h>



namespace MODEL_CHIP_NAMESPACE {

  class MauLogicalRow;

  class MauLogicalRowReg : public MauObject {

    static constexpr int kType = RmtTypes::kRmtTypeMauLogicalRowReg;
    static constexpr int kLogicalColumns = MauDefs::kLogicalColumnsPerMau;

 public:
    MauLogicalRowReg(RmtObjectManager *om,
                     int pipeIndex, int mauIndex, int logrowIndex,
                     Mau *mau, MauLogicalRow *mauLogicalRow,
                     int physrowIndex, int physrowWhich);
    virtual ~MauLogicalRowReg();

    inline uint16_t get_addr_mux_vals() { return addr_mux_vals_; }
    inline uint32_t get_homerow_fallback_action_addr() {
      return selector_action_adr_fallback_homerow_.selector_action_adr_fallback();
    }
    inline uint32_t get_oflo_fallback_action_addr() {
      return selector_action_adr_fallback_oflo_.selector_action_adr_fallback();
    }

    uint32_t oflow_addr(bool ingress, uint8_t addrtype);
    uint32_t oflow2_addr(bool ingress, uint8_t addrtype);

    // Get ALU index/type corresponding to oflow_addr (-1 if none)
    int get_oflow_alu(uint8_t *addrtype, uint8_t *alutype=NULL);


 private:
    void addr_mux_change_callback(uint32_t lr, uint32_t logcol);
    void oflow_handle(bool ingress, uint32_t *addr, uint8_t addrtype,
                      int lc, int pri);
    void oflow2_handle(bool ingress, uint32_t *addr, uint8_t addrtype,
                       int lc, int pri);


 private:
    MauLogicalRow                            *mau_logical_row_;
    int                                       logrowIndex_;
    int                                       physrowIndex_;
    int                                       physrowWhich_;
    uint16_t                                  addr_mux_vals_;
    register_classes::SelectorActionAdrFallback   selector_action_adr_fallback_homerow_;
    register_classes::SelectorActionAdrFallback   selector_action_adr_fallback_oflo_;
    register_classes::AdrDistOfloAdrXbarCtl       adr_dist_oflo_xbar_;
    register_classes::AdrDistOflo2AdrXbarCtl      adr_dist_oflo2_xbar_;
    register_classes::RamAddressMuxCtlArray2      ram_address_mux_ctl_array_;
  };
}
#endif // _SHARED_MAU_LOGICAL_ROW_REG_
