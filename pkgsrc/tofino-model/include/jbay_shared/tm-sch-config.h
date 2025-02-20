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

#ifndef __JBAY_SHARED_TM_SCH_CONFIG__
#define __JBAY_SHARED_TM_SCH_CONFIG__
#include <mcn_test.h>
#include <tm-object.h>
#include <tm-structs.h>
#include <tm-defines.h>
#include <model_core/register_block.h>
#include <register_includes/sch_port_config_r_array_mutable.h>
#include <register_includes/sch_l1_config_r_array_mutable.h>
#include <register_includes/sch_queue_config_r_array_mutable.h>
#include <register_includes/sch_byte_count_adjust_r_mutable.h>

#if MCN_TEST(MODEL_CHIP_NAMESPACE, jbay) || MCN_TEST(MODEL_CHIP_NAMESPACE, jbayB0)
#define BFN_MEM_TM_SCHA(y)    BFN_CONCAT(bfn_mem::jbay_mem_tm_tm_scha_sch_pipe_mem_,y)
#define BFN_MEM_TM_SCHB(y)    BFN_CONCAT(bfn_mem::jbay_mem_tm_tm_schb_sch_pipe_mem_,y)
#define BFN_REG_TM_SCHA(y)    BFN_CONCAT(jbay_reg_device_select_tm_top_tm_scha_top_sch_,y)
#define BFN_REG_TM_SCHB(y)    BFN_CONCAT(jbay_reg_device_select_tm_top_tm_schb_top_sch_,y)


#else
static_assert(false, "Unexpected MODEL_CHIP_NAMESPACE");
#endif

namespace MODEL_CHIP_NAMESPACE {

class TmSchConfig;

// Classes for Memories
// Port Max Rate
class tm_sch_port_max_rate_static_mem : public model_core::RegisterBlockIndirect<RegisterCallback> {
 public:
  tm_sch_port_max_rate_static_mem(int pipe_index, TmSchConfig *sch_cfg);
  uint64_t get_mem_offset(int pipe_index);
 private:
  bool write(uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T);
  bool read(uint64_t offset,uint64_t* data0,uint64_t* data1, uint64_t T) const;
  std::string to_string(bool print_zeros, std::string indent_string) const {return "";}
  std::string to_string(uint64_t offset, bool print_zeros, std::string indent_string) const {return "";}
  TmSchConfig *sch_cfg = nullptr;
};

// L1 Max Rate
class tm_sch_l1_max_rate_static_mem : public model_core::RegisterBlockIndirect<RegisterCallback> {
 public:
  tm_sch_l1_max_rate_static_mem(int pipe_index, TmSchConfig *sch_cfg);
  uint64_t get_mem_offset(int pipe_index);
 private:
  bool write(uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T);
  bool read(uint64_t offset,uint64_t* data0,uint64_t* data1, uint64_t T) const;
  std::string to_string(bool print_zeros, std::string indent_string) const {return "";}
  std::string to_string(uint64_t offset, bool print_zeros, std::string indent_string) const {return "";}
  TmSchConfig *sch_cfg = nullptr;
};

// Q Max Rate
class tm_sch_q_max_rate_static_mem : public model_core::RegisterBlockIndirect<RegisterCallback> {
 public:
  tm_sch_q_max_rate_static_mem(int pipe_index, TmSchConfig *sch_cfg);
  uint64_t get_mem_offset(int pipe_index);
 private:
  bool write(uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T);
  bool read(uint64_t offset,uint64_t* data0,uint64_t* data1, uint64_t T) const;
  std::string to_string(bool print_zeros, std::string indent_string) const {return "";}
  std::string to_string(uint64_t offset, bool print_zeros, std::string indent_string) const {return "";}
  TmSchConfig *sch_cfg = nullptr;
};

// L1 Min Rate
class tm_sch_l1_min_rate_static_mem : public model_core::RegisterBlockIndirect<RegisterCallback> {
 public:
  tm_sch_l1_min_rate_static_mem(int pipe_index, TmSchConfig *sch_cfg);
  uint64_t get_mem_offset(int pipe_index);
 private:
  bool write(uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T);
  bool read(uint64_t offset,uint64_t* data0,uint64_t* data1, uint64_t T) const;
  std::string to_string(bool print_zeros, std::string indent_string) const {return "";}
  std::string to_string(uint64_t offset, bool print_zeros, std::string indent_string) const {return "";}
  TmSchConfig *sch_cfg = nullptr;
};

// Q Min Rate
class tm_sch_q_min_rate_static_mem : public model_core::RegisterBlockIndirect<RegisterCallback> {
 public:
  tm_sch_q_min_rate_static_mem(int pipe_index, TmSchConfig *sch_cfg);
  uint64_t get_mem_offset(int pipe_index);
 private:
  bool write(uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T);
  bool read(uint64_t offset,uint64_t* data0,uint64_t* data1, uint64_t T) const;
  std::string to_string(bool print_zeros, std::string indent_string) const {return "";}
  std::string to_string(uint64_t offset, bool print_zeros, std::string indent_string) const {return "";}
  TmSchConfig *sch_cfg = nullptr;
};

// L1 Exc Rate
class tm_sch_l1_exc_rate_static_mem : public model_core::RegisterBlockIndirect<RegisterCallback> {
 public:
  tm_sch_l1_exc_rate_static_mem(int pipe_index, TmSchConfig *sch_cfg);
  uint64_t get_mem_offset(int pipe_index);
 private:
  bool write(uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T);
  bool read(uint64_t offset,uint64_t* data0,uint64_t* data1, uint64_t T) const;
  std::string to_string(bool print_zeros, std::string indent_string) const {return "";}
  std::string to_string(uint64_t offset, bool print_zeros, std::string indent_string) const {return "";}
  TmSchConfig *sch_cfg = nullptr;
};

// Q Exc Rate
class tm_sch_q_exc_rate_static_mem : public model_core::RegisterBlockIndirect<RegisterCallback> {
 public:
  tm_sch_q_exc_rate_static_mem(int pipe_index, TmSchConfig *sch_cfg);
  uint64_t get_mem_offset(int pipe_index);
 private:
  bool write(uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T);
  bool read(uint64_t offset,uint64_t* data0,uint64_t* data1, uint64_t T) const;
  std::string to_string(bool print_zeros, std::string indent_string) const {return "";}
  std::string to_string(uint64_t offset, bool print_zeros, std::string indent_string) const {return "";}
  TmSchConfig *sch_cfg = nullptr;
};

class TmSchConfig : public TmObject {
 public:
  TmSchConfig(RmtObjectManager *om, uint8_t pipe_index);
  uint8_t get_pipe_index();
  void set_port_max_rate_config(uint8_t port_id, uint32_t value);
  void set_l1_max_rate_config(uint16_t l1_id, uint32_t value);
  void set_q_max_rate_config(uint16_t q_id, uint32_t value);
  void set_l1_min_rate_config(uint16_t l1_id, uint32_t value);
  void set_q_min_rate_config(uint16_t q_id, uint32_t value);
  void set_l1_exc_rate_config(uint16_t l1_id, uint32_t value);
  void set_q_exc_rate_config(uint16_t q_id, uint32_t value);

