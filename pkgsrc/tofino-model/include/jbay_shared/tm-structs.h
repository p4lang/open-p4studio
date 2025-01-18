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

#ifndef __TM_STRUCTS__
#define __TM_STRUCTS__
struct sch_deq_info {
  uint8_t port;
  uint16_t l1;
  uint16_t qid;
  uint8_t min_pkt;
  uint8_t ecos;
};

struct pex_bupd_info {
  uint8_t port;
  uint16_t l1;
  uint16_t qid;
  uint16_t bytes;
  uint8_t min_pkt;
  uint8_t eop;
  uint8_t ecos;
};

struct pex_crd_info {
  uint8_t port;
  uint8_t ecos;
};

struct psc_pex_enq_info {
  uint8_t port;
  uint16_t l1;
  uint16_t qid;
  uint8_t min_pkt;
  uint16_t pkt_size;
  uint8_t ecos;
};

struct sch_port_config {
  uint32_t enb : 1;
  uint32_t pfc_upd_enb : 1;
  uint32_t max_rate_enb : 1;
  uint32_t pps : 1;
  uint32_t port_speed_mode : 3;
};

union sch_port_config_u {
  uint32_t u32;
  sch_port_config fields;
};

struct sch_l1_config {
  uint32_t enb : 1;
  uint32_t min_rate_enb : 1;
  uint32_t max_rate_enb : 1;
  uint32_t pps : 1;
  uint32_t l1_port : 3;
  uint32_t min_rate_pri : 3;
  uint32_t max_rate_pri : 3;
  uint32_t pri_prop : 1;
};

union sch_l1_config_u {
  uint32_t u32;
  sch_l1_config fields;
};

struct sch_q_config {
  uint32_t enb : 1;
  uint32_t pfc_upd_enb : 1;
  uint32_t min_rate_enb : 1;
  uint32_t max_rate_enb : 1;
  uint32_t l1_id : 5;
  uint32_t pfc_pri : 3;
  uint32_t min_rate_pri : 3;
  uint32_t max_rate_pri : 3;
  uint32_t adv_fc_mode : 1;
  uint32_t pps : 1;
};

union sch_q_config_u {
  uint32_t u32;
  sch_q_config fields;
};

struct sch_max_lb_static {
  uint32_t pps : 1;
  uint32_t bs_exp : 4;
  uint32_t bs_mant : 8;
  uint32_t rate_exp : 4;
  uint32_t rate_mant : 12;
  uint32_t reserved : 3;
};

struct sch_min_lb_static {
  uint32_t pps : 1;
  uint32_t bs_exp : 4;
  uint32_t bs_mant : 8;
  uint32_t rate_exp : 4;
  uint32_t rate_mant : 12;
  uint32_t reserved : 3;
};

struct sch_exc_lb_static {
  uint32_t pps : 1;
  uint32_t wt : 10;
};

union sch_port_max_lb_static_u {
  uint32_t u32;
  sch_max_lb_static fields;
};

union sch_l1_max_lb_static_u {
  uint32_t u32;
  sch_max_lb_static fields;
};

union sch_q_max_lb_static_u {
  uint32_t u32;
  sch_max_lb_static fields;
};

union sch_l1_min_lb_static_u {
  uint32_t u32;
  sch_min_lb_static fields;
};

union sch_q_min_lb_static_u {
  uint32_t u32;
  sch_min_lb_static fields;
};

union sch_l1_exc_lb_static_u {
  uint32_t u32;
  sch_exc_lb_static fields;
};

union sch_q_exc_lb_static_u {
  uint32_t u32;
  sch_exc_lb_static fields;
};

struct sch_glb_bcnt_adj {
  uint32_t value : 8;
  uint32_t l1_byte_adjust : 8;
};

union sch_glb_bcnt_adj_u {
  uint32_t u32;
  sch_glb_bcnt_adj fields;
};

#endif
