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


#ifndef __KERNEL__
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#else
#include <bf_types/bf_types.h>
#endif
#include "lld.h"
#include "lld_log.h"
#include "lld_map.h"
#include "lld_dev.h"
#include <tofino_regs/tofino.h>
#include <lld/lld_reg_if.h>
#include <lld/lld_sku.h>
#include "lld_tof_interrupt.h"
#include <lld/tofino_defs.h>
#include "lld_memory_mapping.h"
#ifndef __KERNEL__
#include <diag/bf_dev_diag.h>
#include <lld/lld_diag_ext.h>
#endif

// for telnet access
extern void *rl_outstream;

extern int get_reg_num_fields(bf_dev_id_t dev_id, uint32_t offset);

void process_host_ints(bf_dev_id_t dev_id, uint32_t sh_ints);
void process_tbus_ints(bf_dev_id_t dev_id, uint32_t sh_ints);
void process_cbus_ints(bf_dev_id_t dev_id, uint32_t sh_ints, int n);
void process_pbus_ints(bf_dev_id_t dev_id,
                       uint32_t sh_ints,
                       int n,
                       int all_ints);
void process_mbus_ints(bf_dev_id_t dev_id, uint32_t sh_ints, int n);

typedef enum {
  lld_tof_shadow_reg_host_bus_0 = 0,
  lld_tof_shadow_reg_tbus_1,
  lld_tof_shadow_reg_cbus_2,
  lld_tof_shadow_reg_cbus_3,
  lld_tof_shadow_reg_pbus_4,
  lld_tof_shadow_reg_pbus_5,
  lld_tof_shadow_reg_pbus_6,
  lld_tof_shadow_reg_pbus_7,
  lld_tof_shadow_reg_mbus_8,
  lld_tof_shadow_reg_mbus_9,
  lld_tof_shadow_reg_mbus_10,
  lld_tof_shadow_reg_mbus_11,
  lld_tof_shadow_reg_mbus_12,
  lld_tof_shadow_reg_mbus_13,
  lld_tof_shadow_reg_mbus_14,
  lld_tof_shadow_reg_mbus_15,
} lld_tof_shadow_reg_bus_t;

typedef struct lld_blk_lvl_int_s {
  uint32_t status_reg;
  uint32_t enable_hi_reg;
  uint32_t enable_lo_reg;
  uint32_t inject_reg;
  uint32_t shadow_mask;
  lld_int_cb cb_fn[BF_MAX_DEV_COUNT];
  void *userdata[BF_MAX_DEV_COUNT];
  uint32_t count[BF_MAX_DEV_COUNT][32];
  uint32_t count_shown[BF_MAX_DEV_COUNT][32];
} lld_blk_lvl_int_t;

lld_blk_lvl_int_t host_blk_lvl_int[] = {
    {DEF_tofino_device_select_pcie_bar01_regs_pcie_regs_pcie_int_stat_address,
     DEF_tofino_device_select_pcie_bar01_regs_pcie_regs_pcie_int_en_address,
     -1,
     DEF_tofino_device_select_pcie_bar01_regs_pcie_regs_pcie_int_inj_address,
     0x001fffff,
     {0},
     {0},
     {{0}},
     {{0}}},
    {DEF_tofino_device_select_misc_regs_int_stat_address,
     DEF_tofino_device_select_misc_regs_int_en_address,
     -1,
     DEF_tofino_device_select_misc_regs_int_inj_address,
     0x03000000,
     {0},
     {0},
     {{0}},
     {{0}}},
};

#define IBP_BLK_INT(mask, pipe, inst)                                                 \
  {DEF_tofino_pipes_pmarb_ibp18_reg_ibp_reg_ing_buf_regs_glb_group_int_stat_address + \
       pipe * DEF_tofino_pipes_array_element_size +                                   \
       inst * DEF_tofino_pipes_pmarb_ibp18_reg_ibp_reg_array_element_size,            \
   DEF_tofino_pipes_pmarb_ibp18_reg_ibp_reg_ing_buf_regs_glb_group_int_en_address +   \
       pipe * DEF_tofino_pipes_array_element_size +                                   \
       inst * DEF_tofino_pipes_pmarb_ibp18_reg_ibp_reg_array_element_size,            \
   -1,                                                                                \
   DEF_tofino_pipes_pmarb_ibp18_reg_ibp_reg_ing_buf_regs_glb_group_int_inj_address +  \
       pipe * DEF_tofino_pipes_array_element_size +                                   \
       inst * DEF_tofino_pipes_pmarb_ibp18_reg_ibp_reg_array_element_size,            \
   mask,                                                                              \
   {0},                                                                               \
   {0},                                                                               \
   {{0}},                                                                             \
   {{0}}},                                                                            \
                                                                                      \
      {DEF_tofino_pipes_pmarb_ibp18_reg_ibp_reg_prsr_reg_intr_status_address +        \
           pipe * DEF_tofino_pipes_array_element_size +                               \
           inst * DEF_tofino_pipes_pmarb_ibp18_reg_ibp_reg_array_element_size,        \
       DEF_tofino_pipes_pmarb_ibp18_reg_ibp_reg_prsr_reg_intr_enable0_address +       \
           pipe * DEF_tofino_pipes_array_element_size +                               \
           inst * DEF_tofino_pipes_pmarb_ibp18_reg_ibp_reg_array_element_size,        \
       DEF_tofino_pipes_pmarb_ibp18_reg_ibp_reg_prsr_reg_intr_enable1_address +       \
           pipe * DEF_tofino_pipes_array_element_size +                               \
           inst * DEF_tofino_pipes_pmarb_ibp18_reg_ibp_reg_array_element_size,        \
       DEF_tofino_pipes_pmarb_ibp18_reg_ibp_reg_prsr_reg_intr_inject_address +        \
           pipe * DEF_tofino_pipes_array_element_size +                               \
           inst * DEF_tofino_pipes_pmarb_ibp18_reg_ibp_reg_array_element_size,        \
       mask,                                                                          \
       {0},                                                                           \
       {0},                                                                           \
       {{0}},                                                                         \
       {{0}}},

lld_blk_lvl_int_t pipe0_pbus_blk_lvl_int_ibp[] = {
    IBP_BLK_INT(0x00000001, 0, 0) IBP_BLK_INT(0x00000002, 0, 1)
        IBP_BLK_INT(0x00000004, 0, 2) IBP_BLK_INT(0x00000008, 0, 3)
            IBP_BLK_INT(0x00000010, 0, 4) IBP_BLK_INT(0x00000020, 0, 5)
                IBP_BLK_INT(0x00000040, 0, 6) IBP_BLK_INT(0x00000080, 0, 7)
                    IBP_BLK_INT(0x00000100, 0, 8) IBP_BLK_INT(0x00000200, 0, 9)
                        IBP_BLK_INT(0x00000400, 0, 10) IBP_BLK_INT(
                            0x00000800, 0, 11) IBP_BLK_INT(0x00001000, 0, 12)
                            IBP_BLK_INT(0x00002000, 0, 13)
                                IBP_BLK_INT(0x00004000, 0, 14)
                                    IBP_BLK_INT(0x00008000, 0, 15)
                                        IBP_BLK_INT(0x00010000, 0, 16)
                                            IBP_BLK_INT(0x00020000, 0, 17)};

lld_blk_lvl_int_t pipe1_pbus_blk_lvl_int_ibp[] = {
    IBP_BLK_INT(0x00000001, 1, 0) IBP_BLK_INT(0x00000002, 1, 1)
        IBP_BLK_INT(0x00000004, 1, 2) IBP_BLK_INT(0x00000008, 1, 3)
            IBP_BLK_INT(0x00000010, 1, 4) IBP_BLK_INT(0x00000020, 1, 5)
                IBP_BLK_INT(0x00000040, 1, 6) IBP_BLK_INT(0x00000080, 1, 7)
                    IBP_BLK_INT(0x00000100, 1, 8) IBP_BLK_INT(0x00000200, 1, 9)
                        IBP_BLK_INT(0x00000400, 1, 10) IBP_BLK_INT(
                            0x00000800, 1, 11) IBP_BLK_INT(0x00001000, 1, 12)
                            IBP_BLK_INT(0x00002000, 1, 13)
                                IBP_BLK_INT(0x00004000, 1, 14)
                                    IBP_BLK_INT(0x00008000, 1, 15)
                                        IBP_BLK_INT(0x00010000, 1, 16)
                                            IBP_BLK_INT(0x00020000, 1, 17)};

lld_blk_lvl_int_t pipe2_pbus_blk_lvl_int_ibp[] = {
    IBP_BLK_INT(0x00000001, 2, 0) IBP_BLK_INT(0x00000002, 2, 1)
        IBP_BLK_INT(0x00000004, 2, 2) IBP_BLK_INT(0x00000008, 2, 3)
            IBP_BLK_INT(0x00000010, 2, 4) IBP_BLK_INT(0x00000020, 2, 5)
                IBP_BLK_INT(0x00000040, 2, 6) IBP_BLK_INT(0x00000080, 2, 7)
                    IBP_BLK_INT(0x00000100, 2, 8) IBP_BLK_INT(0x00000200, 2, 9)
                        IBP_BLK_INT(0x00000400, 2, 10) IBP_BLK_INT(
                            0x00000800, 2, 11) IBP_BLK_INT(0x00001000, 2, 12)
                            IBP_BLK_INT(0x00002000, 2, 13)
                                IBP_BLK_INT(0x00004000, 2, 14)
                                    IBP_BLK_INT(0x00008000, 2, 15)
                                        IBP_BLK_INT(0x00010000, 2, 16)
                                            IBP_BLK_INT(0x00020000, 2, 17)};

lld_blk_lvl_int_t pipe3_pbus_blk_lvl_int_ibp[] = {
    IBP_BLK_INT(0x00000001, 3, 0) IBP_BLK_INT(0x00000002, 3, 1)
        IBP_BLK_INT(0x00000004, 3, 2) IBP_BLK_INT(0x00000008, 3, 3)
            IBP_BLK_INT(0x00000010, 3, 4) IBP_BLK_INT(0x00000020, 3, 5)
                IBP_BLK_INT(0x00000040, 3, 6) IBP_BLK_INT(0x00000080, 3, 7)
                    IBP_BLK_INT(0x00000100, 3, 8) IBP_BLK_INT(0x00000200, 3, 9)
                        IBP_BLK_INT(0x00000400, 3, 10) IBP_BLK_INT(
                            0x00000800, 3, 11) IBP_BLK_INT(0x00001000, 3, 12)
                            IBP_BLK_INT(0x00002000, 3, 13)
                                IBP_BLK_INT(0x00004000, 3, 14)
                                    IBP_BLK_INT(0x00008000, 3, 15)
                                        IBP_BLK_INT(0x00010000, 3, 16)
                                            IBP_BLK_INT(0x00020000, 3, 17)};

// lld_blk_lvl_int_t pbus_blk_lvl_int_parb[] = {
//};

// lld_blk_lvl_int_t pbus_blk_lvl_int_prsmrg[] = {
//};

#define PBUSSTAT_INT(pipe)                                    \
  {DEF_tofino_pipes_pmarb_pbusstat_reg_intr_status_address +  \
       pipe * DEF_tofino_pipes_array_element_size,            \
   DEF_tofino_pipes_pmarb_pbusstat_reg_intr_enable0_address + \
       pipe * DEF_tofino_pipes_array_element_size,            \
   DEF_tofino_pipes_pmarb_pbusstat_reg_intr_enable1_address + \
       pipe * DEF_tofino_pipes_array_element_size,            \
   DEF_tofino_pipes_pmarb_pbusstat_reg_intr_inject_address +  \
       pipe * DEF_tofino_pipes_array_element_size,            \
   0xffffffff,                                                \
   {0},                                                       \
   {0},                                                       \
   {{0}},                                                     \
   {{0}}},

lld_blk_lvl_int_t pipe0_pbus_blk_lvl_int_pbusstat[] = {PBUSSTAT_INT(0)};

lld_blk_lvl_int_t pipe1_pbus_blk_lvl_int_pbusstat[] = {PBUSSTAT_INT(1)};

lld_blk_lvl_int_t pipe2_pbus_blk_lvl_int_pbusstat[] = {PBUSSTAT_INT(2)};

lld_blk_lvl_int_t pipe3_pbus_blk_lvl_int_pbusstat[] = {PBUSSTAT_INT(3)};

#define PGR_INT(pipe)                                           \
  {DEF_tofino_pipes_pmarb_pgr_reg_pgr_common_int_stat_address + \
       pipe * DEF_tofino_pipes_array_element_size,              \
   DEF_tofino_pipes_pmarb_pgr_reg_pgr_common_int_en0_address +  \
       pipe * DEF_tofino_pipes_array_element_size,              \
   DEF_tofino_pipes_pmarb_pgr_reg_pgr_common_int_en1_address +  \
       pipe * DEF_tofino_pipes_array_element_size,              \
   DEF_tofino_pipes_pmarb_pgr_reg_pgr_common_int_inj_address +  \
       pipe * DEF_tofino_pipes_array_element_size,              \
   0xffffffff,                                                  \
   {0},                                                         \
   {0},                                                         \
   {{0}},                                                       \
   {{0}}},

lld_blk_lvl_int_t pipe0_pbus_blk_lvl_int_pgr[] = {PGR_INT(0)};

lld_blk_lvl_int_t pipe1_pbus_blk_lvl_int_pgr[] = {PGR_INT(1)};

lld_blk_lvl_int_t pipe2_pbus_blk_lvl_int_pgr[] = {PGR_INT(2)};

lld_blk_lvl_int_t pipe3_pbus_blk_lvl_int_pgr[] = {PGR_INT(3)};

// lld_blk_lvl_int_t pbus_blk_lvl_int_party_glue[] = {
//};

#define EBP_BLK_INT(mask, pipe, inst)                                                 \
  {DEF_tofino_pipes_pmarb_ebp18_reg_ebp_reg_prsr_reg_intr_status_address +            \
       pipe * DEF_tofino_pipes_array_element_size +                                   \
       inst * DEF_tofino_pipes_pmarb_ebp18_reg_ebp_reg_array_element_size,            \
   DEF_tofino_pipes_pmarb_ebp18_reg_ebp_reg_prsr_reg_intr_enable0_address +           \
       pipe * DEF_tofino_pipes_array_element_size +                                   \
       inst * DEF_tofino_pipes_pmarb_ebp18_reg_ebp_reg_array_element_size,            \
   DEF_tofino_pipes_pmarb_ebp18_reg_ebp_reg_prsr_reg_intr_enable1_address +           \
       pipe * DEF_tofino_pipes_array_element_size +                                   \
       inst * DEF_tofino_pipes_pmarb_ebp18_reg_ebp_reg_array_element_size,            \
   DEF_tofino_pipes_pmarb_ebp18_reg_ebp_reg_prsr_reg_intr_inject_address +            \
       pipe * DEF_tofino_pipes_array_element_size +                                   \
       inst * DEF_tofino_pipes_pmarb_ebp18_reg_ebp_reg_array_element_size,            \
   mask,                                                                              \
   {0},                                                                               \
   {0},                                                                               \
   {{0}},                                                                             \
   {{0}}},                                                                            \
                                                                                      \
      {DEF_tofino_pipes_pmarb_ebp18_reg_ebp_reg_epb_prsr_port_regs_int_stat_address + \
           pipe * DEF_tofino_pipes_array_element_size +                               \
           inst * DEF_tofino_pipes_pmarb_ebp18_reg_ebp_reg_array_element_size,        \
       DEF_tofino_pipes_pmarb_ebp18_reg_ebp_reg_epb_prsr_port_regs_int_en_address +   \
           pipe * DEF_tofino_pipes_array_element_size +                               \
           inst * DEF_tofino_pipes_pmarb_ebp18_reg_ebp_reg_array_element_size,        \
       -1,                                                                            \
       DEF_tofino_pipes_pmarb_ebp18_reg_ebp_reg_epb_prsr_port_regs_int_inj_address +  \
           pipe * DEF_tofino_pipes_array_element_size +                               \
           inst * DEF_tofino_pipes_pmarb_ebp18_reg_ebp_reg_array_element_size,        \
       mask,                                                                          \
       {0},                                                                           \
       {0},                                                                           \
       {{0}},                                                                         \
       {{0}}},

lld_blk_lvl_int_t pipe0_pbus_blk_lvl_int_ebp[] = {
    EBP_BLK_INT(0x00000001, 0, 0) EBP_BLK_INT(0x00000002, 0, 1)
        EBP_BLK_INT(0x00000004, 0, 2) EBP_BLK_INT(0x00000008, 0, 3)
            EBP_BLK_INT(0x00000010, 0, 4) EBP_BLK_INT(0x00000020, 0, 5)
                EBP_BLK_INT(0x00000040, 0, 6) EBP_BLK_INT(0x00000080, 0, 7)
                    EBP_BLK_INT(0x00000100, 0, 8) EBP_BLK_INT(0x00000200, 0, 9)
                        EBP_BLK_INT(0x00000400, 0, 10) EBP_BLK_INT(
                            0x00000800, 0, 11) EBP_BLK_INT(0x00001000, 0, 12)
                            EBP_BLK_INT(0x00002000, 0, 13)
                                EBP_BLK_INT(0x00004000, 0, 14)
                                    EBP_BLK_INT(0x00008000, 0, 15)
                                        EBP_BLK_INT(0x00010000, 0, 16)
                                            EBP_BLK_INT(0x00020000, 0, 17)};

lld_blk_lvl_int_t pipe1_pbus_blk_lvl_int_ebp[] = {
    EBP_BLK_INT(0x00000001, 1, 0) EBP_BLK_INT(0x00000002, 1, 1)
        EBP_BLK_INT(0x00000004, 1, 2) EBP_BLK_INT(0x00000008, 1, 3)
            EBP_BLK_INT(0x00000010, 1, 4) EBP_BLK_INT(0x00000020, 1, 5)
                EBP_BLK_INT(0x00000040, 1, 6) EBP_BLK_INT(0x00000080, 1, 7)
                    EBP_BLK_INT(0x00000100, 1, 8) EBP_BLK_INT(0x00000200, 1, 9)
                        EBP_BLK_INT(0x00000400, 1, 10) EBP_BLK_INT(
                            0x00000800, 1, 11) EBP_BLK_INT(0x00001000, 1, 12)
                            EBP_BLK_INT(0x00002000, 1, 13)
                                EBP_BLK_INT(0x00004000, 1, 14)
                                    EBP_BLK_INT(0x00008000, 1, 15)
                                        EBP_BLK_INT(0x00010000, 1, 16)
                                            EBP_BLK_INT(0x00020000, 1, 17)};

lld_blk_lvl_int_t pipe2_pbus_blk_lvl_int_ebp[] = {
    EBP_BLK_INT(0x00000001, 2, 0) EBP_BLK_INT(0x00000002, 2, 1)
        EBP_BLK_INT(0x00000004, 2, 2) EBP_BLK_INT(0x00000008, 2, 3)
            EBP_BLK_INT(0x00000010, 2, 4) EBP_BLK_INT(0x00000020, 2, 5)
                EBP_BLK_INT(0x00000040, 2, 6) EBP_BLK_INT(0x00000080, 2, 7)
                    EBP_BLK_INT(0x00000100, 2, 8) EBP_BLK_INT(0x00000200, 2, 9)
                        EBP_BLK_INT(0x00000400, 2, 10) EBP_BLK_INT(
                            0x00000800, 2, 11) EBP_BLK_INT(0x00001000, 2, 12)
                            EBP_BLK_INT(0x00002000, 2, 13)
                                EBP_BLK_INT(0x00004000, 2, 14)
                                    EBP_BLK_INT(0x00008000, 2, 15)
                                        EBP_BLK_INT(0x00010000, 2, 16)
                                            EBP_BLK_INT(0x00020000, 2, 17)};

lld_blk_lvl_int_t pipe3_pbus_blk_lvl_int_ebp[] = {
    EBP_BLK_INT(0x00000001, 3, 0) EBP_BLK_INT(0x00000002, 3, 1)
        EBP_BLK_INT(0x00000004, 3, 2) EBP_BLK_INT(0x00000008, 3, 3)
            EBP_BLK_INT(0x00000010, 3, 4) EBP_BLK_INT(0x00000020, 3, 5)
                EBP_BLK_INT(0x00000040, 3, 6) EBP_BLK_INT(0x00000080, 3, 7)
                    EBP_BLK_INT(0x00000100, 3, 8) EBP_BLK_INT(0x00000200, 3, 9)
                        EBP_BLK_INT(0x00000400, 3, 10) EBP_BLK_INT(
                            0x00000800, 3, 11) EBP_BLK_INT(0x00001000, 3, 12)
                            EBP_BLK_INT(0x00002000, 3, 13)
                                EBP_BLK_INT(0x00004000, 3, 14)
                                    EBP_BLK_INT(0x00008000, 3, 15)
                                        EBP_BLK_INT(0x00010000, 3, 16)
                                            EBP_BLK_INT(0x00020000, 3, 17)};

#define EGRNX_BLK_INT(mask, pipe, inst)                                                 \
  {DEF_tofino_pipes_pmarb_ebp18_reg_egrNx_reg_ebuf_disp_regs_int_stat_address +         \
       pipe * DEF_tofino_pipes_array_element_size +                                     \
       inst * DEF_tofino_pipes_pmarb_ebp18_reg_egrNx_reg_array_element_size,            \
   DEF_tofino_pipes_pmarb_ebp18_reg_egrNx_reg_ebuf_disp_regs_int_en_address +           \
       pipe * DEF_tofino_pipes_array_element_size +                                     \
       inst * DEF_tofino_pipes_pmarb_ebp18_reg_egrNx_reg_array_element_size,            \
   -1,                                                                                  \
   DEF_tofino_pipes_pmarb_ebp18_reg_egrNx_reg_ebuf_disp_regs_int_inj_address +          \
       pipe * DEF_tofino_pipes_array_element_size +                                     \
       inst * DEF_tofino_pipes_pmarb_ebp18_reg_egrNx_reg_array_element_size,            \
   mask,                                                                                \
   {0},                                                                                 \
   {0},                                                                                 \
   {{0}},                                                                               \
   {{0}}},                                                                              \
                                                                                        \
      {DEF_tofino_pipes_pmarb_ebp18_reg_egrNx_reg_ebuf_fifo_regs_int_stat_address +     \
           pipe * DEF_tofino_pipes_array_element_size +                                 \
           inst *                                                                       \
               DEF_tofino_pipes_pmarb_ebp18_reg_egrNx_reg_array_element_size,           \
       DEF_tofino_pipes_pmarb_ebp18_reg_egrNx_reg_ebuf_fifo_regs_int_en_address +       \
           pipe * DEF_tofino_pipes_array_element_size +                                 \
           inst *                                                                       \
               DEF_tofino_pipes_pmarb_ebp18_reg_egrNx_reg_array_element_size,           \
       -1,                                                                              \
       DEF_tofino_pipes_pmarb_ebp18_reg_egrNx_reg_ebuf_fifo_regs_int_inj_address +      \
           pipe * DEF_tofino_pipes_array_element_size +                                 \
           inst *                                                                       \
               DEF_tofino_pipes_pmarb_ebp18_reg_egrNx_reg_array_element_size,           \
       mask,                                                                            \
       {0},                                                                             \
       {0},                                                                             \
       {{0}},                                                                           \
       {{0}}},                                                                          \
                                                                                        \
      {DEF_tofino_pipes_pmarb_ebp18_reg_egrNx_reg_epb_dprs_regs_int_stat_address +      \
           pipe * DEF_tofino_pipes_array_element_size +                                 \
           inst *                                                                       \
               DEF_tofino_pipes_pmarb_ebp18_reg_egrNx_reg_array_element_size,           \
       DEF_tofino_pipes_pmarb_ebp18_reg_egrNx_reg_epb_dprs_regs_int_en_address +        \
           pipe * DEF_tofino_pipes_array_element_size +                                 \
           inst *                                                                       \
               DEF_tofino_pipes_pmarb_ebp18_reg_egrNx_reg_array_element_size,           \
       -1,                                                                              \
       DEF_tofino_pipes_pmarb_ebp18_reg_egrNx_reg_epb_dprs_regs_int_inj_address +       \
           pipe * DEF_tofino_pipes_array_element_size +                                 \
           inst *                                                                       \
               DEF_tofino_pipes_pmarb_ebp18_reg_egrNx_reg_array_element_size,           \
       mask,                                                                            \
       {0},                                                                             \
       {0},                                                                             \
       {{0}},                                                                           \
       {{0}}},                                                                          \
                                                                                        \
      {DEF_tofino_pipes_pmarb_ebp18_reg_egrNx_reg_epb_disp_port_regs_int_stat_address + \
           pipe * DEF_tofino_pipes_array_element_size +                                 \
           inst *                                                                       \
               DEF_tofino_pipes_pmarb_ebp18_reg_egrNx_reg_array_element_size,           \
       DEF_tofino_pipes_pmarb_ebp18_reg_egrNx_reg_epb_disp_port_regs_int_en_address +   \
           pipe * DEF_tofino_pipes_array_element_size +                                 \
           inst *                                                                       \
               DEF_tofino_pipes_pmarb_ebp18_reg_egrNx_reg_array_element_size,           \
       -1,                                                                              \
       DEF_tofino_pipes_pmarb_ebp18_reg_egrNx_reg_epb_disp_port_regs_int_inj_address +  \
           pipe * DEF_tofino_pipes_array_element_size +                                 \
           inst *                                                                       \
               DEF_tofino_pipes_pmarb_ebp18_reg_egrNx_reg_array_element_size,           \
       mask,                                                                            \
       {0},                                                                             \
       {0},                                                                             \
       {{0}},                                                                           \
       {{0}}},

