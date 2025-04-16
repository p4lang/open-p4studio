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

#include <utests/test_util.h>
#include <iostream>
#include <string>
#include <array>
#include <cassert>

#include "gtest.h"

#include <bitvector.h>
#include <rmt-object-manager.h>
#include <model_core/model.h>
#include <register_utils.h>
#include <mem_utils.h>
#include <chip.h>
#include <other-tm-objects.h>
#include <tm-defines.h>
#include <tm-sch-config.h>
#include <rmt-defs.h>

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;

namespace MODEL_CHIP_TEST_NAMESPACE {

  bool tm_print = true;

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;


  TEST(BFN_DISABLE_CB(BFN_TEST_NAME(TmTest)),SchAllFeature) {
    // TODO: WIP: fix test and remove BFN_DISABLE
    GLOBAL_MODEL->Reset();
    if (tm_print) RMT_UT_LOG_INFO("test_tm_sch_all_feature()\n");

    int chip_id = 0;
    int pipe_id = 0;
    int stage_id = 0;

    // Create our TestUtil class
    // Instantiating new TestUtil obj should free all existing
    // RmtObjectManagers (if this has not already occurred) then
    // recreate a RmtObjectManager just for chip
    TestUtil *tu = new TestUtil(GLOBAL_MODEL.get(), chip_id, pipe_id, stage_id);
    RmtObjectManager *om = tu->get_objmgr();
    ASSERT_TRUE(om != NULL);
    Chip *chip = om->chip();
    TmSchModelWrapper* m_sch_model_wrap;

    // Set up SCH Instance in Model
    uint32_t sch_pipe_id = 3, sch_port_id = 8, sch_l1_id = 62, sch_q_id = 128,
      sch_port_exp_rate = 350000, sch_l1_exp_rate = 350000, sch_q_exp_rate[3] = {150000, 80000, 120000},
      sch_port_act_rate, sch_l1_act_rate, sch_q_act_rate[3], rate_tolerance_pct = 1, rate_diff,
      reg_base_addr;

    bool vq_bmp[TmDefs::kNumVqPerMac];

    // Get Model Wrapper Instance
    m_sch_model_wrap = om->sch_model_wrap_get(sch_pipe_id);

    /*
    // Set up logging
    uint64_t MODEL_ALL_DEBUG_FLAGS = UINT64_C(0xFFFFFFFFFFFFFFFF);
    om->update_log_flags(MODEL_ALL_DEBUG_FLAGS, MODEL_ALL_DEBUG_FLAGS,
			 MODEL_ALL_DEBUG_FLAGS, MODEL_ALL_DEBUG_FLAGS,
			 MODEL_ALL_DEBUG_FLAGS, MODEL_ALL_DEBUG_FLAGS,
			 MODEL_ALL_DEBUG_FLAGS);
    */

    // Send Test Configuration to Model
    // Port Config
    reg_base_addr = (sch_pipe_id < 2) ?  BFN_REG_TM_SCHA(port_config_address) :
      BFN_REG_TM_SCHB(port_config_address);
    uint32_t reg_addr = ((sch_pipe_id % 2) * BFN_REG_TM_SCHA(array_element_size)) +
                        (sch_port_id * BFN_REG_TM_SCHA(port_config_array_element_size)) +
                        reg_base_addr;
    uint32_t reg_data = 0;
    setp_sch_port_config_r_enb(&reg_data, 1);
    setp_sch_port_config_r_max_rate_enb(&reg_data, 1);
    setp_sch_port_config_r_port_speed_mode(&reg_data, 7);
    chip->OutWord(reg_addr, reg_data);

    // Port Max Rate
    uint64_t mem_addr, mem_base_addr;
    uint64_t mem_data0, mem_data1;
    mem_base_addr = (sch_pipe_id < 2) ? BFN_MEM_TM_SCHA(port_max_lb_static_mem_address) :
      BFN_MEM_TM_SCHB(port_max_lb_static_mem_address);
    mem_addr =  ((sch_pipe_id % 2) * BFN_MEM_TM_SCHA(array_element_size)) +
                (sch_port_id * BFN_MEM_TM_SCHA(port_max_lb_static_mem_entry_array_element_size)) +
                mem_base_addr;
    mem_data0 = 0;
    bfn_mem::setp_tm_sch_pipe_mem_rspec_port_max_lb_static_mem_entry_bs_exp(&mem_data0, 0xa);
    bfn_mem::setp_tm_sch_pipe_mem_rspec_port_max_lb_static_mem_entry_bs_mant(&mem_data0, 0xa);
    bfn_mem::setp_tm_sch_pipe_mem_rspec_port_max_lb_static_mem_entry_rate_mant(&mem_data0, 3500);
    mem_data1 = 0;
    GLOBAL_MODEL->IndirectWrite(chip_id, (mem_addr >> 4), mem_data0, mem_data1);

    // L1 Config
    reg_base_addr = (sch_pipe_id < 2) ? BFN_REG_TM_SCHA(l1_config_address) :
      BFN_REG_TM_SCHB(l1_config_address);
    reg_addr = ((sch_pipe_id % 2) * BFN_REG_TM_SCHA(array_element_size)) +
               (sch_l1_id * BFN_REG_TM_SCHA(l1_config_array_element_size)) +
               reg_base_addr;
    reg_data = 0x0;
    setp_sch_l1_config_r_enb(&reg_data, 1);
    setp_sch_l1_config_r_max_rate_enb(&reg_data, 1);
    chip->OutWord(reg_addr, reg_data);

    // L1 Max Rate
    mem_base_addr = (sch_pipe_id < 2) ? BFN_MEM_TM_SCHA(l1_max_lb_static_mem_address) :
      BFN_MEM_TM_SCHB(l1_max_lb_static_mem_address);
    mem_addr =  ((sch_pipe_id % 2) * BFN_MEM_TM_SCHA(array_element_size)) +
                (sch_l1_id * BFN_MEM_TM_SCHA(l1_max_lb_static_mem_entry_array_element_size)) +
                mem_base_addr;
    mem_data0 = 0;
    bfn_mem::setp_tm_sch_pipe_mem_rspec_l1_max_lb_static_mem_entry_bs_exp(&mem_data0, 0xa);
    bfn_mem::setp_tm_sch_pipe_mem_rspec_l1_max_lb_static_mem_entry_bs_mant(&mem_data0, 0xa);
    bfn_mem::setp_tm_sch_pipe_mem_rspec_l1_max_lb_static_mem_entry_rate_mant(&mem_data0, 4000);
    mem_data1 = 0;
    GLOBAL_MODEL->IndirectWrite(chip_id, (mem_addr >> 4), mem_data0, mem_data1);

    // Q Config
    for (int queue=0; queue<3; queue++) {
      reg_base_addr = (sch_pipe_id < 2) ? BFN_REG_TM_SCHA(queue_config_address) :
        BFN_REG_TM_SCHB(queue_config_address);
      reg_addr = ((sch_pipe_id % 2) * BFN_REG_TM_SCHA(array_element_size)) +
                 (sch_q_id * BFN_REG_TM_SCHA(queue_config_array_element_size)) +
                 reg_base_addr;
      reg_data = 0;
      setp_sch_queue_config_r_enb(&reg_data, 1);
      setp_sch_queue_config_r_min_rate_enb(&reg_data, 1);
      setp_sch_queue_config_r_l1_id(&reg_data, (sch_l1_id - TmDefs::kNumL1PerMac));
      if (queue == 0) {
	setp_sch_queue_config_r_max_rate_enb(&reg_data, 1);
	setp_sch_queue_config_r_min_rate_pri(&reg_data, 7);
	setp_sch_queue_config_r_max_rate_pri(&reg_data, 7);
      } else {
	setp_sch_queue_config_r_min_rate_pri(&reg_data, 5);
	setp_sch_queue_config_r_max_rate_pri(&reg_data, 5);
      }
      chip->OutWord(reg_addr, reg_data);

      // Q Max Rate
      mem_base_addr = (sch_pipe_id < 2) ? BFN_MEM_TM_SCHA(q_max_lb_static_mem_address) :
        BFN_MEM_TM_SCHB(q_max_lb_static_mem_address);
      mem_addr =  ((sch_pipe_id % 2) * BFN_MEM_TM_SCHA(array_element_size)) +
                  (sch_q_id * BFN_MEM_TM_SCHA(q_max_lb_static_mem_entry_array_element_size)) +
                  mem_base_addr;
      mem_data0 = 0;
      bfn_mem::setp_tm_sch_pipe_mem_rspec_q_max_lb_static_mem_entry_bs_exp(&mem_data0, 0xa);
      bfn_mem::setp_tm_sch_pipe_mem_rspec_q_max_lb_static_mem_entry_bs_mant(&mem_data0, 0xa);
      bfn_mem::setp_tm_sch_pipe_mem_rspec_q_max_lb_static_mem_entry_rate_mant(&mem_data0, 1500);
      mem_data1 = 0;
      GLOBAL_MODEL->IndirectWrite(chip_id, (mem_addr >> 4), mem_data0, mem_data1);

      // Q Min Rate
      mem_base_addr = (sch_pipe_id < 2) ? BFN_MEM_TM_SCHA(q_min_lb_static_mem_address) :
        BFN_MEM_TM_SCHB(q_min_lb_static_mem_address);
      mem_addr =  ((sch_pipe_id % 2) * BFN_MEM_TM_SCHA(array_element_size)) +
                  (sch_q_id * BFN_MEM_TM_SCHA(q_min_lb_static_mem_entry_array_element_size)) +
                  mem_base_addr;
      mem_data0 = 0;
      bfn_mem::setp_tm_sch_pipe_mem_rspec_q_min_lb_static_mem_entry_bs_exp(&mem_data0, 0xa);
      bfn_mem::setp_tm_sch_pipe_mem_rspec_q_min_lb_static_mem_entry_bs_mant(&mem_data0, 0xa);
      if (queue == 0)
	bfn_mem::setp_tm_sch_pipe_mem_rspec_q_min_lb_static_mem_entry_rate_mant(&mem_data0, 1000);
      else if (queue == 1)
	bfn_mem::setp_tm_sch_pipe_mem_rspec_q_min_lb_static_mem_entry_rate_mant(&mem_data0, 500);
      else
	bfn_mem::setp_tm_sch_pipe_mem_rspec_q_min_lb_static_mem_entry_rate_mant(&mem_data0, 0);
      mem_data1 = 0;
      GLOBAL_MODEL->IndirectWrite(chip_id, (mem_addr >> 4), mem_data0, mem_data1);

      // Q Excess Weight
      mem_base_addr = (sch_pipe_id < 2) ? BFN_MEM_TM_SCHA(q_exc_static_mem_address) :
        BFN_MEM_TM_SCHB(q_exc_static_mem_address);
      mem_addr =  ((sch_pipe_id % 2) * BFN_MEM_TM_SCHA(array_element_size)) +
                  (sch_q_id * BFN_MEM_TM_SCHA(q_exc_static_mem_entry_array_element_size)) +
                  mem_base_addr;
      mem_data0 = 0;
      if (queue == 0)
	bfn_mem::setp_tm_sch_pipe_mem_rspec_q_exc_static_mem_entry_wt(&mem_data0, 0);
      else if (queue == 1)
	bfn_mem::setp_tm_sch_pipe_mem_rspec_q_exc_static_mem_entry_wt(&mem_data0, 72);
      else
	bfn_mem::setp_tm_sch_pipe_mem_rspec_q_exc_static_mem_entry_wt(&mem_data0, 288);
      mem_data1 = 0;
      GLOBAL_MODEL->IndirectWrite(chip_id, (mem_addr >> 4), mem_data0, mem_data1);

      // Set up the queues
      memset(vq_bmp, 0, sizeof(vq_bmp));
      vq_bmp[sch_q_id % TmDefs::kNumVqPerMac] = 1;
      m_sch_model_wrap->set_active_vq_in_mac((sch_port_id / TmDefs::kNumPortPerMac), vq_bmp);

      // Increment Qid
      sch_q_id++;
    }

    // Global Byte Adjust
    reg_base_addr = (sch_pipe_id < 2) ? BFN_REG_TM_SCHA(global_bytecnt_adj_address) :
                    BFN_REG_TM_SCHB(global_bytecnt_adj_address);
    reg_addr = ((sch_pipe_id % 2) * BFN_REG_TM_SCHA(array_element_size)) + reg_base_addr;
    reg_data = 0;
    setp_sch_byte_count_adjust_r_value(&reg_data, 20);
    chip->OutWord(reg_addr, reg_data);

    // Run SCH Model
    m_sch_model_wrap->run_sch_model();

    // Get Rates from Model & Check
    // Port Level
    sch_port_act_rate = m_sch_model_wrap->get_port_rate_mbps(sch_port_id);
    rate_diff = (sch_port_act_rate > sch_port_exp_rate) ? (sch_port_act_rate - sch_port_exp_rate) : (sch_port_exp_rate - sch_port_act_rate);
    ASSERT_LT (rate_diff, ((rate_tolerance_pct * sch_port_exp_rate) / 100)) << "act rate=" << sch_port_act_rate << "exp rate=" << sch_port_exp_rate;

    // L1 Level
    sch_l1_act_rate = m_sch_model_wrap->get_l1_rate_mbps(sch_l1_id);
    rate_diff = (sch_l1_act_rate > sch_l1_exp_rate) ? (sch_l1_act_rate - sch_l1_exp_rate) : (sch_l1_exp_rate - sch_l1_act_rate);
    ASSERT_LT (rate_diff, ((rate_tolerance_pct * sch_l1_exp_rate) / 100));

    // Queue Level
    sch_q_id = 128;
    for (int queue=0; queue<3; queue++) {
      sch_q_act_rate[queue] = m_sch_model_wrap->get_q_rate_mbps(sch_q_id);
      rate_diff = (sch_q_act_rate[queue] > sch_q_exp_rate[queue]) ? (sch_q_act_rate[queue] - sch_q_exp_rate[queue]) : (sch_q_exp_rate[queue] - sch_q_act_rate[queue]);
      ASSERT_LT (rate_diff, ((rate_tolerance_pct * sch_q_exp_rate[queue]) / 100));
      sch_q_id++;
    }

    // End of Test
    tu->quieten_log_flags();
    delete tu;
  }
}
