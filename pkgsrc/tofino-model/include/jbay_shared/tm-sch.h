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

#ifndef __JBAY_SHARED_TM_SCH_BLK__
#define __JBAY_SHARED_TM_SCH_BLK__
#include <tm-object.h>
#include <tm-sch-occ.h>
#include <tm-defines.h>
#include <tm-structs.h>
#include <tm-sch-config.h>
#include <tm-sch-state.h>
#include <tm-sch-max-rate.h>
#include <tm-sch-min-rate.h>
#include <tm-sch-exc-rate.h>
#include <tm-sch-pex-cr.h>
#include <tm-sch-sel.h>
#include <tm-sch.h>
namespace MODEL_CHIP_NAMESPACE {

class TmSch : public TmObject {
 public:
  // QLC->SCH Fifos
  std::vector<sch_deq_info> qlc_sch_fifo[TmDefs::kNumQlcSchUpdIfc];

  // SCH->QLC Fifo
  std::vector<sch_deq_info> sch_qlc_fifo;

  // PEX->SCH BUpd Fifo
  std::vector<pex_bupd_info> pex_sch_bupd_fifo[TmDefs::kNumPexSchCrdUpdIfc];

  // PEX->SCH Credit Fifos
  std::vector<pex_crd_info> pex_sch_crd_fifo[TmDefs::kNumPexSchCrdUpdIfc];

  TmSch(RmtObjectManager *om, uint8_t pipe_index);
  void init(TmSchConfig *sch_config, TmSchState *sch_state);
  void pps_tick();
  void one_ns_tick();
  void move_data();
  TmSchConfig* get_sch_config();

 private:
  TmSchConfig m_config;
  TmSchState m_state;
  TmSchOcc m_occ;
  TmSchMaxRate m_max_rate;
  TmSchMinRate m_min_rate;
  TmSchExcRate m_exc_rate;
  TmSchPexCr m_pex_cr;
  TmSchSel m_sel;
};

}
#endif