lld_blk_lvl_int_t pipe0_pbus_blk_lvl_int_egrNx[] = {
    EGRNX_BLK_INT(0x00000001, 0, 0) EGRNX_BLK_INT(0x00000002, 0, 1)
        EGRNX_BLK_INT(0x00000004, 0, 2) EGRNX_BLK_INT(0x00000008, 0, 3)
            EGRNX_BLK_INT(0x00000010, 0, 4) EGRNX_BLK_INT(0x00000020, 0, 5)
                EGRNX_BLK_INT(0x00000040, 0, 6) EGRNX_BLK_INT(0x00000080, 0, 7)
                    EGRNX_BLK_INT(0x00000100, 0, 8) EGRNX_BLK_INT(
                        0x00000200, 0, 9) EGRNX_BLK_INT(0x00000400, 0, 10)
                        EGRNX_BLK_INT(0x00000800, 0, 11) EGRNX_BLK_INT(
                            0x00001000, 0, 12) EGRNX_BLK_INT(0x00002000, 0, 13)
                            EGRNX_BLK_INT(0x00004000, 0, 14)
                                EGRNX_BLK_INT(0x00008000, 0, 15)
                                    EGRNX_BLK_INT(0x00010000, 0, 16)
                                        EGRNX_BLK_INT(0x00020000, 0, 17)};

lld_blk_lvl_int_t pipe1_pbus_blk_lvl_int_egrNx[] = {
    EGRNX_BLK_INT(0x00000001, 1, 0) EGRNX_BLK_INT(0x00000002, 1, 1)
        EGRNX_BLK_INT(0x00000004, 1, 2) EGRNX_BLK_INT(0x00000008, 1, 3)
            EGRNX_BLK_INT(0x00000010, 1, 4) EGRNX_BLK_INT(0x00000020, 1, 5)
                EGRNX_BLK_INT(0x00000040, 1, 6) EGRNX_BLK_INT(0x00000080, 1, 7)
                    EGRNX_BLK_INT(0x00000100, 1, 8) EGRNX_BLK_INT(
                        0x00000200, 1, 9) EGRNX_BLK_INT(0x00000400, 1, 10)
                        EGRNX_BLK_INT(0x00000800, 1, 11) EGRNX_BLK_INT(
                            0x00001000, 1, 12) EGRNX_BLK_INT(0x00002000, 1, 13)
                            EGRNX_BLK_INT(0x00004000, 1, 14)
                                EGRNX_BLK_INT(0x00008000, 1, 15)
                                    EGRNX_BLK_INT(0x00010000, 1, 16)
                                        EGRNX_BLK_INT(0x00020000, 1, 17)};

lld_blk_lvl_int_t pipe2_pbus_blk_lvl_int_egrNx[] = {
    EGRNX_BLK_INT(0x00000001, 2, 0) EGRNX_BLK_INT(0x00000002, 2, 1)
        EGRNX_BLK_INT(0x00000004, 2, 2) EGRNX_BLK_INT(0x00000008, 2, 3)
            EGRNX_BLK_INT(0x00000010, 2, 4) EGRNX_BLK_INT(0x00000020, 2, 5)
                EGRNX_BLK_INT(0x00000040, 2, 6) EGRNX_BLK_INT(0x00000080, 2, 7)
                    EGRNX_BLK_INT(0x00000100, 2, 8) EGRNX_BLK_INT(
                        0x00000200, 2, 9) EGRNX_BLK_INT(0x00000400, 2, 10)
                        EGRNX_BLK_INT(0x00000800, 2, 11) EGRNX_BLK_INT(
                            0x00001000, 2, 12) EGRNX_BLK_INT(0x00002000, 2, 13)
                            EGRNX_BLK_INT(0x00004000, 2, 14)
                                EGRNX_BLK_INT(0x00008000, 2, 15)
                                    EGRNX_BLK_INT(0x00010000, 2, 16)
                                        EGRNX_BLK_INT(0x00020000, 2, 17)};

lld_blk_lvl_int_t pipe3_pbus_blk_lvl_int_egrNx[] = {
    EGRNX_BLK_INT(0x00000001, 3, 0) EGRNX_BLK_INT(0x00000002, 3, 1)
        EGRNX_BLK_INT(0x00000004, 3, 2) EGRNX_BLK_INT(0x00000008, 3, 3)
            EGRNX_BLK_INT(0x00000010, 3, 4) EGRNX_BLK_INT(0x00000020, 3, 5)
                EGRNX_BLK_INT(0x00000040, 3, 6) EGRNX_BLK_INT(0x00000080, 3, 7)
                    EGRNX_BLK_INT(0x00000100, 3, 8) EGRNX_BLK_INT(
                        0x00000200, 3, 9) EGRNX_BLK_INT(0x00000400, 3, 10)
                        EGRNX_BLK_INT(0x00000800, 3, 11) EGRNX_BLK_INT(
                            0x00001000, 3, 12) EGRNX_BLK_INT(0x00002000, 3, 13)
                            EGRNX_BLK_INT(0x00004000, 3, 14)
                                EGRNX_BLK_INT(0x00008000, 3, 15)
                                    EGRNX_BLK_INT(0x00010000, 3, 16)
                                        EGRNX_BLK_INT(0x00020000, 3, 17)};

#define DPRSR_INT(pipe)                                                                       \
  {DEF_tofino_pipes_deparser_inp_icr_intr_status_address +                                    \
       pipe * DEF_tofino_pipes_array_element_size,                                            \
   DEF_tofino_pipes_deparser_inp_icr_intr_enable0_address +                                   \
       pipe * DEF_tofino_pipes_array_element_size,                                            \
   DEF_tofino_pipes_deparser_inp_icr_intr_enable1_address +                                   \
       pipe * DEF_tofino_pipes_array_element_size,                                            \
   DEF_tofino_pipes_deparser_inp_icr_intr_inject_address +                                    \
       pipe * DEF_tofino_pipes_array_element_size,                                            \
   0xffffffff,                                                                                \
   {0},                                                                                       \
   {0},                                                                                       \
   {{0}},                                                                                     \
   {{0}}},                                                                                    \
      {DEF_tofino_pipes_deparser_out_ingr_regs_intr_status_address +                          \
           pipe * DEF_tofino_pipes_array_element_size,                                        \
       DEF_tofino_pipes_deparser_out_ingr_regs_intr_enable0_address +                         \
           pipe * DEF_tofino_pipes_array_element_size,                                        \
       DEF_tofino_pipes_deparser_out_ingr_regs_intr_enable1_address +                         \
           pipe * DEF_tofino_pipes_array_element_size,                                        \
       DEF_tofino_pipes_deparser_out_ingr_regs_intr_inject_address +                          \
           pipe * DEF_tofino_pipes_array_element_size,                                        \
       0xffffffff,                                                                            \
       {0},                                                                                   \
       {0},                                                                                   \
       {{0}},                                                                                 \
       {{0}}},                                                                                \
      {DEF_tofino_pipes_deparser_out_egr_regs_intr_status_address +                           \
           pipe * DEF_tofino_pipes_array_element_size,                                        \
       DEF_tofino_pipes_deparser_out_egr_regs_intr_enable0_address +                          \
           pipe * DEF_tofino_pipes_array_element_size,                                        \
       DEF_tofino_pipes_deparser_out_egr_regs_intr_enable1_address +                          \
           pipe * DEF_tofino_pipes_array_element_size,                                        \
       DEF_tofino_pipes_deparser_out_egr_regs_intr_inject_address +                           \
           pipe * DEF_tofino_pipes_array_element_size,                                        \
       0xffffffff,                                                                            \
       {0},                                                                                   \
       {0},                                                                                   \
       {{0}},                                                                                 \
       {{0}}},                                                                                \
      {DEF_tofino_pipes_deparser_mirror_mir_buf_regs_mir_glb_group_mir_int_stat_address +     \
           pipe * DEF_tofino_pipes_array_element_size,                                        \
       DEF_tofino_pipes_deparser_mirror_mir_buf_regs_mir_glb_group_mir_int_en_address +       \
           pipe * DEF_tofino_pipes_array_element_size,                                        \
       -1,                                                                                    \
       DEF_tofino_pipes_deparser_mirror_mir_buf_regs_mir_glb_group_mir_int_dual_inj_address + \
           pipe * DEF_tofino_pipes_array_element_size,                                        \
       0xffffffff,                                                                            \
       {0},                                                                                   \
       {0},                                                                                   \
       {{0}},                                                                                 \
       {{0}}},                                                                                \
      {DEF_tofino_pipes_deparser_hdr_hir_ingr_intr_status_address +                           \
           pipe * DEF_tofino_pipes_array_element_size,                                        \
       DEF_tofino_pipes_deparser_hdr_hir_ingr_intr_enable0_address +                          \
           pipe * DEF_tofino_pipes_array_element_size,                                        \
       DEF_tofino_pipes_deparser_hdr_hir_ingr_intr_enable1_address +                          \
           pipe * DEF_tofino_pipes_array_element_size,                                        \
       DEF_tofino_pipes_deparser_hdr_hir_ingr_intr_inject_address +                           \
           pipe * DEF_tofino_pipes_array_element_size,                                        \
       0xffffffff,                                                                            \
       {0},                                                                                   \
       {0},                                                                                   \
       {{0}},                                                                                 \
       {{0}}},

lld_blk_lvl_int_t pipe0_pbus_blk_lvl_int_dprsr[] = {DPRSR_INT(0)};

lld_blk_lvl_int_t pipe1_pbus_blk_lvl_int_dprsr[] = {DPRSR_INT(1)};

lld_blk_lvl_int_t pipe2_pbus_blk_lvl_int_dprsr[] = {DPRSR_INT(2)};

lld_blk_lvl_int_t pipe3_pbus_blk_lvl_int_dprsr[] = {DPRSR_INT(3)};

