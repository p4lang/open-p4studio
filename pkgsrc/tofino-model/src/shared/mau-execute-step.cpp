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
#include <nxt-tab.h>
#include <mau-lookup-result.h>
#include <mau-execute-step.h>

namespace MODEL_CHIP_NAMESPACE {

void MauExecuteStepMauInEgFunc::execute(MauExecuteState* s) {
  if (skip(s)) return;
  (mau_->*func_)(true,s);   // ingress 
  (mau_->*func_)(false,s);  // egress
}

void MauExecuteStepMauFunc::execute(MauExecuteState* s) {
  if (skip(s)) return;
  (mau_->*func_)(s);
}

void MauExecuteStepMauAddrDistFunc::execute(MauExecuteState* s) {
  if (skip(s)) return;
  (mau_->mau_addr_dist()->*func_)(s);
}

void MauExecuteStepRunRows::execute(MauExecuteState* s) {
  if (skip(s)) return;
  for (int i = 0; i < Mau::kSramRows; i++) {
    MauSramRow *row = mau_->sram_row_lookup(i);
    if (row != NULL)
      (row->*func_)();
  }
}

void MauExecuteStepRunRowsWithState::execute(MauExecuteState* s) {
  if (skip(s)) return;
  for (int i = 0; i < Mau::kSramRows; i++) {
    MauSramRow *row = mau_->sram_row_lookup(i);
    if (row != NULL)
      (row->*func_)(s);
  }
}

void MauExecuteStepRunTables::execute(MauExecuteState* s) {
  if (skip(s)) return;

  for (int ie = 0; ((ie == 0) || (ie == 1)); ie++) {
    bool ingress = (ie == 0);
    uint16_t lts_active = mau_->pred_lt_info(ingress? Pred::kIngActive :Pred::kEgrActive);

    for (int lt = 0; lt < Mau::kLogicalTables; lt++) {
      if (((lts_active >> lt) & 1) == 1) {
        MauLogicalTable *table = mau_->logical_table_lookup(lt);
        RMT_ASSERT((table != NULL) && (table->is_active()) &&
                   (table->check_ingress_egress(ingress)));
        MauLookupResult* result = mau_->mau_lookup_result(lt);                
        (table->*func_)(s->match_phv_, result, ingress);
      }
    }
  }
}


void MauExecuteStepRunTablesEop::execute(MauExecuteState* s) {
  if (skip(s)) return;

  for (int ti = 0; ti < Mau::kLogicalTables; ti++) {
    MauLogicalTable *table = mau_->logical_table_lookup(ti);
    if (table != NULL) (table->*func_)(s->eop_);
  }

}


void MauExecuteStepTableFunc::execute(MauExecuteState* s) {
  if (skip(s)) return;
  int ti = static_cast<int>(s->logical_table_);
  if ((ti >= 0) && (ti < Mau::kLogicalTables)) {
    MauLogicalTable *table = mau_->logical_table_lookup(ti);
    if (table != NULL) (table->*func_)(s);
  }
}

}