  register_classes::SchPortConfigRArrayMutable& get_port_config();
  register_classes::SchL1ConfigRArrayMutable& get_l1_config();
  register_classes::SchQueueConfigRArrayMutable& get_q_config();
  register_classes::SchByteCountAdjustRMutable& get_glb_bcnt_adj();

  // API to get memory config into logic
  sch_max_lb_static get_port_max_rate_config(uint8_t port_id);
  sch_max_lb_static get_l1_max_rate_config(uint16_t l1_id);
  sch_max_lb_static get_q_max_rate_config(uint16_t q_id);
  sch_min_lb_static get_l1_min_rate_config(uint16_t l1_id);
  sch_min_lb_static get_q_min_rate_config(uint16_t q_id);
  sch_exc_lb_static get_l1_exc_rate_config(uint16_t l1_id);
  sch_exc_lb_static get_q_exc_rate_config(uint16_t q_id);

  // API to send memory config to read
  uint32_t get_port_max_rate_config_rd(uint8_t port_id);
  uint32_t get_l1_max_rate_config_rd(uint16_t l1_id);
  uint32_t get_q_max_rate_config_rd(uint16_t q_id);
  uint32_t get_l1_min_rate_config_rd(uint16_t l1_id);
  uint32_t get_q_min_rate_config_rd(uint16_t q_id);
  uint32_t get_l1_exc_rate_config_rd(uint16_t l1_id);
  uint32_t get_q_exc_rate_config_rd(uint16_t q_id);
  bool prop_pri_for_queue(uint16_t q_id);

  // Classes to trap & store register write
  register_classes::SchPortConfigRArrayMutable sch_port_config_reg;
  register_classes::SchL1ConfigRArrayMutable sch_l1_config_reg;
  register_classes::SchQueueConfigRArrayMutable sch_q_config_reg;
  register_classes::SchByteCountAdjustRMutable sch_glb_bcnt_adj_reg;

 private:
  // Classes to trap memory write
  tm_sch_port_max_rate_static_mem sch_port_max_rate_static_mem;
  tm_sch_l1_max_rate_static_mem sch_l1_max_rate_static_mem;
  tm_sch_q_max_rate_static_mem sch_q_max_rate_static_mem;
  tm_sch_l1_min_rate_static_mem sch_l1_min_rate_static_mem;
  tm_sch_q_min_rate_static_mem sch_q_min_rate_static_mem;
  tm_sch_l1_exc_rate_static_mem sch_l1_exc_rate_static_mem;
  tm_sch_q_exc_rate_static_mem sch_q_exc_rate_static_mem;

  // Data Structures to store memory contents
  sch_port_max_lb_static_u prt_max_rate_cfg[TmDefs::kNumPortPerPipe];
  sch_l1_max_lb_static_u l1_max_rate_cfg[TmDefs::kNumL1PerPipe];
  sch_q_max_lb_static_u q_max_rate_cfg[TmDefs::kNumVqPerPipe];

  sch_l1_min_lb_static_u l1_min_rate_cfg[TmDefs::kNumL1PerPipe];
  sch_q_min_lb_static_u q_min_rate_cfg[TmDefs::kNumVqPerPipe];

  sch_l1_exc_lb_static_u l1_exc_rate_cfg[TmDefs::kNumL1PerPipe];
  sch_q_exc_lb_static_u q_exc_rate_cfg[TmDefs::kNumVqPerPipe];
};

}
#endif
