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

#include <iostream>
#include <tm-sch-config.h>
#include <register_adapters.h>
#include <mcn_test.h>
//#include <register_includes/mem.h>

namespace MODEL_CHIP_NAMESPACE {

TmSchConfig::TmSchConfig(RmtObjectManager *om, uint8_t pipe_index) :
  TmObject(om, pipe_index),
  sch_port_config_reg(tm_sch_adapter(sch_port_config_reg, 0, (pipe_index / 2), (pipe_index % 2))),
  sch_l1_config_reg(tm_sch_adapter(sch_l1_config_reg, 0, (pipe_index / 2), (pipe_index % 2))),
  sch_q_config_reg(tm_sch_adapter(sch_q_config_reg, 0, (pipe_index / 2), (pipe_index % 2))),
  sch_glb_bcnt_adj_reg(tm_sch_adapter(sch_glb_bcnt_adj_reg, 0, (pipe_index / 2), (pipe_index % 2))),
  sch_port_max_rate_static_mem(pipe_index, this),
  sch_l1_max_rate_static_mem(pipe_index, this),
  sch_q_max_rate_static_mem(pipe_index, this),
  sch_l1_min_rate_static_mem(pipe_index, this),
  sch_q_min_rate_static_mem(pipe_index, this),
  sch_l1_exc_rate_static_mem(pipe_index, this),
  sch_q_exc_rate_static_mem(pipe_index, this),
  prt_max_rate_cfg{{ }},
  l1_max_rate_cfg{{ }},
  q_max_rate_cfg{{ }},
  l1_min_rate_cfg{{ }},
  q_min_rate_cfg{{ }},
  l1_exc_rate_cfg{{ }},
  q_exc_rate_cfg{{ }}
{
  sch_port_config_reg.reset();
  sch_l1_config_reg.reset();
  sch_q_config_reg.reset();
  sch_glb_bcnt_adj_reg.reset();
}

void TmSchConfig::set_port_max_rate_config(uint8_t port_id, uint32_t value) {
  prt_max_rate_cfg[port_id].u32 = value;
}

void TmSchConfig::set_l1_max_rate_config(uint16_t l1_id, uint32_t value) {
  l1_max_rate_cfg[l1_id].u32 = value;
}

void TmSchConfig::set_q_max_rate_config(uint16_t q_id, uint32_t value) {
  q_max_rate_cfg[q_id].u32 = value;
}

void TmSchConfig::set_l1_min_rate_config(uint16_t l1_id, uint32_t value) {
  l1_min_rate_cfg[l1_id].u32 = value;
}

void TmSchConfig::set_q_min_rate_config(uint16_t q_id, uint32_t value) {
  q_min_rate_cfg[q_id].u32 = value;
}

void TmSchConfig::set_l1_exc_rate_config(uint16_t l1_id, uint32_t value) {
  l1_exc_rate_cfg[l1_id].u32 = value;
}

void TmSchConfig::set_q_exc_rate_config(uint16_t q_id, uint32_t value) {
  q_exc_rate_cfg[q_id].u32 = value;
}

register_classes::SchPortConfigRArrayMutable& TmSchConfig::get_port_config() {
  return(sch_port_config_reg);
}

register_classes::SchL1ConfigRArrayMutable& TmSchConfig::get_l1_config() {
  return(sch_l1_config_reg);
}

register_classes::SchQueueConfigRArrayMutable& TmSchConfig::get_q_config() {
  return(sch_q_config_reg);
}

register_classes::SchByteCountAdjustRMutable& TmSchConfig::get_glb_bcnt_adj() {
  return(sch_glb_bcnt_adj_reg);
}

sch_max_lb_static TmSchConfig::get_port_max_rate_config(uint8_t port_id) {
  return(prt_max_rate_cfg[port_id].fields);
}

sch_max_lb_static TmSchConfig::get_l1_max_rate_config(uint16_t l1_id) {
  return(l1_max_rate_cfg[l1_id].fields);
}

sch_max_lb_static TmSchConfig::get_q_max_rate_config(uint16_t q_id) {
  return(q_max_rate_cfg[q_id].fields);
}

sch_min_lb_static TmSchConfig::get_l1_min_rate_config(uint16_t l1_id) {
  return(l1_min_rate_cfg[l1_id].fields);
}

sch_min_lb_static TmSchConfig::get_q_min_rate_config(uint16_t q_id) {
  return(q_min_rate_cfg[q_id].fields);
}

sch_exc_lb_static TmSchConfig::get_l1_exc_rate_config(uint16_t l1_id) {
  return(l1_exc_rate_cfg[l1_id].fields);
}

sch_exc_lb_static TmSchConfig::get_q_exc_rate_config(uint16_t q_id) {
  return(q_exc_rate_cfg[q_id].fields);
}

uint32_t TmSchConfig::get_port_max_rate_config_rd(uint8_t port_id) {
  return(prt_max_rate_cfg[port_id].u32);
}

uint32_t TmSchConfig::get_l1_max_rate_config_rd(uint16_t l1_id) {
  return(l1_max_rate_cfg[l1_id].u32);
}

uint32_t TmSchConfig::get_q_max_rate_config_rd(uint16_t q_id) {
  return(q_max_rate_cfg[q_id].u32);
}

uint32_t TmSchConfig::get_l1_min_rate_config_rd(uint16_t l1_id) {
  return(l1_min_rate_cfg[l1_id].u32);
}

uint32_t TmSchConfig::get_q_min_rate_config_rd(uint16_t q_id) {
  return(q_min_rate_cfg[q_id].u32);
}

uint32_t TmSchConfig::get_l1_exc_rate_config_rd(uint16_t l1_id) {
  return(l1_exc_rate_cfg[l1_id].u32);
}

uint32_t TmSchConfig::get_q_exc_rate_config_rd(uint16_t q_id) {
  return(q_exc_rate_cfg[q_id].u32);
}

bool TmSchConfig::prop_pri_for_queue(uint16_t q_id) {
  uint8_t mac_id;
  uint16_t l1_pipe_id;

  mac_id = (q_id / TmDefs::kNumVqPerMac);
  l1_pipe_id = (mac_id * TmDefs::kNumL1PerMac) + get_q_config().l1_id(q_id);
  return(get_l1_config().pri_prop(l1_pipe_id));
}

// Constructors for Memories
// Port Max Rate
uint64_t tm_sch_port_max_rate_static_mem::get_mem_offset(int pipe_index) {
  switch((pipe_index / 2)) {
  case 0: // SCHA, 16B alligned address
    return((BFN_MEM_TM_SCHA(port_max_lb_static_mem_entry_address) + ((pipe_index % 2) * BFN_MEM_TM_SCHA(array_element_size))) / 16);
  case 1: // SCHB, 16B alligned address
    return((BFN_MEM_TM_SCHB(port_max_lb_static_mem_entry_address) + ((pipe_index % 2) * BFN_MEM_TM_SCHB(array_element_size))) / 16);
  default: return(0);
  }
}


tm_sch_port_max_rate_static_mem::tm_sch_port_max_rate_static_mem(int pipe_index, TmSchConfig *sch_cfg) :
  RegisterBlockIndirect(0,
			get_mem_offset(pipe_index),
			BFN_MEM_TM_SCHA(port_max_lb_static_mem_entry_array_count),
			false,
			nullptr,
			nullptr,
			"SCH Port Max Static Memory"),
  sch_cfg(sch_cfg)
{
}

bool tm_sch_port_max_rate_static_mem::write(uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T) {
  int index = offset;
  sch_cfg->set_port_max_rate_config(index, (data0 & 0xffffffff));
  return true;
}

bool tm_sch_port_max_rate_static_mem::read(uint64_t offset, uint64_t *data0, uint64_t *data1, uint64_t T) const {
  int index = offset;
  uint64_t hi=0, lo=0;
  lo = sch_cfg->get_port_max_rate_config_rd(index);
  *data1 = hi;
  *data0 = lo;
  return true;
}

// L1 Max Rate
uint64_t tm_sch_l1_max_rate_static_mem::get_mem_offset(int pipe_index) {
  switch((pipe_index / 2)) {
  case 0: // SCHA, 16B alligned address
    return((BFN_MEM_TM_SCHA(l1_max_lb_static_mem_entry_address) + ((pipe_index % 2) * BFN_MEM_TM_SCHA(array_element_size))) / 16);
  case 1: // SCHB, 16B alligned address
    return((BFN_MEM_TM_SCHB(l1_max_lb_static_mem_entry_address) + ((pipe_index % 2) * BFN_MEM_TM_SCHB(array_element_size))) / 16);
  default: return(0);
  }
}

tm_sch_l1_max_rate_static_mem::tm_sch_l1_max_rate_static_mem(int pipe_index, TmSchConfig *sch_cfg) :
  RegisterBlockIndirect(0,
			get_mem_offset(pipe_index),
			BFN_MEM_TM_SCHA(l1_max_lb_static_mem_entry_array_count),
			false,
			nullptr,
			nullptr,
			"SCH L1 Max Static Memory"),
  sch_cfg(sch_cfg)
{
}

bool tm_sch_l1_max_rate_static_mem::write(uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T) {
  int index = offset;
  sch_cfg->set_l1_max_rate_config(index, (data0 & 0xffffffff));
  return true;
}

bool tm_sch_l1_max_rate_static_mem::read(uint64_t offset, uint64_t *data0, uint64_t *data1, uint64_t T) const {
  int index = offset;
  uint64_t hi=0, lo=0;
  lo = sch_cfg->get_l1_max_rate_config_rd(index);
  *data1 = hi;
  *data0 = lo;
  return true;
}

// Q Max Rate
uint64_t tm_sch_q_max_rate_static_mem::get_mem_offset(int pipe_index) {
  switch((pipe_index / 2)) {
  case 0: // SCHA, 16B alligned address
    return((BFN_MEM_TM_SCHA(q_max_lb_static_mem_entry_address) + ((pipe_index % 2) * BFN_MEM_TM_SCHA(array_element_size))) / 16);
  case 1: // SCHB, 16B alligned address
    return((BFN_MEM_TM_SCHB(q_max_lb_static_mem_entry_address) + ((pipe_index % 2) * BFN_MEM_TM_SCHB(array_element_size))) / 16);
  default: return(0);
  }
}

tm_sch_q_max_rate_static_mem::tm_sch_q_max_rate_static_mem(int pipe_index, TmSchConfig *sch_cfg) :
  RegisterBlockIndirect(0,
			get_mem_offset(pipe_index),
			BFN_MEM_TM_SCHA(q_max_lb_static_mem_entry_array_count),
			false,
			nullptr,
			nullptr,
			"SCH Q Max Static Memory"),
  sch_cfg(sch_cfg)
{
}

bool tm_sch_q_max_rate_static_mem::write(uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T) {
  int index = offset;
  sch_cfg->set_q_max_rate_config(index, (data0 & 0xffffffff));
  return true;
}

bool tm_sch_q_max_rate_static_mem::read(uint64_t offset, uint64_t *data0, uint64_t *data1, uint64_t T) const {
  int index = offset;
  uint64_t hi=0, lo=0;
  lo = sch_cfg->get_q_max_rate_config_rd(index);
  *data1 = hi;
  *data0 = lo;
  return true;
}

// L1 Min Rate
uint64_t tm_sch_l1_min_rate_static_mem::get_mem_offset(int pipe_index) {
  switch((pipe_index / 2)) {
  case 0: // SCHA, 16B alligned address
    return((BFN_MEM_TM_SCHA(l1_min_lb_static_mem_entry_address) + ((pipe_index % 2) * BFN_MEM_TM_SCHA(array_element_size))) / 16);
  case 1: // SCHB, 16B alligned address
    return((BFN_MEM_TM_SCHB(l1_min_lb_static_mem_entry_address) + ((pipe_index % 2) * BFN_MEM_TM_SCHB(array_element_size))) / 16);
  default: return(0);
  }
}

tm_sch_l1_min_rate_static_mem::tm_sch_l1_min_rate_static_mem(int pipe_index, TmSchConfig *sch_cfg) :
  RegisterBlockIndirect(0,
			get_mem_offset(pipe_index),
			BFN_MEM_TM_SCHA(l1_min_lb_static_mem_entry_array_count),
			false,
			nullptr,
			nullptr,
			"SCH L1 Min Static Memory"),
  sch_cfg(sch_cfg)
{
}

bool tm_sch_l1_min_rate_static_mem::write(uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T) {
  int index = offset;
  sch_cfg->set_l1_min_rate_config(index, (data0 & 0xffffffff));
  return true;
}

bool tm_sch_l1_min_rate_static_mem::read(uint64_t offset, uint64_t *data0, uint64_t *data1, uint64_t T) const {
  int index = offset;
  uint64_t hi=0, lo=0;
  lo = sch_cfg->get_l1_min_rate_config_rd(index);
  *data1 = hi;
  *data0 = lo;
  return true;
}

// Q Min Rate
uint64_t tm_sch_q_min_rate_static_mem::get_mem_offset(int pipe_index) {
  switch((pipe_index / 2)) {
  case 0: // SCHA, 16B alligned address
    return((BFN_MEM_TM_SCHA(q_min_lb_static_mem_entry_address) + ((pipe_index % 2) * BFN_MEM_TM_SCHA(array_element_size))) / 16);
  case 1: // SCHB, 16B alligned address
    return((BFN_MEM_TM_SCHB(q_min_lb_static_mem_entry_address) + ((pipe_index % 2) * BFN_MEM_TM_SCHB(array_element_size))) / 16);
  default: return(0);
  }
}

tm_sch_q_min_rate_static_mem::tm_sch_q_min_rate_static_mem(int pipe_index, TmSchConfig *sch_cfg) :
  RegisterBlockIndirect(0,
			get_mem_offset(pipe_index),
			BFN_MEM_TM_SCHA(q_min_lb_static_mem_entry_array_count),
			false,
			nullptr,
			nullptr,
			"SCH Q Min Static Memory"),
  sch_cfg(sch_cfg)
{
}

bool tm_sch_q_min_rate_static_mem::write(uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T) {
  int index = offset;
  sch_cfg->set_q_min_rate_config(index, (data0 & 0xffffffff));
  return true;
}

bool tm_sch_q_min_rate_static_mem::read(uint64_t offset, uint64_t *data0, uint64_t *data1, uint64_t T) const {
  int index = offset;
  uint64_t hi=0, lo=0;
  lo = sch_cfg->get_q_min_rate_config_rd(index);
  *data1 = hi;
  *data0 = lo;
  return true;
}

// L1 Exc Rate
uint64_t tm_sch_l1_exc_rate_static_mem::get_mem_offset(int pipe_index) {
  switch((pipe_index / 2)) {
  case 0: // SCHA, 16B alligned address
    return((BFN_MEM_TM_SCHA(l1_exc_static_mem_entry_address) + ((pipe_index % 2) * BFN_MEM_TM_SCHA(array_element_size))) / 16);
  case 1: // SCHB, 16B alligned address
    return((BFN_MEM_TM_SCHB(l1_exc_static_mem_entry_address) + ((pipe_index % 2) * BFN_MEM_TM_SCHB(array_element_size))) / 16);
  default: return(0);
  }
}

tm_sch_l1_exc_rate_static_mem::tm_sch_l1_exc_rate_static_mem(int pipe_index, TmSchConfig *sch_cfg) :
  RegisterBlockIndirect(0,
			get_mem_offset(pipe_index),
			BFN_MEM_TM_SCHA(l1_exc_static_mem_entry_array_count),
			false,
			nullptr,
			nullptr,
			"SCH L1 Exc Static Memory"),
  sch_cfg(sch_cfg)
{
}

bool tm_sch_l1_exc_rate_static_mem::write(uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T) {
  int index = offset;
  sch_cfg->set_l1_exc_rate_config(index, (data0 & 0xffffffff));
  return true;
}

bool tm_sch_l1_exc_rate_static_mem::read(uint64_t offset, uint64_t *data0, uint64_t *data1, uint64_t T) const {
  int index = offset;
  uint64_t hi=0, lo=0;
  lo = sch_cfg->get_l1_exc_rate_config_rd(index);
  *data1 = hi;
  *data0 = lo;
  return true;
}

// Q Exc Rate
uint64_t tm_sch_q_exc_rate_static_mem::get_mem_offset(int pipe_index) {
  switch((pipe_index / 2)) {
  case 0: // SCHA, 16B alligned address
    return((BFN_MEM_TM_SCHA(q_exc_static_mem_entry_address) + ((pipe_index % 2) * BFN_MEM_TM_SCHA(array_element_size))) / 16);
  case 1: // SCHB, 16B alligned address
    return((BFN_MEM_TM_SCHB(q_exc_static_mem_entry_address) + ((pipe_index % 2) * BFN_MEM_TM_SCHB(array_element_size))) / 16);
  default: return(0);
  }
}

tm_sch_q_exc_rate_static_mem::tm_sch_q_exc_rate_static_mem(int pipe_index, TmSchConfig *sch_cfg) :
  RegisterBlockIndirect(0,
			get_mem_offset(pipe_index),
			BFN_MEM_TM_SCHA(q_exc_static_mem_entry_array_count),
			false,
			nullptr,
			nullptr,
			"SCH Q Exc Static Memory"),
  sch_cfg(sch_cfg)
{
}

bool tm_sch_q_exc_rate_static_mem::write(uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T) {
  int index = offset;
  sch_cfg->set_q_exc_rate_config(index, (data0 & 0xffffffff));
  return true;
}

bool tm_sch_q_exc_rate_static_mem::read(uint64_t offset, uint64_t *data0, uint64_t *data1, uint64_t T) const {
  int index = offset;
  uint64_t hi=0, lo=0;
  lo = sch_cfg->get_q_exc_rate_config_rd(index);
  *data1 = hi;
  *data0 = lo;
  return true;
}

}
