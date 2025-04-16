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

#ifndef __JBAY_SHARED_TM_SCH_MODEL_WRAPPER__
#define __JBAY_SHARED_TM_SCH_MODEL_WRAPPER__
#include <common/rmt-util.h>
#include <tm-sch-config.h>
#include <tm-sch-state.h>
#include <tm-sch.h>
#include <tm-qlc.h>
#include <tm-pex.h>
#include <rmt-log.h>
#include <rmt-object-manager.h>

namespace MODEL_CHIP_NAMESPACE {

class TmSchModelWrapper : public DefaultLogger {
 public:
  TmSchModelWrapper(RmtObjectManager *om, uint8_t pipe_index);
  ~TmSchModelWrapper();
  void set_active_vq_in_mac(uint8_t mac, bool vq_bmp[TmDefs::kNumVqPerMac]);
  void run_sch_model();
  uint32_t get_port_rate_mbps(uint8_t port);
  uint32_t get_l1_rate_mbps(uint16_t l1_pipe_id);
  uint32_t get_q_rate_mbps(uint16_t q_pipe_id);

 private:
  DISALLOW_COPY_AND_ASSIGN(TmSchModelWrapper);
  // Functions
  void move_data();
  void compute_l1_rates(uint8_t port, float sim_time);
  void compute_q_rates(uint16_t l1_pipe_id, float sim_time);

  // QLC
  TmQlc* m_qlc;

  // PEX
  TmPex* m_pex;

  // SCH
  TmSch* m_sch;

  // Rate Measurement Variables
  unsigned int packets_processed_port[TmDefs::kNumPortPerPipe];
  unsigned int packets_processed_l1[TmDefs::kNumL1PerPipe];
  unsigned int packets_processed_q[TmDefs::kNumVqPerPipe];

  uint64_t bytes_acc_port[TmDefs::kNumPortPerPipe];
  uint64_t bytes_acc_l1[TmDefs::kNumL1PerPipe];
  uint64_t bytes_acc_q[TmDefs::kNumVqPerPipe];

  float port_rate[TmDefs::kNumPortPerPipe];
  float l1_rate[TmDefs::kNumL1PerPipe];
  float q_rate[TmDefs::kNumVqPerPipe];

};

}
#endif