lld_blk_lvl_int_t pbus_blk_lvl_int_pbc[] = {
    {DEF_tofino_device_select_pbc_pbc_pbus_int_stat0_address,
     DEF_tofino_device_select_pbc_pbc_pbus_int_en0_0_address,
     DEF_tofino_device_select_pbc_pbc_pbus_int_en0_1_address,
     DEF_tofino_device_select_pbc_pbc_pbus_int_inj_address,  // non-standard
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
    {DEF_tofino_device_select_pbc_pbc_pbus_int_stat1_address,
     DEF_tofino_device_select_pbc_pbc_pbus_int_en1_0_address,
     DEF_tofino_device_select_pbc_pbc_pbus_int_en1_1_address,
     DEF_tofino_device_select_pbc_pbc_pbus_int_inj_address,  // non-standard
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
    {DEF_tofino_device_select_pbc_pbc_pbus_int_stat2_address,
     DEF_tofino_device_select_pbc_pbc_pbus_int_en2_0_address,
     DEF_tofino_device_select_pbc_pbc_pbus_int_en2_1_address,
     DEF_tofino_device_select_pbc_pbc_pbus_int_inj_address,  // non-standard
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
    {DEF_tofino_device_select_pbc_pbc_pbus_int_stat3_address,
     DEF_tofino_device_select_pbc_pbc_pbus_int_en3_0_address,
     DEF_tofino_device_select_pbc_pbc_pbus_int_en3_1_address,
     DEF_tofino_device_select_pbc_pbc_pbus_int_inj_address,  // non-standard
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
};

lld_blk_lvl_int_t tbus_blk_lvl_int[] = {
    {DEF_tofino_device_select_tbc_tbc_tbus_int_stat0_address,
     DEF_tofino_device_select_tbc_tbc_tbus_int_en0_0_address,
     DEF_tofino_device_select_tbc_tbc_tbus_int_en0_1_address,
     DEF_tofino_device_select_tbc_tbc_tbus_int_inj_address,  // non-standard
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
    {DEF_tofino_device_select_tbc_tbc_tbus_int_stat1_address,
     DEF_tofino_device_select_tbc_tbc_tbus_int_en1_0_address,
     DEF_tofino_device_select_tbc_tbc_tbus_int_en1_1_address,
     DEF_tofino_device_select_tbc_tbc_tbus_int_inj_address,  // non-standard
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
    {DEF_tofino_device_select_tbc_tbc_tbus_int_stat2_address,
     DEF_tofino_device_select_tbc_tbc_tbus_int_en2_0_address,
     DEF_tofino_device_select_tbc_tbc_tbus_int_en2_1_address,
     DEF_tofino_device_select_tbc_tbc_tbus_int_inj_address,  // non-standard
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
};

lld_blk_lvl_int_t cbus_lfltr0_blk_lvl_int[] = {
    {DEF_tofino_device_select_lfltr0_ctrl_int_stat_address,
     DEF_tofino_device_select_lfltr0_ctrl_int_en0_address,
     DEF_tofino_device_select_lfltr0_ctrl_int_en1_address,
     DEF_tofino_device_select_lfltr0_ctrl_int_inj_address,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
};

lld_blk_lvl_int_t cbus_lfltr1_blk_lvl_int[] = {
    {DEF_tofino_device_select_lfltr1_ctrl_int_stat_address,
     DEF_tofino_device_select_lfltr1_ctrl_int_en0_address,
     DEF_tofino_device_select_lfltr1_ctrl_int_en1_address,
     DEF_tofino_device_select_lfltr1_ctrl_int_inj_address,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
};

lld_blk_lvl_int_t cbus_lfltr2_blk_lvl_int[] = {
    {DEF_tofino_device_select_lfltr2_ctrl_int_stat_address,
     DEF_tofino_device_select_lfltr2_ctrl_int_en0_address,
     DEF_tofino_device_select_lfltr2_ctrl_int_en1_address,
     DEF_tofino_device_select_lfltr2_ctrl_int_inj_address,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
};

lld_blk_lvl_int_t cbus_lfltr3_blk_lvl_int[] = {
    {DEF_tofino_device_select_lfltr3_ctrl_int_stat_address,
     DEF_tofino_device_select_lfltr3_ctrl_int_en0_address,
     DEF_tofino_device_select_lfltr3_ctrl_int_en1_address,
     DEF_tofino_device_select_lfltr3_ctrl_int_inj_address,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
};

lld_blk_lvl_int_t cbus_wac_blk_lvl_int[] = {
    {DEF_tofino_device_select_tm_top_tm_wac_top_wac_pipe_wac_reg_intr_status_address +
         0 * DEF_tofino_device_select_tm_top_tm_wac_top_wac_pipe_array_element_size,
     DEF_tofino_device_select_tm_top_tm_wac_top_wac_pipe_wac_reg_intr_enable0_address +
         0 * DEF_tofino_device_select_tm_top_tm_wac_top_wac_pipe_array_element_size,
     DEF_tofino_device_select_tm_top_tm_wac_top_wac_pipe_wac_reg_intr_enable1_address +
         0 * DEF_tofino_device_select_tm_top_tm_wac_top_wac_pipe_array_element_size,
     DEF_tofino_device_select_tm_top_tm_wac_top_wac_pipe_wac_reg_intr_inject_address +
         0 * DEF_tofino_device_select_tm_top_tm_wac_top_wac_pipe_array_element_size,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
    {DEF_tofino_device_select_tm_top_tm_wac_top_wac_pipe_wac_reg_intr_status_address +
         1 * DEF_tofino_device_select_tm_top_tm_wac_top_wac_pipe_array_element_size,
     DEF_tofino_device_select_tm_top_tm_wac_top_wac_pipe_wac_reg_intr_enable0_address +
         1 * DEF_tofino_device_select_tm_top_tm_wac_top_wac_pipe_array_element_size,
     DEF_tofino_device_select_tm_top_tm_wac_top_wac_pipe_wac_reg_intr_enable1_address +
         1 * DEF_tofino_device_select_tm_top_tm_wac_top_wac_pipe_array_element_size,
     DEF_tofino_device_select_tm_top_tm_wac_top_wac_pipe_wac_reg_intr_inject_address +
         1 * DEF_tofino_device_select_tm_top_tm_wac_top_wac_pipe_array_element_size,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
    {DEF_tofino_device_select_tm_top_tm_wac_top_wac_pipe_wac_reg_intr_status_address +
         2 * DEF_tofino_device_select_tm_top_tm_wac_top_wac_pipe_array_element_size,
     DEF_tofino_device_select_tm_top_tm_wac_top_wac_pipe_wac_reg_intr_enable0_address +
         2 * DEF_tofino_device_select_tm_top_tm_wac_top_wac_pipe_array_element_size,
     DEF_tofino_device_select_tm_top_tm_wac_top_wac_pipe_wac_reg_intr_enable1_address +
         2 * DEF_tofino_device_select_tm_top_tm_wac_top_wac_pipe_array_element_size,
     DEF_tofino_device_select_tm_top_tm_wac_top_wac_pipe_wac_reg_intr_inject_address +
         2 * DEF_tofino_device_select_tm_top_tm_wac_top_wac_pipe_array_element_size,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
    {DEF_tofino_device_select_tm_top_tm_wac_top_wac_pipe_wac_reg_intr_status_address +
         3 * DEF_tofino_device_select_tm_top_tm_wac_top_wac_pipe_array_element_size,
     DEF_tofino_device_select_tm_top_tm_wac_top_wac_pipe_wac_reg_intr_enable0_address +
         3 * DEF_tofino_device_select_tm_top_tm_wac_top_wac_pipe_array_element_size,
     DEF_tofino_device_select_tm_top_tm_wac_top_wac_pipe_wac_reg_intr_enable1_address +
         3 * DEF_tofino_device_select_tm_top_tm_wac_top_wac_pipe_array_element_size,
     DEF_tofino_device_select_tm_top_tm_wac_top_wac_pipe_wac_reg_intr_inject_address +
         3 * DEF_tofino_device_select_tm_top_tm_wac_top_wac_pipe_array_element_size,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
};

lld_blk_lvl_int_t cbus_caa_blk_lvl_int[] = {
    {DEF_tofino_device_select_tm_top_tm_caa_intr_status_address,
     DEF_tofino_device_select_tm_top_tm_caa_intr_enable0_address,
     DEF_tofino_device_select_tm_top_tm_caa_intr_enable1_address,
     DEF_tofino_device_select_tm_top_tm_caa_intr_inject_address,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
    {DEF_tofino_device_select_tm_top_tm_caa_cdm_intr_status_address,
     DEF_tofino_device_select_tm_top_tm_caa_cdm_intr_enable0_address,
     DEF_tofino_device_select_tm_top_tm_caa_cdm_intr_enable1_address,
     DEF_tofino_device_select_tm_top_tm_caa_cdm_intr_inject_address,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
};

lld_blk_lvl_int_t cbus_qac_blk_lvl_int[] = {
    {DEF_tofino_device_select_tm_top_tm_qac_top_qac_pipe_qac_reg_intr_status_address +
         0 * DEF_tofino_device_select_tm_top_tm_qac_top_qac_pipe_array_element_size,
     DEF_tofino_device_select_tm_top_tm_qac_top_qac_pipe_qac_reg_intr_enable0_address +
         0 * DEF_tofino_device_select_tm_top_tm_qac_top_qac_pipe_array_element_size,
     DEF_tofino_device_select_tm_top_tm_qac_top_qac_pipe_qac_reg_intr_enable1_address +
         0 * DEF_tofino_device_select_tm_top_tm_qac_top_qac_pipe_array_element_size,
     DEF_tofino_device_select_tm_top_tm_qac_top_qac_pipe_qac_reg_intr_inject_address +
         0 * DEF_tofino_device_select_tm_top_tm_qac_top_qac_pipe_array_element_size,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
    {DEF_tofino_device_select_tm_top_tm_qac_top_qac_pipe_qac_reg_intr_status_address +
         1 * DEF_tofino_device_select_tm_top_tm_qac_top_qac_pipe_array_element_size,
     DEF_tofino_device_select_tm_top_tm_qac_top_qac_pipe_qac_reg_intr_enable0_address +
         1 * DEF_tofino_device_select_tm_top_tm_qac_top_qac_pipe_array_element_size,
     DEF_tofino_device_select_tm_top_tm_qac_top_qac_pipe_qac_reg_intr_enable1_address +
         1 * DEF_tofino_device_select_tm_top_tm_qac_top_qac_pipe_array_element_size,
     DEF_tofino_device_select_tm_top_tm_qac_top_qac_pipe_qac_reg_intr_inject_address +
         1 * DEF_tofino_device_select_tm_top_tm_qac_top_qac_pipe_array_element_size,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
    {DEF_tofino_device_select_tm_top_tm_qac_top_qac_pipe_qac_reg_intr_status_address +
         2 * DEF_tofino_device_select_tm_top_tm_qac_top_qac_pipe_array_element_size,
     DEF_tofino_device_select_tm_top_tm_qac_top_qac_pipe_qac_reg_intr_enable0_address +
         2 * DEF_tofino_device_select_tm_top_tm_qac_top_qac_pipe_array_element_size,
     DEF_tofino_device_select_tm_top_tm_qac_top_qac_pipe_qac_reg_intr_enable1_address +
         2 * DEF_tofino_device_select_tm_top_tm_qac_top_qac_pipe_array_element_size,
     DEF_tofino_device_select_tm_top_tm_qac_top_qac_pipe_qac_reg_intr_inject_address +
         2 * DEF_tofino_device_select_tm_top_tm_qac_top_qac_pipe_array_element_size,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
    {DEF_tofino_device_select_tm_top_tm_qac_top_qac_pipe_qac_reg_intr_status_address +
         3 * DEF_tofino_device_select_tm_top_tm_qac_top_qac_pipe_array_element_size,
     DEF_tofino_device_select_tm_top_tm_qac_top_qac_pipe_qac_reg_intr_enable0_address +
         3 * DEF_tofino_device_select_tm_top_tm_qac_top_qac_pipe_array_element_size,
     DEF_tofino_device_select_tm_top_tm_qac_top_qac_pipe_qac_reg_intr_enable1_address +
         3 * DEF_tofino_device_select_tm_top_tm_qac_top_qac_pipe_array_element_size,
     DEF_tofino_device_select_tm_top_tm_qac_top_qac_pipe_qac_reg_intr_inject_address +
         3 * DEF_tofino_device_select_tm_top_tm_qac_top_qac_pipe_array_element_size,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
};

lld_blk_lvl_int_t cbus_sch_blk_lvl_int[] = {
    {DEF_tofino_device_select_tm_top_tm_sch_top_sch_intr_status_address +
         0 * DEF_tofino_device_select_tm_top_tm_sch_top_sch_array_element_size,
     DEF_tofino_device_select_tm_top_tm_sch_top_sch_intr_enable0_address +
         0 * DEF_tofino_device_select_tm_top_tm_sch_top_sch_array_element_size,
     DEF_tofino_device_select_tm_top_tm_sch_top_sch_intr_enable1_address +
         0 * DEF_tofino_device_select_tm_top_tm_sch_top_sch_array_element_size,
     DEF_tofino_device_select_tm_top_tm_sch_top_sch_intr_inject_address +
         0 * DEF_tofino_device_select_tm_top_tm_sch_top_sch_array_element_size,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
    {DEF_tofino_device_select_tm_top_tm_sch_top_sch_intr_status_address +
         1 * DEF_tofino_device_select_tm_top_tm_sch_top_sch_array_element_size,
     DEF_tofino_device_select_tm_top_tm_sch_top_sch_intr_enable0_address +
         1 * DEF_tofino_device_select_tm_top_tm_sch_top_sch_array_element_size,
     DEF_tofino_device_select_tm_top_tm_sch_top_sch_intr_enable1_address +
         1 * DEF_tofino_device_select_tm_top_tm_sch_top_sch_array_element_size,
     DEF_tofino_device_select_tm_top_tm_sch_top_sch_intr_inject_address +
         1 * DEF_tofino_device_select_tm_top_tm_sch_top_sch_array_element_size,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
    {DEF_tofino_device_select_tm_top_tm_sch_top_sch_intr_status_address +
         2 * DEF_tofino_device_select_tm_top_tm_sch_top_sch_array_element_size,
     DEF_tofino_device_select_tm_top_tm_sch_top_sch_intr_enable0_address +
         2 * DEF_tofino_device_select_tm_top_tm_sch_top_sch_array_element_size,
     DEF_tofino_device_select_tm_top_tm_sch_top_sch_intr_enable1_address +
         2 * DEF_tofino_device_select_tm_top_tm_sch_top_sch_array_element_size,
     DEF_tofino_device_select_tm_top_tm_sch_top_sch_intr_inject_address +
         2 * DEF_tofino_device_select_tm_top_tm_sch_top_sch_array_element_size,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
    {DEF_tofino_device_select_tm_top_tm_sch_top_sch_intr_status_address +
         3 * DEF_tofino_device_select_tm_top_tm_sch_top_sch_array_element_size,
     DEF_tofino_device_select_tm_top_tm_sch_top_sch_intr_enable0_address +
         3 * DEF_tofino_device_select_tm_top_tm_sch_top_sch_array_element_size,
     DEF_tofino_device_select_tm_top_tm_sch_top_sch_intr_enable1_address +
         3 * DEF_tofino_device_select_tm_top_tm_sch_top_sch_array_element_size,
     DEF_tofino_device_select_tm_top_tm_sch_top_sch_intr_inject_address +
         3 * DEF_tofino_device_select_tm_top_tm_sch_top_sch_array_element_size,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
};

lld_blk_lvl_int_t cbus_clc_blk_lvl_int[] = {
    {DEF_tofino_device_select_tm_top_tm_clc_top_clc_intr_status_address +
         0 * DEF_tofino_device_select_tm_top_tm_clc_top_clc_array_element_size,
     DEF_tofino_device_select_tm_top_tm_clc_top_clc_intr_enable0_address +
         0 * DEF_tofino_device_select_tm_top_tm_clc_top_clc_array_element_size,
     DEF_tofino_device_select_tm_top_tm_clc_top_clc_intr_enable1_address +
         0 * DEF_tofino_device_select_tm_top_tm_clc_top_clc_array_element_size,
     DEF_tofino_device_select_tm_top_tm_clc_top_clc_intr_inject_address +
         0 * DEF_tofino_device_select_tm_top_tm_clc_top_clc_array_element_size,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
    {DEF_tofino_device_select_tm_top_tm_clc_top_clc_intr_status_address +
         1 * DEF_tofino_device_select_tm_top_tm_clc_top_clc_array_element_size,
     DEF_tofino_device_select_tm_top_tm_clc_top_clc_intr_enable0_address +
         1 * DEF_tofino_device_select_tm_top_tm_clc_top_clc_array_element_size,
     DEF_tofino_device_select_tm_top_tm_clc_top_clc_intr_enable1_address +
         1 * DEF_tofino_device_select_tm_top_tm_clc_top_clc_array_element_size,
     DEF_tofino_device_select_tm_top_tm_clc_top_clc_intr_inject_address +
         1 * DEF_tofino_device_select_tm_top_tm_clc_top_clc_array_element_size,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
    {DEF_tofino_device_select_tm_top_tm_clc_top_clc_intr_status_address +
         2 * DEF_tofino_device_select_tm_top_tm_clc_top_clc_array_element_size,
     DEF_tofino_device_select_tm_top_tm_clc_top_clc_intr_enable0_address +
         2 * DEF_tofino_device_select_tm_top_tm_clc_top_clc_array_element_size,
     DEF_tofino_device_select_tm_top_tm_clc_top_clc_intr_enable1_address +
         2 * DEF_tofino_device_select_tm_top_tm_clc_top_clc_array_element_size,
     DEF_tofino_device_select_tm_top_tm_clc_top_clc_intr_inject_address +
         2 * DEF_tofino_device_select_tm_top_tm_clc_top_clc_array_element_size,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
    {DEF_tofino_device_select_tm_top_tm_clc_top_clc_intr_status_address +
         3 * DEF_tofino_device_select_tm_top_tm_clc_top_clc_array_element_size,
     DEF_tofino_device_select_tm_top_tm_clc_top_clc_intr_enable0_address +
         3 * DEF_tofino_device_select_tm_top_tm_clc_top_clc_array_element_size,
     DEF_tofino_device_select_tm_top_tm_clc_top_clc_intr_enable1_address +
         3 * DEF_tofino_device_select_tm_top_tm_clc_top_clc_array_element_size,
     DEF_tofino_device_select_tm_top_tm_clc_top_clc_intr_inject_address +
         3 * DEF_tofino_device_select_tm_top_tm_clc_top_clc_array_element_size,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
    {DEF_tofino_device_select_tm_top_tm_clc_top_pex_intr_status_address +
         0 * DEF_tofino_device_select_tm_top_tm_clc_top_pex_array_element_size,
     DEF_tofino_device_select_tm_top_tm_clc_top_pex_intr_enable0_address +
         0 * DEF_tofino_device_select_tm_top_tm_clc_top_pex_array_element_size,
     DEF_tofino_device_select_tm_top_tm_clc_top_pex_intr_enable1_address +
         0 * DEF_tofino_device_select_tm_top_tm_clc_top_pex_array_element_size,
     DEF_tofino_device_select_tm_top_tm_clc_top_pex_intr_inject_address +
         0 * DEF_tofino_device_select_tm_top_tm_clc_top_pex_array_element_size,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
    {DEF_tofino_device_select_tm_top_tm_clc_top_pex_intr_status_address +
         1 * DEF_tofino_device_select_tm_top_tm_clc_top_pex_array_element_size,
     DEF_tofino_device_select_tm_top_tm_clc_top_pex_intr_enable0_address +
         1 * DEF_tofino_device_select_tm_top_tm_clc_top_pex_array_element_size,
     DEF_tofino_device_select_tm_top_tm_clc_top_pex_intr_enable1_address +
         1 * DEF_tofino_device_select_tm_top_tm_clc_top_pex_array_element_size,
     DEF_tofino_device_select_tm_top_tm_clc_top_pex_intr_inject_address +
         1 * DEF_tofino_device_select_tm_top_tm_clc_top_pex_array_element_size,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
    {DEF_tofino_device_select_tm_top_tm_clc_top_pex_intr_status_address +
         2 * DEF_tofino_device_select_tm_top_tm_clc_top_pex_array_element_size,
     DEF_tofino_device_select_tm_top_tm_clc_top_pex_intr_enable0_address +
         2 * DEF_tofino_device_select_tm_top_tm_clc_top_pex_array_element_size,
     DEF_tofino_device_select_tm_top_tm_clc_top_pex_intr_enable1_address +
         2 * DEF_tofino_device_select_tm_top_tm_clc_top_pex_array_element_size,
     DEF_tofino_device_select_tm_top_tm_clc_top_pex_intr_inject_address +
         2 * DEF_tofino_device_select_tm_top_tm_clc_top_pex_array_element_size,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
    {DEF_tofino_device_select_tm_top_tm_clc_top_pex_intr_status_address +
         3 * DEF_tofino_device_select_tm_top_tm_clc_top_pex_array_element_size,
     DEF_tofino_device_select_tm_top_tm_clc_top_pex_intr_enable0_address +
         3 * DEF_tofino_device_select_tm_top_tm_clc_top_pex_array_element_size,
     DEF_tofino_device_select_tm_top_tm_clc_top_pex_intr_enable1_address +
         3 * DEF_tofino_device_select_tm_top_tm_clc_top_pex_array_element_size,
     DEF_tofino_device_select_tm_top_tm_clc_top_pex_intr_inject_address +
         3 * DEF_tofino_device_select_tm_top_tm_clc_top_pex_array_element_size,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
};

lld_blk_lvl_int_t cbus_qlc_blk_lvl_int[] = {
    {DEF_tofino_device_select_tm_top_tm_qlc_top_qlc_intr_status_address +
         0 * DEF_tofino_device_select_tm_top_tm_qlc_top_qlc_array_element_size,
     DEF_tofino_device_select_tm_top_tm_qlc_top_qlc_intr_enable0_address +
         0 * DEF_tofino_device_select_tm_top_tm_qlc_top_qlc_array_element_size,
     DEF_tofino_device_select_tm_top_tm_qlc_top_qlc_intr_enable1_address +
         0 * DEF_tofino_device_select_tm_top_tm_qlc_top_qlc_array_element_size,
     DEF_tofino_device_select_tm_top_tm_qlc_top_qlc_intr_inject_address +
         0 * DEF_tofino_device_select_tm_top_tm_qlc_top_qlc_array_element_size,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
    {DEF_tofino_device_select_tm_top_tm_qlc_top_qlc_intr_status_address +
         1 * DEF_tofino_device_select_tm_top_tm_qlc_top_qlc_array_element_size,
     DEF_tofino_device_select_tm_top_tm_qlc_top_qlc_intr_enable0_address +
         1 * DEF_tofino_device_select_tm_top_tm_qlc_top_qlc_array_element_size,
     DEF_tofino_device_select_tm_top_tm_qlc_top_qlc_intr_enable1_address +
         1 * DEF_tofino_device_select_tm_top_tm_qlc_top_qlc_array_element_size,
     DEF_tofino_device_select_tm_top_tm_qlc_top_qlc_intr_inject_address +
         1 * DEF_tofino_device_select_tm_top_tm_qlc_top_qlc_array_element_size,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
    {DEF_tofino_device_select_tm_top_tm_qlc_top_qlc_intr_status_address +
         2 * DEF_tofino_device_select_tm_top_tm_qlc_top_qlc_array_element_size,
     DEF_tofino_device_select_tm_top_tm_qlc_top_qlc_intr_enable0_address +
         2 * DEF_tofino_device_select_tm_top_tm_qlc_top_qlc_array_element_size,
     DEF_tofino_device_select_tm_top_tm_qlc_top_qlc_intr_enable1_address +
         2 * DEF_tofino_device_select_tm_top_tm_qlc_top_qlc_array_element_size,
     DEF_tofino_device_select_tm_top_tm_qlc_top_qlc_intr_inject_address +
         2 * DEF_tofino_device_select_tm_top_tm_qlc_top_qlc_array_element_size,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
    {DEF_tofino_device_select_tm_top_tm_qlc_top_qlc_intr_status_address +
         3 * DEF_tofino_device_select_tm_top_tm_qlc_top_qlc_array_element_size,
     DEF_tofino_device_select_tm_top_tm_qlc_top_qlc_intr_enable0_address +
         3 * DEF_tofino_device_select_tm_top_tm_qlc_top_qlc_array_element_size,
     DEF_tofino_device_select_tm_top_tm_qlc_top_qlc_intr_enable1_address +
         3 * DEF_tofino_device_select_tm_top_tm_qlc_top_qlc_array_element_size,
     DEF_tofino_device_select_tm_top_tm_qlc_top_qlc_intr_inject_address +
         3 * DEF_tofino_device_select_tm_top_tm_qlc_top_qlc_array_element_size,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
};

lld_blk_lvl_int_t cbus_prc_blk_lvl_int[] = {
    {DEF_tofino_device_select_tm_top_tm_prc_top_prc_intr_status_address +
         0 * DEF_tofino_device_select_tm_top_tm_prc_top_prc_array_element_size,
     DEF_tofino_device_select_tm_top_tm_prc_top_prc_intr_enable0_address +
         0 * DEF_tofino_device_select_tm_top_tm_prc_top_prc_array_element_size,
     DEF_tofino_device_select_tm_top_tm_prc_top_prc_intr_enable1_address +
         0 * DEF_tofino_device_select_tm_top_tm_prc_top_prc_array_element_size,
     DEF_tofino_device_select_tm_top_tm_prc_top_prc_intr_inject_address +
         0 * DEF_tofino_device_select_tm_top_tm_prc_top_prc_array_element_size,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
    {DEF_tofino_device_select_tm_top_tm_prc_top_prc_intr_status_address +
         1 * DEF_tofino_device_select_tm_top_tm_prc_top_prc_array_element_size,
     DEF_tofino_device_select_tm_top_tm_prc_top_prc_intr_enable0_address +
         1 * DEF_tofino_device_select_tm_top_tm_prc_top_prc_array_element_size,
     DEF_tofino_device_select_tm_top_tm_prc_top_prc_intr_enable1_address +
         1 * DEF_tofino_device_select_tm_top_tm_prc_top_prc_array_element_size,
     DEF_tofino_device_select_tm_top_tm_prc_top_prc_intr_inject_address +
         1 * DEF_tofino_device_select_tm_top_tm_prc_top_prc_array_element_size,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
    {DEF_tofino_device_select_tm_top_tm_prc_top_prc_intr_status_address +
         2 * DEF_tofino_device_select_tm_top_tm_prc_top_prc_array_element_size,
     DEF_tofino_device_select_tm_top_tm_prc_top_prc_intr_enable0_address +
         2 * DEF_tofino_device_select_tm_top_tm_prc_top_prc_array_element_size,
     DEF_tofino_device_select_tm_top_tm_prc_top_prc_intr_enable1_address +
         2 * DEF_tofino_device_select_tm_top_tm_prc_top_prc_array_element_size,
     DEF_tofino_device_select_tm_top_tm_prc_top_prc_intr_inject_address +
         2 * DEF_tofino_device_select_tm_top_tm_prc_top_prc_array_element_size,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
    {DEF_tofino_device_select_tm_top_tm_prc_top_prc_intr_status_address +
         3 * DEF_tofino_device_select_tm_top_tm_prc_top_prc_array_element_size,
     DEF_tofino_device_select_tm_top_tm_prc_top_prc_intr_enable0_address +
         3 * DEF_tofino_device_select_tm_top_tm_prc_top_prc_array_element_size,
     DEF_tofino_device_select_tm_top_tm_prc_top_prc_intr_enable1_address +
         3 * DEF_tofino_device_select_tm_top_tm_prc_top_prc_array_element_size,
     DEF_tofino_device_select_tm_top_tm_prc_top_prc_intr_inject_address +
         3 * DEF_tofino_device_select_tm_top_tm_prc_top_prc_array_element_size,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
};

lld_blk_lvl_int_t cbus_pre_blk_lvl_int[] = {
    {DEF_tofino_device_select_tm_top_tm_pre_top_pre_int_stat_address +
         0 * DEF_tofino_device_select_tm_top_tm_pre_top_pre_array_element_size,
     DEF_tofino_device_select_tm_top_tm_pre_top_pre_int_en0_address +
         0 * DEF_tofino_device_select_tm_top_tm_pre_top_pre_array_element_size,
     DEF_tofino_device_select_tm_top_tm_pre_top_pre_int_en1_address +
         0 * DEF_tofino_device_select_tm_top_tm_pre_top_pre_array_element_size,
     DEF_tofino_device_select_tm_top_tm_pre_top_pre_int_inj_address +
         0 * DEF_tofino_device_select_tm_top_tm_pre_top_pre_array_element_size,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
    {DEF_tofino_device_select_tm_top_tm_pre_top_pre_int_stat_address +
         1 * DEF_tofino_device_select_tm_top_tm_pre_top_pre_array_element_size,
     DEF_tofino_device_select_tm_top_tm_pre_top_pre_int_en0_address +
         1 * DEF_tofino_device_select_tm_top_tm_pre_top_pre_array_element_size,
     DEF_tofino_device_select_tm_top_tm_pre_top_pre_int_en1_address +
         1 * DEF_tofino_device_select_tm_top_tm_pre_top_pre_array_element_size,
     DEF_tofino_device_select_tm_top_tm_pre_top_pre_int_inj_address +
         1 * DEF_tofino_device_select_tm_top_tm_pre_top_pre_array_element_size,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
    {DEF_tofino_device_select_tm_top_tm_pre_top_pre_int_stat_address +
         2 * DEF_tofino_device_select_tm_top_tm_pre_top_pre_array_element_size,
     DEF_tofino_device_select_tm_top_tm_pre_top_pre_int_en0_address +
         2 * DEF_tofino_device_select_tm_top_tm_pre_top_pre_array_element_size,
     DEF_tofino_device_select_tm_top_tm_pre_top_pre_int_en1_address +
         2 * DEF_tofino_device_select_tm_top_tm_pre_top_pre_array_element_size,
     DEF_tofino_device_select_tm_top_tm_pre_top_pre_int_inj_address +
         2 * DEF_tofino_device_select_tm_top_tm_pre_top_pre_array_element_size,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
    {DEF_tofino_device_select_tm_top_tm_pre_top_pre_int_stat_address +
         3 * DEF_tofino_device_select_tm_top_tm_pre_top_pre_array_element_size,
     DEF_tofino_device_select_tm_top_tm_pre_top_pre_int_en0_address +
         3 * DEF_tofino_device_select_tm_top_tm_pre_top_pre_array_element_size,
     DEF_tofino_device_select_tm_top_tm_pre_top_pre_int_en1_address +
         3 * DEF_tofino_device_select_tm_top_tm_pre_top_pre_array_element_size,
     DEF_tofino_device_select_tm_top_tm_pre_top_pre_int_inj_address +
         3 * DEF_tofino_device_select_tm_top_tm_pre_top_pre_array_element_size,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
    {DEF_tofino_device_select_tm_top_tm_pre_top_pre_common_port_down_port_down_0_9_address,
     DEF_tofino_device_select_tm_top_tm_pre_top_pre_common_port_mask_port_mask_0_9_address,
     -1,
     -1 /* no inject */,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
    {DEF_tofino_device_select_tm_top_tm_pre_top_pre_common_port_down_port_down_1_9_address,
     DEF_tofino_device_select_tm_top_tm_pre_top_pre_common_port_mask_port_mask_1_9_address,
     -1,
     -1 /* no inject */,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
    {DEF_tofino_device_select_tm_top_tm_pre_top_pre_common_port_down_port_down_2_9_address,
     DEF_tofino_device_select_tm_top_tm_pre_top_pre_common_port_mask_port_mask_2_9_address,
     -1,
     -1 /* no inject */,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
    {DEF_tofino_device_select_tm_top_tm_pre_top_pre_common_port_down_port_down_3_9_address,
     DEF_tofino_device_select_tm_top_tm_pre_top_pre_common_port_mask_port_mask_3_9_address,
     -1,
     -1 /* no inject */,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
    {DEF_tofino_device_select_tm_top_tm_pre_top_pre_common_port_down_port_down_4_9_address,
     DEF_tofino_device_select_tm_top_tm_pre_top_pre_common_port_mask_port_mask_4_9_address,
     -1,
     -1 /* no inject */,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
    {DEF_tofino_device_select_tm_top_tm_pre_top_pre_common_port_down_port_down_5_9_address,
     DEF_tofino_device_select_tm_top_tm_pre_top_pre_common_port_mask_port_mask_5_9_address,
     -1,
     -1 /* no inject */,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
    {DEF_tofino_device_select_tm_top_tm_pre_top_pre_common_port_down_port_down_6_9_address,
     DEF_tofino_device_select_tm_top_tm_pre_top_pre_common_port_mask_port_mask_6_9_address,
     -1,
     -1 /* no inject */,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
    {DEF_tofino_device_select_tm_top_tm_pre_top_pre_common_port_down_port_down_7_9_address,
     DEF_tofino_device_select_tm_top_tm_pre_top_pre_common_port_mask_port_mask_7_9_address,
     -1,
     -1 /* no inject */,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
    {DEF_tofino_device_select_tm_top_tm_pre_top_pre_common_port_down_port_down_8_9_address,
     DEF_tofino_device_select_tm_top_tm_pre_top_pre_common_port_mask_port_mask_8_9_address,
     -1,
     -1 /* no inject */,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
};

lld_blk_lvl_int_t cbus_psc_blk_lvl_int[] = {
    {DEF_tofino_device_select_tm_top_tm_psc_top_psc_intr_status_address +
         0 * DEF_tofino_device_select_tm_top_tm_psc_top_psc_array_element_size,
     DEF_tofino_device_select_tm_top_tm_psc_top_psc_intr_enable0_address +
         0 * DEF_tofino_device_select_tm_top_tm_psc_top_psc_array_element_size,
     DEF_tofino_device_select_tm_top_tm_psc_top_psc_intr_enable1_address +
         0 * DEF_tofino_device_select_tm_top_tm_psc_top_psc_array_element_size,
     DEF_tofino_device_select_tm_top_tm_psc_top_psc_intr_inject_address +
         0 * DEF_tofino_device_select_tm_top_tm_psc_top_psc_array_element_size,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
    {DEF_tofino_device_select_tm_top_tm_psc_top_psc_intr_status_address +
         1 * DEF_tofino_device_select_tm_top_tm_psc_top_psc_array_element_size,
     DEF_tofino_device_select_tm_top_tm_psc_top_psc_intr_enable0_address +
         1 * DEF_tofino_device_select_tm_top_tm_psc_top_psc_array_element_size,
     DEF_tofino_device_select_tm_top_tm_psc_top_psc_intr_enable1_address +
         1 * DEF_tofino_device_select_tm_top_tm_psc_top_psc_array_element_size,
     DEF_tofino_device_select_tm_top_tm_psc_top_psc_intr_inject_address +
         1 * DEF_tofino_device_select_tm_top_tm_psc_top_psc_array_element_size,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
    {DEF_tofino_device_select_tm_top_tm_psc_top_psc_intr_status_address +
         2 * DEF_tofino_device_select_tm_top_tm_psc_top_psc_array_element_size,
     DEF_tofino_device_select_tm_top_tm_psc_top_psc_intr_enable0_address +
         2 * DEF_tofino_device_select_tm_top_tm_psc_top_psc_array_element_size,
     DEF_tofino_device_select_tm_top_tm_psc_top_psc_intr_enable1_address +
         2 * DEF_tofino_device_select_tm_top_tm_psc_top_psc_array_element_size,
     DEF_tofino_device_select_tm_top_tm_psc_top_psc_intr_inject_address +
         2 * DEF_tofino_device_select_tm_top_tm_psc_top_psc_array_element_size,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
    {DEF_tofino_device_select_tm_top_tm_psc_top_psc_intr_status_address +
         3 * DEF_tofino_device_select_tm_top_tm_psc_top_psc_array_element_size,
     DEF_tofino_device_select_tm_top_tm_psc_top_psc_intr_enable0_address +
         3 * DEF_tofino_device_select_tm_top_tm_psc_top_psc_array_element_size,
     DEF_tofino_device_select_tm_top_tm_psc_top_psc_intr_enable1_address +
         3 * DEF_tofino_device_select_tm_top_tm_psc_top_psc_array_element_size,
     DEF_tofino_device_select_tm_top_tm_psc_top_psc_intr_inject_address +
         3 * DEF_tofino_device_select_tm_top_tm_psc_top_psc_array_element_size,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
    {DEF_tofino_device_select_tm_top_tm_psc_top_psc_common_intr_status_address,
     DEF_tofino_device_select_tm_top_tm_psc_top_psc_common_intr_enable0_address,
     DEF_tofino_device_select_tm_top_tm_psc_top_psc_common_intr_enable1_address,
     DEF_tofino_device_select_tm_top_tm_psc_top_psc_common_intr_inject_address,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
};

lld_blk_lvl_int_t cbus_cbc_blk_lvl_int[] = {
    {DEF_tofino_device_select_cbc_cbc_cbus_int_stat_address,
     DEF_tofino_device_select_cbc_cbc_cbus_int_en_0_address,
     DEF_tofino_device_select_cbc_cbc_cbus_int_en_1_address,
     DEF_tofino_device_select_cbc_cbc_cbus_int_inj_address,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
};

lld_blk_lvl_int_t mbus_blk_lvl_mbc_int[] = {
    {DEF_tofino_device_select_mbc_mbc_mbus_int_stat_address,
     DEF_tofino_device_select_mbc_mbc_mbus_int_en_0_address,
     DEF_tofino_device_select_mbc_mbc_mbus_int_en_1_address,
     DEF_tofino_device_select_mbc_mbc_mbus_int_inj_address,
     0xffffffff,
     {0},
     {0},
     {{0}},
     {{0}}},
};

#define MAC_INTS(mask, port)                        \
  {DEF_tofino_macs_eth_regs_int_stat_address +      \
       (port * DEF_tofino_macs_array_element_size), \
   DEF_tofino_macs_eth_regs_int_en_address +        \
       (port * DEF_tofino_macs_array_element_size), \
   -1,                                              \
   DEF_tofino_macs_eth_regs_int_inj_address +       \
       (port * DEF_tofino_macs_array_element_size), \
   mask,                                            \
   {0},                                             \
   {0},                                             \
   {{0}},                                           \
   {{0}}},

lld_blk_lvl_int_t mbus_blk_lvl_int[] = {
    MAC_INTS(0x00000003, 0) MAC_INTS(0x0000000c, 1) MAC_INTS(
        0x00000030, 2) MAC_INTS(0x000000c0,
                                3) MAC_INTS(0x00000300,
                                            4) MAC_INTS(0x00000c00,
                                                        5) MAC_INTS(0x00003000,
                                                                    6)
        MAC_INTS(0x0000c000, 7) MAC_INTS(0x00030000, 8) MAC_INTS(
            0x000c0000,
            9) MAC_INTS(0x00300000,
                        10) MAC_INTS(0x00c00000,
                                     11) MAC_INTS(0x03000000,
                                                  12) MAC_INTS(0x0c000000, 13)
            MAC_INTS(0x30000000, 14) MAC_INTS(0xc0000000, 15) MAC_INTS(0x00000003, 16) MAC_INTS(
                0x0000000c,
                17) MAC_INTS(0x00000030, 18)
                MAC_INTS(0x000000c0, 19) MAC_INTS(0x00000300, 20) MAC_INTS(
                    0x00000c00,
                    21) MAC_INTS(0x00003000, 22)
                    MAC_INTS(0x0000c000, 23) MAC_INTS(0x00030000, 24) MAC_INTS(
                        0x000c0000,
                        25) MAC_INTS(0x00300000, 26)
                        MAC_INTS(0x00c00000, 27) MAC_INTS(0x03000000, 28) MAC_INTS(
                            0x0c000000,
                            29) MAC_INTS(0x30000000, 30)
                            MAC_INTS(0xc0000000, 31) MAC_INTS(0x00000003, 32) MAC_INTS(
                                0x0000000c,
                                33) MAC_INTS(0x00000030, 34)
                                MAC_INTS(0x000000c0, 35) MAC_INTS(0x00000300, 36) MAC_INTS(
                                    0x00000c00,
                                    37) MAC_INTS(0x00003000, 38)
                                    MAC_INTS(0x0000c000, 39) MAC_INTS(0x00030000, 40) MAC_INTS(
                                        0x000c0000,
                                        41) MAC_INTS(0x00300000, 42)
                                        MAC_INTS(0x00c00000, 43) MAC_INTS(
                                            0x03000000,
                                            44) MAC_INTS(0x0c000000, 45)
                                            MAC_INTS(0x30000000, 46) MAC_INTS(
                                                0xc0000000,
                                                47) MAC_INTS(0x00000003, 48)
                                                MAC_INTS(0x0000000c, 49) MAC_INTS(
                                                    0x00000030,
                                                    50) MAC_INTS(0x000000c0, 51)
                                                    MAC_INTS(0x00000300, 52) MAC_INTS(
                                                        0x00000c00,
                                                        53) MAC_INTS(0x00003000, 54)
                                                        MAC_INTS(0x0000c000, 55) MAC_INTS(
                                                            0x00030000,
                                                            56) MAC_INTS(0x000c0000, 57)
                                                            MAC_INTS(0x00300000, 58) MAC_INTS(
                                                                0x00c00000,
                                                                59) MAC_INTS(0x03000000, 60)
                                                                MAC_INTS(
                                                                    0x0c000000,
                                                                    61)
                                                                    MAC_INTS(
                                                                        0x30000000,
                                                                        62)
                                                                        MAC_INTS(
                                                                            0xc0000000,
                                                                            63)
                                                                            MAC_INTS(
                                                                                0x00000003,
                                                                                64)

    // 2x GPIO blocks (stn-65 & stn-66)
    {DEF_tofino_ethgpiobr_gpio_common_regs_int_stat_address,
     DEF_tofino_ethgpiobr_gpio_common_regs_int_en_address,
     -1,
     DEF_tofino_ethgpiobr_gpio_common_regs_int_inj_address,
     0x0000000c,
     {0},
     {0},
     {{0}},
     {{0}}},
    {DEF_tofino_ethgpiotl_gpio_common_regs_int_stat_address,
     DEF_tofino_ethgpiotl_gpio_common_regs_int_en_address,
     -1,
     DEF_tofino_ethgpiotl_gpio_common_regs_int_inj_address,
     0x00000030,
     {0},
     {0},
     {{0}},
     {{0}}},

    // MBC (stn-67)
    {DEF_tofino_device_select_mbc_mbc_mbus_int_stat_address,
     DEF_tofino_device_select_mbc_mbc_mbus_int_en_0_address,
     DEF_tofino_device_select_mbc_mbc_mbus_int_en_1_address,
     DEF_tofino_device_select_mbc_mbc_mbus_int_inj_address,
     0x000000c0,
     {0},
     {0},
     {{0}},
     {{0}}},
};

typedef struct blk_lvl_int_list_s {
  uint32_t mask;
  lld_blk_lvl_int_t *blk_lvl_int;
  uint32_t n;
} blk_lvl_int_list_t;

// 2nd level interrupt table for parser
blk_lvl_int_list_t pipe0_pbus_prsr_ints[] = {
    {0x00000001,
     pipe0_pbus_blk_lvl_int_ibp,
     sizeof(pipe0_pbus_blk_lvl_int_ibp) /
         sizeof(pipe0_pbus_blk_lvl_int_ibp[0])},
    //    { 0x00000002, pipe0_pbus_blk_lvl_int_parb,
    //    sizeof(pipe0_pbus_blk_lvl_int_parb)/sizeof(pipe0_pbus_blk_lvl_int_parb[0])
    //    },
    //    { 0x00000004,
    //    pipe0_pbus_blk_lvl_int_prsmrg,sizeof(pipe0_pbus_blk_lvl_int_prsmrg)/sizeof(pipe0_pbus_blk_lvl_int_prsmrg[0])
    //    },
    {0x00000008,
     pipe0_pbus_blk_lvl_int_pbusstat,
     sizeof(pipe0_pbus_blk_lvl_int_pbusstat) /
         sizeof(pipe0_pbus_blk_lvl_int_pbusstat[0])},
    {0x00000010,
     pipe0_pbus_blk_lvl_int_pgr,
     sizeof(pipe0_pbus_blk_lvl_int_pgr) /
         sizeof(pipe0_pbus_blk_lvl_int_pgr[0])},
    //    { 0x00000020, pipe0_pbus_blk_lvl_int_party_glue
    //    sizeof(pipe0_pbus_blk_lvl_int_party_glue)/sizeof(pipe0_pbus_blk_lvl_int_party_glue[0])
    //    },
    {0x00000040,
     pipe0_pbus_blk_lvl_int_ebp,
     sizeof(pipe0_pbus_blk_lvl_int_ebp) /
         sizeof(pipe0_pbus_blk_lvl_int_ebp[0])},
    {0x00000080,
     pipe0_pbus_blk_lvl_int_egrNx,
     sizeof(pipe0_pbus_blk_lvl_int_egrNx) /
         sizeof(pipe0_pbus_blk_lvl_int_egrNx[0])},
};

blk_lvl_int_list_t pipe1_pbus_prsr_ints[] = {
    {0x00000001,
     pipe1_pbus_blk_lvl_int_ibp,
     sizeof(pipe1_pbus_blk_lvl_int_ibp) /
         sizeof(pipe1_pbus_blk_lvl_int_ibp[0])},
    //    { 0x00000002, pipe1_pbus_blk_lvl_int_parb,
    //    sizeof(pipe1_pbus_blk_lvl_int_parb)/sizeof(pipe1_pbus_blk_lvl_int_parb[0])
    //    },
    //    { 0x00000004,
    //    pipe1_pbus_blk_lvl_int_prsmrg,sizeof(pipe1_pbus_blk_lvl_int_prsmrg)/sizeof(pipe1_pbus_blk_lvl_int_prsmrg[0])
    //    },
    {0x00000008,
     pipe1_pbus_blk_lvl_int_pbusstat,
     sizeof(pipe1_pbus_blk_lvl_int_pbusstat) /
         sizeof(pipe1_pbus_blk_lvl_int_pbusstat[0])},
    {0x00000010,
     pipe1_pbus_blk_lvl_int_pgr,
     sizeof(pipe1_pbus_blk_lvl_int_pgr) /
         sizeof(pipe1_pbus_blk_lvl_int_pgr[0])},
    //    { 0x00000020, pipe1_pbus_blk_lvl_int_party_glue
    //    sizeof(pipe1_pbus_blk_lvl_int_party_glue)/sizeof(pipe1_pbus_blk_lvl_int_party_glue[0])
    //    },
    {0x00000040,
     pipe1_pbus_blk_lvl_int_ebp,
     sizeof(pipe1_pbus_blk_lvl_int_ebp) /
         sizeof(pipe1_pbus_blk_lvl_int_ebp[0])},
    {0x00000080,
     pipe1_pbus_blk_lvl_int_egrNx,
     sizeof(pipe1_pbus_blk_lvl_int_egrNx) /
         sizeof(pipe1_pbus_blk_lvl_int_egrNx[0])},
};

blk_lvl_int_list_t pipe2_pbus_prsr_ints[] = {
    {0x00000001,
     pipe2_pbus_blk_lvl_int_ibp,
     sizeof(pipe2_pbus_blk_lvl_int_ibp) /
         sizeof(pipe2_pbus_blk_lvl_int_ibp[0])},
    //    { 0x00000002, pipe2_pbus_blk_lvl_int_parb,
    //    sizeof(pipe2_pbus_blk_lvl_int_parb)/sizeof(pipe2_pbus_blk_lvl_int_parb[0])
    //    },
    //    { 0x00000004,
    //    pipe2_pbus_blk_lvl_int_prsmrg,sizeof(pipe2_pbus_blk_lvl_int_prsmrg)/sizeof(pipe2_pbus_blk_lvl_int_prsmrg[0])
    //    },
    {0x00000008,
     pipe2_pbus_blk_lvl_int_pbusstat,
     sizeof(pipe2_pbus_blk_lvl_int_pbusstat) /
         sizeof(pipe2_pbus_blk_lvl_int_pbusstat[0])},
    {0x00000010,
     pipe2_pbus_blk_lvl_int_pgr,
     sizeof(pipe2_pbus_blk_lvl_int_pgr) /
         sizeof(pipe2_pbus_blk_lvl_int_pgr[0])},
    //    { 0x00000020, pipe2_pbus_blk_lvl_int_party_glue
    //    sizeof(pipe2_pbus_blk_lvl_int_party_glue)/sizeof(pipe2_pbus_blk_lvl_int_party_glue[0])
    //    },
    {0x00000040,
     pipe2_pbus_blk_lvl_int_ebp,
     sizeof(pipe2_pbus_blk_lvl_int_ebp) /
         sizeof(pipe2_pbus_blk_lvl_int_ebp[0])},
    {0x00000080,
     pipe2_pbus_blk_lvl_int_egrNx,
     sizeof(pipe2_pbus_blk_lvl_int_egrNx) /
         sizeof(pipe2_pbus_blk_lvl_int_egrNx[0])},
};

blk_lvl_int_list_t pipe3_pbus_prsr_ints[] = {
    {0x00000001,
     pipe3_pbus_blk_lvl_int_ibp,
     sizeof(pipe3_pbus_blk_lvl_int_ibp) /
         sizeof(pipe3_pbus_blk_lvl_int_ibp[0])},
    //    { 0x00000002, pipe3_pbus_blk_lvl_int_parb,
    //    sizeof(pipe3_pbus_blk_lvl_int_parb)/sizeof(pipe3_pbus_blk_lvl_int_parb[0])
    //    },
    //    { 0x00000004,
    //    pipe3_pbus_blk_lvl_int_prsmrg,sizeof(pipe3_pbus_blk_lvl_int_prsmrg)/sizeof(pipe3_pbus_blk_lvl_int_prsmrg[0])
    //    },
    {0x00000008,
     pipe3_pbus_blk_lvl_int_pbusstat,
     sizeof(pipe3_pbus_blk_lvl_int_pbusstat) /
         sizeof(pipe3_pbus_blk_lvl_int_pbusstat[0])},
    {0x00000010,
     pipe3_pbus_blk_lvl_int_pgr,
     sizeof(pipe3_pbus_blk_lvl_int_pgr) /
         sizeof(pipe3_pbus_blk_lvl_int_pgr[0])},
    //    { 0x00000020, pipe3_pbus_blk_lvl_int_party_glue
    //    sizeof(pipe3_pbus_blk_lvl_int_party_glue)/sizeof(pipe3_pbus_blk_lvl_int_party_glue[0])
    //    },
    {0x00000040,
     pipe3_pbus_blk_lvl_int_ebp,
     sizeof(pipe3_pbus_blk_lvl_int_ebp) /
         sizeof(pipe3_pbus_blk_lvl_int_ebp[0])},
    {0x00000080,
     pipe3_pbus_blk_lvl_int_egrNx,
     sizeof(pipe3_pbus_blk_lvl_int_egrNx) /
         sizeof(pipe3_pbus_blk_lvl_int_egrNx[0])},
};

#define mau_stride DEF_tofino_pipes_mau_array_element_size

#define MAU_UNIT_RAM_ROW(pipe, mau, row)                                       \
  {DEF_tofino_pipes_mau_rams_array_row_intr_status_mau_unit_ram_row_address +  \
       pipe * DEF_tofino_pipes_array_element_size + mau * mau_stride +         \
       row * DEF_tofino_pipes_mau_rams_array_row_array_element_size,           \
   DEF_tofino_pipes_mau_rams_array_row_intr_enable0_mau_unit_ram_row_address + \
       pipe * DEF_tofino_pipes_array_element_size + mau * mau_stride +         \
       row * DEF_tofino_pipes_mau_rams_array_row_array_element_size,           \
   DEF_tofino_pipes_mau_rams_array_row_intr_enable1_mau_unit_ram_row_address + \
       pipe * DEF_tofino_pipes_array_element_size + mau * mau_stride +         \
       row * DEF_tofino_pipes_mau_rams_array_row_array_element_size,           \
   DEF_tofino_pipes_mau_rams_array_row_intr_inject_mau_unit_ram_row_address +  \
       pipe * DEF_tofino_pipes_array_element_size + mau * mau_stride +         \
       row * DEF_tofino_pipes_mau_rams_array_row_array_element_size,           \
   0xffffffff,                                                                 \
   {0},                                                                        \
   {0},                                                                        \
   {{0}},                                                                      \
   {{0}}},

#define MAU_UNIT_RAM_INTS(pipe, mau) \
  MAU_UNIT_RAM_ROW(pipe, mau, 0)     \
  MAU_UNIT_RAM_ROW(pipe, mau, 1)     \
  MAU_UNIT_RAM_ROW(pipe, mau, 2)     \
  MAU_UNIT_RAM_ROW(pipe, mau, 3)     \
  MAU_UNIT_RAM_ROW(pipe, mau, 4)     \
  MAU_UNIT_RAM_ROW(pipe, mau, 5)     \
  MAU_UNIT_RAM_ROW(pipe, mau, 6)     \
  MAU_UNIT_RAM_ROW(pipe, mau, 7)

#define MAU_MAP_ALU_ROW(pipe, mau, row)                                                      \
  {DEF_tofino_pipes_mau_rams_map_alu_row_adrmux_intr_status_mau_adrmux_row_address +         \
       pipe * DEF_tofino_pipes_array_element_size + mau * mau_stride +                       \
       row * DEF_tofino_pipes_mau_rams_map_alu_row_array_element_size,                       \
   DEF_tofino_pipes_mau_rams_map_alu_row_adrmux_intr_enable0_mau_adrmux_row_address +        \
       pipe * DEF_tofino_pipes_array_element_size + mau * mau_stride +                       \
       row * DEF_tofino_pipes_mau_rams_map_alu_row_array_element_size,                       \
   DEF_tofino_pipes_mau_rams_map_alu_row_adrmux_intr_enable1_mau_adrmux_row_address +        \
       pipe * DEF_tofino_pipes_array_element_size + mau * mau_stride +                       \
       row * DEF_tofino_pipes_mau_rams_map_alu_row_array_element_size,                       \
   DEF_tofino_pipes_mau_rams_map_alu_row_adrmux_intr_inject_mau_adrmux_row_address +         \
       pipe * DEF_tofino_pipes_array_element_size + mau * mau_stride +                       \
       row * DEF_tofino_pipes_mau_rams_map_alu_row_array_element_size,                       \
   0xffffffff,                                                                               \
   {0},                                                                                      \
   {0},                                                                                      \
   {{0}},                                                                                    \
   {{0}}},                                                                                   \
      {DEF_tofino_pipes_mau_rams_map_alu_row_i2portctl_intr_status_mau_synth2port_address +  \
           pipe * DEF_tofino_pipes_array_element_size + mau * mau_stride +                   \
           row * DEF_tofino_pipes_mau_rams_map_alu_row_array_element_size,                   \
       DEF_tofino_pipes_mau_rams_map_alu_row_i2portctl_intr_enable0_mau_synth2port_address + \
           pipe * DEF_tofino_pipes_array_element_size + mau * mau_stride +                   \
           row * DEF_tofino_pipes_mau_rams_map_alu_row_array_element_size,                   \
       DEF_tofino_pipes_mau_rams_map_alu_row_i2portctl_intr_enable1_mau_synth2port_address + \
           pipe * DEF_tofino_pipes_array_element_size + mau * mau_stride +                   \
           row * DEF_tofino_pipes_mau_rams_map_alu_row_array_element_size,                   \
       DEF_tofino_pipes_mau_rams_map_alu_row_i2portctl_intr_inject_mau_synth2port_address +  \
           pipe * DEF_tofino_pipes_array_element_size + mau * mau_stride +                   \
           row * DEF_tofino_pipes_mau_rams_map_alu_row_array_element_size,                   \
       0xffffffff,                                                                           \
       {0},                                                                                  \
       {0},                                                                                  \
       {{0}},                                                                                \
       {{0}}},

#define MAU_MAP_ALU_STATS_GROUP(pipe, mau, grp)                                            \
  {DEF_tofino_pipes_mau_rams_map_alu_stats_wrap_stats_intr_status_mau_stats_alu_address +  \
       pipe * DEF_tofino_pipes_array_element_size + mau * mau_stride +                     \
       grp * DEF_tofino_pipes_mau_rams_map_alu_stats_wrap_array_element_size,              \
   DEF_tofino_pipes_mau_rams_map_alu_stats_wrap_stats_intr_enable0_mau_stats_alu_address + \
       pipe * DEF_tofino_pipes_array_element_size + mau * mau_stride +                     \
       grp * DEF_tofino_pipes_mau_rams_map_alu_stats_wrap_array_element_size,              \
   DEF_tofino_pipes_mau_rams_map_alu_stats_wrap_stats_intr_enable1_mau_stats_alu_address + \
       pipe * DEF_tofino_pipes_array_element_size + mau * mau_stride +                     \
       grp * DEF_tofino_pipes_mau_rams_map_alu_stats_wrap_array_element_size,              \
   DEF_tofino_pipes_mau_rams_map_alu_stats_wrap_stats_intr_inject_mau_stats_alu_address +  \
       pipe * DEF_tofino_pipes_array_element_size + mau * mau_stride +                     \
       grp * DEF_tofino_pipes_mau_rams_map_alu_stats_wrap_array_element_size,              \
   0xffffffff,                                                                             \
   {0},                                                                                    \
   {0},                                                                                    \
   {{0}},                                                                                  \
   {{0}}},

#define MAU_MAP_ALU_METER_GROUP(pipe, mau, grp)                                                   \
  {DEF_tofino_pipes_mau_rams_map_alu_meter_group_selector_intr_status_mau_selector_alu_address +  \
       pipe * DEF_tofino_pipes_array_element_size + mau * mau_stride +                            \
       grp * DEF_tofino_pipes_mau_rams_map_alu_meter_group_array_element_size,                    \
   DEF_tofino_pipes_mau_rams_map_alu_meter_group_selector_intr_enable0_mau_selector_alu_address + \
       pipe * DEF_tofino_pipes_array_element_size + mau * mau_stride +                            \
       grp * DEF_tofino_pipes_mau_rams_map_alu_meter_group_array_element_size,                    \
   DEF_tofino_pipes_mau_rams_map_alu_meter_group_selector_intr_enable1_mau_selector_alu_address + \
       pipe * DEF_tofino_pipes_array_element_size + mau * mau_stride +                            \
       grp * DEF_tofino_pipes_mau_rams_map_alu_meter_group_array_element_size,                    \
   DEF_tofino_pipes_mau_rams_map_alu_meter_group_selector_intr_inject_mau_selector_alu_address +  \
       pipe * DEF_tofino_pipes_array_element_size + mau * mau_stride +                            \
       grp * DEF_tofino_pipes_mau_rams_map_alu_meter_group_array_element_size,                    \
   0xffffffff,                                                                                    \
   {0},                                                                                           \
   {0},                                                                                           \
   {{0}},                                                                                         \
   {{0}}},

#define MAU_MAP_ALU_INTS(pipe, mau)     \
  MAU_MAP_ALU_ROW(pipe, mau, 0)         \
  MAU_MAP_ALU_ROW(pipe, mau, 1)         \
  MAU_MAP_ALU_ROW(pipe, mau, 2)         \
  MAU_MAP_ALU_ROW(pipe, mau, 3)         \
  MAU_MAP_ALU_ROW(pipe, mau, 4)         \
  MAU_MAP_ALU_ROW(pipe, mau, 5)         \
  MAU_MAP_ALU_ROW(pipe, mau, 6)         \
  MAU_MAP_ALU_ROW(pipe, mau, 7)         \
  MAU_MAP_ALU_STATS_GROUP(pipe, mau, 0) \
  MAU_MAP_ALU_STATS_GROUP(pipe, mau, 1) \
  MAU_MAP_ALU_STATS_GROUP(pipe, mau, 2) \
  MAU_MAP_ALU_STATS_GROUP(pipe, mau, 3) \
  MAU_MAP_ALU_METER_GROUP(pipe, mau, 0) \
  MAU_MAP_ALU_METER_GROUP(pipe, mau, 1) \
  MAU_MAP_ALU_METER_GROUP(pipe, mau, 2) \
  MAU_MAP_ALU_METER_GROUP(pipe, mau, 3)

#define MAU_MISC(pipe, mau)                                               \
  {DEF_tofino_pipes_mau_rams_match_adrdist_intr_status_mau_ad_address +   \
       pipe * DEF_tofino_pipes_array_element_size + mau * mau_stride,     \
   DEF_tofino_pipes_mau_rams_match_adrdist_intr_enable0_mau_ad_address +  \
       pipe * DEF_tofino_pipes_array_element_size + mau * mau_stride,     \
   DEF_tofino_pipes_mau_rams_match_adrdist_intr_enable1_mau_ad_address +  \
       pipe * DEF_tofino_pipes_array_element_size + mau * mau_stride,     \
   DEF_tofino_pipes_mau_rams_match_adrdist_intr_inject_mau_ad_address +   \
       pipe * DEF_tofino_pipes_array_element_size + mau * mau_stride,     \
   0xffffffff,                                                            \
   {0},                                                                   \
   {0},                                                                   \
   {{0}},                                                                 \
   {{0}}},                                                                \
      {DEF_tofino_pipes_mau_cfg_regs_intr_status_mau_cfg_address +        \
           pipe * DEF_tofino_pipes_array_element_size + mau * mau_stride, \
       DEF_tofino_pipes_mau_cfg_regs_intr_enable0_mau_cfg_address +       \
           pipe * DEF_tofino_pipes_array_element_size + mau * mau_stride, \
       DEF_tofino_pipes_mau_cfg_regs_intr_enable1_mau_cfg_address +       \
           pipe * DEF_tofino_pipes_array_element_size + mau * mau_stride, \
       DEF_tofino_pipes_mau_cfg_regs_intr_inject_mau_cfg_address +        \
           pipe * DEF_tofino_pipes_array_element_size + mau * mau_stride, \
       0xffffffff,                                                        \
       {0},                                                               \
       {0},                                                               \
       {{0}},                                                             \
       {{0}}},                                                            \
      {DEF_tofino_pipes_mau_tcams_intr_status_mau_tcam_array_address +    \
           pipe * DEF_tofino_pipes_array_element_size + mau * mau_stride, \
       DEF_tofino_pipes_mau_tcams_intr_enable0_mau_tcam_array_address +   \
           pipe * DEF_tofino_pipes_array_element_size + mau * mau_stride, \
       DEF_tofino_pipes_mau_tcams_intr_enable1_mau_tcam_array_address +   \
           pipe * DEF_tofino_pipes_array_element_size + mau * mau_stride, \
       DEF_tofino_pipes_mau_tcams_intr_inject_mau_tcam_array_address +    \
           pipe * DEF_tofino_pipes_array_element_size + mau * mau_stride, \
       0xffffffff,                                                        \
       {0},                                                               \
       {0},                                                               \
       {{0}},                                                             \
       {{0}}},                                                            \
      {DEF_tofino_pipes_mau_dp_intr_status_mau_imem_address +             \
           pipe * DEF_tofino_pipes_array_element_size + mau * mau_stride, \
       DEF_tofino_pipes_mau_dp_intr_enable0_mau_imem_address +            \
           pipe * DEF_tofino_pipes_array_element_size + mau * mau_stride, \
       DEF_tofino_pipes_mau_dp_intr_enable1_mau_imem_address +            \
           pipe * DEF_tofino_pipes_array_element_size + mau * mau_stride, \
       DEF_tofino_pipes_mau_dp_intr_inject_mau_imem_address +             \
           pipe * DEF_tofino_pipes_array_element_size + mau * mau_stride, \
       0xffffffff,                                                        \
       {0},                                                               \
       {0},                                                               \
       {{0}},                                                             \
       {{0}}},                                                            \
      {DEF_tofino_pipes_mau_dp_intr_status_mau_snapshot_address +         \
           pipe * DEF_tofino_pipes_array_element_size + mau * mau_stride, \
       DEF_tofino_pipes_mau_dp_intr_enable0_mau_snapshot_address +        \
           pipe * DEF_tofino_pipes_array_element_size + mau * mau_stride, \
       DEF_tofino_pipes_mau_dp_intr_enable1_mau_snapshot_address +        \
           pipe * DEF_tofino_pipes_array_element_size + mau * mau_stride, \
       DEF_tofino_pipes_mau_dp_intr_inject_mau_snapshot_address +         \
           pipe * DEF_tofino_pipes_array_element_size + mau * mau_stride, \
       0xffffffff,                                                        \
       {0},                                                               \
       {0},                                                               \
       {{0}},                                                             \
       {{0}}},                                                            \
      {DEF_tofino_pipes_mau_dp_intr_status_mau_gfm_hash_address +         \
           pipe * DEF_tofino_pipes_array_element_size + mau * mau_stride, \
       DEF_tofino_pipes_mau_dp_intr_enable0_mau_gfm_hash_address +        \
           pipe * DEF_tofino_pipes_array_element_size + mau * mau_stride, \
       DEF_tofino_pipes_mau_dp_intr_enable1_mau_gfm_hash_address +        \
           pipe * DEF_tofino_pipes_array_element_size + mau * mau_stride, \
       DEF_tofino_pipes_mau_dp_intr_inject_mau_gfm_hash_address +         \
           pipe * DEF_tofino_pipes_array_element_size + mau * mau_stride, \
       0xffffffff,                                                        \
       {0},                                                               \
       {0},                                                               \
       {{0}},                                                             \
       {{0}}},

#define MAU_MISC_INTS(pipe, mau) MAU_MISC(pipe, mau)

#define MAU_INTS(pipe, mau)    \
  MAU_MISC_INTS(pipe, mau)     \
  MAU_UNIT_RAM_INTS(pipe, mau) \
  MAU_MAP_ALU_INTS(pipe, mau)

lld_blk_lvl_int_t pipe0_mau0_blk_lvl_int[] = {MAU_INTS(0, 0)};
lld_blk_lvl_int_t pipe0_mau1_blk_lvl_int[] = {MAU_INTS(0, 1)};
lld_blk_lvl_int_t pipe0_mau2_blk_lvl_int[] = {MAU_INTS(0, 2)};
lld_blk_lvl_int_t pipe0_mau3_blk_lvl_int[] = {MAU_INTS(0, 3)};
lld_blk_lvl_int_t pipe0_mau4_blk_lvl_int[] = {MAU_INTS(0, 4)};
lld_blk_lvl_int_t pipe0_mau5_blk_lvl_int[] = {MAU_INTS(0, 5)};
lld_blk_lvl_int_t pipe0_mau6_blk_lvl_int[] = {MAU_INTS(0, 6)};
lld_blk_lvl_int_t pipe0_mau7_blk_lvl_int[] = {MAU_INTS(0, 7)};
lld_blk_lvl_int_t pipe0_mau8_blk_lvl_int[] = {MAU_INTS(0, 8)};
lld_blk_lvl_int_t pipe0_mau9_blk_lvl_int[] = {MAU_INTS(0, 9)};
lld_blk_lvl_int_t pipe0_mau10_blk_lvl_int[] = {MAU_INTS(0, 10)};
lld_blk_lvl_int_t pipe0_mau11_blk_lvl_int[] = {MAU_INTS(0, 11)};

lld_blk_lvl_int_t pipe1_mau0_blk_lvl_int[] = {MAU_INTS(1, 0)};
lld_blk_lvl_int_t pipe1_mau1_blk_lvl_int[] = {MAU_INTS(1, 1)};
lld_blk_lvl_int_t pipe1_mau2_blk_lvl_int[] = {MAU_INTS(1, 2)};
lld_blk_lvl_int_t pipe1_mau3_blk_lvl_int[] = {MAU_INTS(1, 3)};
lld_blk_lvl_int_t pipe1_mau4_blk_lvl_int[] = {MAU_INTS(1, 4)};
lld_blk_lvl_int_t pipe1_mau5_blk_lvl_int[] = {MAU_INTS(1, 5)};
lld_blk_lvl_int_t pipe1_mau6_blk_lvl_int[] = {MAU_INTS(1, 6)};
lld_blk_lvl_int_t pipe1_mau7_blk_lvl_int[] = {MAU_INTS(1, 7)};
lld_blk_lvl_int_t pipe1_mau8_blk_lvl_int[] = {MAU_INTS(1, 8)};
lld_blk_lvl_int_t pipe1_mau9_blk_lvl_int[] = {MAU_INTS(1, 9)};
lld_blk_lvl_int_t pipe1_mau10_blk_lvl_int[] = {MAU_INTS(1, 10)};
lld_blk_lvl_int_t pipe1_mau11_blk_lvl_int[] = {MAU_INTS(1, 11)};

lld_blk_lvl_int_t pipe2_mau0_blk_lvl_int[] = {MAU_INTS(2, 0)};
lld_blk_lvl_int_t pipe2_mau1_blk_lvl_int[] = {MAU_INTS(2, 1)};
lld_blk_lvl_int_t pipe2_mau2_blk_lvl_int[] = {MAU_INTS(2, 2)};
lld_blk_lvl_int_t pipe2_mau3_blk_lvl_int[] = {MAU_INTS(2, 3)};
lld_blk_lvl_int_t pipe2_mau4_blk_lvl_int[] = {MAU_INTS(2, 4)};
lld_blk_lvl_int_t pipe2_mau5_blk_lvl_int[] = {MAU_INTS(2, 5)};
lld_blk_lvl_int_t pipe2_mau6_blk_lvl_int[] = {MAU_INTS(2, 6)};
lld_blk_lvl_int_t pipe2_mau7_blk_lvl_int[] = {MAU_INTS(2, 7)};
lld_blk_lvl_int_t pipe2_mau8_blk_lvl_int[] = {MAU_INTS(2, 8)};
lld_blk_lvl_int_t pipe2_mau9_blk_lvl_int[] = {MAU_INTS(2, 9)};
lld_blk_lvl_int_t pipe2_mau10_blk_lvl_int[] = {MAU_INTS(2, 10)};
lld_blk_lvl_int_t pipe2_mau11_blk_lvl_int[] = {MAU_INTS(2, 11)};

lld_blk_lvl_int_t pipe3_mau0_blk_lvl_int[] = {MAU_INTS(3, 0)};
lld_blk_lvl_int_t pipe3_mau1_blk_lvl_int[] = {MAU_INTS(3, 1)};
lld_blk_lvl_int_t pipe3_mau2_blk_lvl_int[] = {MAU_INTS(3, 2)};
lld_blk_lvl_int_t pipe3_mau3_blk_lvl_int[] = {MAU_INTS(3, 3)};
lld_blk_lvl_int_t pipe3_mau4_blk_lvl_int[] = {MAU_INTS(3, 4)};
lld_blk_lvl_int_t pipe3_mau5_blk_lvl_int[] = {MAU_INTS(3, 5)};
lld_blk_lvl_int_t pipe3_mau6_blk_lvl_int[] = {MAU_INTS(3, 6)};
lld_blk_lvl_int_t pipe3_mau7_blk_lvl_int[] = {MAU_INTS(3, 7)};
lld_blk_lvl_int_t pipe3_mau8_blk_lvl_int[] = {MAU_INTS(3, 8)};
lld_blk_lvl_int_t pipe3_mau9_blk_lvl_int[] = {MAU_INTS(3, 9)};
lld_blk_lvl_int_t pipe3_mau10_blk_lvl_int[] = {MAU_INTS(3, 10)};
lld_blk_lvl_int_t pipe3_mau11_blk_lvl_int[] = {MAU_INTS(3, 11)};

blk_lvl_int_list_t pipe0_pbus_sh_ints[] = {
    {0x00000003,
     pipe0_mau0_blk_lvl_int,
     sizeof(pipe0_mau0_blk_lvl_int) / sizeof(pipe0_mau0_blk_lvl_int[0])},
    {0x0000000c,
     pipe0_mau1_blk_lvl_int,
     sizeof(pipe0_mau1_blk_lvl_int) / sizeof(pipe0_mau1_blk_lvl_int[0])},
    {0x00000030,
     pipe0_mau2_blk_lvl_int,
     sizeof(pipe0_mau2_blk_lvl_int) / sizeof(pipe0_mau2_blk_lvl_int[0])},
    {0x000000c0,
     pipe0_mau3_blk_lvl_int,
     sizeof(pipe0_mau3_blk_lvl_int) / sizeof(pipe0_mau3_blk_lvl_int[0])},
    {0x00000300,
     pipe0_mau4_blk_lvl_int,
     sizeof(pipe0_mau4_blk_lvl_int) / sizeof(pipe0_mau4_blk_lvl_int[0])},
    {0x00000c00,
     pipe0_mau5_blk_lvl_int,
     sizeof(pipe0_mau5_blk_lvl_int) / sizeof(pipe0_mau5_blk_lvl_int[0])},
    {0x00003000,
     pipe0_mau6_blk_lvl_int,
     sizeof(pipe0_mau6_blk_lvl_int) / sizeof(pipe0_mau6_blk_lvl_int[0])},
    {0x0000c000,
     pipe0_mau7_blk_lvl_int,
     sizeof(pipe0_mau7_blk_lvl_int) / sizeof(pipe0_mau7_blk_lvl_int[0])},
    {0x00030000,
     pipe0_mau8_blk_lvl_int,
     sizeof(pipe0_mau8_blk_lvl_int) / sizeof(pipe0_mau8_blk_lvl_int[0])},
    {0x000c0000,
     pipe0_mau9_blk_lvl_int,
     sizeof(pipe0_mau9_blk_lvl_int) / sizeof(pipe0_mau9_blk_lvl_int[0])},
    {0x00300000,
     pipe0_mau10_blk_lvl_int,
     sizeof(pipe0_mau10_blk_lvl_int) / sizeof(pipe0_mau10_blk_lvl_int[0])},
    {0x00c00000,
     pipe0_mau11_blk_lvl_int,
     sizeof(pipe0_mau11_blk_lvl_int) / sizeof(pipe0_mau11_blk_lvl_int[0])},

    {0x03000000, NULL, 0},  // note: "special-case" for prsr hierarchy
    {0x0c000000,
     pipe0_pbus_blk_lvl_int_dprsr,
     sizeof(pipe0_pbus_blk_lvl_int_dprsr) /
         sizeof(pipe0_pbus_blk_lvl_int_dprsr[0])},
    {0xc0000000,
     pbus_blk_lvl_int_pbc,
     sizeof(pbus_blk_lvl_int_pbc) / sizeof(pbus_blk_lvl_int_pbc[0])},
};

blk_lvl_int_list_t pipe1_pbus_sh_ints[] = {
    {0x00000003,
     pipe1_mau0_blk_lvl_int,
     sizeof(pipe1_mau0_blk_lvl_int) / sizeof(pipe1_mau0_blk_lvl_int[0])},
    {0x0000000c,
     pipe1_mau1_blk_lvl_int,
     sizeof(pipe1_mau1_blk_lvl_int) / sizeof(pipe1_mau1_blk_lvl_int[0])},
    {0x00000030,
     pipe1_mau2_blk_lvl_int,
     sizeof(pipe1_mau2_blk_lvl_int) / sizeof(pipe1_mau2_blk_lvl_int[0])},
    {0x000000c0,
     pipe1_mau3_blk_lvl_int,
     sizeof(pipe1_mau3_blk_lvl_int) / sizeof(pipe1_mau3_blk_lvl_int[0])},
    {0x00000300,
     pipe1_mau4_blk_lvl_int,
     sizeof(pipe1_mau4_blk_lvl_int) / sizeof(pipe1_mau4_blk_lvl_int[0])},
    {0x00000c00,
     pipe1_mau5_blk_lvl_int,
     sizeof(pipe1_mau5_blk_lvl_int) / sizeof(pipe1_mau5_blk_lvl_int[0])},
    {0x00003000,
     pipe1_mau6_blk_lvl_int,
     sizeof(pipe1_mau6_blk_lvl_int) / sizeof(pipe1_mau6_blk_lvl_int[0])},
    {0x0000c000,
     pipe1_mau7_blk_lvl_int,
     sizeof(pipe1_mau7_blk_lvl_int) / sizeof(pipe1_mau7_blk_lvl_int[0])},
    {0x00030000,
     pipe1_mau8_blk_lvl_int,
     sizeof(pipe1_mau8_blk_lvl_int) / sizeof(pipe1_mau8_blk_lvl_int[0])},
    {0x000c0000,
     pipe1_mau9_blk_lvl_int,
     sizeof(pipe1_mau9_blk_lvl_int) / sizeof(pipe1_mau9_blk_lvl_int[0])},
    {0x00300000,
     pipe1_mau10_blk_lvl_int,
     sizeof(pipe1_mau10_blk_lvl_int) / sizeof(pipe1_mau10_blk_lvl_int[0])},
    {0x00c00000,
     pipe1_mau11_blk_lvl_int,
     sizeof(pipe1_mau11_blk_lvl_int) / sizeof(pipe1_mau11_blk_lvl_int[0])},

    {0x03000000, NULL, 0},  // note: "special-case" for prsr hierarchy
    {0x0c000000,
     pipe1_pbus_blk_lvl_int_dprsr,
     sizeof(pipe1_pbus_blk_lvl_int_dprsr) /
         sizeof(pipe1_pbus_blk_lvl_int_dprsr[0])},
    {0xc0000000,
     pbus_blk_lvl_int_pbc,
     sizeof(pbus_blk_lvl_int_pbc) / sizeof(pbus_blk_lvl_int_pbc[0])},
};

blk_lvl_int_list_t pipe2_pbus_sh_ints[] = {
    {0x00000003,
     pipe2_mau0_blk_lvl_int,
     sizeof(pipe2_mau0_blk_lvl_int) / sizeof(pipe2_mau0_blk_lvl_int[0])},
    {0x0000000c,
     pipe2_mau1_blk_lvl_int,
     sizeof(pipe2_mau1_blk_lvl_int) / sizeof(pipe2_mau1_blk_lvl_int[0])},
    {0x00000030,
     pipe2_mau2_blk_lvl_int,
     sizeof(pipe2_mau2_blk_lvl_int) / sizeof(pipe2_mau2_blk_lvl_int[0])},
    {0x000000c0,
     pipe2_mau3_blk_lvl_int,
     sizeof(pipe2_mau3_blk_lvl_int) / sizeof(pipe2_mau3_blk_lvl_int[0])},
    {0x00000300,
     pipe2_mau4_blk_lvl_int,
     sizeof(pipe2_mau4_blk_lvl_int) / sizeof(pipe2_mau4_blk_lvl_int[0])},
    {0x00000c00,
     pipe2_mau5_blk_lvl_int,
     sizeof(pipe2_mau5_blk_lvl_int) / sizeof(pipe2_mau5_blk_lvl_int[0])},
    {0x00003000,
     pipe2_mau6_blk_lvl_int,
     sizeof(pipe2_mau6_blk_lvl_int) / sizeof(pipe2_mau6_blk_lvl_int[0])},
    {0x0000c000,
     pipe2_mau7_blk_lvl_int,
     sizeof(pipe2_mau7_blk_lvl_int) / sizeof(pipe2_mau7_blk_lvl_int[0])},
    {0x00030000,
     pipe2_mau8_blk_lvl_int,
     sizeof(pipe2_mau8_blk_lvl_int) / sizeof(pipe2_mau8_blk_lvl_int[0])},
    {0x000c0000,
     pipe2_mau9_blk_lvl_int,
     sizeof(pipe2_mau9_blk_lvl_int) / sizeof(pipe2_mau9_blk_lvl_int[0])},
    {0x00300000,
     pipe2_mau10_blk_lvl_int,
     sizeof(pipe2_mau10_blk_lvl_int) / sizeof(pipe2_mau10_blk_lvl_int[0])},
    {0x00c00000,
     pipe2_mau11_blk_lvl_int,
     sizeof(pipe2_mau11_blk_lvl_int) / sizeof(pipe2_mau11_blk_lvl_int[0])},

    {0x03000000, NULL, 0},  // note: "special-case" for prsr hierarchy
    {0x0c000000,
     pipe2_pbus_blk_lvl_int_dprsr,
     sizeof(pipe2_pbus_blk_lvl_int_dprsr) /
         sizeof(pipe2_pbus_blk_lvl_int_dprsr[0])},
    {0xc0000000,
     pbus_blk_lvl_int_pbc,
     sizeof(pbus_blk_lvl_int_pbc) / sizeof(pbus_blk_lvl_int_pbc[0])},
};

blk_lvl_int_list_t pipe3_pbus_sh_ints[] = {
    {0x00000003,
     pipe3_mau0_blk_lvl_int,
     sizeof(pipe3_mau0_blk_lvl_int) / sizeof(pipe3_mau0_blk_lvl_int[0])},
    {0x0000000c,
     pipe3_mau1_blk_lvl_int,
     sizeof(pipe3_mau1_blk_lvl_int) / sizeof(pipe3_mau1_blk_lvl_int[0])},
    {0x00000030,
     pipe3_mau2_blk_lvl_int,
     sizeof(pipe3_mau2_blk_lvl_int) / sizeof(pipe3_mau2_blk_lvl_int[0])},
    {0x000000c0,
     pipe3_mau3_blk_lvl_int,
     sizeof(pipe3_mau3_blk_lvl_int) / sizeof(pipe3_mau3_blk_lvl_int[0])},
    {0x00000300,
     pipe3_mau4_blk_lvl_int,
     sizeof(pipe3_mau4_blk_lvl_int) / sizeof(pipe3_mau4_blk_lvl_int[0])},
    {0x00000c00,
     pipe3_mau5_blk_lvl_int,
     sizeof(pipe3_mau5_blk_lvl_int) / sizeof(pipe3_mau5_blk_lvl_int[0])},
    {0x00003000,
     pipe3_mau6_blk_lvl_int,
     sizeof(pipe3_mau6_blk_lvl_int) / sizeof(pipe3_mau6_blk_lvl_int[0])},
    {0x0000c000,
     pipe3_mau7_blk_lvl_int,
     sizeof(pipe3_mau7_blk_lvl_int) / sizeof(pipe3_mau7_blk_lvl_int[0])},
    {0x00030000,
     pipe3_mau8_blk_lvl_int,
     sizeof(pipe3_mau8_blk_lvl_int) / sizeof(pipe3_mau8_blk_lvl_int[0])},
    {0x000c0000,
     pipe3_mau9_blk_lvl_int,
     sizeof(pipe3_mau9_blk_lvl_int) / sizeof(pipe3_mau9_blk_lvl_int[0])},
    {0x00300000,
     pipe3_mau10_blk_lvl_int,
     sizeof(pipe3_mau10_blk_lvl_int) / sizeof(pipe3_mau10_blk_lvl_int[0])},
    {0x00c00000,
     pipe3_mau11_blk_lvl_int,
     sizeof(pipe3_mau11_blk_lvl_int) / sizeof(pipe3_mau11_blk_lvl_int[0])},

    {0x03000000, NULL, 0},  // note: "special-case" for prsr hierarchy
    {0x0c000000,
     pipe3_pbus_blk_lvl_int_dprsr,
     sizeof(pipe3_pbus_blk_lvl_int_dprsr) /
         sizeof(pipe3_pbus_blk_lvl_int_dprsr[0])},
    {0xc0000000,
     pbus_blk_lvl_int_pbc,
     sizeof(pbus_blk_lvl_int_pbc) / sizeof(pbus_blk_lvl_int_pbc[0])},
};

blk_lvl_int_list_t mbus_sh_ints[] = {
    {0xffffffff,
     mbus_blk_lvl_int,
     sizeof(mbus_blk_lvl_int) / sizeof(mbus_blk_lvl_int[0])},
};

blk_lvl_int_list_t cbus_sh_ints[] = {
    {0x00000003,
     cbus_lfltr0_blk_lvl_int,
     sizeof(cbus_lfltr0_blk_lvl_int) / sizeof(cbus_lfltr0_blk_lvl_int[0])},
    {0x0000000c,
     cbus_lfltr1_blk_lvl_int,
     sizeof(cbus_lfltr1_blk_lvl_int) / sizeof(cbus_lfltr1_blk_lvl_int[0])},
    {0x00000030,
     cbus_lfltr2_blk_lvl_int,
     sizeof(cbus_lfltr2_blk_lvl_int) / sizeof(cbus_lfltr2_blk_lvl_int[0])},
    {0x000000c0,
     cbus_lfltr3_blk_lvl_int,
     sizeof(cbus_lfltr3_blk_lvl_int) / sizeof(cbus_lfltr3_blk_lvl_int[0])},
    {0x00000300,
     cbus_wac_blk_lvl_int,
     sizeof(cbus_wac_blk_lvl_int) / sizeof(cbus_wac_blk_lvl_int[0])},
    {0x00000c00,
     cbus_caa_blk_lvl_int,
     sizeof(cbus_caa_blk_lvl_int) / sizeof(cbus_caa_blk_lvl_int[0])},
    {0x00003000,
     cbus_qac_blk_lvl_int,
     sizeof(cbus_qac_blk_lvl_int) / sizeof(cbus_qac_blk_lvl_int[0])},
    {0x0000c000,
     cbus_sch_blk_lvl_int,
     sizeof(cbus_sch_blk_lvl_int) / sizeof(cbus_sch_blk_lvl_int[0])},
    {0x00030000,
     cbus_clc_blk_lvl_int,
     sizeof(cbus_clc_blk_lvl_int) / sizeof(cbus_clc_blk_lvl_int[0])},
    {0x000c0000,
     cbus_qlc_blk_lvl_int,
     sizeof(cbus_qlc_blk_lvl_int) / sizeof(cbus_qlc_blk_lvl_int[0])},
    {0x00300000,
     cbus_prc_blk_lvl_int,
     sizeof(cbus_prc_blk_lvl_int) / sizeof(cbus_prc_blk_lvl_int[0])},
    {0x00c00000,
     cbus_pre_blk_lvl_int,
     sizeof(cbus_pre_blk_lvl_int) / sizeof(cbus_pre_blk_lvl_int[0])},
    {0x03000000,
     cbus_psc_blk_lvl_int,
     sizeof(cbus_psc_blk_lvl_int) / sizeof(cbus_psc_blk_lvl_int[0])},
    {0xc0000000,
     cbus_cbc_blk_lvl_int,
     sizeof(cbus_cbc_blk_lvl_int) / sizeof(cbus_cbc_blk_lvl_int[0])},
};

blk_lvl_int_list_t all_blk_lvl_ints[] = {

    {0x0,
     host_blk_lvl_int,
     sizeof(host_blk_lvl_int) / sizeof(host_blk_lvl_int[0])},
    {0x0,
     tbus_blk_lvl_int,
     sizeof(tbus_blk_lvl_int) / sizeof(tbus_blk_lvl_int[0])},
    {0x0,
     pbus_blk_lvl_int_pbc,
     sizeof(pbus_blk_lvl_int_pbc) / sizeof(pbus_blk_lvl_int_pbc[0])},

    {0x0,
     pipe0_mau0_blk_lvl_int,
     sizeof(pipe0_mau0_blk_lvl_int) / sizeof(pipe0_mau0_blk_lvl_int[0])},
    {0x0,
     pipe0_mau1_blk_lvl_int,
     sizeof(pipe0_mau1_blk_lvl_int) / sizeof(pipe0_mau1_blk_lvl_int[0])},
    {0x0,
     pipe0_mau2_blk_lvl_int,
     sizeof(pipe0_mau2_blk_lvl_int) / sizeof(pipe0_mau2_blk_lvl_int[0])},
    {0x0,
     pipe0_mau3_blk_lvl_int,
     sizeof(pipe0_mau3_blk_lvl_int) / sizeof(pipe0_mau3_blk_lvl_int[0])},
    {0x0,
     pipe0_mau4_blk_lvl_int,
     sizeof(pipe0_mau4_blk_lvl_int) / sizeof(pipe0_mau4_blk_lvl_int[0])},
    {0x0,
     pipe0_mau5_blk_lvl_int,
     sizeof(pipe0_mau5_blk_lvl_int) / sizeof(pipe0_mau5_blk_lvl_int[0])},
    {0x0,
     pipe0_mau6_blk_lvl_int,
     sizeof(pipe0_mau6_blk_lvl_int) / sizeof(pipe0_mau6_blk_lvl_int[0])},
    {0x0,
     pipe0_mau7_blk_lvl_int,
     sizeof(pipe0_mau7_blk_lvl_int) / sizeof(pipe0_mau7_blk_lvl_int[0])},
    {0x0,
     pipe0_mau8_blk_lvl_int,
     sizeof(pipe0_mau8_blk_lvl_int) / sizeof(pipe0_mau8_blk_lvl_int[0])},
    {0x0,
     pipe0_mau9_blk_lvl_int,
     sizeof(pipe0_mau9_blk_lvl_int) / sizeof(pipe0_mau9_blk_lvl_int[0])},
    {0x0,
     pipe0_mau10_blk_lvl_int,
     sizeof(pipe0_mau10_blk_lvl_int) / sizeof(pipe0_mau10_blk_lvl_int[0])},
    {0x0,
     pipe0_mau11_blk_lvl_int,
     sizeof(pipe0_mau11_blk_lvl_int) / sizeof(pipe0_mau11_blk_lvl_int[0])},
    {0x0,
     pipe0_pbus_blk_lvl_int_dprsr,
     sizeof(pipe0_pbus_blk_lvl_int_dprsr) /
         sizeof(pipe0_pbus_blk_lvl_int_dprsr[0])},
    {0x0,
     pipe0_pbus_blk_lvl_int_ibp,
     sizeof(pipe0_pbus_blk_lvl_int_ibp) /
         sizeof(pipe0_pbus_blk_lvl_int_ibp[0])},
    {0x0,
     pipe0_pbus_blk_lvl_int_ebp,
     sizeof(pipe0_pbus_blk_lvl_int_ebp) /
         sizeof(pipe0_pbus_blk_lvl_int_ebp[0])},
    {0x0,
     pipe0_pbus_blk_lvl_int_egrNx,
     sizeof(pipe0_pbus_blk_lvl_int_egrNx) /
         sizeof(pipe0_pbus_blk_lvl_int_egrNx[0])},
    {0x0,
     pipe0_pbus_blk_lvl_int_pbusstat,
     sizeof(pipe0_pbus_blk_lvl_int_pbusstat) /
         sizeof(pipe0_pbus_blk_lvl_int_pbusstat[0])},
    {0x0,
     pipe0_pbus_blk_lvl_int_pgr,
     sizeof(pipe0_pbus_blk_lvl_int_pgr) /
         sizeof(pipe0_pbus_blk_lvl_int_pgr[0])},

    {0x0,
     pipe1_mau0_blk_lvl_int,
     sizeof(pipe1_mau0_blk_lvl_int) / sizeof(pipe1_mau0_blk_lvl_int[0])},
    {0x0,
     pipe1_mau1_blk_lvl_int,
     sizeof(pipe1_mau1_blk_lvl_int) / sizeof(pipe1_mau1_blk_lvl_int[0])},
    {0x0,
     pipe1_mau2_blk_lvl_int,
     sizeof(pipe1_mau2_blk_lvl_int) / sizeof(pipe1_mau2_blk_lvl_int[0])},
    {0x0,
     pipe1_mau3_blk_lvl_int,
     sizeof(pipe1_mau3_blk_lvl_int) / sizeof(pipe1_mau3_blk_lvl_int[0])},
    {0x0,
     pipe1_mau4_blk_lvl_int,
     sizeof(pipe1_mau4_blk_lvl_int) / sizeof(pipe1_mau4_blk_lvl_int[0])},
    {0x0,
     pipe1_mau5_blk_lvl_int,
     sizeof(pipe1_mau5_blk_lvl_int) / sizeof(pipe1_mau5_blk_lvl_int[0])},
    {0x0,
     pipe1_mau6_blk_lvl_int,
     sizeof(pipe1_mau6_blk_lvl_int) / sizeof(pipe1_mau6_blk_lvl_int[0])},
    {0x0,
     pipe1_mau7_blk_lvl_int,
     sizeof(pipe1_mau7_blk_lvl_int) / sizeof(pipe1_mau7_blk_lvl_int[0])},
    {0x0,
     pipe1_mau8_blk_lvl_int,
     sizeof(pipe1_mau8_blk_lvl_int) / sizeof(pipe1_mau8_blk_lvl_int[0])},
    {0x0,
     pipe1_mau9_blk_lvl_int,
     sizeof(pipe1_mau9_blk_lvl_int) / sizeof(pipe1_mau9_blk_lvl_int[0])},
    {0x0,
     pipe1_mau10_blk_lvl_int,
     sizeof(pipe1_mau10_blk_lvl_int) / sizeof(pipe1_mau10_blk_lvl_int[0])},
    {0x0,
     pipe1_mau11_blk_lvl_int,
     sizeof(pipe1_mau11_blk_lvl_int) / sizeof(pipe1_mau11_blk_lvl_int[0])},
    {0x0,
     pipe1_pbus_blk_lvl_int_dprsr,
     sizeof(pipe1_pbus_blk_lvl_int_dprsr) /
         sizeof(pipe1_pbus_blk_lvl_int_dprsr[0])},
    {0x0,
     pipe1_pbus_blk_lvl_int_ibp,
     sizeof(pipe1_pbus_blk_lvl_int_ibp) /
         sizeof(pipe1_pbus_blk_lvl_int_ibp[0])},
    {0x0,
     pipe1_pbus_blk_lvl_int_ebp,
     sizeof(pipe1_pbus_blk_lvl_int_ebp) /
         sizeof(pipe1_pbus_blk_lvl_int_ebp[0])},
    {0x0,
     pipe1_pbus_blk_lvl_int_egrNx,
     sizeof(pipe1_pbus_blk_lvl_int_egrNx) /
         sizeof(pipe1_pbus_blk_lvl_int_egrNx[0])},
    {0x0,
     pipe1_pbus_blk_lvl_int_pbusstat,
     sizeof(pipe1_pbus_blk_lvl_int_pbusstat) /
         sizeof(pipe1_pbus_blk_lvl_int_pbusstat[0])},
    {0x0,
     pipe1_pbus_blk_lvl_int_pgr,
     sizeof(pipe1_pbus_blk_lvl_int_pgr) /
         sizeof(pipe1_pbus_blk_lvl_int_pgr[0])},

    {0x0,
     pipe2_mau0_blk_lvl_int,
     sizeof(pipe2_mau0_blk_lvl_int) / sizeof(pipe2_mau0_blk_lvl_int[0])},
    {0x0,
     pipe2_mau1_blk_lvl_int,
     sizeof(pipe2_mau1_blk_lvl_int) / sizeof(pipe2_mau1_blk_lvl_int[0])},
    {0x0,
     pipe2_mau2_blk_lvl_int,
     sizeof(pipe2_mau2_blk_lvl_int) / sizeof(pipe2_mau2_blk_lvl_int[0])},
    {0x0,
     pipe2_mau3_blk_lvl_int,
     sizeof(pipe2_mau3_blk_lvl_int) / sizeof(pipe2_mau3_blk_lvl_int[0])},
    {0x0,
     pipe2_mau4_blk_lvl_int,
     sizeof(pipe2_mau4_blk_lvl_int) / sizeof(pipe2_mau4_blk_lvl_int[0])},
    {0x0,
     pipe2_mau5_blk_lvl_int,
     sizeof(pipe2_mau5_blk_lvl_int) / sizeof(pipe2_mau5_blk_lvl_int[0])},
    {0x0,
     pipe2_mau6_blk_lvl_int,
     sizeof(pipe2_mau6_blk_lvl_int) / sizeof(pipe2_mau6_blk_lvl_int[0])},
    {0x0,
     pipe2_mau7_blk_lvl_int,
     sizeof(pipe2_mau7_blk_lvl_int) / sizeof(pipe2_mau7_blk_lvl_int[0])},
    {0x0,
     pipe2_mau8_blk_lvl_int,
     sizeof(pipe2_mau8_blk_lvl_int) / sizeof(pipe2_mau8_blk_lvl_int[0])},
    {0x0,
     pipe2_mau9_blk_lvl_int,
     sizeof(pipe2_mau9_blk_lvl_int) / sizeof(pipe2_mau9_blk_lvl_int[0])},
    {0x0,
     pipe2_mau10_blk_lvl_int,
     sizeof(pipe2_mau10_blk_lvl_int) / sizeof(pipe2_mau10_blk_lvl_int[0])},
    {0x0,
     pipe2_mau11_blk_lvl_int,
     sizeof(pipe2_mau11_blk_lvl_int) / sizeof(pipe2_mau11_blk_lvl_int[0])},
    {0x0,
     pipe2_pbus_blk_lvl_int_dprsr,
     sizeof(pipe2_pbus_blk_lvl_int_dprsr) /
         sizeof(pipe2_pbus_blk_lvl_int_dprsr[0])},
    {0x0,
     pipe2_pbus_blk_lvl_int_ibp,
     sizeof(pipe2_pbus_blk_lvl_int_ibp) /
         sizeof(pipe2_pbus_blk_lvl_int_ibp[0])},
    {0x0,
     pipe2_pbus_blk_lvl_int_ebp,
     sizeof(pipe2_pbus_blk_lvl_int_ebp) /
         sizeof(pipe2_pbus_blk_lvl_int_ebp[0])},
    {0x0,
     pipe2_pbus_blk_lvl_int_egrNx,
     sizeof(pipe2_pbus_blk_lvl_int_egrNx) /
         sizeof(pipe2_pbus_blk_lvl_int_egrNx[0])},
    {0x0,
     pipe2_pbus_blk_lvl_int_pbusstat,
     sizeof(pipe2_pbus_blk_lvl_int_pbusstat) /
         sizeof(pipe2_pbus_blk_lvl_int_pbusstat[0])},
    {0x0,
     pipe2_pbus_blk_lvl_int_pgr,
     sizeof(pipe2_pbus_blk_lvl_int_pgr) /
         sizeof(pipe2_pbus_blk_lvl_int_pgr[0])},

    {0x0,
     pipe3_mau0_blk_lvl_int,
     sizeof(pipe3_mau0_blk_lvl_int) / sizeof(pipe3_mau0_blk_lvl_int[0])},
    {0x0,
     pipe3_mau1_blk_lvl_int,
     sizeof(pipe3_mau1_blk_lvl_int) / sizeof(pipe3_mau1_blk_lvl_int[0])},
    {0x0,
     pipe3_mau2_blk_lvl_int,
     sizeof(pipe3_mau2_blk_lvl_int) / sizeof(pipe3_mau2_blk_lvl_int[0])},
    {0x0,
     pipe3_mau3_blk_lvl_int,
     sizeof(pipe3_mau3_blk_lvl_int) / sizeof(pipe3_mau3_blk_lvl_int[0])},
    {0x0,
     pipe3_mau4_blk_lvl_int,
     sizeof(pipe3_mau4_blk_lvl_int) / sizeof(pipe3_mau4_blk_lvl_int[0])},
    {0x0,
     pipe3_mau5_blk_lvl_int,
     sizeof(pipe3_mau5_blk_lvl_int) / sizeof(pipe3_mau5_blk_lvl_int[0])},
    {0x0,
     pipe3_mau6_blk_lvl_int,
     sizeof(pipe3_mau6_blk_lvl_int) / sizeof(pipe3_mau6_blk_lvl_int[0])},
    {0x0,
     pipe3_mau7_blk_lvl_int,
     sizeof(pipe3_mau7_blk_lvl_int) / sizeof(pipe3_mau7_blk_lvl_int[0])},
    {0x0,
     pipe3_mau8_blk_lvl_int,
     sizeof(pipe3_mau8_blk_lvl_int) / sizeof(pipe3_mau8_blk_lvl_int[0])},
    {0x0,
     pipe3_mau9_blk_lvl_int,
     sizeof(pipe3_mau9_blk_lvl_int) / sizeof(pipe3_mau9_blk_lvl_int[0])},
    {0x0,
     pipe3_mau10_blk_lvl_int,
     sizeof(pipe3_mau10_blk_lvl_int) / sizeof(pipe3_mau10_blk_lvl_int[0])},
    {0x0,
     pipe3_mau11_blk_lvl_int,
     sizeof(pipe3_mau11_blk_lvl_int) / sizeof(pipe3_mau11_blk_lvl_int[0])},
    {0x0,
     pipe3_pbus_blk_lvl_int_dprsr,
     sizeof(pipe3_pbus_blk_lvl_int_dprsr) /
         sizeof(pipe3_pbus_blk_lvl_int_dprsr[0])},
    {0x0,
     pipe3_pbus_blk_lvl_int_ibp,
     sizeof(pipe3_pbus_blk_lvl_int_ibp) /
         sizeof(pipe3_pbus_blk_lvl_int_ibp[0])},
    {0x0,
     pipe3_pbus_blk_lvl_int_ebp,
     sizeof(pipe3_pbus_blk_lvl_int_ebp) /
         sizeof(pipe3_pbus_blk_lvl_int_ebp[0])},
    {0x0,
     pipe3_pbus_blk_lvl_int_egrNx,
     sizeof(pipe3_pbus_blk_lvl_int_egrNx) /
         sizeof(pipe3_pbus_blk_lvl_int_egrNx[0])},
    {0x0,
     pipe3_pbus_blk_lvl_int_pbusstat,
     sizeof(pipe3_pbus_blk_lvl_int_pbusstat) /
         sizeof(pipe3_pbus_blk_lvl_int_pbusstat[0])},
    {0x0,
     pipe3_pbus_blk_lvl_int_pgr,
     sizeof(pipe3_pbus_blk_lvl_int_pgr) /
         sizeof(pipe3_pbus_blk_lvl_int_pgr[0])},

    {0x0,
     cbus_lfltr0_blk_lvl_int,
     sizeof(cbus_lfltr0_blk_lvl_int) / sizeof(cbus_lfltr0_blk_lvl_int[0])},
    {0x0,
     cbus_lfltr1_blk_lvl_int,
     sizeof(cbus_lfltr1_blk_lvl_int) / sizeof(cbus_lfltr1_blk_lvl_int[0])},
    {0x0,
     cbus_lfltr2_blk_lvl_int,
     sizeof(cbus_lfltr2_blk_lvl_int) / sizeof(cbus_lfltr2_blk_lvl_int[0])},
    {0x0,
     cbus_lfltr3_blk_lvl_int,
     sizeof(cbus_lfltr3_blk_lvl_int) / sizeof(cbus_lfltr3_blk_lvl_int[0])},
    {0x0,
     cbus_wac_blk_lvl_int,
     sizeof(cbus_wac_blk_lvl_int) / sizeof(cbus_wac_blk_lvl_int[0])},
    {0x0,
     cbus_caa_blk_lvl_int,
     sizeof(cbus_caa_blk_lvl_int) / sizeof(cbus_caa_blk_lvl_int[0])},
    {0x0,
     cbus_qac_blk_lvl_int,
     sizeof(cbus_qac_blk_lvl_int) / sizeof(cbus_qac_blk_lvl_int[0])},
    {0x0,
     cbus_sch_blk_lvl_int,
     sizeof(cbus_sch_blk_lvl_int) / sizeof(cbus_sch_blk_lvl_int[0])},
    {0x0,
     cbus_clc_blk_lvl_int,
     sizeof(cbus_clc_blk_lvl_int) / sizeof(cbus_clc_blk_lvl_int[0])},
    {0x0,
     cbus_qlc_blk_lvl_int,
     sizeof(cbus_qlc_blk_lvl_int) / sizeof(cbus_qlc_blk_lvl_int[0])},
    {0x0,
     cbus_prc_blk_lvl_int,
     sizeof(cbus_prc_blk_lvl_int) / sizeof(cbus_prc_blk_lvl_int[0])},
    {0x0,
     cbus_pre_blk_lvl_int,
     sizeof(cbus_pre_blk_lvl_int) / sizeof(cbus_pre_blk_lvl_int[0])},
    {0x0,
     cbus_psc_blk_lvl_int,
     sizeof(cbus_psc_blk_lvl_int) / sizeof(cbus_psc_blk_lvl_int[0])},
    {0x0,
     cbus_cbc_blk_lvl_int,
     sizeof(cbus_cbc_blk_lvl_int) / sizeof(cbus_cbc_blk_lvl_int[0])},
    {0x0,
     mbus_blk_lvl_int,
     sizeof(mbus_blk_lvl_int) / sizeof(mbus_blk_lvl_int[0])},
    {0x0,
     mbus_blk_lvl_mbc_int,
     sizeof(mbus_blk_lvl_mbc_int) / sizeof(mbus_blk_lvl_mbc_int[0])},
};

static lld_blk_lvl_int_t *lld_int_find_blk_lvl_int(uint32_t offset) {
  int i, j;

  for (i = 0; i < (int)(sizeof(all_blk_lvl_ints) / sizeof(all_blk_lvl_ints[0]));
       i++) {
    for (j = 0; j < (int)(all_blk_lvl_ints[i].n); j++) {
      if (all_blk_lvl_ints[i].blk_lvl_int[j].status_reg == offset) {
        return (&all_blk_lvl_ints[i].blk_lvl_int[j]);
      }
    }
  }
  return NULL;
}
bf_status_t lld_tof_inject_ints_with_offset(bf_dev_id_t dev_id,
                                            uint32_t offset) {
  bf_status_t status = BF_SUCCESS;
  bf_subdev_id_t subdev_id = 0;
  lld_blk_lvl_int_t *blk_lvl_int = lld_int_find_blk_lvl_int(offset);
  if (blk_lvl_int != NULL) {
    status = lld_tof_inject_all_ints_cb(dev_id, subdev_id, blk_lvl_int);
  }
  return status;
}

bf_status_t lld_tof_int_traverse_blk_lvl_int(bf_dev_id_t dev_id,
                                             lld_blk_int_traverse_cb cb_fn) {
  int i, j;
  bf_status_t status = BF_SUCCESS;

  for (i = 0; i < (int)(sizeof(all_blk_lvl_ints) / sizeof(all_blk_lvl_ints[0]));
       i++) {
    for (j = 0; j < (int)(all_blk_lvl_ints[i].n); j++) {
      status |= cb_fn(dev_id, 0, (void *)&all_blk_lvl_ints[i].blk_lvl_int[j]);
    }
  }
  return status;
}

bf_status_t lld_tof_clear_all_ints_cb(bf_dev_id_t dev_id,
                                      bf_subdev_id_t subdev_id,
                                      void *blk_lvl_int_vd) {
  int i;
  lld_blk_lvl_int_t *blk_lvl_int = (lld_blk_lvl_int_t *)blk_lvl_int_vd;
  (void)subdev_id;
  for (i = 0; i < 32; i++) {
    if (blk_lvl_int->count[dev_id][i] != blk_lvl_int->count_shown[dev_id][i]) {
      blk_lvl_int->count_shown[dev_id][i] = blk_lvl_int->count[dev_id][i];
    }
  }
  return BF_SUCCESS;
}

bf_status_t lld_tof_clear_ints_cb(bf_dev_id_t dev_id,
                                  bf_subdev_id_t subdev_id,
                                  void *blk_lvl_int_vd) {
  int i, n_flds = 32;
  lld_blk_lvl_int_t *blk_lvl_int = (lld_blk_lvl_int_t *)blk_lvl_int_vd;
  (void)subdev_id;
  for (i = 0; i < n_flds; i++) {
    blk_lvl_int->count[dev_id][i] = 0;
  }
  return BF_SUCCESS;
}
int lld_tof_int_register_cb(bf_dev_id_t dev_id,
                            uint32_t offset,
                            lld_int_cb cb_fn,
                            void *userdata) {
  lld_blk_lvl_int_t *blk_lvl_int = lld_int_find_blk_lvl_int(offset);
  int overwrite = 0;

  if (blk_lvl_int != NULL) {
    if (blk_lvl_int->cb_fn[dev_id] != NULL) {
      overwrite = 1;
    }
    blk_lvl_int->cb_fn[dev_id] = cb_fn;
    blk_lvl_int->userdata[dev_id] = userdata;
    return overwrite;
  }
  return -1;
}

lld_int_cb lld_tof_get_int_cb(bf_dev_id_t dev_id,
                              uint32_t offset,
                              void **userdata) {
  lld_blk_lvl_int_t *blk_lvl_int = lld_int_find_blk_lvl_int(offset);

  if (blk_lvl_int != NULL) {
    *userdata = blk_lvl_int->userdata[dev_id];
    return blk_lvl_int->cb_fn[dev_id];
  }
  return NULL;
}

static bool lld_tof_reg_security_check(bf_dev_id_t dev_id, uint32_t reg) {
  uint32_t num_pipes;
  bf_dev_pipe_t pipe;
  bf_dev_pipe_t phy_pipe;
  lld_err_t rc = LLD_OK;
  bf_dev_port_t dev_port;
  if (lld_is_pipe_addr(dev_id, reg)) {
    lld_sku_get_num_active_pipes(dev_id, &num_pipes);
    phy_pipe = lld_get_pipe_from_addr(dev_id, reg);
    rc = lld_sku_map_phy_pipe_id_to_pipe_id(dev_id, phy_pipe, &pipe);
    if (rc != LLD_OK || pipe >= num_pipes) return false;
  } else if ((reg >= 0x1000000) && (reg < 0x1820000)) {
    /* MAC register.  Check for accesses to eth100_p1...p65. */
    int mac = ((reg - 0x1000000) / 0x20000);
    if (mac >= 0 && mac <= 64) {
      /* Ensure the MAC is on an enabled pipe. */
      rc = lld_sku_map_mac_ch_to_dev_port_id(dev_id, mac, 0, &dev_port);
      if (rc != LLD_OK) {
        return false;
      }
    }
  }
  return true;
}

bf_status_t lld_tof_enable_all_pipe_ints_cb(bf_dev_id_t dev_id,
                                            bf_subdev_id_t subdev_id,
                                            void *blk_lvl_int_vd) {
  lld_blk_lvl_int_t *blk_lvl_int = (lld_blk_lvl_int_t *)blk_lvl_int_vd;
  (void)subdev_id;
  if (!lld_tof_reg_security_check(dev_id, blk_lvl_int->enable_hi_reg)) {
    return BF_SUCCESS;
  }

  if (blk_lvl_int->enable_hi_reg != 0xffffffff) {
    lld_write_register(dev_id, blk_lvl_int->enable_hi_reg, 0xffffffff);
  }
  return BF_SUCCESS;
}
bf_status_t lld_tof_int_disable_all_cb(bf_dev_id_t dev_id,
                                       bf_subdev_id_t subdev_id,
                                       void *blk_lvl_int) {
  (void)subdev_id;

  if (!lld_tof_reg_security_check(
          dev_id, ((lld_blk_lvl_int_t *)blk_lvl_int)->enable_hi_reg)) {
    return BF_SUCCESS;
  }

  if (((lld_blk_lvl_int_t *)blk_lvl_int)->enable_hi_reg != 0xffffffff) {
    lld_write_register(
        dev_id, ((lld_blk_lvl_int_t *)blk_lvl_int)->enable_hi_reg, 0x0);
  }
  if (((lld_blk_lvl_int_t *)blk_lvl_int)->enable_lo_reg != 0xffffffff) {
    lld_write_register(
        dev_id, ((lld_blk_lvl_int_t *)blk_lvl_int)->enable_lo_reg, 0x0);
  }
  return BF_SUCCESS;
}

bf_status_t lld_tof_inject_all_ints_cb(bf_dev_id_t dev_id,
                                       bf_subdev_id_t subdev_id,
                                       void *blk_lvl_int_vd) {
  (void)subdev_id;
  if (((lld_blk_lvl_int_t *)blk_lvl_int_vd)->inject_reg != 0xffffffff) {
    lld_blk_lvl_int_t *blk_lvl_int = (lld_blk_lvl_int_t *)blk_lvl_int_vd;

    if (!lld_tof_reg_security_check(dev_id, blk_lvl_int->enable_hi_reg)) {
      return BF_SUCCESS;
    }

    /* Do not write parity error bits on interrupt registers,
       this will cause x86 hang
    */
    if ((blk_lvl_int->inject_reg ==
         DEF_tofino_device_select_cbc_cbc_cbus_int_inj_address)) {
      lld_write_register(dev_id, blk_lvl_int->inject_reg, 0x7fff);
    } else if (blk_lvl_int->inject_reg ==
               DEF_tofino_device_select_pbc_pbc_pbus_int_inj_address) {
      lld_write_register(dev_id, blk_lvl_int->inject_reg, 0x1ffffff);
    } else if (blk_lvl_int->inject_reg ==
               DEF_tofino_device_select_mbc_mbc_mbus_int_inj_address) {
      lld_write_register(dev_id, blk_lvl_int->inject_reg, 0xff);
    } else {
      lld_write_register(dev_id, blk_lvl_int->inject_reg, 0xffffffff);
    }
  }
  return BF_SUCCESS;
}
/****************************************************************************
 *
 * Shadow Interrupt registers:
 *
 *  31:0  : host : 1x 32b register
 *  63:32 : tbus : 1x 32b register
 * 127:64 : cbus : 2x 32b registers (2nd one unused)
 * 255:128: pbus : 4x 32b registers (one per-pipe)
 * 511:256: mbus : 8x 32b registers (0-4 used, 5-7 unused, only fist 140b used)
 *
 * pbus:
 *     0:0  : pipe0:mau0 low pri
 *     1:1  : pipe0:mau0 hi  pri
 *     2:2  : pipe0:mau1 low pri
 *     3:3  : pipe0:mau1 hi  pri
 *     ...
 *    22:22 : pipe0:mau11 low pri
 *    23:23 : pipe0:mau11 hi  pri
 *    24:24 : pipe0:prsr  low pri  :
 *tofino_pipes_pmarb_party_glue_reg_parde_intr_status0_address
 *    25:25 : pipe0:prsr  hi  pri  :
 *tofino_pipes_pmarb_party_glue_reg_parde_intr_status1_address
 *    26:26 : pipe0:dprsr low pri
 *    27:27 : pipe0:dprsr hi  pri
 *    29:28 : unused
 *    30:30 : pipe0 pbus ctrlr low pri
 *    31:31 : pipe0 pbus ctrlr hi  pri
 *
 *  [ similar for pipes 1-3 ]
 *
 *
 * 2nd level (hierarchical) interrupt registers for pbus.parde
 *
 * pbus.parde_intr_status0:
 *     0:0 : ibp                   :
 *tofino_pipes_pmarb_party_glue_reg_ibp18_intr_status0_address
 *     1:1 : parb
 *     2:2 : prsr_merge
 *     3:3 : pbusstat
 *     4:4 : pgr
 *     5:5 : party_glue
 *     6:6 : ebp                   :
 *tofino_pipes_pmarb_party_glue_reg_ebp18_intr_status0_address
 *     7:7 : egr                   :
 *tofino_pipes_pmarb_party_glue_reg_egr18_intr_status0_address
 *
 * pbus.parde_intr_status1:
 *     0:0 : ibp                   :
 *tofino_pipes_pmarb_party_glue_reg_ibp18_intr_status1_address
 *     1:1 : parb
 *     2:2 : prsr_merge
 *     3:3 : pbusstat
 *     4:4 : pgr
 *     5:5 : party_glue
 *     6:6 : ebp                   :
 *tofino_pipes_pmarb_party_glue_reg_ebp18_intr_status1_address
 *     7:7 : egr                   :
 *tofino_pipes_pmarb_party_glue_reg_egr18_intr_status1_address
 *
 * 3rd level (hierarchical) interrupt registers for pbus.parde.[ibp ebp egr]:
 *
 * For ibp, ebp, egr, each indirect stauts register contains 18b, one register
 *for low-pri, one for hi-pri
 *
 *    17:0 : sub-block instance <n>
 *
 *
 *
 * cbus:
 *     0:0  : lfltr0 low pri
 *     1:1  : lfltr0 hi  pri
 *     2:2  : lfltr1 low pri
 *     3:3  : lfltr1 hi  pri
 *     4:4  : lfltr2 low pri
 *     5:5  : lfltr2 hi  pri
 *     6:6  : lfltr3 low pri
 *     7:7  : lfltr3 hi  pri
 *     8:8  : wac    low pri
 *     9:9  : wac    hi  pri
 *    10:10 : caa    low pri
 *    11:11 : caa    hi  pri
 *    12:12 : qac    low pri
 *    13:13 : qac    hi  pri
 *    14:14 : sch    low pri
 *    15:15 : sch    hi  pri
 *    16:16 : clc    low pri
 *    17:17 : clc    hi  pri
 *    18:18 : qlc    low pri
 *    19:19 : qlc    hi  pri
 *    20:20 : prc    low pri
 *    21:21 : prc    hi  pri
 *    22:22 : pre    low pri
 *    23:23 : pre    hi  pri
 *    24:24 : psc    low pri
 *    25:25 : psc    hi  pri
 *    29:26 : unused
 *    30:30 : cbc    low pri
 *    31:31 : cbc    hi  pri
 *
 * mbus:
 *     0:0  : mac[0] low pri
 *     1:1  : mac[0] hi  pri
 *     2:2  : mac[1] low pri
 *     3:3  : mac[1] hi  pri
 *     ...
 *   138:138: mac[69] low pri
 *   139:139: mac[69] hi  pri
 *     ...    rest unused
 *
 */
bf_status_t lld_tof_int_poll(bf_dev_id_t dev_id, bool all_ints) {
  uint32_t glb_ints;
  uint32_t shadow_int_reg, shadow_msk_reg, sh_ints, n;

  lld_read_register(
      dev_id,
      DEF_tofino_device_select_pcie_bar01_regs_pcie_regs_glb_shadow_int_address,
      &glb_ints);
  if ((glb_ints == 0) && (all_ints == 0)) return BF_SUCCESS;

  shadow_int_reg =
      DEF_tofino_device_select_pcie_bar01_regs_pcie_regs_shadow_int_address;
  shadow_msk_reg =
      DEF_tofino_device_select_pcie_bar01_regs_pcie_regs_shadow_msk_address;

  for (n = 0; n < 16; n++) {
    if ((glb_ints & (1u << n)) == 0) continue;

    lld_write_register(dev_id,
                       shadow_msk_reg + (4 * n),
                       0xffffffff);  // mask all at this level
    lld_read_register(dev_id, shadow_int_reg + (4 * n), &sh_ints);

    if (all_ints) {  // pretend all are interrupting
      sh_ints = 0xffffffff;
    }
    if (sh_ints != 0) {
      switch (n) {
        case lld_tof_shadow_reg_host_bus_0:
          process_host_ints(dev_id, sh_ints);
          break;
        case lld_tof_shadow_reg_tbus_1:
          process_tbus_ints(dev_id, sh_ints);
          break;
        case lld_tof_shadow_reg_cbus_2:
        case lld_tof_shadow_reg_cbus_3:
          process_cbus_ints(dev_id, sh_ints, n - 2);
          break;
        case lld_tof_shadow_reg_pbus_4:
        case lld_tof_shadow_reg_pbus_5:
        case lld_tof_shadow_reg_pbus_6:
        case lld_tof_shadow_reg_pbus_7:
          process_pbus_ints(dev_id, sh_ints, n - 4, all_ints);
          break;
        case lld_tof_shadow_reg_mbus_8:
        case lld_tof_shadow_reg_mbus_9:
        case lld_tof_shadow_reg_mbus_10:
        case lld_tof_shadow_reg_mbus_11:
        case lld_tof_shadow_reg_mbus_12:
        case lld_tof_shadow_reg_mbus_13:
        case lld_tof_shadow_reg_mbus_14:
        case lld_tof_shadow_reg_mbus_15:
          process_mbus_ints(dev_id, sh_ints, n - 8);
          break;
      }
    }
    lld_write_register(
        dev_id, shadow_msk_reg + (4 * n), 0x0);  // un-mask all at this level
  }
  return BF_SUCCESS;
}

void process_host_ints(bf_dev_id_t dev_id, uint32_t sh_ints) {
  lld_blk_lvl_int_t *host_ints = &host_blk_lvl_int[0];
  int b;
  uint32_t pcie_ints, misc_ints, rc;

  pcie_ints = sh_ints & host_ints[0].shadow_mask;
  if (pcie_ints) {
    // count all first
    for (b = 0; b < 32; b++) {
      if ((1u << b) & pcie_ints) {
        host_ints[0].count[dev_id][b]++;
      }
    }
    if (host_ints[0].cb_fn[dev_id] != NULL) {
      rc = host_ints[0].cb_fn[dev_id](dev_id,
                                      0,
                                      host_ints[0].status_reg,
                                      pcie_ints,
                                      host_ints[0].enable_hi_reg,
                                      host_ints[0].enable_lo_reg,
                                      host_ints[0].userdata[dev_id]);
      (void)rc;  // no definition for return value (yet)
    }
  }

  // misc ints are indirect, so need to read misc int_stat
  misc_ints = sh_ints & host_ints[1].shadow_mask;
  if (misc_ints) {
    lld_read_register(dev_id, host_ints[1].status_reg, &misc_ints);
    // count all first
    for (b = 0; b < 32; b++) {
      if ((1u << b) & misc_ints) {
        host_ints[1].count[dev_id][b]++;
      }
    }
    if (host_ints[1].cb_fn[dev_id] != NULL) {
      rc = host_ints[1].cb_fn[dev_id](dev_id,
                                      0,
                                      host_ints[1].status_reg,
                                      misc_ints,
                                      host_ints[1].enable_hi_reg,
                                      host_ints[1].enable_lo_reg,
                                      host_ints[1].userdata[dev_id]);
      (void)rc;  // no definition for return value (yet)
    }
  }
}

void process_tbus_ints(bf_dev_id_t dev_id, uint32_t sh_ints) {
  uint32_t tbus_ints;

  tbus_ints = sh_ints & 0x3;  // only 1 stn id?
  if (tbus_ints) {
    int i;

    for (i = 0;
         i < (int)(sizeof(tbus_blk_lvl_int) / sizeof(tbus_blk_lvl_int[0]));
         i++) {
      lld_blk_lvl_int_t *tbus_blk_ints = &tbus_blk_lvl_int[0];
      int b;

      lld_read_register(dev_id, tbus_blk_ints[i].status_reg, &tbus_ints);
      // count all first
      for (b = 0; b < 32; b++) {
        if ((1u << b) & tbus_ints) {
          tbus_blk_ints[i].count[dev_id][b]++;
        }
      }
      if (tbus_blk_ints[i].cb_fn[dev_id] != NULL) {
        uint32_t rc;

        rc = tbus_blk_ints[i].cb_fn[dev_id](dev_id,
                                            0,
                                            tbus_blk_ints[i].status_reg,
                                            tbus_ints,
                                            tbus_blk_ints[i].enable_hi_reg,
                                            tbus_blk_ints[i].enable_lo_reg,
                                            tbus_blk_ints[i].userdata[dev_id]);
        (void)rc;  // no definition for return value (yet)
      }
    }
  }
}

void process_cbus_ints(bf_dev_id_t dev_id, uint32_t sh_ints, int n) {
  int i, j;

  if (n > 0) return;  // no ints defined in second word (yet)

  for (i = 0; i < (int)(sizeof(cbus_sh_ints) / sizeof(cbus_sh_ints[0])); i++) {
    lld_blk_lvl_int_t *cbus_blk_int = cbus_sh_ints[i].blk_lvl_int;

    if (sh_ints & cbus_sh_ints[i].mask) {
      for (j = 0; j < (int)cbus_sh_ints[i].n; j++) {
        int b;
        uint32_t cbus_ints;

        lld_read_register(dev_id, cbus_blk_int[j].status_reg, &cbus_ints);
        // count all first
        for (b = 0; b < 32; b++) {
          if ((1u << b) & cbus_ints) {
            cbus_blk_int[j].count[dev_id][b]++;
          }
        }
        if (cbus_blk_int[j].cb_fn[dev_id] != NULL) {
          uint32_t rc;

          rc = cbus_blk_int[j].cb_fn[dev_id](dev_id,
                                             0,
                                             cbus_blk_int[j].status_reg,
                                             cbus_ints,
                                             cbus_blk_int[j].enable_hi_reg,
                                             cbus_blk_int[j].enable_lo_reg,
                                             cbus_blk_int[j].userdata[dev_id]);
          (void)rc;  // no definition for return value (yet)
        }
      }
    }
  }
  (void)dev_id;
  (void)sh_ints;
  (void)n;
}

void process_pbus_ints(bf_dev_id_t dev_id,
                       uint32_t sh_ints,
                       int n,
                       int all_ints) {
  int i, j, k, sh_int_list_len;
  lld_dev_t *dev_p = lld_map_dev_id_to_dev_p(dev_id);
  uint32_t num_pipes = 0, num_stages = 0;
  uint32_t logical_pipe_nbr;
  bf_dev_pipe_t pipe = n;
  uint32_t base = pipe * DEF_tofino_pipes_array_element_size;

  if (!dev_p) {
    lld_log("%s:%d ERROR: Failed to get dev_p for dev_id <%d>",
            __func__,
            __LINE__,
            dev_id);
    return;
  }

  lld_sku_get_num_active_pipes(dev_id, &num_pipes);
  // for tofino, all pipes have the same number of active mau stages
  lld_sku_get_num_active_mau_stages(dev_id, &num_stages, 0);

  // convert phy->log pipe nbr
  for (logical_pipe_nbr = 0; logical_pipe_nbr < num_pipes; logical_pipe_nbr++) {
    if (dev_p->pipe_log2phy[logical_pipe_nbr] == (uint32_t)pipe) {
      break;
    }
  }
  if (logical_pipe_nbr >= num_pipes) return;  // process only valid pipe regs

  switch (pipe) {
    case 0:
      sh_int_list_len =
          (int)(sizeof(pipe0_pbus_sh_ints) / sizeof(pipe0_pbus_sh_ints[0]));
      break;
    case 1:
      sh_int_list_len =
          (int)(sizeof(pipe1_pbus_sh_ints) / sizeof(pipe1_pbus_sh_ints[0]));
      break;
    case 2:
      sh_int_list_len =
          (int)(sizeof(pipe2_pbus_sh_ints) / sizeof(pipe2_pbus_sh_ints[0]));
      break;
    case 3:
      sh_int_list_len =
          (int)(sizeof(pipe3_pbus_sh_ints) / sizeof(pipe3_pbus_sh_ints[0]));
      break;
    default:
      lld_log("ERROR: invalid pipe <%d>", pipe);
      sh_int_list_len = 0;
      break;
  }

  for (i = 0; i < sh_int_list_len; i++) {
    lld_blk_lvl_int_t *pbus_blk_int = NULL;
    uint32_t sh_mask = 0;

    switch (pipe) {
      case 0:
        pbus_blk_int = pipe0_pbus_sh_ints[i].blk_lvl_int;
        sh_mask = pipe0_pbus_sh_ints[i].mask;
        break;
      case 1:
        pbus_blk_int = pipe1_pbus_sh_ints[i].blk_lvl_int;
        sh_mask = pipe1_pbus_sh_ints[i].mask;
        break;
      case 2:
        pbus_blk_int = pipe2_pbus_sh_ints[i].blk_lvl_int;
        sh_mask = pipe2_pbus_sh_ints[i].mask;
        break;
      case 3:
        pbus_blk_int = pipe3_pbus_sh_ints[i].blk_lvl_int;
        sh_mask = pipe3_pbus_sh_ints[i].mask;
        break;
    }

    if (sh_ints & sh_mask) {
      int b;
      uint32_t pbus_ints, pbus_ints_both;

      if (pbus_blk_int == NULL) {  // special-case multi-level parser ints
        blk_lvl_int_list_t *pbus_prsr_int_list = NULL;
        int prsr_list_len;

        switch (pipe) {
          case 0:
            pbus_prsr_int_list = pipe0_pbus_prsr_ints;
            prsr_list_len = (int)(sizeof(pipe0_pbus_prsr_ints) /
                                  sizeof(pipe0_pbus_prsr_ints[0]));
            break;
          case 1:
            pbus_prsr_int_list = pipe1_pbus_prsr_ints;
            prsr_list_len = (int)(sizeof(pipe1_pbus_prsr_ints) /
                                  sizeof(pipe1_pbus_prsr_ints[0]));
            break;
          case 2:
            pbus_prsr_int_list = pipe2_pbus_prsr_ints;
            prsr_list_len = (int)(sizeof(pipe2_pbus_prsr_ints) /
                                  sizeof(pipe2_pbus_prsr_ints[0]));
            break;
          case 3:
            pbus_prsr_int_list = pipe3_pbus_prsr_ints;
            prsr_list_len = (int)(sizeof(pipe3_pbus_prsr_ints) /
                                  sizeof(pipe3_pbus_prsr_ints[0]));
            break;
        }

        lld_read_register(
            dev_id,
            base +
                DEF_tofino_pipes_pmarb_party_glue_reg_parde_intr_status0_address,
            &pbus_ints);
        lld_read_register(
            dev_id,
            base +
                DEF_tofino_pipes_pmarb_party_glue_reg_parde_intr_status1_address,
            &pbus_ints_both);

        pbus_ints_both |= pbus_ints;
        if (all_ints) {
          pbus_ints_both = 0xffffffff;
        }

        for (j = 0; j < prsr_list_len; j++) {
          lld_blk_lvl_int_t *pbus_blk_lvl2_int =
              pbus_prsr_int_list[j].blk_lvl_int;

          if (pbus_ints_both & pbus_prsr_int_list[j].mask) {
            if (j == 0) {  // ibp
              uint32_t ibp_ints, ibp_ints_both;
              lld_blk_lvl_int_t *pbus_blk_lvl3_int = NULL;

              switch (pipe) {
                case 0:
                  pbus_blk_lvl3_int = pipe0_pbus_blk_lvl_int_ibp;
                  break;
                case 1:
                  pbus_blk_lvl3_int = pipe1_pbus_blk_lvl_int_ibp;
                  break;
                case 2:
                  pbus_blk_lvl3_int = pipe2_pbus_blk_lvl_int_ibp;
                  break;
                case 3:
                  pbus_blk_lvl3_int = pipe3_pbus_blk_lvl_int_ibp;
                  break;
              }

              lld_read_register(
                  dev_id,
                  base +
                      DEF_tofino_pipes_pmarb_party_glue_reg_ibp18_intr_status0_address,
                  &ibp_ints);
              lld_read_register(
                  dev_id,
                  base +
                      DEF_tofino_pipes_pmarb_party_glue_reg_ibp18_intr_status1_address,
                  &ibp_ints_both);

              ibp_ints_both |= ibp_ints;
              if (all_ints) {
                ibp_ints_both = 0xffffffff;
              }
              for (k = 0;
                   ibp_ints_both != 0 &&
                   k < (int)
                           DEF_tofino_pipes_pmarb_ibp18_reg_ibp_reg_array_count;
                   k++) {
                if ((1u << k) & ibp_ints_both) {
                  int r;
                  uint32_t instance_ints, entry_base;
                  int entries_per_instance =
                      (int)((sizeof(pipe0_pbus_blk_lvl_int_ibp) /
                             DEF_tofino_pipes_pmarb_ibp18_reg_ibp_reg_array_count) /
                            sizeof(pipe0_pbus_blk_lvl_int_ibp[0]));

                  entry_base = k * entries_per_instance;
                  for (r = 0; r < entries_per_instance; r++) {
                    int e = entry_base + r;

                    lld_read_register(dev_id,
                                      pbus_blk_lvl3_int[e].status_reg,
                                      &instance_ints);
                    if (all_ints) {
                      instance_ints = 0xffffffff;
                    } else if (!instance_ints) {
                      continue;
                    }
                    // count all first
                    for (b = 0; b < 32; b++) {
                      if ((1u << b) & instance_ints) {
                        pbus_blk_lvl3_int[e].count[dev_id][b]++;
                      }
                    }
                    if (pbus_blk_lvl3_int[e].cb_fn[dev_id] != NULL) {
                      uint32_t rc;

                      rc = pbus_blk_lvl3_int[e].cb_fn[dev_id](
                          dev_id,
                          0,
                          pbus_blk_lvl3_int[e].status_reg,
                          instance_ints,
                          pbus_blk_lvl3_int[e].enable_hi_reg,
                          pbus_blk_lvl3_int[e].enable_lo_reg,
                          pbus_blk_lvl3_int[e].userdata[dev_id]);
                      (void)rc;  // no definition for return value (yet)
                    }
                  }
                }
              }
            } else if (j == 3) {  // ebp
              uint32_t ebp_ints, ebp_ints_both;
              lld_blk_lvl_int_t *pbus_blk_lvl3_int = NULL;

              switch (pipe) {
                case 0:
                  pbus_blk_lvl3_int = pipe0_pbus_blk_lvl_int_ebp;
                  break;
                case 1:
                  pbus_blk_lvl3_int = pipe1_pbus_blk_lvl_int_ebp;
                  break;
                case 2:
                  pbus_blk_lvl3_int = pipe2_pbus_blk_lvl_int_ebp;
                  break;
                case 3:
                  pbus_blk_lvl3_int = pipe3_pbus_blk_lvl_int_ebp;
                  break;
              }

              lld_read_register(
                  dev_id,
                  base +
                      DEF_tofino_pipes_pmarb_party_glue_reg_ebp18_intr_status0_address,
                  &ebp_ints);
              lld_read_register(
                  dev_id,
                  base +
                      DEF_tofino_pipes_pmarb_party_glue_reg_ebp18_intr_status1_address,
                  &ebp_ints_both);

              ebp_ints_both |= ebp_ints;
              if (all_ints) {
                ebp_ints_both = 0xffffffff;
              }
              for (k = 0;
                   ebp_ints_both != 0 &&
                   k < (int)
                           DEF_tofino_pipes_pmarb_ebp18_reg_ebp_reg_array_count;
                   k++) {
                if ((1u << k) & ebp_ints_both) {
                  int r;
                  uint32_t instance_ints, entry_base;
                  int entries_per_instance =
                      (int)((sizeof(pipe0_pbus_blk_lvl_int_ebp) /
                             DEF_tofino_pipes_pmarb_ebp18_reg_ebp_reg_array_count) /
                            sizeof(pipe0_pbus_blk_lvl_int_ebp[0]));

                  entry_base = k * entries_per_instance;
                  for (r = 0; r < entries_per_instance; r++) {
                    int e = entry_base + r;

                    lld_read_register(dev_id,
                                      pbus_blk_lvl3_int[e].status_reg,
                                      &instance_ints);
                    if (all_ints) {
                      instance_ints = 0xffffffff;
                    } else if (!instance_ints) {
                      continue;
                    }
                    // count all first
                    for (b = 0; b < 32; b++) {
                      if ((1u << b) & instance_ints) {
                        pbus_blk_lvl3_int[e].count[dev_id][b]++;
                      }
                    }
                    if (pbus_blk_lvl3_int[e].cb_fn[dev_id] != NULL) {
                      uint32_t rc;

                      rc = pbus_blk_lvl3_int[e].cb_fn[dev_id](
                          dev_id,
                          0,
                          pbus_blk_lvl3_int[e].status_reg,
                          instance_ints,
                          pbus_blk_lvl3_int[e].enable_hi_reg,
                          pbus_blk_lvl3_int[e].enable_lo_reg,
                          pbus_blk_lvl3_int[e].userdata[dev_id]);
                      (void)rc;  // no definition for return value (yet)
                    }
                  }
                }
              }
            } else if (j == 4) {  // egr
              uint32_t egr_ints, egr_ints_both;
              lld_blk_lvl_int_t *pbus_blk_lvl3_int = NULL;

              switch (pipe) {
                case 0:
                  pbus_blk_lvl3_int = pipe0_pbus_blk_lvl_int_egrNx;
                  break;
                case 1:
                  pbus_blk_lvl3_int = pipe1_pbus_blk_lvl_int_egrNx;
                  break;
                case 2:
                  pbus_blk_lvl3_int = pipe2_pbus_blk_lvl_int_egrNx;
                  break;
                case 3:
                  pbus_blk_lvl3_int = pipe3_pbus_blk_lvl_int_egrNx;
                  break;
              }

              lld_read_register(
                  dev_id,
                  base +
                      DEF_tofino_pipes_pmarb_party_glue_reg_egr18_intr_status0_address,
                  &egr_ints);
              lld_read_register(
                  dev_id,
                  base +
                      DEF_tofino_pipes_pmarb_party_glue_reg_egr18_intr_status1_address,
                  &egr_ints_both);

              egr_ints_both |= egr_ints;
              if (all_ints) {
                egr_ints_both = 0xffffffff;
              }
              for (
                  k = 0;
                  egr_ints_both != 0 &&
                  k < (int)
                          DEF_tofino_pipes_pmarb_ebp18_reg_egrNx_reg_array_count;
                  k++) {
                if ((1u << k) & egr_ints_both) {
                  int r;
                  uint32_t instance_ints, entry_base;
                  int entries_per_instance =
                      (int)((sizeof(pipe0_pbus_blk_lvl_int_egrNx) /
                             DEF_tofino_pipes_pmarb_ebp18_reg_egrNx_reg_array_count) /
                            sizeof(pipe0_pbus_blk_lvl_int_egrNx[0]));

                  entry_base = k * entries_per_instance;
                  for (r = 0; r < entries_per_instance; r++) {
                    int e = entry_base + r;

                    lld_read_register(dev_id,
                                      pbus_blk_lvl3_int[e].status_reg,
                                      &instance_ints);
                    if (all_ints) {
                      instance_ints = 0xffffffff;
                    } else if (!instance_ints) {
                      continue;
                    }
                    // count all first
                    for (b = 0; b < 32; b++) {
                      if ((1u << b) & instance_ints) {
                        pbus_blk_lvl3_int[e].count[dev_id][b]++;
                      }
                    }
                    if (pbus_blk_lvl3_int[e].cb_fn[dev_id] != NULL) {
                      uint32_t rc;

                      rc = pbus_blk_lvl3_int[e].cb_fn[dev_id](
                          dev_id,
                          0,
                          pbus_blk_lvl3_int[e].status_reg,
                          instance_ints,
                          pbus_blk_lvl3_int[e].enable_hi_reg,
                          pbus_blk_lvl3_int[e].enable_lo_reg,
                          pbus_blk_lvl3_int[e].userdata[dev_id]);
                      (void)rc;  // no definition for return value (yet)
                    }
                  }
                }
              }
            } else {  // no more levels
              uint32_t prsr_ints;
              k = 0;

              lld_read_register(
                  dev_id, pbus_blk_lvl2_int[k].status_reg, &prsr_ints);
              if (all_ints) {
                prsr_ints = 0xffffffff;
              } else if (!prsr_ints) {
                continue;
              }
              // count all first
              for (b = 0; b < 32; b++) {
                if ((1u << b) & prsr_ints) {
                  pbus_blk_lvl2_int[k].count[dev_id][b]++;
                }
              }
              if (pbus_blk_lvl2_int[k].cb_fn[dev_id] != NULL) {
                uint32_t rc;

                rc = pbus_blk_lvl2_int[k].cb_fn[dev_id](
                    dev_id,
                    0,
                    pbus_blk_lvl2_int[k].status_reg,
                    prsr_ints,
                    pbus_blk_lvl2_int[k].enable_hi_reg,
                    pbus_blk_lvl2_int[k].enable_lo_reg,
                    pbus_blk_lvl2_int[k].userdata[dev_id]);
                (void)rc;  // no definition for return value (yet)
              }
            }
          }
        }
      } else {
        int pbus_sh_int_list_len = 0;

        switch (pipe) {
          case 0:
            pbus_sh_int_list_len = (int)pipe0_pbus_sh_ints[i].n;
            break;
          case 1:
            pbus_sh_int_list_len = (int)pipe1_pbus_sh_ints[i].n;
            break;
          case 2:
            pbus_sh_int_list_len = (int)pipe2_pbus_sh_ints[i].n;
            break;
          case 3:
            pbus_sh_int_list_len = (int)pipe3_pbus_sh_ints[i].n;
            break;
        }

        for (j = 0; j < pbus_sh_int_list_len; j++) {
          uint32_t addr_pipe, addr_stage, addr_is_pipe;

          // make sure pipe and stage are present
          // note: pbus controller (pb) is included in the pbus
          //       ints but is actually a device_select address.
          addr_is_pipe = (pbus_blk_int[j].status_reg >> 25) & 1;
          if (addr_is_pipe) {
            addr_pipe = (pbus_blk_int[j].status_reg >> 23) & 0x3;
            addr_stage = (pbus_blk_int[j].status_reg >> 19) & 0xf;
            if (addr_pipe > 3) continue;
            if (addr_stage < 12) {
              if (addr_stage > num_stages)
                continue;  // filter for different rtl builds
            }
            // else assume its prsr (0xe or dprsr (0xf)
          }
          lld_read_register(dev_id, pbus_blk_int[j].status_reg, &pbus_ints);
          if (all_ints) {
            pbus_ints = 0xffffffff;
          } else if (!pbus_ints) {
            continue;
          }
          // count all first
          for (b = 0; b < 32; b++) {
            if ((1u << b) & pbus_ints) {
              pbus_blk_int[j].count[dev_id][b]++;
            }
          }
          if (pbus_blk_int[j].cb_fn[dev_id] != NULL) {
            uint32_t rc;

            rc =
                pbus_blk_int[j].cb_fn[dev_id](dev_id,
                                              0,
                                              pbus_blk_int[j].status_reg,
                                              pbus_ints,
                                              pbus_blk_int[j].enable_hi_reg,
                                              pbus_blk_int[j].enable_lo_reg,
                                              pbus_blk_int[j].userdata[dev_id]);
            (void)rc;  // no definition for return value (yet)
          }
        }
      }
    }
  }
}

void process_mbus_ints(bf_dev_id_t dev_id, uint32_t sh_ints, int n) {
  int i, j;

  for (i = 0; i < (int)(sizeof(mbus_sh_ints) / sizeof(mbus_sh_ints[0])); i++) {
    lld_blk_lvl_int_t *mbus_blk_int = mbus_sh_ints[i].blk_lvl_int;

    if (sh_ints & mbus_sh_ints[i].mask) {
      for (j = n * 16; j < ((n * 16) + 16); j++) {
        int b;
        uint32_t mbus_ints;

        if (j > 67) break;

        if (sh_ints & mbus_blk_int[j].shadow_mask) {
          lld_read_register(dev_id, mbus_blk_int[j].status_reg, &mbus_ints);
          // count all first
          for (b = 0; b < 32; b++) {
            if ((1u << b) & mbus_ints) {
              // bit 20 indicates an interrupt from the umac
              // note: dont bother counting these as they are really
              //       just an indirection to a lower layer of bits
              if (b == 20) {  // Comira interrupt
                int ch;

                for (ch = 0; ch < 4; ch++) {
                  if (lld_ctx->mac_int_poll_cb) {
                    lld_ctx->mac_int_poll_cb(dev_id, j /*mac_block*/, ch);
                  }
                }
              } else {
                mbus_blk_int[j].count[dev_id][b]++;
              }
            }
          }
          if (mbus_blk_int[j].cb_fn[dev_id] != NULL) {
            uint32_t rc;

            rc =
                mbus_blk_int[j].cb_fn[dev_id](dev_id,
                                              0,
                                              mbus_blk_int[j].status_reg,
                                              mbus_ints,
                                              mbus_blk_int[j].enable_hi_reg,
                                              mbus_blk_int[j].enable_lo_reg,
                                              mbus_blk_int[j].userdata[dev_id]);
            (void)rc;  // no definition for return value (yet)
          }
        }
      }
    }
  }

  (void)dev_id;
  (void)sh_ints;
  (void)n;
}

uint32_t lld_tof_int_get_glb_status(bf_dev_id_t dev_id) {
  uint32_t glb_ints;

  lld_read_register(
      dev_id,
      DEF_tofino_device_select_pcie_bar01_regs_pcie_regs_glb_shadow_int_address,
      &glb_ints);
  return glb_ints;
}

uint32_t lld_tof_int_get_shadow_int_status(bf_dev_id_t dev_id,
                                           uint16_t sh_int_reg) {
  uint32_t shadow_int, shadow_int_reg;

  shadow_int_reg =
      (4 * sh_int_reg) +
      DEF_tofino_device_select_pcie_bar01_regs_pcie_regs_shadow_int_address;

  lld_read_register(dev_id, shadow_int_reg, &shadow_int);
  return shadow_int;
}

uint32_t lld_tof_int_get_shadow_msk_status(bf_dev_id_t dev_id,
                                           uint16_t sh_msk_reg) {
  uint32_t shadow_msk, shadow_msk_reg;

  shadow_msk_reg =
      (4 * sh_msk_reg) +
      DEF_tofino_device_select_pcie_bar01_regs_pcie_regs_shadow_msk_address;

  lld_read_register(dev_id, shadow_msk_reg, &shadow_msk);
  return shadow_msk;
}

void lld_tof_int_set_shadow_msk_status(bf_dev_id_t dev_id,
                                       uint16_t sh_msk_reg,
                                       uint32_t value) {
  uint32_t shadow_msk_reg;

  shadow_msk_reg =
      (4 * sh_msk_reg) +
      DEF_tofino_device_select_pcie_bar01_regs_pcie_regs_shadow_msk_address;

  lld_write_register(dev_id, shadow_msk_reg, value);
  return;
}
void lld_tof_int_svc(bf_dev_id_t dev_id,
                     uint32_t sh_int_val,
                     uint16_t sh_int_reg) {
  // call the appropriate service fn
  switch (sh_int_reg) {
    case lld_tof_shadow_reg_host_bus_0:
      process_host_ints(dev_id, sh_int_val);
      break;
    case lld_tof_shadow_reg_tbus_1:
      process_tbus_ints(dev_id, sh_int_val);
      break;
    case lld_tof_shadow_reg_cbus_2:
    case lld_tof_shadow_reg_cbus_3:
      process_cbus_ints(dev_id, sh_int_val, sh_int_reg - 2);
      break;
    case lld_tof_shadow_reg_pbus_4:
    case lld_tof_shadow_reg_pbus_5:
    case lld_tof_shadow_reg_pbus_6:
    case lld_tof_shadow_reg_pbus_7:
      process_pbus_ints(dev_id, sh_int_val, sh_int_reg - 4, false);
      break;
    case lld_tof_shadow_reg_mbus_8:
    case lld_tof_shadow_reg_mbus_9:
    case lld_tof_shadow_reg_mbus_10:
    case lld_tof_shadow_reg_mbus_11:
    case lld_tof_shadow_reg_mbus_12:
    case lld_tof_shadow_reg_mbus_13:
    case lld_tof_shadow_reg_mbus_14:
    case lld_tof_shadow_reg_mbus_15:
      process_mbus_ints(dev_id, sh_int_val, sh_int_reg - 8);
      break;
  }
  return;
}
void lld_tof_int_ena(bf_dev_id_t dev_id,
                     uint32_t sh_int_reg,
                     uint32_t sh_int_bit) {
  uint32_t bit_fld = (1u << sh_int_bit);
  uint32_t shadow_msk, shadow_msk_reg;

  shadow_msk_reg =
      (4 * sh_int_reg) +
      DEF_tofino_device_select_pcie_bar01_regs_pcie_regs_shadow_msk_address;

  // un-mask the interrupt
  lld_read_register(dev_id, shadow_msk_reg, &shadow_msk);
  lld_write_register(dev_id, shadow_msk_reg, (shadow_msk & ~bit_fld));
  return;
}

void lld_tof_int_msk(bf_dev_id_t dev_id,
                     uint32_t sh_int_reg,
                     uint32_t sh_int_bit) {
  uint32_t bit_fld = (1u << sh_int_bit);
  uint32_t shadow_msk, shadow_msk_reg;

  shadow_msk_reg =
      (4 * sh_int_reg) +
      DEF_tofino_device_select_pcie_bar01_regs_pcie_regs_shadow_msk_address;

  // mask off the interrupt
  lld_read_register(dev_id, shadow_msk_reg, &shadow_msk);
  lld_write_register(dev_id, shadow_msk_reg, (shadow_msk | bit_fld));
  return;
}
/** \brief Set or clear hierarchical interrupt masks
 *
 * \param dev_id : bf_dev_id_t : system-assigned identifier
 *(0..BF_MAX_DEV_COUNT-1)
 * \param en     : bool        : true=enable, false=disable
 *
 * \return: nothing
 *
 */
void lld_tof_int_gbl_en_set(bf_dev_id_t dev_id, bool en) {
  uint32_t i;
  for (
      i = 0;
      i <
      DEF_tofino_device_select_pcie_bar01_regs_pcie_regs_shadow_msk_array_count;
      i++) {
    lld_write_register(
        dev_id,
        DEF_tofino_device_select_pcie_bar01_regs_pcie_regs_shadow_msk_address +
            (4 * i),
        en ? 0 : 0xffffffff);
  }
}
static void lld_tof_int_leaf_enable_set_cb(bf_dev_id_t dev_id,
                                    lld_blk_lvl_int_t *blk_lvl_int,
                                    bool en) {
  if (blk_lvl_int->enable_hi_reg != 0xffffffff) {
    lld_write_register(dev_id, blk_lvl_int->enable_hi_reg, en ? 0xffffffff : 0);
  } else if (blk_lvl_int->enable_lo_reg != 0xffffffff) {
    lld_write_register(dev_id, blk_lvl_int->enable_lo_reg, en ? 0xffffffff : 0);
  }
}

static void lld_tof_int_leaf_enable_set(bf_dev_id_t dev_id,
                                 lld_blk_lvl_int_t *blk_lvl_int,
                                 int n_entries,
                                 bool en) {
  int i;

  for (i = 0; i < n_entries; i++) {
    lld_tof_int_leaf_enable_set_cb(dev_id, &blk_lvl_int[i], en);
  }
}

void lld_tof_int_host_leaf_enable_set(bf_dev_id_t dev_id, bool en) {
  lld_tof_int_leaf_enable_set(
      dev_id,
      host_blk_lvl_int,
      (int)sizeof(host_blk_lvl_int) / sizeof(host_blk_lvl_int[0]),
      en);
}

void lld_tof_int_mbus_leaf_enable_set(bf_dev_id_t dev_id, bool en) {
  lld_tof_int_leaf_enable_set(
      dev_id,
      mbus_blk_lvl_int,
      (int)sizeof(mbus_blk_lvl_int) / sizeof(mbus_blk_lvl_int[0]),
      en);
}
uint32_t lld_tof_get_status_reg(void *blk_lvl_int) {
  if (blk_lvl_int != NULL) {
    return (((lld_blk_lvl_int_t *)blk_lvl_int)->status_reg);
  }
  return -1;
}
#ifndef __KERNEL__
extern int dump_field_name_by_offset(bf_dev_id_t dev_id,
                                     uint32_t target_offset,
                                     int bit);

bf_status_t lld_tof_dump_int_list_cb(bf_dev_id_t dev_id,
                                     bf_subdev_id_t subdev_id,
                                     void *blk_lvl_int_vd) {
  int i;
  lld_blk_lvl_int_t *blk_lvl_int = (lld_blk_lvl_int_t *)blk_lvl_int_vd;
  (void)subdev_id;
  char *path = get_full_reg_path_name(dev_id, blk_lvl_int->status_reg);
  fprintf(rl_outstream,
          "%08x : %08x : %08x : %08x : %p : %s\n",
          blk_lvl_int->status_reg,
          blk_lvl_int->enable_hi_reg,
          blk_lvl_int->enable_lo_reg,
          blk_lvl_int->shadow_mask,
          blk_lvl_int->userdata[dev_id],
          (path) ? path : "NONE");

  for (i = 0; i < 32; i++) {
    if (blk_lvl_int->count[dev_id][i] != blk_lvl_int->count_shown[dev_id][i]) {
      int rc;

      fprintf(rl_outstream,
              ": [%2d:%2d] : %d : ",
              i,
              i,
              blk_lvl_int->count[dev_id][i]);
      rc = dump_field_name_by_offset((int)dev_id, blk_lvl_int->status_reg, i);
      fprintf(rl_outstream, "\n");
      // update "shown"
      blk_lvl_int->count_shown[dev_id][i] = blk_lvl_int->count[dev_id][i];
      if (rc != 0) break;
    }
  }
  return BF_SUCCESS;
}
bf_status_t lld_tof_dump_new_ints_cb(bf_dev_id_t dev_id,
                                     bf_subdev_id_t subdev_id,
                                     void *blk_lvl_int_vd) {
  int i, dump = 0;
  lld_blk_lvl_int_t *blk_lvl_int = (lld_blk_lvl_int_t *)blk_lvl_int_vd;
  (void)subdev_id;
  for (i = 0; i < 32; i++) {
    if (blk_lvl_int->count[dev_id][i] != blk_lvl_int->count_shown[dev_id][i]) {
      dump = 1;
    }
  }
  if (dump == 0) return BF_SUCCESS;

  char *path = get_full_reg_path_name(dev_id, blk_lvl_int->status_reg);
  fprintf(rl_outstream,
          "%08x : %08x : %08x : %08x : %p : %s\n",
          blk_lvl_int->status_reg,
          blk_lvl_int->enable_hi_reg,
          blk_lvl_int->enable_lo_reg,
          blk_lvl_int->shadow_mask,
          blk_lvl_int->userdata[dev_id],
          (path) ? path : "NONE");

  for (i = 0; i < 32; i++) {
    if (blk_lvl_int->count[dev_id][i] != blk_lvl_int->count_shown[dev_id][i]) {
      fprintf(
          rl_outstream,
          ": [%2d:%2d] : %d : ",
          i,
          i,
          blk_lvl_int->count[dev_id][i] - blk_lvl_int->count_shown[dev_id][i]);
      dump_field_name_by_offset((int)dev_id, blk_lvl_int->status_reg, i);
      fprintf(rl_outstream, "\n");
      // update "shown"
      blk_lvl_int->count_shown[dev_id][i] = blk_lvl_int->count[dev_id][i];
    }
  }
  return BF_INVALID_ARG;
}

bf_status_t lld_tof_clear_new_ints_cb(bf_dev_id_t dev_id,
                                      bf_subdev_id_t subdev_id,
                                      void *blk_lvl_int_vd) {
  int i;
  lld_blk_lvl_int_t *blk_lvl_int = (lld_blk_lvl_int_t *)blk_lvl_int_vd;
  (void)subdev_id;

  for (i = 0; i < 32; i++) {
    blk_lvl_int->count_shown[dev_id][i] = blk_lvl_int->count[dev_id][i];
  }
  return BF_INVALID_ARG;
}
bf_status_t lld_tof_dump_all_ints_cb(bf_dev_id_t dev_id,
                                     bf_subdev_id_t subdev_id,
                                     void *blk_lvl_int_vd) {
  int i, dump = 0;
  lld_blk_lvl_int_t *blk_lvl_int = (lld_blk_lvl_int_t *)blk_lvl_int_vd;
  (void)subdev_id;
  for (i = 0; i < 32; i++) {
    if (blk_lvl_int->count[dev_id][i] != 0) {
      dump = 1;
    }
  }
  if (dump == 0) return BF_SUCCESS;

  char *path = get_full_reg_path_name(dev_id, blk_lvl_int->status_reg);
  fprintf(rl_outstream,
          "%08x : %08x : %08x : %08x : %p : %s\n",
          blk_lvl_int->status_reg,
          blk_lvl_int->enable_hi_reg,
          blk_lvl_int->enable_lo_reg,
          blk_lvl_int->shadow_mask,
          blk_lvl_int->userdata[dev_id],
          (path) ? path : "NONE");

  for (i = 0; i < 32; i++) {
    if (blk_lvl_int->count[dev_id][i] != 0) {
      fprintf(rl_outstream,
              ": [%2d:%2d] : %d : ",
              i,
              i,
              blk_lvl_int->count[dev_id][i]);
      dump_field_name_by_offset((int)dev_id, blk_lvl_int->status_reg, i);
      fprintf(rl_outstream, "\n");
      // update "shown"
      blk_lvl_int->count_shown[dev_id][i] = blk_lvl_int->count[dev_id][i];
    }
  }
  return BF_INVALID_ARG;
}
bf_status_t lld_tof_dump_unfired_ints_cb(bf_dev_id_t dev_id,
                                         bf_subdev_id_t subdev_id,
                                         void *blk_lvl_int_vd) {
  int i, dump = 0;
  lld_blk_lvl_int_t *blk_lvl_int = (lld_blk_lvl_int_t *)blk_lvl_int_vd;
  int n_flds = get_reg_num_fields(dev_id, blk_lvl_int->status_reg);
  (void)subdev_id;
  for (i = 0; i < n_flds; i++) {
    if (blk_lvl_int->count[dev_id][i] == 0) {
      dump = 1;
      break;
    }
  }
  if (dump == 0) return BF_SUCCESS;
  char *path = get_full_reg_path_name(dev_id, blk_lvl_int->status_reg);
  fprintf(rl_outstream,
          "%08x : %08x : %08x : %08x : %p : %s\n",
          blk_lvl_int->status_reg,
          blk_lvl_int->enable_hi_reg,
          blk_lvl_int->enable_lo_reg,
          blk_lvl_int->shadow_mask,
          blk_lvl_int->userdata[dev_id],
          (path) ? path : "NONE");
  lld_inttest_error_set(get_full_reg_path_name(dev_id, blk_lvl_int->status_reg),
                        blk_lvl_int->status_reg,
                        0xffffffff,
                        blk_lvl_int->status_reg);

  for (i = 0; i < n_flds; i++) {
    if (blk_lvl_int->count[dev_id][i] == 0) {
      fprintf(rl_outstream,
              ": [%2d:%2d] : %d : ",
              i,
              i,
              blk_lvl_int->count[dev_id][i]);
      dump_field_name_by_offset((int)dev_id, blk_lvl_int->status_reg, i);
      fprintf(rl_outstream, "\n");
    }
  }
  return BF_INVALID_ARG;
}
#endif /* __KERNEL__ */
